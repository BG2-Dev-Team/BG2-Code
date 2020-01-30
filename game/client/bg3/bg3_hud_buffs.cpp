/*
BG3 - simple icon display of the buff currently applied to the local player
*/
#include "cbase.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "c_playerresource.h"
#include "clientmode_hl2mpnormal.h"
#include <vgui_controls/controls.h>
#include <vgui_controls/panel.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <KeyValues.h>
#include "IGameUIFuncs.h" // for key bindings
#include "c_baseplayer.h"
#include "c_hl2mp_player.h"
#include "c_team.h"
#include "c_te_effect_dispatch.h"

#include "../../shared/bg3/bg3_buffs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define ICON_WIDTH_HEIGHT 64
#define ICON_VIBRATE_DURATION 2

//==============================================
// CBuffIcons
// Displays buff status
//==============================================
class CBuffIcons : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CBuffIcons, vgui::Panel);
public:
	CBuffIcons(const char *pElementName);
	void Init(void);
	void VidInit(void);
	virtual bool ShouldDraw(void);
	virtual void SetVisible(bool bVisible) override;
	virtual void Paint(void);
	virtual void ApplySchemeSettings(vgui::IScheme *scheme);
	virtual void ApplySettings(KeyValues* inResourceData) override;
	//void MsgFunc_flagstatus( bf_read &msg );

	//Use different paint functions for officer/normal view
	void PaintOfficerView			();
	void PaintDefaultView			();

	void HideShowLabels(bool bVisible);
	
	void LocalizeOfficerViewLabels();
	void LocalizeEffectLabels(int iRallyFlags);
	void PositionEffectLabels();
	
	friend void RallyEffectCallback(const CEffectData& data);
private:
	bool			m_bActivated = false;
	int				m_iRallyFlags = 0; //local copy so we don't have to access the local player all the time
	int				m_iTeam = TEAM_BRITISH; //to reduce stack size on paint functions
	C_HL2MP_Player* m_pPlayer = nullptr;
	bool			m_bOfficerView = false;
	CHudTexture*	m_pCurIcon;
	float			m_flNextCommandLabelUpdate; //let's not do the expensive look-up too often

	vgui::Label*	m_ppCommandLabels[RALLY_NUM_RALLIES];
	vgui::Label*	m_pBuffEffectLabel;
	
};

float g_flLastBuffTime = -FLT_MAX;
CHudTexture* g_pEmptyDarkIcon;
Color g_ColourWhite(255, 255, 255, 255);

using namespace vgui;

DECLARE_HUDELEMENT(CBuffIcons);

CBuffIcons* g_pBuffIcons;

//==============================================
// CBuffIcons
// Constructor
//==============================================
CBuffIcons::CBuffIcons(const char *pElementName) :
CHudElement(pElementName), BaseClass(NULL, "BuffIcons")
{
	Warning("Creating BuffIcons!\n");
	g_pBuffIcons = this;
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent(pParent);
	Color ColourWhite(255, 255, 255, 255);
	SetHiddenBits(HIDEHUD_MISCSTATUS);
	SetSize(ScreenWidth(), ScreenHeight()); //For whatever reason, this has to be set, otherwise the game won't set the dimensions properly. -HairyPotter
	
	m_pCurIcon = NULL;
	m_flNextCommandLabelUpdate = 0.0f;

	//Initialize labels for command menu
	for (int i = 0; i < RALLY_NUM_RALLIES; i++) {
		m_ppCommandLabels[i] = new vgui::Label(this, "BuffIcons_labels", "DEFAULT");
		m_ppCommandLabels[i]->SetPaintBackgroundEnabled(false);
		m_ppCommandLabels[i]->SizeToContents();
		m_ppCommandLabels[i]->SetContentAlignment(vgui::Label::a_center);
		m_ppCommandLabels[i]->SetFgColor(g_ColourWhite);
	}

	m_pBuffEffectLabel = new Label(this, "BuffIcons_info", "");
	m_pBuffEffectLabel->SetPaintBackgroundEnabled(false);
	m_pBuffEffectLabel->SizeToContents();
	m_pBuffEffectLabel->SetContentAlignment(Label::a_west);
	m_pBuffEffectLabel->SetFgColor(g_ColourWhite);
}

//==============================================
// CBuffIcons's ApplySchemeSettings
// applies the schemes
//==============================================
void CBuffIcons::ApplySchemeSettings(IScheme *scheme)
{
	vgui::HFont font = scheme->GetFont("HudBG2Font");

	for (int i = 0; i < RALLY_NUM_RALLIES; i++) {
		m_ppCommandLabels[i]->SetFont(font);
	}
	m_pBuffEffectLabel->SetFont(font);

	BaseClass::ApplySchemeSettings(scheme);

	SetPaintBackgroundEnabled(false);
}

//==============================================
// CBuffIcons's ApplySchemeSettings
// sets up label locations
//==============================================
void CBuffIcons::ApplySettings(KeyValues *inResourceData)
{
	int x = ICON_WIDTH_HEIGHT / 2 - 2;
	int y = ICON_WIDTH_HEIGHT + 2;

	for (int i = 0; i < RALLY_NUM_RALLIES; i++) {
		m_ppCommandLabels[i]->SetPos(x, y);
		x += ICON_WIDTH_HEIGHT;
	}

	BaseClass::ApplySettings(inResourceData);
}

//==============================================
// CHudFlags's Init
// Inits any vars needed
//==============================================
void CBuffIcons::Init(void)
{
}

//==============================================
// CHudFlags's VidInit
// Inits any textures needed
//==============================================
void CBuffIcons::VidInit(void)
{
	BG3Buffs::InitializeIcons();
	BG3Buffs::InitializeTexts();
	//Get a default icon
	m_pCurIcon = BG3Buffs::RallyIconFrom(0);

	//get dark icon for officer's fill-o-meter
	g_pEmptyDarkIcon = gHUD.GetIcon("buff_empty_dark");
}

//==============================================
// CHudFlags's ShouldDraw
// whether the panel should be drawing
//==============================================
bool CBuffIcons::ShouldDraw(void)
{
	m_pPlayer = static_cast<C_HL2MP_Player*>(C_HL2MP_Player::GetLocalPlayer());

	if (!m_pPlayer || m_pPlayer->IsPlayerDead())
		return false;

	// Don't draw hud if we're using Iron Sights. -HairyPotter
	C_BaseCombatWeapon *wpn = m_pPlayer->GetActiveWeapon();
	if (wpn && wpn->m_bIsIronsighted)
		return false;
	//

	m_iRallyFlags = m_pPlayer->RallyGetCurrentRallies();

	m_bOfficerView = m_pPlayer->GetClass() == CLASS_OFFICER /*&& !m_iRallyFlags*/;

	//don't draw icons if we don't have a buff
	//or if we're not an officer
	bool bShow = (m_iRallyFlags || m_bOfficerView) && CHudElement::ShouldDraw();
	if (bShow)
		m_pBuffEffectLabel->SetVisible(m_iRallyFlags);

	return bShow;
}

void CBuffIcons::SetVisible(bool bVisible) {
	BaseClass::SetVisible(bVisible);
}

//==============================================
// CBuffIcon's Paint
// errr... paints the panel
//==============================================
void CBuffIcons::Paint() {
	m_pPlayer = static_cast<C_HL2MP_Player*> (C_HL2MP_Player::GetLocalPlayer());

	if (m_pPlayer) {
		//update variables
		m_pCurIcon = BG3Buffs::RallyIconFrom(m_iRallyFlags);
		m_iTeam = m_pPlayer->GetTeamNumber();

		if (m_bOfficerView) {
			PaintOfficerView();
			HideShowLabels(true);
		}
		else {
			PaintDefaultView();
			HideShowLabels(false);
		}
	}
	
}

//==============================================
// CBuffIcon's Paint for rallied players
// includes rallied officers
//==============================================
#define ICON_GROWTH_SIZE 8

//we offset non-vibrating icons such that the vibrating ones can be at 0
#define ICON_GROWTH_OFFSET ((ICON_GROWTH_SIZE) / 2) 
void CBuffIcons::PaintDefaultView() {

	int iBaseSize = ICON_WIDTH_HEIGHT;
	int x, y;
	x = y = ICON_GROWTH_OFFSET;

	//calculate time for which we've been rallied
	float flTimeRallied = gpGlobals->curtime - g_flLastBuffTime;//gpGlobals->curtime - (BG3Buffs::GetEndRallyTime(m_iTeam) - BG3Buffs::GetRallyDuration(m_iRallyFlags));

	if (flTimeRallied < ICON_VIBRATE_DURATION) {
		flTimeRallied *= 20;

		//convert to int then use that to determine whether to grow size
		int iTimeRallied = flTimeRallied;

		if (iTimeRallied & 1) {
			iBaseSize += ICON_GROWTH_SIZE;
			x = y = 0;
		}
	}

	m_pCurIcon->DrawSelf(x, y, iBaseSize, iBaseSize, g_ColourWhite);
}

//==============================================
// CBuffIcon's Paint for officers
//		shows all icons at once
//		the icon fills up until another rally is available
//==============================================
void CBuffIcons::PaintOfficerView() {

	int iBaseSize = ICON_WIDTH_HEIGHT;
	int x, y;
	x = y = ICON_GROWTH_OFFSET;

	CHudTexture* pExcluded = nullptr;
	float xExcluded, yExcluded;
	xExcluded = yExcluded = 0;

	for (int i = 0; i < BG3Buffs::NONE; i++) {
		m_pCurIcon = BG3Buffs::g_ppIcons[i];

		//don't draw the active icon, we'll draw it last instead
		if (m_pCurIcon == BG3Buffs::RallyIconFrom(m_iRallyFlags)) {
			pExcluded = m_pCurIcon;
			xExcluded = x; yExcluded = y;
		}
		else {
			//First draw the background icon
			m_pCurIcon->DrawSelf(x, y, iBaseSize, iBaseSize, g_ColourWhite);

			//now to draw the black bar... it grows from the top and shrinks from the bottom as time passes
			//get a float ratio of how much of the icon should be "empty"
			float flRatioEmpty = BG3Buffs::GetTimeUntilNextRally(m_iTeam) / RALLY_INTERVAL;
			Clamp(flRatioEmpty, 0.0f, 1.0f);

			g_pEmptyDarkIcon->DrawSelfCropped(x, y, 0, 0,
				g_pEmptyDarkIcon->Width(), g_pEmptyDarkIcon->Height() * flRatioEmpty,
				iBaseSize, iBaseSize * flRatioEmpty, g_ColourWhite);
		}
		x += ICON_WIDTH_HEIGHT;
	}

	//Finally, draw the icon we missed, if we missed one
	if (pExcluded) {

		//Get time rallied
		float flTimeRallied = gpGlobals->curtime - g_flLastBuffTime;//gpGlobals->curtime - (BG3Buffs::GetEndRallyTime(m_iTeam) - BG3Buffs::GetRallyDuration(m_iRallyFlags));
		
		//Vibrate the icon if we were rallied recently
		if (flTimeRallied < ICON_VIBRATE_DURATION) {
			flTimeRallied *= 20;

			//convert to int then use that to determine whether to grow size
			int iTimeRallied = flTimeRallied;

			if (iTimeRallied & 1) {
				iBaseSize += ICON_GROWTH_SIZE;
				xExcluded -= ICON_GROWTH_OFFSET;
				yExcluded -= ICON_GROWTH_OFFSET; //pulled coordinates in so we're still centered
			}
		}

		pExcluded->DrawSelf(xExcluded, yExcluded, iBaseSize, iBaseSize, g_ColourWhite);
	}

	//Now draw all the labels
	LocalizeOfficerViewLabels();
}

//========================================================================
// CBuffIcon's Paint for officers
//		shows the label for the key
//			the player needs to press for each command
//========================================================================
void CBuffIcons::LocalizeOfficerViewLabels() {
	if (gpGlobals->curtime > m_flNextCommandLabelUpdate) {
		m_ppCommandLabels[0]->SetText(KeyCodeToDisplayString(gameuifuncs->GetButtonCodeForBind("vcomm_advance")));
		m_ppCommandLabels[1]->SetText(KeyCodeToDisplayString(gameuifuncs->GetButtonCodeForBind("vcomm_fire")));
		m_ppCommandLabels[2]->SetText(KeyCodeToDisplayString(gameuifuncs->GetButtonCodeForBind("vcomm_rally")));
		m_ppCommandLabels[3]->SetText(KeyCodeToDisplayString(gameuifuncs->GetButtonCodeForBind("vcomm_retreat")));

		for (int i = 0; i < RALLY_NUM_RALLIES; i++) {
			m_ppCommandLabels[i]->SizeToContents();
		}

		m_flNextCommandLabelUpdate = gpGlobals->curtime + 1.0f;
	}
}


//========================================================================
// Status effect localiztion
//========================================================================

void CBuffIcons::LocalizeEffectLabels(int iRallyFlags) {
	wchar_t* text = L"";
	switch (iRallyFlags) {
	case RALLY_ADVANCE:
		text = BG3Buffs::GetTextForBuff(BG3Buffs::ADVANCE);
		break;
	case RALLY_FIRE:
		text = BG3Buffs::GetTextForBuff(BG3Buffs::FIRE);
		break;
	case RALLY_RALLY_ROUND:
		text = BG3Buffs::GetTextForBuff(BG3Buffs::RALLY_ROUND);
		break;
	case RALLY_RETREAT:
		text = BG3Buffs::GetTextForBuff(BG3Buffs::RETREAT);
		break;
	}
	m_pBuffEffectLabel->SetText(text);
	m_pBuffEffectLabel->SizeToContents();
}

void CBuffIcons::PositionEffectLabels() {
	if (m_bOfficerView) {
		m_pBuffEffectLabel->SetPos(ICON_WIDTH_HEIGHT * 4, 0);
	}
	else {
		m_pBuffEffectLabel->SetPos(ICON_WIDTH_HEIGHT, 0);
	}
}

//========================================================================
// CBuffIcon's Sets visiblity of labels
//========================================================================
void CBuffIcons::HideShowLabels(bool bVisible) {
	for (int i = 0; i < RALLY_NUM_RALLIES; i++)
		m_ppCommandLabels[i]->SetVisible(bVisible);
}

void RallyEffectCallBack(const CEffectData& data) {
	C_HL2MP_Player* pPlayer = C_HL2MP_Player::GetLocalHL2MPPlayer();

	if (pPlayer->RallyGetCurrentRallies()) {
		g_pBuffIcons->LocalizeEffectLabels(pPlayer->RallyGetCurrentRallies());
		g_pBuffIcons->PositionEffectLabels();
	}
	g_flLastBuffTime = gpGlobals->curtime;
}

DECLARE_CLIENT_EFFECT("RalEnab", RallyEffectCallBack);