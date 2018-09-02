/*
The Battle Grounds 3 - A Source modification
Copyright (C) 2017, The Battle Grounds 3 Team and Contributors

The Battle Grounds 3 free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

The Battle Grounds 3 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Contact information:
Chel "Awesome" Trunk		mail, in reverse: com . gmail @ latrunkster

You may also contact the (future) team via the Battle Grounds website and/or forum at:
www.bg2mod.com

Note that because of the sheer volume of files in the Source SDK this
notice cannot be put in all of them, but merely the ones that have any
changes from the original SDK.
In order to facilitate easy searching, all changes are and must be
commented on the following form:

//BG3 - <name of contributer>[ - <small description>]
*/

#include "cbase.h"
#include <cdll_client_int.h>
#include <globalvars_base.h>
#include <cdll_util.h>
#include <KeyValues.h>

#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/IPanel.h>
/*#include <vgui_controls/ImageList.h>
#include <vgui_controls/MenuItem.h>*/

#include <stdio.h> // _snprintf define

//#include "spectatorgui.h"
#include "c_team.h"
#include "vguicenterprint.h"

#include <game/client/iviewport.h>
#include "commandmenu.h"
#include "hltvcamera.h"

#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Menu.h>
#include "IGameUIFuncs.h" // for key bindings
#include <imapoverview.h>
#include <shareddefs.h>
#include <igameresources.h>
#include "engine/IEngineSound.h"

//#include "classmenu.h"
//#include "hl2mp_gamerules.h" // BG2 - VisualMelon - moved to classmenu.h so it can get the ammo counts

// Included for the port. -HairyPotter
#include "c_hl2mp_player.h"
//

#include "bg3_teammenu.h"
#include "bg3_classmenu.h"
#include "../shared/bg3/Math/bg3_rand.h"
#include "../shared/bg3/bg3_class_quota.h"
#include "bg3/vgui/bg3_fonts.h"


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"



#if 1
extern IGameUIFuncs *gameuifuncs; // for key binding details

using namespace vgui;
using namespace NMenuSounds;


#define CVAR_FLAGS	( FCVAR_ARCHIVE | FCVAR_USERINFO )

//--------------------------------------------------------------------------------------------
// Purpose: CTeamButton functions perform operations and overlays for the two big team buttons
//--------------------------------------------------------------------------------------------
void CTeamButton::OnMousePressed(vgui::MouseCode code) {
	UpdateGameRulesData();

	if (m_bEnabled) {
		PerformCommand();
		PlaySelectSound();
	}
	else {
		g_pTeamMenu->m_pTeamFullLabel->Activate(3.0f);
		//UpdateImage();
	}
}

void CTeamButton::OnCursorEntered(void) {
	m_bMouseOver = true;
	UpdateGameRulesData();
	//UpdateImage();
	PlayHoverSound();
}

void CTeamButton::OnCursorExited(void) {
	m_bMouseOver = false;
	//UpdateImage();
}

void CTeamButton::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);

	SetPaintBorderEnabled(false); //no borders
}

void CTeamButton::PerformCommand() {
	CTeamMenu* pParent = (CTeamMenu*)GetParent();

	if (pParent) {
		g_iCurrentTeammenuTeam = m_iTeam;
		pParent->SwitchToClassmenu();
	}
}

void CTeamButton::UpdateGameRulesData() {
	C_HL2MP_Player* pPlayer = C_HL2MP_Player::GetLocalHL2MPPlayer();
	if (pPlayer) {
		C_HL2MP_Player* pDummyBot;
		m_bEnabled = NClassQuota::PlayerMayJoinTeam(pPlayer, m_iTeam, &pDummyBot);
	}
}

//--------------------------------------------------------------------------------------------
// Purpose: CSpectateButton enters the player into spectate mode
//--------------------------------------------------------------------------------------------
void CSpectateButton::ApplySchemeSettings(IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);


	SetPaintBorderEnabled(false);
}

void CSpectateButton::OnMousePressed(MouseCode code) {
	CTeamMenu* pParent = (CTeamMenu*)GetParent();

	if (pParent) {
		engine->ServerCmd("spectate");
		pParent->ShowPanel(false);
		PlaySelectSound();
	}
}

void CSpectateButton::OnCursorEntered() {
	PlayHoverSound();
}

/*void CSpectateButton::OnCursorExited() {
}*/

//--------------------------------------------------------------------------------------------
// Purpose: CConscriptButton enters player into random team and opens class menu
//--------------------------------------------------------------------------------------------
void CConscriptButton::OnMousePressed(MouseCode code) {
	CTeamMenu::s_pAmericanButton->UpdateGameRulesData();
	CTeamMenu::s_pBritishButton->UpdateGameRulesData();

	CTeamButton* pButton;

	if (!CTeamMenu::s_pAmericanButton->IsEnabled())
		pButton = CTeamMenu::s_pBritishButton;
	else if (!CTeamMenu::s_pBritishButton->IsEnabled())
		pButton = CTeamMenu::s_pAmericanButton;
	else
		pButton = RndBool() ? CTeamMenu::s_pAmericanButton : CTeamMenu::s_pBritishButton;

	pButton->PerformCommand();
	PlaySelectSound();
}

void CConscriptButton::ApplySchemeSettings(IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);

	SetPaintBorderEnabled(false); //no border
}

void CConscriptButton::OnCursorEntered(void) {
	PlayHoverSound();
}

//--------------------------------------------------------------------------------------------
// Purpose: CTeamMenu wraps buttons, background together and publicizes interface
//--------------------------------------------------------------------------------------------
CTeamMenu::CTeamMenu(VPANEL pParent) : Frame(NULL, PANEL_TEAMS) {
	g_pTeamMenu = this;
	SetParent(pParent);
	SetScheme("ClientScheme");
	m_pChooseTeamLabel = new Label(this, "ChooseTeamLabel", "");
	m_pChooseTeamLabel->SetContentAlignment(Label::a_north);

	m_pAmericanLabel = new Label(this, "AmericanLabel", "");
	m_pAmericanLabel->SetContentAlignment(Label::a_south);
	m_pBritishLabel = new Label(this, "BritishLabel", "");
	m_pBritishLabel->SetContentAlignment(Label::a_south);

	m_pTeamFullLabel = new TimedLabel(this, "TeamFullLabel", "");
	m_pTeamFullLabel->SetContentAlignment(Label::Alignment::a_center);

	s_pAmericanButton = new CTeamButton(this, "AmericanButton", "", TEAM_AMERICANS);
	s_pBritishButton = new CTeamButton(this, "BritishButton", "", TEAM_BRITISH);

	m_pSpectateButton = new CSpectateButton(this, "SpectateButton", "");
	m_pConscriptButton = new CConscriptButton(this, "ConscriptButton", "");


	m_pBackground		= scheme()->GetImage("teammenu/background", false);
	m_pLeftHighlight	= scheme()->GetImage("teammenu/british_select", false);
	m_pRightHighlight	= scheme()->GetImage("teammenu/american_select", false);
	m_pLeftDarken		= scheme()->GetImage("teammenu/left_darken", false);
	m_pRightDarken		= scheme()->GetImage("teammenu/right_darken", false);

	m_pMOTD = new CMOTDHTML(this, "");
	m_pMOTD->SetBgColor(Color(0, 0, 0, 0));
}

CTeamMenu::~CTeamMenu() {

}

void CTeamMenu::SetVisible(bool bVisible) {
	BaseClass::SetVisible(bVisible);
}

void CTeamMenu::Update(void) {
	m_flNextGameRulesUpdate = gpGlobals->curtime + 2.0f;
	//Msg("Updating teammenu!\n");

	s_pAmericanButton->UpdateGameRulesData();
	s_pBritishButton->UpdateGameRulesData();

	if (g_Teams.Count()) {
		wchar buffer[32];
		V_snwprintf(buffer, ARRAYSIZE(buffer), L"(%i) : %s", g_Teams[TEAM_BRITISH]->GetNumPlayers(), g_pVGuiLocalize->Find("#BG3_Team_British"));
		m_pBritishLabel->SetText(buffer);

		V_snwprintf(buffer, ARRAYSIZE(buffer), L"%s : (%i)", g_pVGuiLocalize->Find("#BG3_Team_American"), g_Teams[TEAM_AMERICANS]->GetNumPlayers());
		m_pAmericanLabel->SetText(buffer);
	}

	m_pMOTD->ManualUpdate();
}

void CTeamMenu::ShowPanel(bool bShow) {
	if (engine->IsPlayingDemo()) //Don't show up in demos -HairyPotter
		return;

	if (BaseClass::IsVisible() == bShow)
		return;

	if (bShow) {
		g_pClassMenu->ShowPanel(false);
		Update();
		m_flNextGameRulesUpdate = gpGlobals->curtime + 0.1f;
		PerformLayout();
		Activate();
		HideCommMenu();
		m_pTeamFullLabel->SetVisible(false);
	}
	else
		SetVisible(false);

	SetMouseInputEnabled(bShow);
	SetKeyBoardInputEnabled(bShow);

	//g_pMOTD->ShowPanel(bShow);
}

void CTeamMenu::Paint(void) {
	m_pBackground->Paint();
	
	if (!s_pBritishButton->m_bEnabled)
		m_pLeftDarken->Paint();
	else if (s_pBritishButton->m_bMouseOver)
		m_pLeftHighlight->Paint();
	if (!s_pAmericanButton->m_bEnabled)
		m_pRightDarken->Paint();
	else if (s_pAmericanButton->m_bMouseOver)
		m_pRightHighlight->Paint();
}

void CTeamMenu::PerformLayout() {
	s_pAmericanButton->MakeReadyForUse();
	s_pBritishButton->MakeReadyForUse();
	m_pSpectateButton->MakeReadyForUse();
	m_pConscriptButton->MakeReadyForUse();

	int width, height;
	width = ScreenWidth();
	height = ScreenHeight();
	SetPos(0, 0);
	SetSize(width, height);

#define set(a) a->SetPos(0,0); a->SetSize(width, height);
	set(m_pBackground);
	set(m_pRightHighlight);
	set(m_pLeftHighlight);
	set(m_pLeftDarken);
	set(m_pRightDarken);
#undef set

	
	float horizontalScale = 1.0f * width / 640;
	float verticalScale = 1.0f * height / 360;

	//choose team label
	int chooseTeamX = 214 * horizontalScale;
	int chooseTeamh = 200;
	m_pChooseTeamLabel->SetPos(chooseTeamX, 0);
	m_pChooseTeamLabel->SetSize(chooseTeamX, chooseTeamh);

	//team buttons
	int teamButtonW = 215 * horizontalScale;
	s_pBritishButton->SetSize(teamButtonW, height);
	s_pBritishButton->SetPos(0, 0);
	s_pAmericanButton->SetSize(teamButtonW, height);
	s_pAmericanButton->SetPos(teamButtonW + chooseTeamX, 0);

	int spectatex = 229 * horizontalScale;
	int spectatey = 58 * verticalScale;
	int spectatew = 93 * horizontalScale;
	int spectateh = 32 * verticalScale;

	int conscriptx = 320 * horizontalScale;
	int conscripty = spectatey;
	int conscriptw = spectatew;
	int conscripth = spectateh;

	m_pSpectateButton->SetPos(spectatex, spectatey);
	m_pSpectateButton->SetSize(spectatew, spectateh);
	m_pConscriptButton->SetPos(conscriptx, conscripty);
	m_pConscriptButton->SetSize(conscriptw, conscripth);
	m_pConscriptButton->SetContentAlignment(Label::Alignment::a_east);

	//Set texts
	m_pChooseTeamLabel->SetText("#BG3_Teammenu_Choose_Team");
	m_pSpectateButton->SetText("#BG3_Teammenu_Spectate");
	m_pConscriptButton->SetText("#BG3_Teammenu_Conscript");
	m_pTeamFullLabel->SetText("#BG3_Teammenu_Full");

	int labelHeight = 20 * verticalScale;
	m_pAmericanLabel->SetSize(120 * horizontalScale, labelHeight);
	m_pBritishLabel->SetSize(120 * horizontalScale, labelHeight);
	m_pBritishLabel->SetPos(teamButtonW - 120 * horizontalScale, height / 1.45f - labelHeight);
	m_pAmericanLabel->SetPos(2 * teamButtonW, height / 1.45f - labelHeight);

	m_pTeamFullLabel->SetPos(spectatex, spectatey / 2);
	m_pTeamFullLabel->SetSize(teamButtonW, spectateh);

	IScheme* pScheme = g_pVGuiSchemeManager->GetIScheme(GetScheme());
	HFont font = GetDefaultBG3FontScaledVertical(pScheme, m_pAmericanLabel);
	m_pAmericanLabel->SetFont(font);
	m_pBritishLabel->SetFont(font);

	
	font = GetDefaultBG3FontScaledHorizontal(pScheme, m_pChooseTeamLabel);
	m_pChooseTeamLabel->SetFont(font);

	font = GetDefaultBG3FontScaledHorizontal(pScheme, m_pTeamFullLabel);
	m_pTeamFullLabel->SetFont(font);

	font = pScheme->GetFont("ClassMenuNames");
	m_pSpectateButton->SetFont(font);
	m_pConscriptButton->SetFont(font);

	int motdw = teamButtonW / 1.2f;
	int inset = (teamButtonW - motdw) / 2;
	m_pMOTD->SetPos(teamButtonW + inset, height / 2.3f);
	m_pMOTD->SetSize(motdw, height / 1.95f);
}

void CTeamMenu::SetData(KeyValues* kv) {
	//if (!Q_strcmp(kv->GetName(), "motddata")) {
		m_pMOTD->SetData(kv);
	//}
}

void CTeamMenu::ApplySchemeSettings(IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);

	SetPaintBorderEnabled(false);

#define set(a) a->SetFgColor(g_cBG3TextColor)
	set(m_pChooseTeamLabel);
	set(m_pSpectateButton);
	set(m_pConscriptButton);
#undef set
}

CTeamButton* CTeamMenu::s_pBritishButton = nullptr;
CTeamButton* CTeamMenu::s_pAmericanButton = nullptr;

extern int			g_iCurrentTeammenuTeam;
CTeamMenu*			g_pTeamMenu;
//Color				g_cBG3TextShadowColor(46, 46, 24, 200);
#endif

ConVar cl_classmenu_sounds("cl_classmenu_sounds", "1", FCVAR_ARCHIVE, "Whether or not to play sounds in the class/team menu");

//--------------------------------------------------------------------------------------------
// Purpose: A cute little namespace for playing menu sounds
//--------------------------------------------------------------------------------------------
namespace NMenuSounds {
	static int g_iLastPlayedSound = 0;
	static float g_flLastPlayedSoundTime = -FLT_MAX;
	void PlayMenuSound(const char* pszSound) {
		/*C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
		if (!pPlayer)
			return;*/

		if (!cl_classmenu_sounds.GetBool() && (g_pClassMenu->IsVisible() || g_pTeamMenu->IsVisible()))
			return;

		if (g_flLastPlayedSoundTime + 0.3f > gpGlobals->curtime)
			return;

		if (g_iLastPlayedSound) {
			enginesound->StopSoundByGuid(g_iLastPlayedSound);
			g_iLastPlayedSound = 0;
		}
			

		enginesound->EmitAmbientSound(pszSound, 1);
		g_iLastPlayedSound = enginesound->GetGuidForLastSoundEmitted();

		//This wonderful hack prevents the sounds from overlapping if you move to a new button before the old sound ends
		/*pPlayer->StopSound(pszSound);

		CLocalPlayerFilter filter;
		C_BaseEntity::EmitSound(filter, SOUND_FROM_LOCAL_PLAYER, pszSound);*/
	}

	void PlayHoverSound() {
		PlayMenuSound("ui\\main_menu_button.mp3");
	}
	void PlaySelectSound() {
		PlayMenuSound("ui\\menu\\click.wav");
	}
}
