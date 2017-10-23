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
#include "c_baseplayer.h"
#include "c_hl2mp_player.h"
#include "c_team.h"

#include "../../shared/bg3/bg3_buffs.h"

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
	virtual void Paint(void);
	virtual void ApplySchemeSettings(vgui::IScheme *scheme);
	//void MsgFunc_flagstatus( bf_read &msg );

	//Use different paint functions for officer/normal view
	void PaintOfficerView();
	void PaintDefaultView();

private:
	int				m_iRallyFlags = 0; //local copy so we don't have to access the local player all the time
	int				m_iTeam = TEAM_BRITISH; //to reduce stack size on paint functions
	C_HL2MP_Player* m_pPlayer = nullptr;
	bool			m_bOfficerView = false;
	CHudTexture*	m_pCurIcon;

};

CHudTexture* g_pEmptyDarkIcon;
Color g_ColourWhite(255, 255, 255, 255);

using namespace vgui;

DECLARE_HUDELEMENT(CBuffIcons);

//==============================================
// CBuffIcons
// Constructor
//==============================================
CBuffIcons::CBuffIcons(const char *pElementName) :
CHudElement(pElementName), BaseClass(NULL, "BuffIcons")
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent(pParent);
	Color ColourWhite(255, 255, 255, 255);
	SetHiddenBits(HIDEHUD_MISCSTATUS);
	SetSize(ScreenWidth(), ScreenHeight()); //For whatever reason, this has to be set, otherwise the game won't set the dimensions properly. -HairyPotter
	
	m_pCurIcon = NULL;
}

//==============================================
// CBuffIcons's ApplySchemeSettings
// applies the schemes
//==============================================
void CBuffIcons::ApplySchemeSettings(IScheme *scheme)
{
	BaseClass::ApplySchemeSettings(scheme);

	SetPaintBackgroundEnabled(false);
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

	if (!m_pPlayer)
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
	bool bShow = m_iRallyFlags && CHudElement::ShouldDraw()
		|| m_bOfficerView;

	return bShow;
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

		if (m_bOfficerView)
			PaintOfficerView();
		else
			PaintDefaultView();
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
	float flTimeRallied = gpGlobals->curtime - (BG3Buffs::GetEndRallyTime(m_iTeam) - BG3Buffs::GetRallyDuration(m_iRallyFlags));

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
		float flTimeRallied = gpGlobals->curtime - (BG3Buffs::GetEndRallyTime(m_iTeam) - BG3Buffs::GetRallyDuration(m_iRallyFlags));
		
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
}