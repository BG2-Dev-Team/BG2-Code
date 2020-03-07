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
battlegrounds3.com

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
#include "baseviewport.h"
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Menu.h>
#include <vgui/IPanel.h>
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

#include "../shared/bg2/weapon_bg2base.h"

#include "../bg2/vgui_Panel_MainMenu.h"
#include "bg3_teammenu.h"
#include "bg3_classmenu.h"
#include "bg3/vgui/bg3_fonts.h"
#include "bg3/vgui/fancy_button.h"
#include "../shared/bg3/Math/bg3_rand.h"
#include "../shared/bg3/bg3_math_shared.h"
#include "../shared/bg3/bg3_class_quota.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#if 1
using namespace vgui;
using namespace NMenuSounds;

CClassButton*	g_pDisplayedClassButton; //which class is currently displayed?
CClassButton*	g_pSelectedClassButton;	//what was the last class clicked on?
CWeaponButton*	g_pWeaponButton;		//we only have one weapon button, which cycles through the possible weapons
CAmmoButton*	g_pCurrentAmmoButton;
CAmmoButton*	g_pDisplayedAmmoButton;
CUniformButton*	g_pCurrentUniformButton;

CClassButton**	g_ppClassButtons		= nullptr;
CAmmoButton**	g_ppAmmoButtons			= nullptr;
CUniformButton**	g_ppUniformButtons	= nullptr;

FancyButton*	g_pGoButton;
GlowyArrowForPlayersWhoDontRealizeYouCanSwitchWeapons* g_pGlowyWeaponArrow;

vgui::IImage* g_pSelectionIcon;
vgui::IImage* g_pHoverIcon;
vgui::IImage* g_pDarkIcon;
vgui::IImage* g_pNoneIcon;

IImage*			g_pClassArt = nullptr;

Label*			g_pClassNameLabel;
Label*			g_pWeaponNameLabel;
Label*			g_pClassDescLabel;
Label*			g_pWeaponDescLabel;
Label*			g_pTeamwidePickingLabel;
Label*			g_pClassBuffLabel;
Label*			g_pOfficerBuffsLabel;

#define NUM_CLASS_BUTTONS 6
#define NUM_AMMO_BUTTONS 2
#define NUM_UNIFORM_BUTTONS 4

int g_iCurrentTeammenuTeam = TEAM_INVALID;




//-----------------------------------------------------------------------------
// Purpose: Begin class button functions
//-----------------------------------------------------------------------------
CClassButton::CClassButton(Panel *parent, const char *panelName, const char *text, int index) : Button(parent, panelName, text), m_iIndex(index) {

	m_pCountLabel = new Label(this, "ClassCountLabel", "--");
}

void CClassButton::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetPaintBorderEnabled(false);
	m_pCountLabel->SetFgColor(g_cBG3TextColor);
	m_pCountLabel->SetPos(0, 0);
	m_pCountLabel->SetSize(30, 20);
	SetDefaultBG3FontScaled(pScheme, m_pCountLabel);
}

void CClassButton::OnMousePressed(vgui::MouseCode code) {
	UpdateGamerulesData();
	if (IsAvailable()) {
		if (IsSelected() && m_flLastClickTime + 0.4f > gpGlobals->curtime) {
			//we've click on what we have selected, so let's try to join the team
			int gun = g_pWeaponButton->GetCurrentGunKit();
			int ammo = g_pCurrentAmmoButton->GetAmmoIndex();
			int skin = g_pCurrentUniformButton->GetUniformIndex();
			int team = g_iCurrentTeammenuTeam;
			int iClass = m_pPlayerClass->m_iClassNumber;
			JoinRequest(gun, ammo, skin, team, iClass);
			g_pClassMenu->ShowPanel(false);
		}
		else if (!IsSelected()) {
			Select();

			//play a sound
			PlaySelectSound();
		}
	}
	else {
		//TODO print "not available!" message
	}
	m_flLastClickTime = gpGlobals->curtime;
}

void CClassButton::OnCursorEntered() {
	UpdateGamerulesData();
	m_bMouseOver = true;
	if (m_eAvailable != CLASS_UNAVAILABLE) {
		UpdateDisplayedButton(this);
		PlayHoverSound();
	}
}

void CClassButton::OnCursorExited() {
	m_bMouseOver = false;
	UpdateDisplayedButton(g_pSelectedClassButton);
}

void CClassButton::ManualPaint() {
	int x, y;
	GetPos(x, y);

	//paint current image
	int w, h;
	GetSize(w, h);
	m_pCurrentImage->SetPos(x, y);
	m_pCurrentImage->SetSize(w, h);
	m_pCurrentImage->Paint();

	//paint any overlays
	if (m_eAvailable == CLASS_FULL || m_eAvailable == CLASS_UNAVAILABLE) {
		g_pDarkIcon->SetPos(x, y);
		g_pDarkIcon->SetSize(w, h);
		g_pDarkIcon->Paint();
	}
	else if (m_bMouseOver) {
		g_pHoverIcon->SetPos(x, y);
		g_pHoverIcon->SetSize(w, h);
		g_pHoverIcon->Paint();
	}

}

CClassButton* CClassButton::FindAvailableButton() {
	//just loop until we find one and return it
	for (int i = 0; i < NUM_CLASS_BUTTONS; i++) {
		CClassButton* pButton = g_ppClassButtons[i];
		if (pButton->IsAvailable())
			return pButton;
	}

	//default return value
	return g_ppClassButtons[0];
}

void CClassButton::Select() {
	//make sure the buttons switch images properly
	CClassButton* prevButton = g_pSelectedClassButton;
	g_pSelectedClassButton = this;
	UpdateDisplayedButton(this);
	if (prevButton)
		prevButton->UpdateImage(); //this will update image for previous button
}

void CClassButton::UpdateGamerulesData() {
	C_HL2MP_Player* pPlayer = C_HL2MP_Player::GetLocalHL2MPPlayer();

	if (!pPlayer)
		return;

	m_eAvailable = NClassQuota::PlayerMayJoinClass(pPlayer, m_pPlayerClass); //m_pPlayerClass->availabilityForPlayer(pPlayer);

	//set label text
	char buffer[8];
	int limit = NClassQuota::GetLimitForClass(m_pPlayerClass);
	int count = count = g_Teams[m_pPlayerClass->m_iDefaultTeam]->GetNumOfNextClassRealPlayers(m_pPlayerClass->m_iClassNumber);
	if (limit < 0) {
		Q_snprintf(buffer, sizeof(buffer), "%i", count);
		m_pCountLabel->SetText(buffer);
	}
	else if (limit == 0 && count == 0) {
		m_pCountLabel->SetText("-/-");
	}
	else {
		Q_snprintf(buffer, sizeof(buffer), "%i/%i", count, limit);
		m_pCountLabel->SetText(buffer);
	}
	m_pCountLabel->SizeToContents();
	m_pCountLabel->SetWide(m_pCountLabel->GetWide() * 1.2f); //italic font extends slightly beyond the edge

	//officer mode overrides everything
	extern bool g_bClassMenuOfficerMode;
	if (g_bClassMenuOfficerMode) m_eAvailable = CLASS_FREE;

	UpdateImage();
}

void CClassButton::UpdateImage() {
	//if (IsAvailable())
		m_pCurrentImage = m_pAvailableImage;
	//else
		//m_pCurrentImage = g_pNoneIcon;
}

void CClassButton::UpdateDisplayedButton(CClassButton* pNext) {

	//Don't bother if we're already displaying this class button on the current team
	if (pNext == g_pDisplayedClassButton && g_pClassMenu->m_iPreviouslyShownTeam == g_iCurrentTeammenuTeam)
		return;

	CClassButton* pOldButton = g_pDisplayedClassButton;
	g_pDisplayedClassButton = pNext;

	if (pOldButton)
		pOldButton->UpdateImage();
	g_pDisplayedClassButton->UpdateImage();

	//update labels
	g_pClassNameLabel->SetText(pNext->m_pPlayerClass->m_pLocalizedName);
	g_pClassDescLabel->SetText(pNext->m_pPlayerClass->m_pLocalizedDesc);

	//update displayed class stats
	NClassWeaponStats::UpdateToMatchClass(pNext->m_pPlayerClass);

	//update weapon button display
	g_pWeaponButton->UpdateToMatchPlayerClass(pNext->m_pPlayerClass);

	//update skin buttons
	CUniformButton::UpdateToMatchClass(pNext->m_pPlayerClass);

	//update class art
	g_pClassArt = pNext->m_pClassArt;
}

void CClassButton::UpdateToMatchClass(const CPlayerClass* pClass) {
	//update internal reference
	m_pPlayerClass = pClass;

	char team = charForTeam(pClass->m_iDefaultTeam);

	//get images for VGUI
	char buffer[40];
	Q_snprintf(buffer, ARRAYSIZE(buffer), "classmenu/classes/%c/%s", team, pClass->m_pszAbbreviation);
	m_pAvailableImage = scheme()->GetImage(buffer, false);

	Q_snprintf(buffer, ARRAYSIZE(buffer), "classmenu/class_art/%c/%s", team, pClass->m_pszAbbreviation);
	m_pClassArt = scheme()->GetImage(buffer, false);
	//go ahead and set sizes too
	int arth = 7 * ScreenHeight() / 10;
	m_pClassArt->SetPos(0, ScreenHeight() - arth);
	m_pClassArt->SetSize(arth * 2, arth);

	//update gamerules data - this will also set our current image
	UpdateGamerulesData();
}

#define DEFAULT_ICON_SIZE 50
int CClassButton::s_iSize = DEFAULT_ICON_SIZE;


bool g_bClassMenuOfficerMode = false;
CON_COMMAND(teamkitmenu, "(Linebattle Mode Only) Sets the class, weapon, uniform, and ammo for the entire team.") {
	g_bClassMenuOfficerMode = true;
	engine->ClientCmd("classmenu");
}


//-----------------------------------------------------------------------------
// Purpose: Looks at the currently selected stuff and sends message to server
//-----------------------------------------------------------------------------
void JoinRequest(int iGun, int iAmmo, int iSkin, int iTeam, int iClass) {
	//BG3 - we've merged the class and kit commands into a single command, classkit, to simplify
	//ordering dependencies on server side

	if (g_bClassMenuOfficerMode) {
		g_bClassMenuOfficerMode = false;

		C_BasePlayer* pLocalPlayer = C_BasePlayer::GetLocalPlayer();
		int team = pLocalPlayer->GetTeamNumber();
		if (!pLocalPlayer || team != iTeam)
			return;

		const char* pszCommandString = team == TEAM_AMERICANS ? "aclasskit" : "bclasskit";

		const char* pszClassAbbreviation = CPlayerClass::fromNums(iTeam, iClass)->m_pszAbbreviation;

		char cmd[32];
		Q_snprintf(cmd, sizeof(cmd), "%s %s %i %i %i", pszCommandString, pszClassAbbreviation, iGun, iSkin, iAmmo);
		engine->ServerCmd(cmd);
	}
	else {
		char cmd[32];
		Q_snprintf(cmd, sizeof(cmd), "classkit %i %i %i %i %i\n", iTeam, iClass, iGun, iAmmo, iSkin);
		engine->ServerCmd(cmd);
	}

	/*
	Q_snprintf(cmd, sizeof(cmd), "kit %i %i %i \n", iGun, iAmmo, iSkin);
	engine->ServerCmd(cmd);*/

	// deprecated
	//Q_snprintf( cmd, sizeof( cmd ), "classskin %i \n", skinId );
	//engine->ServerCmd( cmd );

	/*Q_snprintf(cmd, sizeof(cmd), "class %i %i \n", iTeam, iClass);
	engine->ServerCmd(cmd);*/ //Send the class AFTER we've selected a gun, otherwise you won't spawn with the right kit.
}







//-----------------------------------------------------------------------------
// Purpose: Begin weapon button functions
//-----------------------------------------------------------------------------
void CWeaponButton::ApplySchemeSettings(IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetPaintBorderEnabled(false);

	int i;
	//make class buttons ready
	for (i = 0; i < NUM_CLASS_BUTTONS; i++) {
		g_ppClassButtons[i]->MakeReadyForUse();
	}

	//make weapon button ready
	g_pWeaponButton->MakeReadyForUse();

	//make ammo buttons ready
	for (i = 0; i < NUM_AMMO_BUTTONS; i++)
		g_ppAmmoButtons[i]->MakeReadyForUse();

	//make uniform buttons ready
	for (i = 0; i < NUM_UNIFORM_BUTTONS; i++) {
		g_ppUniformButtons[i]->MakeReadyForUse();
	}
}

void CWeaponButton::OnMousePressed(MouseCode code) {
	//cycle weapon count
	SetCurrentGunKit(m_iCurrentWeapon + 1);

	PlaySelectSound();
}

void CWeaponButton::OnCursorEntered() {
	m_bMouseOver = true;
	PlayHoverSound();
	if (m_iWeaponCount > 1) {
		g_pGlowyWeaponArrow->SetImage(MENU_ARROW_HOVER);
	}
}

void CWeaponButton::OnCursorExited() { 
	m_bMouseOver = false; 
	if (m_iWeaponCount > 1) {
		g_pGlowyWeaponArrow->SetImage(MENU_ARROW_HOVER);
	}
}

void CWeaponButton::ManualPaint() {
	int x, y;
	GetPos(x, y);

	int w, h;
	GetSize(w, h);
	m_pCurrentImage->SetPos(x, y);
	m_pCurrentImage->SetSize(w, h);
	m_pCurrentImage->Paint();

	if (m_bMouseOver) {
		g_pHoverIcon->SetPos(x, y);
		g_pHoverIcon->SetSize(w, h);
		g_pHoverIcon->Paint();
	}
}

void CWeaponButton::UpdateToMatchPlayerClass(const CPlayerClass* pClass) {
	m_iWeaponCount = pClass->numChooseableWeapons();
	g_pGlowyWeaponArrow->SetVisible(m_iWeaponCount > 1);

	SetCurrentGunKit(pClass->GetDefaultWeapon());
}

void CWeaponButton::SetCurrentGunKit(int iKit) {

	//clamp value, cycling
	if (iKit >= m_iWeaponCount)
		iKit = 0;
	if (iKit < 0) iKit = 0;
	m_iCurrentWeapon = iKit;

	//update default weapon
	GetDisplayedClass()->SetDefaultWeapon(m_iCurrentWeapon);

	//update image
	const CGunKit* pKit = GetDisplayedClass()->getWeaponKitChooseable(m_iCurrentWeapon);
	//Msg("Updating weapon to %s\n", pKit->m_pszWeaponName);
	UpdateImage(pKit);
	
	//update weapon name label
	g_pWeaponNameLabel->SetText(pKit->GetLocalizedName());
	g_pWeaponDescLabel->SetText(pKit->GetLocalizedDesc());

	//update the current ammo button
	CAmmoButton::UpdateToMatchClassAndWeapon(GetDisplayedClass(), pKit);

	//update weapon stats
	NClassWeaponStats::UpdateWeaponStatsAllCurrent();
}

void CWeaponButton::UpdateImage(const CGunKit* pKit) {
	char buffer[128];
	const char* imageName = pKit->m_pszLocalizedNameOverride ? pKit->m_pszLocalizedNameOverride : pKit->m_pszWeaponPrimaryName;
	Q_snprintf(buffer, ARRAYSIZE(buffer), "classmenu/weapons/%s", imageName);
	m_pCurrentImage = scheme()->GetImage(buffer, false);
}


//-----------------------------------------------------------------------------
// Purpose: Glowing/pulsing arrow indicates that weapons can be swapped
//-----------------------------------------------------------------------------
GlowyArrowForPlayersWhoDontRealizeYouCanSwitchWeapons::GlowyArrowForPlayersWhoDontRealizeYouCanSwitchWeapons() {
	m_pImage = scheme()->GetImage(MENU_ARROW_HOVER, false);
}


void GlowyArrowForPlayersWhoDontRealizeYouCanSwitchWeapons::PaintToScreen() {
	uint8 pulse = m_iPulseSize * DistanceFromEven(gpGlobals->curtime * 2);
	uint8 halfpulse = pulse / 2;
	m_pImage->SetPos(m_iX - halfpulse, m_iY - halfpulse);
	m_pImage->SetSize(m_iWH + pulse, m_iWH + pulse);
	m_pImage->Paint();
}

void GlowyArrowForPlayersWhoDontRealizeYouCanSwitchWeapons::SetImage(const char* pszImgName) 	{ 
	m_pImage = scheme()->GetImage(pszImgName, false); 
}


//-----------------------------------------------------------------------------
// Purpose: Begin class/weapon stats display
//-----------------------------------------------------------------------------
namespace NClassWeaponStats {
	float xBase = 0;
	float yBase = 0;

	IImage*		g_imgStatBar;
	IImage*		g_imgStatBarOverlay;

	const int	g_iDefaultBarWidth = 66;
	const int	g_iDefaultBarHeight = 11;
	const int	g_iDefaultRowSpacing = 35;
	int			g_iBarWidth = g_iDefaultBarWidth;
	int			g_iBarHeight = g_iDefaultBarHeight;
	int			g_iRowSpacing = g_iDefaultRowSpacing;

	//widths of the stat bars
	int g_iClassSpeedWidth;
	//int g_iClassNotesWidth;
	int g_iAccuracyWidth;
	int g_iBulletDamageWidth;
	int g_iReloadSpeedWidth;
	int g_iLockTimeWidth;
	int g_iMeleeRangeWidth;
	int g_iMeleeDamageWidth;
	int g_iMeleeToleranceWidth;
	int g_iMeleeSpeedWidth;
	

	void UpdateToMatchClass(const CPlayerClass* pClass) {
		//update the buff label
		if (pClass->m_bHasImplicitDamageResistance) {
			g_pClassBuffLabel->SetText("#BG3_Classmenu_Buff_DmgRst");
			g_pClassBuffLabel->SetVisible(true);
			g_pClassBuffLabel->SizeToContents();
		}
		else if (pClass->m_bHasImplicitDamageWeakness) {
			g_pClassBuffLabel->SetText("#BG3_Classmenu_Buff_DmgWk");
			g_pClassBuffLabel->SetVisible(true);
			g_pClassBuffLabel->SizeToContents();
		}
		else {
			g_pClassBuffLabel->SetVisible(false);
		}
	}

	void UpdateWeaponStatsAllCurrent() {

		//Update player movement speed also, because that can be weapon-dependent!
		float speed = GetDisplayedClass()->m_flBaseSpeed;
		speed += GetDisplayedClass()->m_aWeapons[g_pWeaponButton->GetCurrentGunKit()].m_iMovementSpeedModifier;
		float relativeSpeed = FLerp(0.4f, 1.0f, SPEED_GRENADIER, SPEED_SKIRMISHER + 5, speed);
		g_iClassSpeedWidth = g_iBarWidth * relativeSpeed;
		char buffer[4];
		Q_snprintf(buffer, sizeof(buffer), "%i", (int)(speed + 0.1f));
		g_pnClassSpeed->SetText(buffer);

		//update displayed weapon stats!
		//we may have multiple weapons, make sure we get all of them!
		const CWeaponDef* pPrimary;
		const CWeaponDef* pSecondary;
		const CWeaponDef* pTertiary; //this is just here for forward-compatibility

		GetDisplayedClass()->getWeaponDef(g_pWeaponButton->GetCurrentGunKit(), &pPrimary, &pSecondary, &pTertiary);

		if (pPrimary)
			NClassWeaponStats::UpdateToMatchWeapon(pPrimary);
		if (pSecondary)
			NClassWeaponStats::UpdateToMatchWeapon(pSecondary);
		if (pTertiary)
			NClassWeaponStats::UpdateToMatchWeapon(pTertiary);

		//if our primary weapon isn't a firearm, hide the firearm stats
		//BG3 - contrib from darthmotta
		if (pPrimary->m_Attackinfos[0].m_iAttacktype != ATTACKTYPE_FIREARM){
			g_iAccuracyWidth = 1;
			g_iBulletDamageWidth = 1;
			g_iReloadSpeedWidth = 1;
			g_iLockTimeWidth = 1;
			g_pnLockTime->SetText("");
			g_pnBulletDamage->SetText("");
			g_pnReloadSpeed->SetText("");
		}
	}

	void UpdateToMatchWeapon(const CWeaponDef* pWeapon) {
#define set(a,b) g_i##a##Width = g_iBarWidth * b

		const attackinfo* bullet = nullptr;
		const attackinfo* melee = nullptr;

		char buffer[12];

		//Display bullet information if we have it
		if (pWeapon->m_Attackinfos[0].m_iAttacktype == ATTACKTYPE_FIREARM) {
			bullet = &pWeapon->m_Attackinfos[0];
			if (pWeapon->m_Attackinfos[1].m_iAttacktype != ATTACKTYPE_NONE) {
				melee = &pWeapon->m_Attackinfos[1];
			}
		}
		//if we only have a melee weapon
		else if (pWeapon->m_Attackinfos[0].m_iAttacktype != ATTACKTYPE_NONE) {
			melee = &pWeapon->m_Attackinfos[0];
		}
		
		//calculate displayed stats from the attackinfos
		if (bullet) {
			bool buckshot = (g_pDisplayedAmmoButton->GetAmmoIndex() == AMMO_KIT_BUCKSHOT || pWeapon->m_bShotOnly);

			//calc accuracy
			float accuracy;
			if (!buckshot) {
				
				//this weird function was designed to emphasize differences around 2.3, where most muskets live, 
				//and around 0.8 where rifles live, but otherwise still
				//leave the function strictly increasing
				//graph it if you'd like
				//(6 * arctan(8r - 18.4))/max(9, abs(8r - 18.4))     + 0.8 * arctan(20r - 16))/max(8, abs(20r - 16)) + r + 0.2
				//hint: 18.4 = 2.3 * 8, 16 = 0.8 * 20
				auto effectiveAccuracy = [](float r) {
					return 
						(6 * atan(8 * r - 18.4)) / max(9, abs(8 * r - 18.4)) + //focuses around 2.3
						(1.6 * atan(20*r - 16)) / max(8, abs(20*r - 16)) + //focuses around 0.8
						r + 0.2;
				};

				/*if (pWeapon->m_bWeaponHasSights)
					accuracy = 1 / (bullet->m_flCrouchAimStill);
				else
					accuracy = 1 / (bullet->m_flStandStill);
				float maxAccuracy = 1 / (CWeaponDef::GetDefForWeapon("weapon_pennsylvania")->m_Attackinfos[0].m_flCrouchAimStill);*/

				if (pWeapon->m_bWeaponHasSights)
					accuracy = effectiveAccuracy(bullet->m_flCrouchAimStill);
				else
					accuracy = (bullet->m_flStandStill);
				float maxAccuracy = effectiveAccuracy(CWeaponDef::GetDefForWeapon("weapon_pennsylvania")->m_Attackinfos[0].m_flCrouchAimStill);

				float diff = accuracy - maxAccuracy; //positive number, because maxAccuracy < accuracy
				float lerp = 1 - (diff / 6.5f);
				accuracy = lerp;

				//the negative value pulls lower accuracies farther to the left, enhancing differentiation
				//accuracy = FLerp(-0.2f, 1.0f, accuracy);
			}
			else {
				accuracy = 1 / pWeapon->m_flShotSpread; //really inaccurate
			}
			set(Accuracy, accuracy);
			
			//calc bullet damage
			int iDamage = bullet->m_iDamage;
			if (buckshot)
				iDamage = 120.f; //TODO find a better rep for shot damage
			float damage = iDamage / 120.f;
			set(BulletDamage, damage);

			//calc reload speed
			float reload = 12.f - pWeapon->m_flApproximateReloadTime;
			reload /= 12.0f;
			set(ReloadSpeed, reload);

			//calc lock time
			float locktime = pWeapon->m_flLockTime + pWeapon->m_flRandomAdditionalLockTimeMax;
			locktime = (0.55 - locktime) / 0.55;
			set(LockTime, locktime);

			//update texts
			if (buckshot) {
				Q_snprintf(buffer, sizeof(buffer), "%i * %i", pWeapon->m_iNumShot, (int)(pWeapon->m_iDamagePerShot + 0.5f));
				g_pnBulletDamage->SetText(buffer);
			}
			else {
				Q_snprintf(buffer, sizeof(buffer), "%i", bullet->m_iDamage);
				g_pnBulletDamage->SetText(buffer);
			}

			Q_snprintf(buffer, sizeof(buffer), "%.1fs", pWeapon->m_flApproximateReloadTime);
			g_pnReloadSpeed->SetText(buffer);

			Q_snprintf(buffer, sizeof(buffer), "%.2fs", pWeapon->m_flLockTime + pWeapon->m_flRandomAdditionalLockTimeMax);
			g_pnLockTime->SetText(buffer);
		}
		if (melee) {
			//calc melee range
			float meleeRange = (melee->m_flRange - 40.f) / 60.f;
			set(MeleeRange, meleeRange);
			Q_snprintf(buffer, sizeof(buffer), "%.0f\"", melee->m_flRange);
			g_pnMeleeRange->SetText(buffer);

			//calc melee damage
			float meleeDamage = melee->m_iDamage / 100.f;
			set(MeleeDamage, meleeDamage);
			Q_snprintf(buffer, sizeof(buffer), "%i", melee->m_iDamage);
			g_pnMeleeDamage->SetText(buffer);

			//calc melee tolerance
			float meleeTolerance = acosf(melee->m_flCosAngleTolerance) + melee->m_flRetraceDuration;
			set(MeleeTolerance, meleeTolerance);
			

			float meleeSpeed = 0.5f / melee->m_flAttackrate;
			if (meleeSpeed > 1.0f) meleeSpeed = 1.0f;
			set(MeleeSpeed, meleeSpeed);
			Q_snprintf(buffer, sizeof(buffer), "%.2fs", melee->m_flAttackrate);
			g_pnMeleeSpeed->SetText(buffer);
		}
#undef set
	}



	void Create(Panel* pParent) {
		//don't create the buttons if we already have them
		if (!g_pClassSpeed) {
			g_pClassSpeed		= new Label(pParent, "ClassSpeedLabel", "");
			g_pAccuracy			= new Label(pParent, "AccuracyLabel", "");
			g_pReloadSpeed		= new Label(pParent, "ReloadSpeedLabel", "");
			g_pBulletDamage		= new Label(pParent, "BulletDamageLabel", "");
			g_pLockTime			= new Label(pParent, "LockTimeLabel", "");
			g_pMeleeDamage		= new Label(pParent, "MeleeDamageLabel", "");
			g_pMeleeRange		= new Label(pParent, "MeleeRangeLabel", "");
			g_pMeleeTolerance	= new Label(pParent, "MeleeToleranceLabel", "");
			g_pMeleeSpeed		= new Label(pParent, "MeleeSpeedLabel", "");

			g_pnClassSpeed		= new Label(pParent, "ClassSpeedNumber", "");
			g_pnReloadSpeed		= new Label(pParent, "ReloadSpeedNumber", "");
			g_pnLockTime		= new Label(pParent, "LockTimeNumber", "");
			g_pnBulletDamage	= new Label(pParent, "BulletDamageNumber", "");
			g_pnMeleeDamage		= new Label(pParent, "MeleeDamageNumber", "");
			g_pnMeleeRange		= new Label(pParent, "MeleeRangeNumber", "");
			g_pnMeleeSpeed		= new Label(pParent, "MeleeSpeedNumber", "");
		}
	}

	void CreateLayout() {
		int offset = yBase - g_iRowSpacing;

#define set(a)  offset += g_iRowSpacing; a->SetPos(xBase, offset); a->SetSize(g_iBarWidth * 1.5, g_iBarHeight);
#define setn(a) a->SetPos(xBase + g_iBarWidth + 3, offset + g_iBarHeight); a->SetSize(80, g_iBarHeight);

		set(g_pClassSpeed);
		setn(g_pnClassSpeed);
		set(g_pAccuracy);
		set(g_pBulletDamage);
		setn(g_pnBulletDamage);
		set(g_pReloadSpeed);
		setn(g_pnReloadSpeed);
		set(g_pLockTime);
		setn(g_pnLockTime);
		set(g_pMeleeRange);
		setn(g_pnMeleeRange);
		set(g_pMeleeDamage);
		setn(g_pnMeleeDamage);
		set(g_pMeleeTolerance);
		set(g_pMeleeSpeed);
		setn(g_pnMeleeSpeed);
#undef setn
#undef set

		IScheme* pScheme = g_pVGuiSchemeManager->GetIScheme(g_pClassSpeed->GetScheme());
		HFont font = GetDefaultBG3FontScaledVertical(pScheme, g_pClassSpeed);

#define set(a) g_p##a->SetFont(font)
		set(ClassSpeed);
		set(nClassSpeed);
		set(Accuracy);
		set(BulletDamage);
		set(nBulletDamage);
		set(ReloadSpeed);
		set(nReloadSpeed);
		set(LockTime);
		set(nLockTime);
		set(MeleeRange);
		set(nMeleeRange);
		set(MeleeDamage);
		set(nMeleeDamage);
		set(MeleeTolerance);
		set(MeleeSpeed);
		set(nMeleeSpeed);
#undef set

		Localize();
	}

	void SetVisible(bool bVisible) {
#define set(a) a->SetVisible(bVisible);
		set(g_pClassSpeed)
		set(g_pAccuracy)
		set(g_pBulletDamage)
		set(g_pReloadSpeed)
		set(g_pLockTime)
		set(g_pMeleeRange)
		set(g_pMeleeDamage)
		set(g_pMeleeTolerance)
		set(g_pMeleeSpeed)
#undef set
	}

	void ManualPaint() {
		g_imgStatBarOverlay->SetSize(g_iBarWidth, g_iBarHeight);

		int y = yBase + g_iBarHeight; //the offset is to separate the images from the labels
		//could easily make this an inline function as well, oh well
#define paint(a) \
			g_imgStatBar->SetSize(g_i##a##Width, g_iBarHeight); \
			g_imgStatBar->SetPos(xBase, y); \
			g_imgStatBar->Paint(); \
			g_imgStatBarOverlay->SetPos(xBase, y); \
			g_imgStatBarOverlay->Paint(); \
			y += g_iRowSpacing;

		paint(ClassSpeed);
		//paint(ClassNotes);
		paint(Accuracy);
		paint(BulletDamage);
		paint(ReloadSpeed);
		paint(LockTime);
		paint(MeleeRange);
		paint(MeleeDamage);
		paint(MeleeTolerance);
		paint(MeleeSpeed);
#undef paint
	}

	void Localize() {
		g_pClassSpeed->SetText("#BG3_STAT_CLASS_SPEED");
		g_pAccuracy->SetText("#BG3_STAT_ACCURACY");
		g_pBulletDamage->SetText("#BG3_STAT_BULLET_DAMAGE");
		g_pReloadSpeed->SetText("#BG3_STAT_RELOAD_SPEED");
		g_pLockTime->SetText("#BG3_STAT_LOCK_TIME");
		g_pMeleeRange->SetText("#BG3_STAT_MELEE_RANGE");
		g_pMeleeDamage->SetText("#BG3_STAT_MELEE_DAMAGE");
		g_pMeleeTolerance->SetText("#BG3_STAT_MELEE_TOLERANCE");
		g_pMeleeSpeed->SetText("#BG3_STAT_MELEE_SPEED");
	}

	Label* g_pClassSpeed = nullptr;
	//Label* g_pClassNotes; //comments on strengths/weaknesses/abilities

	Label* g_pAccuracy;
	Label* g_pBulletDamage;
	Label* g_pReloadSpeed;
	Label* g_pLockTime;
	Label* g_pMeleeRange;
	Label* g_pMeleeDamage;
	Label* g_pMeleeTolerance; //COS tolerance + melee duration
	Label* g_pMeleeSpeed;

	Label* g_pnClassSpeed;
	Label* g_pnBulletDamage;
	Label* g_pnReloadSpeed;
	Label* g_pnLockTime;
	Label* g_pnMeleeRange;
	Label* g_pnMeleeDamage;
	Label* g_pnMeleeSpeed;
}







//-----------------------------------------------------------------------------
// Purpose: Begin ammo button functions
//-----------------------------------------------------------------------------
CAmmoButton::CAmmoButton(Panel* parent, const char* panelName, const char* text, int index) : Button(parent, panelName, text), m_iIndex(index) {
	//load images here
	if (m_iIndex == AMMO_KIT_BALL) {
		m_pCurrentImage = scheme()->GetImage(MENU_AMMO_BALL, false);
	}
	else {
		m_pCurrentImage = scheme()->GetImage(MENU_AMMO_BUCKSHOT, false);
	}
}

void CAmmoButton::ApplySchemeSettings(IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);

	SetPaintBorderEnabled(false);
}

void CAmmoButton::OnMousePressed(MouseCode code) {
	SetAsCurrent();

	PlaySelectSound();
}

void CAmmoButton::OnCursorEntered() {
	//mark changes
	g_pDisplayedAmmoButton = this;
	m_bMouseOver = true;
	
	//play sound
	PlayHoverSound();

	//update displayed weapon stats
	NClassWeaponStats::UpdateWeaponStatsAllCurrent();
}

void CAmmoButton::OnCursorExited() { 
	g_pDisplayedAmmoButton = g_pCurrentAmmoButton;

	//update displayed weapon stats
	NClassWeaponStats::UpdateWeaponStatsAllCurrent();

	m_bMouseOver = false;
}

void CAmmoButton::ManualPaint() {
	int x, y;
	GetPos(x, y);

	int w, h;
	GetSize(w, h);
	m_pCurrentImage->SetPos(x, y);
	m_pCurrentImage->SetSize(w, h);
	m_pCurrentImage->Paint();

	if (m_bMouseOver) {
		g_pHoverIcon->SetPos(x, y);
		g_pHoverIcon->SetSize(w, h);
		g_pHoverIcon->Paint();
	}
}

void CAmmoButton::SetAsCurrent() {
	//change image of old button
	//g_pCurrentAmmoButton->m_pCurrentImage = g_pCurrentAmmoButton->m_pAvailableImage;

	//switch current button to us
	g_pCurrentAmmoButton = this;
	g_pDisplayedAmmoButton = this;

	//update our image
	//m_pCurrentImage = m_pSelectedImage;

	//update our class' default ammo, if this weapon has the choice
	if (s_pGunKit->m_bAllowBuckshot)
		GetDisplayedClass()->SetDefaultAmmo(GetAmmoIndex());
}

void CAmmoButton::UpdateToMatchClassAndWeapon(const CPlayerClass* pClass, const CGunKit* pKit) {
	//assume ball, then check for buckshot
	CAmmoButton* pButtonToSelect = g_ppAmmoButtons[AMMO_KIT_BALL];
	s_pGunKit = pKit;
	bool visible = false;

	//we use checks instead of straight assignment to clamp out-of-range values
	if (pKit->m_bAllowBuckshot) {
		visible = true;
		if (pClass->GetDefaultAmmo() == AMMO_KIT_BUCKSHOT) {
			pButtonToSelect = g_ppAmmoButtons[AMMO_KIT_BUCKSHOT];
		}
	}
	g_ppAmmoButtons[AMMO_KIT_BUCKSHOT]->SetVisible(visible);
	g_ppAmmoButtons[AMMO_KIT_BALL]->SetVisible(visible);

	//select the button
	pButtonToSelect->SetAsCurrent();
}

const CGunKit* CAmmoButton::s_pGunKit;





//-----------------------------------------------------------------------------
// Purpose: Begin uniform button functions
//-----------------------------------------------------------------------------
void CUniformButton::ApplySchemeSettings(IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetPaintBorderEnabled(false);
}

void CUniformButton::OnMousePressed(MouseCode code) {
	SetAsCurrent();
	PlaySelectSound();
}

void CUniformButton::OnCursorEntered() {
	PlayHoverSound();
}

void CUniformButton::ManualPaint() {
	int x, y;
	GetPos(x, y);

	int w, h;
	GetSize(w, h);
	m_pCurrentImage->SetPos(x, y);
	m_pCurrentImage->SetSize(w, h);
	m_pCurrentImage->Paint();
}

void CUniformButton::SetAsCurrent() {
	//change image of old button
	//g_pCurrentUniformButton->m_pCurrentImage = g_pCurrentUniformButton->m_pAvailableImage;

	//switch current button to us
	g_pCurrentUniformButton = this;

	//update our image
	//m_pCurrentImage = m_pSelectedImage;

	//update our class' default uniform
	GetDisplayedClass()->SetDefaultUniform(GetUniformIndex());
}

void CUniformButton::UpdateToMatchClass(const CPlayerClass* pClass) {
	int idealUniform = pClass->GetDefaultUniform();

	//clamp value
	if (idealUniform >= pClass->numChooseableUniformsForPlayer(CBasePlayer::GetLocalPlayer()))
		idealUniform = pClass->numChooseableUniformsForPlayer(CBasePlayer::GetLocalPlayer());
	if (idealUniform < 0)
		idealUniform = 0;

	//set selected button
	g_ppUniformButtons[idealUniform]->SetAsCurrent();

	//hide/show buttons based on uniform count
	char buffer[32];
	for (int i = 0; i < NUM_UNIFORM_BUTTONS; i++) {
		bool bVisible = i < pClass->numChooseableUniformsForPlayer(CBasePlayer::GetLocalPlayer());
		CUniformButton* bt = g_ppUniformButtons[i];
		bt->SetVisible(bVisible);
		if (bVisible) {
			//update our image here
			Q_snprintf(buffer, sizeof(buffer), "classmenu/uniforms/%c/%s%i", charForTeam(pClass->m_iDefaultTeam), pClass->m_pszAbbreviation, bt->GetUniformIndex());
			bt->m_pCurrentImage = scheme()->GetImage(buffer, false);
		}
	}

	//don't show 1 button if there's only 1 uniform
	g_ppUniformButtons[0]->SetVisible(pClass->numChooseableUniformsForPlayer(CBasePlayer::GetLocalPlayer()) > 1);
}


//-----------------------------------------------------------------------------
// Purpose: Team toggle button switches between teams instantly
//-----------------------------------------------------------------------------
CTeamToggleButton::CTeamToggleButton(const char* pszImgName) : m_bMouseOver(false),
	Button(g_pClassMenu, "TeamToggleButton", "") { 
	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);

	m_pImage = scheme()->GetImage(pszImgName, false);
}

void CTeamToggleButton::OnMousePressed(MouseCode code) {
	//if (!g_pTeamMenu->MayJoinOtherTeam(g_iCurrentTeammenuTeam))
		//return;

	int nextTeam = g_iCurrentTeammenuTeam == TEAM_AMERICANS ? TEAM_BRITISH : TEAM_AMERICANS;
	g_pClassMenu->UpdateToMatchTeam(nextTeam);
	PlaySelectSound();
}

void CTeamToggleButton::Paint() {

	int w, h;
	GetSize(w, h);
	m_pImage->SetSize(w, h);
	m_pImage->SetPos(0, 0);
	m_pImage->Paint();

	if (m_bMouseOver) {
		g_pSelectionIcon->SetPos(0, 0);
		g_pSelectionIcon->SetSize(w, h);
		g_pSelectionIcon->Paint();
	}
}

void CTeamToggleButton::OnCursorEntered() {
	m_bMouseOver = true; 
	PlayHoverSound();
}

//-----------------------------------------------------------------------------
// Purpose: Begin class menu functions
//-----------------------------------------------------------------------------
CClassMenu::CClassMenu(vgui::VPANEL pParent) : Frame(NULL, PANEL_CLASSES) {
	g_pClassMenu = this;

	SetParent(pParent);
	SetTitleBarVisible(false);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(false);
	SetSizeable(false);
	SetMoveable(false);
	SetProportional(true);
	SetVisible(false);

	SetScheme("ClientScheme");

	// Size of the panel
	int wide, tall;
	surface()->GetScreenSize(wide, tall);
	SetSize(wide, tall);

	m_pBackground = NULL;

	//Construct class buttons
	g_ppClassButtons = new CClassButton*[NUM_CLASS_BUTTONS];
	for (int i = 0; i < NUM_CLASS_BUTTONS; i++) {
		g_ppClassButtons[i] = new CClassButton(this, "ClassButton", "", i);
	}

	//Construct weapon button
	g_pWeaponButton = new CWeaponButton(this, "WeaponButton", "");

	//Create glowy arrow
	g_pGlowyWeaponArrow = new GlowyArrowForPlayersWhoDontRealizeYouCanSwitchWeapons();

	//Construct ammo buttons
	g_ppAmmoButtons = new CAmmoButton*[NUM_AMMO_BUTTONS];
	g_ppAmmoButtons[AMMO_KIT_BALL] = new CAmmoButton(this, "AmmoButton", "", AMMO_KIT_BALL);
	g_ppAmmoButtons[AMMO_KIT_BUCKSHOT] = new CAmmoButton(this, "Ammo", "", AMMO_KIT_BUCKSHOT);


	//Construct uniform buttons
	g_ppUniformButtons = new CUniformButton*[NUM_UNIFORM_BUTTONS];
	for (int i = 0; i < NUM_UNIFORM_BUTTONS; i++) {
		g_ppUniformButtons[i] = new CUniformButton(this, "UniformButton", "", i);
	}

	//Create class/weapon stats layout
	NClassWeaponStats::Create(this);

	g_pGoButton = new FancyButton(this, "GoButton", MENU_GO_ICON, MENU_GO_ICON);
	g_pGoButton->SetSizeAll(200, 100);
	g_pGoButton->SetText("#BG3_Classmenu_go");
	g_pGoButton->SetCommandFunc([]() {
		//double press it to activate it!
		g_pSelectedClassButton->SimulateMousePressed();
		g_pSelectedClassButton->SimulateMousePressed();
	});
	g_pGoButton->SetVisible(true);
	g_pGoButton->MakeReadyForUse();

	//Construct labels
	g_pClassNameLabel = new Label(this, "ClassNameLabel", "");
	g_pWeaponNameLabel = new Label(this, "WeaponNameLabel", "");
	g_pClassDescLabel = new Label(this, "ClassDescLabel", "");
	g_pWeaponDescLabel = new Label(this, "ClassDescLabel", "");
	g_pTeamwidePickingLabel = new Label(this, "TeamwidePickingLabel", "");
	g_pClassBuffLabel = new Label(this, "ClassBuffLabel", "");
	g_pOfficerBuffsLabel = new Label(this, "OfficerBuffsLabel", "");

	//Build TeamToggle and Close Buttons
	m_pCloseButton = new CCloseButton(this, "ClassMenuCloseButton", MENU_CLOSE_ICON);
	m_pTeamToggleButton = new CTeamToggleButton(MENU_TEAM_SWITCH_ICON);

	//Be 100% sure that the runtime data we need from the classes has been initialized
	//CPlayerClass::InitClientRunTimeData();


	//find images
	NClassWeaponStats::g_imgStatBar = scheme()->GetImage(MENU_STATBAR, false);
	NClassWeaponStats::g_imgStatBarOverlay = scheme()->GetImage(MENU_STATBARMETER, false);

	//Find selection and darken images
	g_pSelectionIcon = scheme()->GetImage(MENU_SELECTION_ICON, false);
	g_pHoverIcon = scheme()->GetImage(MENU_HOVER_ICON, false);
	g_pDarkIcon = scheme()->GetImage(MENU_DARK_ICON, false);
	g_pNoneIcon = scheme()->GetImage(MENU_NONE_ICON, false);
	//g_pGoButton->m_pCurrentImage = scheme()->GetImage(MENU_GO_ICON, false);

	PerformLayout();
}

CClassMenu::~CClassMenu() {

}

void CClassMenu::Update() {
	m_flNextGameRulesUpdate = gpGlobals->curtime + 5.0f;

	//this ensures that our teamnumber is valid
	if (g_iCurrentTeammenuTeam == TEAM_INVALID) {
		UpdateToMatchTeam(TEAM_AMERICANS);
	}

	//have class buttons recalculate their availability
	for (int i = 0; i < NUM_CLASS_BUTTONS; i++)
		if (g_ppClassButtons[i]->IsVisible())
			g_ppClassButtons[i]->UpdateGamerulesData();
}

void CClassMenu::ShowPanel(bool bShow) {
	if (!bShow)
		g_bClassMenuOfficerMode = false;//closing the menu always disables officer mode

	if (engine->IsPlayingDemo()) //Don't show up in demos -HairyPotter
		return;

	if (BaseClass::IsVisible() == bShow)
		return;

	if (bShow) {
		//sometimes after being forced to switch teams, our current team
		//may not be what it was when we first picked it in the teammenu
		//so we have to update it according to what team the player is
		//currently on... IFF they didn't come from the teammenu
		if (!g_pTeamMenu->IsVisible()) {
			C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
			if (pPlayer && pPlayer->GetTeamNumber() >= TEAM_AMERICANS) {
				//Msg("Updating g_iCurrentTeammenuTeam to match player");
				g_iCurrentTeammenuTeam = pPlayer->GetTeamNumber();
			}
		}

		g_pTeamMenu->ShowPanel(false);
		CPlayerClass::InitClientRunTimeData();
		HideCommMenu();
		Activate();
		PerformLayout();

		m_pTeamToggleButton->SetVisible(!g_bClassMenuOfficerMode); //disable team toggle in officer mode
		g_pTeamwidePickingLabel->SetVisible(g_bClassMenuOfficerMode);
		for (int i = 0; i < NUM_CLASS_BUTTONS; i++) {
			g_ppClassButtons[i]->m_pCountLabel->SetVisible(!g_bClassMenuOfficerMode);
		}

		if (g_iCurrentTeammenuTeam != TEAM_INVALID)
			UpdateToMatchTeam(g_iCurrentTeammenuTeam);
		Update();
	}
	else
		SetVisible(false);

	SetMouseInputEnabled(bShow);
	SetKeyBoardInputEnabled(bShow);
}

void CClassMenu::Paint() {
	//Paint background
	m_pBackground->Paint();

	if (g_pClassArt)
		g_pClassArt->Paint();

	/****************************************************
	* Begin manual painting!
	* bcs for some reason the images don't paint in the
	* built-in painting scheme, but they do here
	*****************************************************/
	int i;
	for (i = 0; i < NUM_CLASS_BUTTONS; i++)
		if (g_ppClassButtons[i]->IsVisible())
			g_ppClassButtons[i]->ManualPaint();
	for (i = 0; i < NUM_UNIFORM_BUTTONS; i++)
		if (g_ppUniformButtons[i]->IsVisible())
			g_ppUniformButtons[i]->ManualPaint();
	for (i = 0; i < NUM_AMMO_BUTTONS; i++) {
		//Msg("%i", g_ppAmmoButtons[i]->IsVisible());
		if (g_ppAmmoButtons[i]->IsVisible())
			g_ppAmmoButtons[i]->ManualPaint();
	}
	
	//Paint weapon button
	g_pWeaponButton->ManualPaint();

	//Paint glowy button on top
	if (g_pGlowyWeaponArrow->IsVisible())
		g_pGlowyWeaponArrow->PaintToScreen();

	//call class weapon stats paint
	//these will paint the images in the current frame
	NClassWeaponStats::ManualPaint();

	//paint selection icons over class, ammo, uniform buttons
	//class button first
	int x, y;
	int w, h;
	g_pSelectedClassButton->GetPos(x, y);
	g_pSelectedClassButton->GetSize(w, h);
	g_pSelectionIcon->SetPos(x, y);
	g_pSelectionIcon->SetSize(w, h);
	g_pSelectionIcon->Paint();

	//g_pGoButton->ManualPaint();

	//selected ammo button
	if (g_pCurrentAmmoButton->IsVisible()) {
		g_pCurrentAmmoButton->GetPos(x, y);
		g_pCurrentAmmoButton->GetSize(w, h);
		g_pSelectionIcon->SetPos(x, y);
		g_pSelectionIcon->SetSize(w, h);
		g_pSelectionIcon->Paint();
	}

	//current uniform button
	if (g_pCurrentUniformButton->IsVisible()) {
		g_pCurrentUniformButton->GetPos(x, y);
		g_pCurrentUniformButton->GetSize(w, h);
		g_pSelectionIcon->SetPos(x, y);
		g_pSelectionIcon->SetSize(w, h);
		g_pSelectionIcon->Paint();
	}

	//the darken overlays for the class buttons are painted in their paint code
	//this way I don't have to write a loop
}

void CClassMenu::OnThink() {
	//PerformLayout();
	BaseClass::OnThink();
}

void CClassMenu::SetVisible(bool bVisible) {
	BaseClass::SetVisible(bVisible);
}

void CClassMenu::OnKeyCodePressed(KeyCode code) {
	//we will simulate a press of one of the class buttons
	//TODO actually get the binds for these instead!
	int index = code - KEY_1; //so that KEY_1 has index 0
	if (index >= 0 && index < NUM_CLASS_BUTTONS)
		g_ppClassButtons[index]->SimulateMousePressed();

	if (code == KEY_N && !g_bClassMenuOfficerMode)
		engine->ClientCmd("teammenu");
}

void CClassMenu::OnKeyCodeTyped(KeyCode code) {
	if (code == KEY_ESCAPE) {
		ShowPanel(false);
		SMenu->Hide();
	}
	else
		BaseClass::OnKeyCodeTyped(code);
}

void CClassMenu::ApplySchemeSettings(IScheme* pScheme) {
	//make all buttons ready for use
	for (int i = 0; i < NUM_CLASS_BUTTONS; i++) {
		g_ppClassButtons[i]->MakeReadyForUse();
	}
	g_pWeaponButton->MakeReadyForUse();
	for (int i = 0; i < NUM_AMMO_BUTTONS; i++) {
		g_ppAmmoButtons[i]->MakeReadyForUse();
	}
	for (int i = 0; i < NUM_UNIFORM_BUTTONS; i++) {
		g_ppUniformButtons[i]->MakeReadyForUse();
	}
	
	//g_pGoButtonLabel->SetText("#BG3_Classmenu_go");
	//g_pGoButtonLabel->MakeReadyForUse();

	//Set fonts of the two titles!
	SetDefaultBG3FontScaled(pScheme, g_pClassNameLabel);
	SetDefaultBG3FontScaled(pScheme, g_pWeaponNameLabel);
	SetDefaultBG3FontScaled(pScheme, g_pGoButton);
	SetDefaultBG3FontScaled(pScheme, g_pTeamwidePickingLabel);

#define set(a) a->SetFgColor(g_cBG3TextColor)
	set(g_pClassNameLabel);
	set(g_pWeaponNameLabel);
	set(g_pClassDescLabel);
	set(g_pWeaponDescLabel);
	set(g_pTeamwidePickingLabel);
	set(g_pClassBuffLabel);
#undef set

	g_pClassBuffLabel->SetPaintBackgroundEnabled(true);
	g_pClassBuffLabel->SetBgColor(Color(0, 0, 0, 100));
}

void CClassMenu::PerformLayout() {
	BaseClass::PerformLayout();

	//Find default background image
	if (!m_pBackground) m_pBackground = scheme()->GetImage(MENU_BACKGROUND_A, false);

	//calculate margins
	int xBase = 50;
	int yBase = 175;
	int width = ScreenWidth(), height = ScreenHeight();
	SetSize(width, height);

	//----------------------------------------------------------
	//CLASS BUTTONS
	//----------------------------------------------------------
	//Set sizes of class buttons
	int& iconsz = CClassButton::s_iSize;

	//default value of s_iSize is 50, so we're gonna assume our base resolution is 640x360 and we'll scale up from there
	float scale = width / 640.f;
	if (scale < 1) scale = 1;
	iconsz = DEFAULT_ICON_SIZE * scale;

	int classw = iconsz * NUM_CLASS_BUTTONS;
	int i;

	//only one row of buttons
	int classy = yBase;
	for (i = 0; i < NUM_CLASS_BUTTONS; i++) {
		g_ppClassButtons[i]->SetSize(iconsz, iconsz);
		int x = xBase + i*iconsz;
		g_ppClassButtons[i]->SetPos(x, classy);
	}

	//----------------------------------------------------------
	//WEAPON BUTTON
	//----------------------------------------------------------

	int weaponx = NUM_CLASS_BUTTONS * iconsz + xBase;
	int weaponw = 4 * iconsz;
	g_pWeaponButton->SetPos(weaponx, yBase);
	g_pWeaponButton->SetSize(weaponw, iconsz);

	//----------------------------------------------------------
	//UNIFORM BUTTONS
	//----------------------------------------------------------
	int uniformy = classy + iconsz + 3;
	int uniformh = iconsz / 3;
	for (i = 0; i < NUM_UNIFORM_BUTTONS; i++) {
		int x = xBase + i*iconsz;
		g_ppUniformButtons[i]->SetSize(iconsz, uniformh);
		g_ppUniformButtons[i]->SetPos(x, uniformy);
	}

	//----------------------------------------------------------
	//GLOWY ARROW OVER WEAPON BUTTON
	//----------------------------------------------------------
	const int ARROW_SIZE = iconsz / 1.5f;
	g_pGlowyWeaponArrow->SetSize(ARROW_SIZE);
	g_pGlowyWeaponArrow->SetPulseSize(iconsz / 8);
	const int ARROW_OFFSET = (iconsz - ARROW_SIZE) / 2;
	g_pGlowyWeaponArrow->SetPosition(weaponx + 3 * iconsz + ARROW_OFFSET, yBase + ARROW_OFFSET);
	g_pGlowyWeaponArrow->SetImage(MENU_ARROW_HOVER);
	

	//----------------------------------------------------------
	//AMMO BUTTONS
	//----------------------------------------------------------
	int icon_half = iconsz / sqrtf(2);
	for (i = 0; i < NUM_AMMO_BUTTONS; i++) {
		int x = weaponx + i*icon_half;
		g_ppAmmoButtons[i]->SetPos(x, uniformy);
		g_ppAmmoButtons[i]->SetSize(icon_half, icon_half);
	}

	//----------------------------------------------------------
	//CLASS WEAPON STATS
	//----------------------------------------------------------
	//determine coordinates
	//float statscale = (scale + 1) / 2;
	NClassWeaponStats::xBase = weaponx + weaponw + iconsz / 4;
	NClassWeaponStats::yBase = classy + iconsz / 4;
	NClassWeaponStats::g_iBarWidth = NClassWeaponStats::g_iDefaultBarWidth * scale / 1.1;
	NClassWeaponStats::g_iBarHeight = NClassWeaponStats::g_iDefaultBarHeight * scale / 1.1;
	NClassWeaponStats::g_iRowSpacing = NClassWeaponStats::g_iDefaultRowSpacing * scale / 1.1;
	//these coordinates are below the ammo buttons
	//now call this to recalculate coordinates
	NClassWeaponStats::CreateLayout();

	//----------------------------------------------------------
	//GLOBAL IMAGES
	//----------------------------------------------------------
	g_pSelectionIcon->SetSize(iconsz, iconsz);
	g_pDarkIcon->SetSize(iconsz, iconsz);
	m_pBackground->SetSize(ScreenWidth(), ScreenHeight());
	m_pBackground->SetPos(0, 0);

	//----------------------------------------------------------
	//GO BUTTON AND LABEL
	//----------------------------------------------------------
	const int goButtonWidthBase = 120;
	int gobuttonw = goButtonWidthBase * scale;
	int gobuttonx = width - gobuttonw;
	int gobuttonh = gobuttonw / 2;
	g_pGoButton->SetPosAll(gobuttonx, 0);
	g_pGoButton->SetSizeAll(gobuttonw, gobuttonh);

	//g_pGoButtonLabel->SetPos(gobuttonx + 18 * scale, 0);
	//g_pGoButtonLabel->SetSize(gobuttonw, gobuttonh);

	//----------------------------------------------------------
	//CLOSE & TEAM SWITCH BUTTONS
	//----------------------------------------------------------
	int smlsz = iconsz / 6;
	m_pCloseButton->SetPos(0, 0);
	m_pCloseButton->SetSize(smlsz, smlsz);

	m_pTeamToggleButton->SetPos(smlsz, 0);
	m_pTeamToggleButton->SetSize(smlsz, smlsz);

	//----------------------------------------------------------
	//GLOBAL TEXTS
	//----------------------------------------------------------
	int yInset = 150;
	int namey = yBase - yInset;
	int nameh = 65;
	g_pClassNameLabel->SetPos(xBase, namey);
	g_pClassNameLabel->SetSize(classw + iconsz, nameh);

	g_pWeaponNameLabel->SetPos(weaponx, namey);
	g_pWeaponNameLabel->SetSize(weaponw, nameh);

	int desch = yInset - nameh;
	int descy = namey + nameh;
	g_pClassDescLabel->SetPos(xBase, descy);
	g_pClassDescLabel->SetSize(classw + iconsz, desch);

	g_pWeaponDescLabel->SetPos(weaponx, descy);
	g_pWeaponDescLabel->SetSize(weaponw, desch);

	g_pTeamwidePickingLabel->SetPos(0, ScreenHeight() - 200);
	g_pTeamwidePickingLabel->SetSize(ScreenWidth() - 100, 200);
	g_pTeamwidePickingLabel->SetText("#BG3_Classmenu_teamwide");

	g_pClassBuffLabel->SetPos(xBase, uniformy + 2);
	g_pClassBuffLabel->SetSize(classw, 30);

	g_pOfficerBuffsLabel->SetPos(weaponx, uniformy + icon_half);
	g_pOfficerBuffsLabel->SetSize(weaponw, 600);
}

void CClassMenu::UpdateToMatchTeam(int iTeam) {
	g_iCurrentTeammenuTeam = iTeam; //just in case

	//don't bother updating if we're already showing this team
	if (m_iPreviouslyShownTeam == iTeam)
		return;

	//update class buttons, their classes and their visibility
	int i = 0;
	int numClasses = CPlayerClass::numClassesForTeam(iTeam);
	for (; i < numClasses; i++) {
		g_ppClassButtons[i]->SetVisible(true);

		const CPlayerClass* pClass = CPlayerClass::fromNums(iTeam, i);
		g_ppClassButtons[i]->UpdateToMatchClass(pClass);
	}
	//hide the rest of the class buttons
	for (; i < NUM_CLASS_BUTTONS; i++)
		g_ppClassButtons[i]->SetVisible(false);

	//find a default button to select
	g_pSelectedClassButton = nullptr;
	CClassButton* pAvailable = CClassButton::FindAvailableButton();
	CClassButton::UpdateDisplayedButton(pAvailable);
	pAvailable->Select();

	//update background image
	const char* pszImage = iTeam == TEAM_AMERICANS ? MENU_BACKGROUND_A : MENU_BACKGROUND_B;
	SetBackground(pszImage);

	m_iPreviouslyShownTeam = iTeam;
}

void CClassMenu::SetBackground(const char* pszImage) {
	//Warning("Setting background to %s\n", pszImage);
	m_pBackground = scheme()->GetImage(pszImage, false);
	m_pBackground->SetSize(ScreenWidth(), ScreenHeight());
	m_pBackground->SetPos(0, 0);

}

CClassMenu* g_pClassMenu = nullptr;

#endif