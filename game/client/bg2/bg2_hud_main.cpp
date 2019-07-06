/*
	The Battle Grounds 2 - A Source modification
	Copyright (C) 2005, The Battle Grounds 2 Team and Contributors

	The Battle Grounds 2 free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The Battle Grounds 2 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

	Contact information:
		Tomas "Tjoppen" Härdin		mail, in reverse: se . gamedev @ tjoppen
		Jason "Draco" Houston		iamlorddraco@gmail.com

	You may also contact us via the Battle Grounds website and/or forum at:
		www.bgmod.com
*/
#include "cbase.h"
#include "hudelement.h"
#include "hl2mp_gamerules.h"
//BG2 - Tjoppen - #includes
#include "engine/IEngineSound.h"
#include "../shared/bg2/weapon_bg2base.h"
#include "../shared/bg3/bg3_class_quota.h"
#include "bg2_hud_main.h"
#include "bg3/bg3_soft_info.h"
#include "bg3/bg3_hud_compass.h"
//

// hintbox header
//#include "hintbox.h" //For now at least. -HairyPotter


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

DECLARE_HUDELEMENT( CHudBG2 );
DECLARE_HUD_MESSAGE( CHudBG2, HitVerif );
DECLARE_HUD_MESSAGE( CHudBG2, WinMusic );
DECLARE_HUD_MESSAGE( CHudBG2, CaptureSounds ); //HairyPotter
DECLARE_HUD_MESSAGE( CHudBG2, VCommSounds );

static char g_hudBuffer[512];

static const int g_iFlagOffset = 100; //how far should top-center HUD flags be from center?
static const int g_iClassCountY = 150;
static const int g_iClassIconSize = 32;

inline Color getColorForHealth(int remainingHealth) {
	int nonRed = remainingHealth * 255 / 100; //white-to-red scale
	return Color(255, nonRed, nonRed, 255);
}

CHudBG2 *CHudBG2::s_pInstance = NULL;
CHudBG2 *CHudBG2::GetInstance()
{
	return CHudBG2::s_pInstance;
}
//==============================================
// CHudBG2's CHudFlags
// Constructor
//==============================================
CHudBG2::CHudBG2( const char *pElementName ) :
	CHudElement( pElementName ), BaseClass( NULL, "HudBG2" ),
	m_IsVictimAccumulator(true), m_IsAttackerAccumulator(false)
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	s_pInstance = this; //BG2 - HairyPotter (Nifty: Why is this here? If you're inside of the object, the "this" keyword is the address to the current object, and it's always available. Outside of the object, you can get access to an object's address by properly getting the reference.)

	SetHiddenBits( HIDEHUD_ALL );//HIDEHUD_MISCSTATUS );

	Color ColourWhite( 255, 255, 255, 255 );

	auto createLabel = [&](Label** ppLabel) {
		*ppLabel = new vgui::Label(this, "RoundState_warmpup", "");
		(*ppLabel)->SetPaintBackgroundEnabled(false);
		(*ppLabel)->SetPaintBorderEnabled(false);
		(*ppLabel)->SizeToContents();
		(*ppLabel)->SetContentAlignment(vgui::Label::a_west);
		(*ppLabel)->SetFgColor(ColourWhite);
	};

	createLabel(&m_pLabelBScore);
	createLabel(&m_pLabelAScore);
	createLabel(&m_pLabelBTickets);
	createLabel(&m_pLabelATickets);
	createLabel(&m_pLabelCurrentRound);
	createLabel(&m_pLabelAmmo);
	createLabel(&m_pLabelDeathMessage);
	createLabel(&m_pLabelRoundTime);
	createLabel(&m_pLabelDamageTaken);
	createLabel(&m_pLabelDamageGiven);
	createLabel(&m_pLabelLMS);
	createLabel(&m_pLabelHealth);
	for (int i = 0; i < 6; i++) {
		createLabel(m_pClassCountLabels + i);
	}

	CBaseEntity::PrecacheScriptSound( "Americans.win" );
	CBaseEntity::PrecacheScriptSound( "British.win" );

	Reset();
}

void CHudBG2::ApplySettings(KeyValues *inResourceData)
{
	float factor = 2;

	//we've designed for half size at 640x480, meaning full size at 1280x960
	//note that this isn't a particularly common resolution, but whatever
	//the following makes it so the hud scales down for lower resolutions but never scales up (consider 1920x1080 or so)
	if (ScreenHeight() < 960)
		factor = ScreenHeight() / 480.0f;

	//since the position and size of the hud is set automagically we need to fix them up here
	//also, I'm a bit paranoid about parsing ypos myself
	//hence I simply invented yposr whose value is "80" instead of "r80" and thus easily parsed
	/*SetPos(factor * inResourceData->GetInt("xpos"),
		   ScreenHeight() - factor * inResourceData->GetInt("yposr"));*/
	SetPos(0, 0);
	//SetWide(factor * inResourceData->GetInt("wide"));
	//SetTall(factor * inResourceData->GetInt("tall"));
	SetSize(ScreenWidth(), ScreenHeight());

#define GET_COORD(name) factor * inResourceData->GetInt(#name)
#define GET(name) name = GET_COORD(name)

	const int halfx = ScreenWidth() / 2;
	const int secondRowOffset = 35;

	//position round score labels
	m_pLabelAScore->SetPos(halfx - g_iFlagOffset + 2, secondRowOffset);
	m_pLabelBScore->SetPos(halfx + 70, secondRowOffset); //leave room on left of flag
	m_pLabelBScore->SetSize(30, 20);
	m_pLabelAScore->SetContentAlignment(Label::Alignment::a_west);
	m_pLabelBScore->SetContentAlignment(Label::Alignment::a_east);

	//position ticket score labels
	m_pLabelATickets->SetPos(halfx - g_iFlagOffset + 2, 2);
	m_pLabelBTickets->SetPos(halfx, 2);
	m_pLabelBTickets->SetSize(100, 20);
	m_pLabelATickets->SetContentAlignment(Label::Alignment::a_west);
	m_pLabelBTickets->SetContentAlignment(Label::Alignment::a_east);

	//position round and respawn times
	m_pLabelRoundTime->SetPos(halfx - 25, secondRowOffset);
	m_pLabelRoundTime->SetContentAlignment(Label::Alignment::a_center);

	//round counter
	m_pLabelCurrentRound->SetPos(halfx - 50, m_pLabelRoundTime->GetYPos() + m_pLabelRoundTime->GetTall());

	//health label
	int marginLeft = 60;
	int marginBottom = ScreenHeight() - 50;
	m_pLabelHealth->SetPos(marginLeft, marginBottom);

	//ammo counter
	m_pLabelAmmo->SetPos(320, marginBottom);

	//death message
	m_pLabelDeathMessage->SetPos(10, ScreenHeight() * 0.8f);

	//class count labels
	for (int i = 0; i < 6; i++) {
		m_pClassCountLabels[i]->SetPos(44, g_iClassCountY + i * 34);
	}

	//grab and scale coordinates and dimensions
	m_pLabelLMS         ->SetPos(GET_COORD(lmsx),      GET_COORD(lmsy));

	BaseClass::ApplySettings(inResourceData);
}

//==============================================
// CHudBG2's ApplySchemeSettings
// applies the schemes
//==============================================
void CHudBG2::ApplySchemeSettings( IScheme *scheme )
{
	vgui::HFont font = scheme->GetFont("HudBG3Font");
	vgui::HFont largeFont = scheme->GetFont("HudBG3FontLarge");
	m_pLabelAScore->SetFont(font);
	m_pLabelBScore->SetFont(font);
	m_pLabelATickets->SetFont(font);
	m_pLabelBTickets->SetFont(font);
	m_pLabelLMS->SetFont(font);
	m_pLabelCurrentRound->SetFont(font);
	m_pLabelRoundTime->SetFont(font);
	m_pLabelDeathMessage->SetFont(largeFont);
	m_pLabelAmmo->SetFont(largeFont);
	m_pLabelHealth->SetFont(largeFont);

	BaseClass::ApplySchemeSettings( scheme );
	SetPaintBackgroundEnabled( false );

}

//==============================================
// CHudBG2's Init
// Inits any vars needed
//==============================================
void CHudBG2::Init( void )
{
	HOOK_HUD_MESSAGE( CHudBG2, HitVerif );
	HOOK_HUD_MESSAGE( CHudBG2, WinMusic );
	HOOK_HUD_MESSAGE( CHudBG2, CaptureSounds );
	HOOK_HUD_MESSAGE( CHudBG2, VCommSounds );
	//BG2 - Tjoppen - serverside blood, a place to hook as good as any
	extern void  __MsgFunc_ServerBlood( bf_read &msg );
	HOOK_MESSAGE( ServerBlood );
	//
}

//==============================================
// CHudBG2's VidInit
// Inits any textures needed
//==============================================
void CHudBG2::VidInit( void )
{

	m_pAmerFlagImage = gHUD.GetIcon("hud_aflag"); //scheme()->GetImage("hud/aflag", false);
	m_pBritFlagImage = gHUD.GetIcon("hud_bflag"); //scheme()->GetImage("hud/bflag", false);
	m_pBlackBackground			= scheme()->GetImage("hud/bg", false);
	m_pStaminaBarImage			= scheme()->GetImage("hud/staminabar", false);
	m_pHealthBarImage[0]		= scheme()->GetImage("hud/healthbar", false);
	m_pHealthBarImage[1]		= scheme()->GetImage("hud/healthbar75", false);
	m_pHealthBarImage[2]		= scheme()->GetImage("hud/healthbar50", false);
	m_pHealthBarImage[3]		= scheme()->GetImage("hud/healthbar25", false);
	m_pHealthBarOverlayImage	= scheme()->GetImage("classmenu/statbarmeter", false);
	m_pHealthSymbolImage[0]		= scheme()->GetImage("hud/health_cross", false);
	m_pHealthSymbolImage[1]		= scheme()->GetImage("hud/health_cross75", false);
	m_pHealthSymbolImage[2]		= scheme()->GetImage("hud/health_cross50", false);
	m_pHealthSymbolImage[3]		= scheme()->GetImage("hud/health_cross25", false);
	m_pBottomLeftBackground		= scheme()->GetImage("hud/bottomleftbackground", false);
	m_pAmmoBallImage			= scheme()->GetImage("hud/ammoball", false);
	m_pSwingometerRedImage		= scheme()->GetImage("hud/swingometer_red", false);
	m_pSwingometerBlueImage		= scheme()->GetImage("hud/swingometer_blue", false);

	char buffer[40];
	for (int i = 0; i < TOTAL_AMER_CLASSES; i++) {
		const CPlayerClass* pClass = CPlayerClass::fromNums(TEAM_AMERICANS, i);
		Q_snprintf(buffer, ARRAYSIZE(buffer), "classmenu/classes/a/%s", pClass->m_pszAbbreviation);
		m_pAmericanClassIcons[i] = scheme()->GetImage(buffer, false);
	}
	for (int i = 0; i < TOTAL_BRIT_CLASSES; i++) {
		const CPlayerClass* pClass = CPlayerClass::fromNums(TEAM_BRITISH, i);
		Q_snprintf(buffer, ARRAYSIZE(buffer), "classmenu/classes/b/%s", pClass->m_pszAbbreviation);
		m_pBritishClassIcons[i] = scheme()->GetImage(buffer, false);
	}
	
}

//==============================================
// CHudBG2's ShouldDraw
// whether the panel should be drawing
//==============================================
bool CHudBG2::ShouldDraw( void )
{
	bool temp = CHudElement::ShouldDraw();
	HideShowAllTeam(temp);	//BG2 - Tjoppen - HACKHACK
	HideShowAllPlayer(temp);	//BG2 - Tjoppen - HACKHACK
	m_pLabelDeathMessage->SetVisible(temp);	//BG2 - Tjoppen - HACKHACK
	return temp;
}

//==============================================
// CHudBG2's Paint
// errr... paints the panel
//==============================================
static ConVar cl_draw_lms_indicator( "cl_draw_lms_indicator", "1", FCVAR_CLIENTDLL, "Draw last man standing indicator string?" );

static Color colorForFlash( float flFlashEnd )
{
	//flash from red back to white
	//hold at red for 250 ms, fade to white in 250 ms
	float value = (gpGlobals->curtime - flFlashEnd) * 4 * 256;
	value = clamp(value, 0, 255);

	return Color(255, value, value, 255);
}

// BG2 - VisualMelon - Complain to me about these being in the wrong place
extern int hitVerificationHairs;
extern int hitVerificationLatency;
ConVar cl_hitveriflatency( "cl_hitveriflatency", "2.0", FCVAR_ARCHIVE | FCVAR_CLIENTDLL, "How many seconds to show hit verification for?" ); // BG2 - VisualMelon - I'm bad at names

bool CHudBG2::ShouldDrawPlayer(C_HL2MP_Player** ppPlayer, C_BaseCombatWeapon** ppWeapon) {
	C_HL2MP_Player *pHL2Player = dynamic_cast<C_HL2MP_Player*>(C_HL2MP_Player::GetLocalPlayer());
	if ((!pHL2Player || !pHL2Player->IsAlive()))
	{
		//spectating
		int n = GetSpectatorTarget();

		if (n <= 0) {
			HideShowAllPlayer(false);
			return false;
		}

		pHL2Player = dynamic_cast<C_HL2MP_Player*>(UTIL_PlayerByIndex(n));

		if (!pHL2Player || !pHL2Player->IsAlive())
		{
			HideShowAllPlayer(false);
			return false;
		}
	}

	C_BaseCombatWeapon *wpn = pHL2Player->GetActiveWeapon();
	if (wpn && wpn->m_bIsIronsighted)
		// Don't draw hud if we're using Iron Sights. -HairyPotter
	{
		HideShowAllPlayer(false);
		return false;
	}

	*ppPlayer = pHL2Player;
	*ppWeapon = wpn;
	HideShowAllPlayer(true);
	HideShowAllTeam(true);
	return true;
}

void CHudBG2::PaintSwingometer(/*C_HL2MP_Player* pHL2Player,*/ int swingScoreA, int swingScoreB) {
	//just using these aliases
	int& swinga = swingScoreA; int& swingb = swingScoreB;

	float swing = m_flLastSwing;
	int tot = swinga + swingb;

	//guard against negatives. you never know..
	if (swinga >= 0 && swingb >= 0 && tot > 0)
		swing = swinga / (float)tot;
	else
		swing = 0.5f;	//returns to center when both scores are zero (round reset for instance)

	//move swing value towards m_flLastSwing
	float swingdiff = swing - m_flLastSwing;	//useful for setting alpha on bars
	float swingadjust = swingdiff;

	//cap swing rate so each percent takes 20 ms (meaning a full swing = 2 sec)
	float swingrate = 0.5f;

	if (swingadjust < -swingrate * gpGlobals->frametime) swingadjust = -swingrate * gpGlobals->frametime;
	if (swingadjust >  swingrate * gpGlobals->frametime) swingadjust = swingrate * gpGlobals->frametime;

	swing = m_flLastSwing + swingadjust;

	//we have 200 pixels, but actual range will be from 20-180 to give enough room for the numbers
	//to stay on their team's background.
	int bluex = ScreenWidth() / 2 - 100;
	int bluew = 20 + 160 * swing;
	m_pSwingometerBlueImage->SetPos(bluex, 0);
	m_pSwingometerBlueImage->SetSize(bluew, 30);
	m_pSwingometerBlueImage->Paint();

	m_pSwingometerRedImage->SetPos(bluex + bluew, 0);
	m_pSwingometerRedImage->SetSize(200 - bluew, 30);
	m_pSwingometerRedImage->Paint();

	m_flLastSwing = swing;

	//figure out whether our ticket meter should flash
	extern ConVar mp_tickets_drain_a, mp_tickets_drain_b;

	C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
	if (pPlayer && HL2MPRules()->UsingTickets())
	{
		//Avoid spamming the player - only flash their team's number
		//HACKHACK: Assume ticket decrease >= drain means we're being drained.
		//          That or a whole bunch of players on our team spawned.
		//          Require the drain/delta to be at least 2 though.
		int minDelta = 2;

		if (pPlayer->GetTeamNumber() == TEAM_AMERICANS && mp_tickets_drain_a.GetInt() >= minDelta &&
			swinga <= m_iLastSwingA - mp_tickets_drain_a.GetInt())
			m_flAFlashEnd = gpGlobals->curtime + 0.5f;

		if (pPlayer->GetTeamNumber() == TEAM_BRITISH   && mp_tickets_drain_b.GetInt() >= minDelta &&
			swingb <= m_iLastSwingB - mp_tickets_drain_b.GetInt())
			m_flBFlashEnd = gpGlobals->curtime + 0.5f;
	}

	m_iLastSwingA = swinga;
	m_iLastSwingB = swingb;
}


void CHudBG2::PaintBackgroundOnto(Label* pLabel) {
	if (pLabel->IsVisible()) {
		int textMargin = 3;
		m_pBlackBackground->SetPos(pLabel->GetXPos() - textMargin, pLabel->GetYPos());
		m_pBlackBackground->SetSize(pLabel->GetWide() + textMargin - 2, pLabel->GetTall());
		m_pBlackBackground->Paint();
	}
}

void CHudBG2::PaintTopCenterHUD(/*C_HL2MP_Player* pPlayer*/) {
	//this part of HUD doesn't scale with screen?
	const int halfx = ScreenWidth() / 2;
	const int flagW = 83;
	const int flagH = 50; // 5 by 3 aspect ratio

	//BLACK ROUNDTIME BACKGROUND
	if (m_pLabelCurrentRound->IsVisible()) PaintBackgroundOnto(m_pLabelCurrentRound);
	PaintBackgroundOnto(m_pLabelRoundTime);
	
	//SWINGOMETER
	//swingometer
	//displays based on tickets in ticket mode, score otherwise
	C_Team *pAmer = GetGlobalTeam(TEAM_AMERICANS);
	C_Team *pBrit = GetGlobalTeam(TEAM_BRITISH);
	int swingScoreA, swingScoreB;
	if (HL2MPRules()->UsingTickets()) {
		swingScoreA = pAmer ? pAmer->m_iTicketsLeft : 0;
		swingScoreB = pBrit ? pBrit->m_iTicketsLeft : 0;
	}
	else {
		swingScoreA = pAmer ? pAmer->Get_Score() : 0;
		swingScoreB = pBrit ? pBrit->Get_Score() : 0;
	}
	PaintSwingometer(/*pPlayer,*/ swingScoreA, swingScoreB);

	//NUMBER OF ROUNDS WON
	Q_snprintf(g_hudBuffer, 512, "%i ", swingScoreB);
	m_pLabelBTickets->SetText(g_hudBuffer);
	//m_pLabelBTickets->SizeToContents();
	m_pLabelBTickets->SetFgColor(colorForFlash(m_flBFlashEnd));
	

	Q_snprintf(g_hudBuffer, 512, "%i ", swingScoreA);
	m_pLabelATickets->SetText(g_hudBuffer);
	m_pLabelATickets->SizeToContents();
	m_pLabelATickets->SetFgColor(colorForFlash(m_flAFlashEnd));

	//FLAGS
	m_pAmerFlagImage->DrawSelf(halfx - g_iFlagOffset - flagW, 0, flagW, flagH, COLOR_WHITE);
	m_pBritFlagImage->DrawSelf(halfx + g_iFlagOffset, 0, flagW, flagH, COLOR_WHITE);

	//SWINGOMETER SCORE LABELS
	if (mp_rounds.GetBool() && pAmer && pBrit) {
		Q_snprintf(g_hudBuffer, 512, "%i ", pBrit->Get_Score());	//BG2 - Tjoppen - avoid NULL
		m_pLabelBScore->SetText(g_hudBuffer);
		//m_pLabelBScore->SizeToContents();
		m_pLabelBScore->SetFgColor(COLOR_WHITE);
		PaintBackgroundOnto(m_pLabelBScore);

		Q_snprintf(g_hudBuffer, 512, "%i ", pAmer->Get_Score());	//BG2 - Tjoppen - avoid NULL
		m_pLabelAScore->SetText(g_hudBuffer);
		m_pLabelAScore->SizeToContents();
		m_pLabelAScore->SetFgColor(COLOR_WHITE);
		PaintBackgroundOnto(m_pLabelAScore);
	}

	//ROUND COUNTER
	extern ConVar mp_rounds;
	bool bHasRoundCount = mp_rounds.GetBool();
	if (bHasRoundCount) {
		Q_snprintf(g_hudBuffer, 512, "Round %i/%i ", HL2MPRules()->m_iCurrentRound, mp_rounds.GetInt());
		m_pLabelCurrentRound->SetText(g_hudBuffer);
		m_pLabelCurrentRound->SizeToContents();
		m_pLabelCurrentRound->SetFgColor(COLOR_WHITE);
	}

	//ROUND TIMER
	//update round timer text
	int roundtime;
	if (bHasRoundCount) {
		roundtime = ceilf(HL2MPRules()->m_fLastRoundRestart + mp_roundtime.GetFloat() - gpGlobals->curtime);
	}
	else {
		roundtime = ceilf(HL2MPRules()->GetMapRemainingTime()); // -gpGlobals->curtime);
	}
	//only show actual time if it's a reasonable time
	if (roundtime < 3600) {
		if (roundtime < 0)
			roundtime = 0; //no negative time
		Q_snprintf(g_hudBuffer, 32, "%i:%02i ", roundtime / 60, roundtime % 60);
		m_pLabelRoundTime->SetText(g_hudBuffer);
	}
	else {
		m_pLabelRoundTime->SetText(" ");
	}
	m_pLabelRoundTime->SizeToContents();
	m_pLabelRoundTime->SetFgColor(COLOR_WHITE);
}

void CHudBG2::PaintBottomLeftHUD(C_HL2MP_Player* pPlayer, C_BaseCombatWeapon* pWeapon) {
	//BACKGROUND
	m_pBottomLeftBackground->SetPos(0, ScreenHeight() - 60);
	m_pBottomLeftBackground->SetSize(600, 60);
	m_pBottomLeftBackground->Paint();

	//HEALTH SYMBOL
	const int SML = 10;
	int marginBottom = ScreenHeight() - 50;
	//pulsating
	int health = pPlayer->GetHealth();
	health = clamp(health, 0, 100);
	//bool pulsesPerformed = health < 100;
	
	const float PULSE_DURATION = 0.05f;
	//check for start of next pulse
	if (gpGlobals->curtime > m_flStartPulseTime) {
		m_flStartPulseTime = gpGlobals->curtime + health / 120.f;
		m_flEndPulseTime = gpGlobals->curtime + PULSE_DURATION;
	}
	int healthIconIndex = 0;
	if (health < 25) healthIconIndex = 3;
	else if (health < 50) healthIconIndex = 2;
	else if (health < 75) healthIconIndex = 1;
	IImage* pHealthIcon = m_pHealthSymbolImage[healthIconIndex];
	bool doPulse = health < 100 && m_flEndPulseTime > gpGlobals->curtime;
	if (doPulse) {
		float remainingTime = m_flEndPulseTime - gpGlobals->curtime;
		float scale = (remainingTime / PULSE_DURATION) + 0.1f;

		int offset = 4 * scale;
		pHealthIcon->SetPos(SML - offset, marginBottom - offset);
		pHealthIcon->SetSize(40 + 2 * offset, 40 + 2 * offset);
	}
	else {
		pHealthIcon->SetPos(10, marginBottom);
		pHealthIcon->SetSize(40, 40);
	}
	pHealthIcon->Paint();

	//HEALTH COUNTER
	Q_snprintf(g_hudBuffer, 4, "%i", health);
	m_pLabelHealth->SetText(g_hudBuffer);
	m_pLabelHealth->SizeToContents();
	m_pLabelHealth->SetFgColor(getColorForHealth(health));


	//HEALTH BAR
	//health label
	int marginLeft = 140;
	IImage* pHealthBar = m_pHealthBarImage[healthIconIndex];
	pHealthBar					->SetPos(marginLeft, marginBottom + 10);
	m_pHealthBarOverlayImage	->SetPos(marginLeft, marginBottom + 10);
	pHealthBar					->SetSize(max(health * 1.5f, 1), 20);
	m_pHealthBarOverlayImage	->SetSize(150, 20);
	//pHealthBar					->SetColor(getColorForHealth(health));
	pHealthBar					->Paint();
	m_pHealthBarOverlayImage	->Paint();

	//STAMINA BAR
	m_pStaminaBarImage->SetPos(marginLeft, marginBottom + 30);
	m_pStaminaBarImage->SetSize(max(1,pPlayer->m_iStamina * 150 / 100), 4);
	m_pStaminaBarImage->Paint();

	if (pWeapon) {
		//AMMO BALLS
		if (pWeapon->Def()->m_Attackinfos[0].m_iAttacktype == ATTACKTYPE_FIREARM) {
			marginLeft += 230;
			int iAmmoCount = pPlayer->GetAmmoCount(pWeapon->GetPrimaryAmmoType());
			m_pAmmoBallImage->SetSize(16, 16);
			int x = marginLeft;
			int y = ScreenHeight() - 50;
			//paint rows of ammo ball images
			for (int i = 0; i < iAmmoCount; i++) {
				m_pAmmoBallImage->SetPos(x, y);
				m_pAmmoBallImage->Paint();
				x += 8;
				/*if (i % 8 == 0) {
					x = marginLeft;
					y += 8;
					}*/
			}
		}

		//AMMO COUNT LABEL
		int iAmmoCount = pPlayer->GetAmmoCount(pWeapon->GetPrimaryAmmoType());
		if (pWeapon->Clip1() > 0)
			iAmmoCount++;
		if (iAmmoCount > 0)
		{
			Q_snprintf(g_hudBuffer, 512, "%i ", iAmmoCount);
			m_pLabelAmmo->SetText(g_hudBuffer);
			if (pWeapon->Clip1() == 0)
			{
				m_pLabelAmmo->SetFgColor(COLOR_RED);
			}
			else
			{
				m_pLabelAmmo->SetFgColor(COLOR_WHITE);
			}
			m_pLabelAmmo->SizeToContents();
		}
		else
			m_pLabelAmmo->SetVisible(false);
	}
}

void CHudBG2::PaintDeathMessage() {
	C_BasePlayer* pLocalPlayer = C_BasePlayer::GetLocalPlayer();
	if (pLocalPlayer && pLocalPlayer->IsPlayerDead() && pLocalPlayer->GetTeamNumber() >= TEAM_AMERICANS) {
		m_pLabelDeathMessage->SetVisible(true);

		int wavetime = ceilf(HL2MPRules()->m_fLastRespawnWave + mp_respawntime.GetFloat() - gpGlobals->curtime);
		//Four modes - skirmish, tickets, tickets with no tickets remaining, LMS/Linebattle
		if (IsLMS()) {
			m_pLabelDeathMessage->SetText(g_pVGuiLocalize->Find("#BG3_Spawn_Next_Round"));
		}
		else {
			//Construct message
			C_Team* pTeam = GetGlobalTeam(pLocalPlayer->GetTeamNumber());
			wchar_t buffer[512];
			//Msg(g_pVGuiLocalize->FindAsUTF8("#BG3_Respawning_In"));

			if (IsSkirmish()) {
				V_snwprintf(buffer, 512,
					g_pVGuiLocalize->Find("#BG3_Respawning_In"), wavetime);
			}
			else if (pTeam->m_iTicketsLeft > 0) {
				wchar_t buffer2[256];
				wchar_t buffer3[256];
				V_snwprintf(buffer2, 256,
					g_pVGuiLocalize->Find("#BG3_Respawning_In"), wavetime);
				V_snwprintf(buffer3, 256, g_pVGuiLocalize->Find("#BG3_Tickets_Remaining"), pTeam->m_iTicketsLeft);
				V_snwprintf(buffer, 512, L"%s\n%s", buffer2, buffer3);

			}
			else if (IsTicketMode()) {
				//ticket mode with no tickets remaining
				V_snwprintf(buffer, 512, L"%s\n%s",
					g_pVGuiLocalize->Find("#BG3_Out_Of_Tickets"), g_pVGuiLocalize->Find("#BG3_Spawn_Next_Round"));
			}

			m_pLabelDeathMessage->SetText(buffer);
		}
		m_pLabelDeathMessage->SizeToContents();
		PaintBackgroundOnto(m_pLabelDeathMessage);
	}
	else {
		m_pLabelDeathMessage->SetVisible(false);
	}

}

void CHudBG2::PaintClassCounts() {
	C_HL2MP_Player* pPlayer = ToHL2MPPlayer(C_BasePlayer::GetLocalPlayer());
	if (pPlayer) {
		int iTeam = pPlayer->GetTeamNumber();
		if (iTeam >= TEAM_AMERICANS) {
			char buffer[16];
			NSoftInfo::Update();

			IImage** ppImages = iTeam == TEAM_AMERICANS ? m_pAmericanClassIcons : m_pBritishClassIcons;
			m_pClassCountLabels[5]->SetText("");
			int numClasses = CPlayerClass::numClassesForTeam(iTeam);
			for (int i = 0; i < numClasses; i++) {
				const CPlayerClass* pClass = CPlayerClass::fromNums(iTeam, i);
				int num = NSoftInfo::GetNumPlayersOfClass(pClass);
				int limit = NClassQuota::GetLimitForClass(pClass);
				if (limit >= 0)
					Q_snprintf(buffer, sizeof(buffer), "%i/%i", num, limit);
				else
					Q_snprintf(buffer, sizeof(buffer), "%i", num);
				m_pClassCountLabels[i]->SetText(buffer);
				m_pClassCountLabels[i]->SizeToContents();
				PaintBackgroundOnto(m_pClassCountLabels[i]);

				ppImages[i]->SetPos(10, g_iClassCountY + (g_iClassIconSize + 2) * i);
				ppImages[i]->SetSize(g_iClassIconSize, g_iClassIconSize);
				ppImages[i]->Paint();
			}
		}
	}
}

void CHudBG2::Paint()
{


	//BG2 - Tjoppen - Always paint damage label, so it becomes visible while using iron sights
	//fade out the last second
	float alphaTaken = (m_flTakenExpireTime - gpGlobals->curtime) * 255.0f;
	float alphaGiven = (m_flGivenExpireTime - gpGlobals->curtime) * 255.0f;

	if( alphaTaken < 0.0f )
		alphaTaken = 0.0f;
	else if( alphaTaken > 255.0f )
		alphaTaken = 255.0f;

	if( alphaGiven < 0.0f )
		alphaGiven = 0.0f;
	else if( alphaGiven > 255.0f )
		alphaGiven = 255.0f;

	if( alphaGiven > 0 )
	{
		m_pLabelDamageGiven->SizeToContents();

		//center and put one somewhat above crosshair
		m_pLabelDamageGiven->SetPos((ScreenWidth()-m_pLabelDamageGiven->GetWide())/2, (ScreenHeight()*5)/7 - m_pLabelDamageTaken->GetTall()); // verticle offset was formerly -20
		//m_pLabelDamageVerificator->SetFgColor( Color( 255, 255, 255, (int)alpha ) );
		m_pLabelDamageGiven->SetAlpha( (int)alphaGiven );

		m_pLabelDamageGiven->SetVisible( true );
	}
	else
		m_pLabelDamageGiven->SetVisible( false );

	if( alphaTaken > 0 )
	{
		m_pLabelDamageTaken->SizeToContents();

		//center and put one somewhat below crosshair
		m_pLabelDamageTaken->SetPos((ScreenWidth()-m_pLabelDamageTaken->GetWide())/2, (ScreenHeight()*5)/7);
		//m_pLabelDamageVerificator->SetFgColor( Color( 255, 255, 255, (int)alpha ) );
		m_pLabelDamageTaken->SetAlpha( (int)alphaTaken );

		m_pLabelDamageTaken->SetVisible( true );

	}
	else
		m_pLabelDamageTaken->SetVisible( false );

	PaintTopCenterHUD(/*pHL2Player*/);
	PaintDeathMessage();

	C_HL2MP_Player* pHL2Player = NULL;
	C_BaseCombatWeapon* wpn = NULL;
	if (!ShouldDrawPlayer(&pHL2Player, &wpn))
		return;

	PaintBottomLeftHUD(pHL2Player, wpn);
	PaintClassCounts();

	m_pLabelLMS->SetText( g_pVGuiLocalize->Find("#LMS") );
	m_pLabelLMS->SizeToContents();
	m_pLabelLMS->SetFgColor( COLOR_WHITE );

}

//==============================================
// CHudBG2's OnThink
// every think check if we're hidden, if so take away the text
//==============================================
void CHudBG2::OnThink()
{

}

void CHudBG2::Reset( void )
{
	//mapchange, clear indicators. and stuff.
	m_flGivenExpireTime = 0;
	m_flTakenExpireTime = 0;
	m_flLastSwing = 0.5f;
	m_flAFlashEnd = m_flBFlashEnd = 0;
	m_iLastSwingA = m_iLastSwingB = 0;
	m_flStartPulseTime = -FLT_MAX;
	m_flEndPulseTime = -FLT_MAX;
	m_bLocalPlayerAlive = false;
	m_flEndClassIconDrawTime = -FLT_MAX;
	NSoftInfo::Reset();
	HideShowAllPlayer( false );
	HideShowAllTeam(false);
	m_pLabelDeathMessage->SetVisible(false);
}

void CHudBG2::HideShowAllTeam( bool visible )
{
	m_pLabelAScore->SetVisible(visible && HL2MPRules()->UsingTickets());
	m_pLabelBScore->SetVisible(visible && HL2MPRules()->UsingTickets());
	m_pLabelBTickets->SetVisible(visible);
	m_pLabelATickets->SetVisible(visible);

	extern ConVar mp_rounds;
	m_pLabelCurrentRound->SetVisible(visible && mp_rounds.GetBool());
	m_pLabelRoundTime->SetVisible(visible);
	for (int i = 0; i < 6; i++) {
		m_pClassCountLabels[i]->SetVisible(visible);
	}
	
	m_pLabelLMS->SetVisible( visible && IsLMSstrict() && cl_draw_lms_indicator.GetBool() );
}

void CHudBG2::HideShowAllPlayer(bool visible) {
	m_pLabelAmmo->SetVisible(visible);
	m_pLabelHealth->SetVisible(visible);
	m_pLabelDamageGiven->SetVisible(m_flGivenExpireTime > gpGlobals->curtime);	//always show damage indicator (unless expired)
	m_pLabelDamageTaken->SetVisible(m_flTakenExpireTime > gpGlobals->curtime);	//always show damage indicator (unless expired)

	for (int i = 0; i < 6; i++) {
		m_pClassCountLabels[i]->SetVisible(visible);
	}
}

//BG2 - Tjoppen - cl_hitverif & cl_winmusic && capturesounds & voice comm sounds //HairyPotter
ConVar	cl_hitverif( "cl_hitverif", "1", FCVAR_ARCHIVE, "Display hit verification? 1 For all. 2 For Melee only. 3 For Firearms only." );
ConVar	cl_hitdamageline( "cl_hitdamageline", "0", FCVAR_ARCHIVE, "Display hit damage line? 1 For all. 2 For Melee only. 3 For Firearms only." ); // BG2 - VisualMelon - default to off
ConVar	cl_winmusic( "cl_winmusic", "1", FCVAR_ARCHIVE, "Play win music?" );
ConVar	cl_capturesounds( "cl_capturesounds", "1", FCVAR_ARCHIVE, "Play flag capture sounds?" );
ConVar cl_vcommsounds("cl_vcommsounds", "1", FCVAR_ARCHIVE, "Allow voice comm sounds?" );
//

ConVar cl_melee_timing_test("cl_melee_timing_test", "0", 0);
void CHudBG2::MsgFunc_HitVerif( bf_read &msg )
{
	int attacker, victim, hitgroup;
	int attackType;
	int damage;
	Color ColourWhite( 255, 255, 255, 255 ); // BG2 - These are now both WHITE, these colours are NOT uses
	//Color ColourGreen( 0, 150, 0, 255 ); // BG2 - VisualMelon - made darker (formerly 0, 200, 0, 255)
	//Color ColourYellow( 226, 226, 0, 255 ); // BG2 - VisualMelon - made yellow (formerly red 200, 0, 0, 255)

	attacker	= msg.ReadByte();
	victim		= msg.ReadByte();
	hitgroup	= msg.ReadByte();
	damage		= msg.ReadShort();
	if (cl_melee_timing_test.GetBool())
		Msg("Received hitverif, damage = %i, time %f\n", damage, gpGlobals->curtime);

	//attack type and hitgroup are packed into the same byte
	attackType = (hitgroup >> 4) & 0xF;
	hitgroup &= 0xF;

	//don't display messages for 0-damage
	if (damage <= 0)
		return;

	//reset health pulse
	if (C_HL2MP_Player::GetLocalPlayer()->entindex() == victim)
		m_flStartPulseTime = gpGlobals->curtime;


	C_HL2MP_Player *pAttacker = dynamic_cast<C_HL2MP_Player*>(UTIL_PlayerByIndex( attacker ));

	if( !pAttacker )
		return;

	C_BaseBG2Weapon *pWeapon = dynamic_cast<C_BaseBG2Weapon*>(pAttacker->GetActiveWeapon());

	if( C_BasePlayer::GetLocalPlayer()->entindex() == attacker )
	{
		if ((cl_hitverif.GetInt() == 1) || 
			((attackType == ATTACKTYPE_STAB || attackType == ATTACKTYPE_SLASH) && cl_hitverif.GetInt() == 3) ||
			((attackType == ATTACKTYPE_FIREARM && cl_hitverif.GetInt() == 2)))
		{
			hitVerificationLatency = cl_hitveriflatency.GetFloat();
			hitVerificationHairs = gpGlobals->curtime + hitVerificationLatency;
		}
	}

	if( !pWeapon || !C_BasePlayer::GetLocalPlayer() )
		return;

	//play melee hit sound if attacker's last attack was a melee attack
	//note: hit sound should always be played, regardless of what cl_hitdamageline is set to
	//in other words: don't mess up the order of these tests
	if( attackType == ATTACKTYPE_STAB || attackType == ATTACKTYPE_SLASH )
	{
		//we need play the hit sound like this since WeaponSound() only plays the local player's weapon's sound, not those of the other players
		const char *sound = pWeapon->GetShootSound( MELEE_HIT );

		if( sound && sound[0] )
		{
			CLocalPlayerFilter filter;
			C_BaseEntity::EmitSound( filter, SOUND_FROM_LOCAL_PLAYER, sound );
		}

		//cl_hitdamageline 3 -> don't display melee hit damage lines
		if( cl_hitdamageline.GetInt() == 3 )
			return;
	}

	//cl_hitdamageline 0 -> don't display any hit damage line, while 2 -> don't display shot hit damage lines
	if( cl_hitdamageline.GetInt() == 0 || (cl_hitdamageline.GetInt() == 2 && attackType == ATTACKTYPE_FIREARM) )
		return;

	//Msg( "MsgFunc_HitVerif: %i %i %i %f\n", attacker, victim, hitgroup, damage );

	//accumulate partial HitVerif messages within a small time window
	//this makes buckshot damage display correctly with and without simulated bullets
	std::string message;

	if( C_BasePlayer::GetLocalPlayer()->entindex() == victim )
	{
		//local player is victim ("You were hit...")
		message = m_IsVictimAccumulator.Accumulate( damage, attacker, hitgroup );
		m_pLabelDamageTaken->SetFgColor( ColourWhite ); // BG2 - VisualMelon - formerly ColourYellow

		m_pLabelDamageTaken->SetText( message.c_str() );
		m_flTakenExpireTime = gpGlobals->curtime + 5.0f;
	}
	else if( C_BasePlayer::GetLocalPlayer()->entindex() == attacker )
	{
		//"You hit..."
		message = m_IsAttackerAccumulator.Accumulate( damage, victim, hitgroup );
		m_pLabelDamageGiven->SetFgColor( ColourWhite ); // BG2 - VisualMelon - formerly ColourGreen

		m_pLabelDamageGiven->SetText( message.c_str() );
		m_flGivenExpireTime = gpGlobals->curtime + 5.0f;
	}	
}

void CHudBG2::MsgFunc_WinMusic( bf_read &msg )
{
	if( !cl_winmusic.GetBool() )
		return;

	int team = msg.ReadByte();

	CLocalPlayerFilter filter;


	switch( team ) //Switches are more efficient.
	{
		case TEAM_AMERICANS:
			C_BaseEntity::EmitSound( filter, SOUND_FROM_LOCAL_PLAYER, "Americans.win" );
			break;
		case TEAM_BRITISH:
			C_BaseEntity::EmitSound( filter, SOUND_FROM_LOCAL_PLAYER, "British.win" );
			break;
	}
}
//Make capture sounds client side. -HairyPotter
void CHudBG2::MsgFunc_CaptureSounds( bf_read &msg ) 
{
	if( !cl_capturesounds.GetBool() )
		return;

	Vector pos;
	char sound[255];

	msg.ReadBitVec3Coord( pos );
	msg.ReadString( sound, sizeof(sound) );
	PlayCaptureSound( pos, sound );
}
void CHudBG2::PlayCaptureSound( const Vector &origin, char sound[255] )
{
	CLocalPlayerFilter filter;

	//Hadto fix this because valve's code doesn't like direct wavs + scripts being played with the same code.
	EmitSound_t params;
	params.m_pSoundName = sound;
	params.m_flSoundTime = 0.0f;
	params.m_pOrigin = &origin;
	/*if ( params.m_flVolume == NULL )
	{
		Msg("The volume params aren't being set right for direct .wav sounds \n");
		params.m_flVolume = 0.8f;
	}*/
	if ( params.m_nChannel == NULL ) //Defualt to CHAN_AUTO for now.
	{
		//Msg("The channel params aren't being set right for direct .wav sounds \n");
		params.m_nChannel = CHAN_AUTO;
	}
	if ( params.m_SoundLevel == NULL )
	{
		//Msg("The soundlevel params aren't being set right for direct .wav sounds \n");
		params.m_SoundLevel = SNDLVL_60dB;
	}


	C_BaseEntity::EmitSound( filter, SOUND_FROM_WORLD, params );
	//
}
//
//Make voice comm sounds client side. -HairyPotter
void CHudBG2::MsgFunc_VCommSounds( bf_read &msg ) 
{
	if ( !cl_vcommsounds.GetBool() )
		return;

	int client	= msg.ReadByte();
	char snd[512];

	msg.ReadString( snd, sizeof(snd) );

	PlayVCommSound( snd, client );
}
void CHudBG2::PlayVCommSound( char snd[512], int playerindex )
{
	C_BasePlayer *pPlayer = UTIL_PlayerByIndex( playerindex );

	if ( pPlayer ) //Just make sure.
		pPlayer->EmitSound( snd );
}
//