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

#include "bg3_teammenu.h"
#include "bg3_classmenu.h"
#include "bg3/vgui/bg3_fonts.h"
#include "../shared/bg3/Math/bg3_rand.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#if 1

CClassButton*	g_pDisplayedClassButton; //which class is currently displayed?
CClassButton*	g_pSelectedClassButton;	//what was the last class clicked on?
CWeaponButton*	g_pWeaponButton;		//we only have one weapon button, which cycles through the possible weapons
CAmmoButton*	g_pCurrentAmmoButton;
CUniformButton*	g_pCurrentUniformButton;

CClassButton**	g_ppClassButtons		= nullptr;
CAmmoButton**	g_ppAmmoButtons			= nullptr;
CUniformButton**	g_ppUniformButtons	= nullptr;

CGoButton*		g_pGoButton;
Label*			g_pGoButtonLabel;

vgui::IImage* g_pSelectionIcon;
vgui::IImage* g_pDarkIcon;
vgui::IImage* g_pNoneIcon;

Label*			g_pClassNameLabel;
Label*			g_pWeaponNameLabel;
Label*			g_pClassDescLabel;
Label*			g_pWeaponDescLabel;

#define NUM_CLASS_BUTTONS 6
#define NUM_AMMO_BUTTONS 2
#define NUM_UNIFORM_BUTTONS 3

int g_iCurrentTeammenuTeam = TEAM_INVALID;

using namespace vgui;
using namespace NMenuSounds;


//-----------------------------------------------------------------------------
// Purpose: Begin class button functions
//-----------------------------------------------------------------------------
void CClassButton::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetPaintBorderEnabled(false);
}

void CClassButton::OnMousePressed(vgui::MouseCode code) {
	UpdateGamerulesData();
	if (IsAvailable()) {
		if (IsSelected()) {
			//we've click on what we have selected, so let's try to join the team
			int gun = g_pWeaponButton->GetCurrentGunKit();
			int ammo = g_pCurrentAmmoButton->GetAmmoIndex();
			int skin = g_pCurrentUniformButton->GetUniformIndex();
			int team = g_iCurrentTeammenuTeam;
			int iClass = m_pPlayerClass->m_iClassNumber;
			JoinRequest(gun, ammo, skin, team, iClass);
			g_pClassMenu->ShowPanel(false);
		}
		else {
			Select();

			//play a sound
			PlaySelectSound();
		}
	}
	else {
		//TODO print "not available!" message
	}
}

void CClassButton::OnCursorEntered() {
	if (m_eAvailable != CLASS_UNAVAILABLE) {
		UpdateDisplayedButton(this);
		PlayHoverSound();
	}
}

void CClassButton::OnCursorExited() {
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
	if (m_eAvailable == CLASS_FULL) {
		g_pDarkIcon->SetPos(x, y);
		g_pDarkIcon->SetPos(w, h);
		g_pDarkIcon->Paint();
	}

}

/*const wchar* CClassButton::GetPlayerClassName() {
	char team = charForTeam(m_pPlayerClass->m_iDefaultTeam);
	const char* abbrev = m_pPlayerClass->m_pszAbbreviation;

	char buffer[32];
	Q_snprintf(buffer, sizeof(buffer), "#BG3_%c%s_name", team, abbrev);
	return g_pVGuiLocalize->Find(buffer);
}*/

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

	m_eAvailable = m_pPlayerClass->availabilityForPlayer(pPlayer);

	UpdateImage();
}

void CClassButton::UpdateImage() {
	if (IsAvailable())
		m_pCurrentImage = m_pAvailableImage;
	else
		m_pCurrentImage = g_pNoneIcon;
}

void CClassButton::UpdateDisplayedButton(CClassButton* pNext) {
	if (pNext == g_pDisplayedClassButton)
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
}

void CClassButton::UpdateToMatchClass(const CPlayerClass* pClass) {
	//update internal reference
	m_pPlayerClass = pClass;

	char team = charForTeam(pClass->m_iDefaultTeam);

	//get images for VGUI
	char buffer[40];
	Q_snprintf(buffer, ARRAYSIZE(buffer), "classmenu/classes/%c/%s", team, pClass->m_pszAbbreviation);
	m_pAvailableImage = scheme()->GetImage(buffer, false);

	//update gamerules data - this will also set our current image
	UpdateGamerulesData();
}

#define DEFAULT_ICON_SIZE 50
int CClassButton::s_iSize = DEFAULT_ICON_SIZE;






//-----------------------------------------------------------------------------
// Purpose: Looks at the currently selected stuff and sends message to server
//-----------------------------------------------------------------------------
void JoinRequest(int iGun, int iAmmo, int iSkin, int iTeam, int iClass) {
	char cmd[32];
	Q_snprintf(cmd, sizeof(cmd), "kit %i %i %i \n", iGun, iAmmo, iSkin);
	engine->ServerCmd(cmd);

	// deprecated
	//Q_snprintf( cmd, sizeof( cmd ), "classskin %i \n", skinId );
	//engine->ServerCmd( cmd );

	Q_snprintf(cmd, sizeof(cmd), "class %i %i \n", iTeam, iClass);
	engine->ServerCmd(cmd); //Send the class AFTER we've selected a gun, otherwise you won't spawn with the right kit.
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
	PlayHoverSound();
}

void CWeaponButton::ManualPaint() {
	int x, y;
	GetPos(x, y);

	int w, h;
	GetSize(w, h);
	m_pCurrentImage->SetPos(x, y);
	m_pCurrentImage->SetSize(w, h);
	m_pCurrentImage->Paint();
}

void CWeaponButton::UpdateToMatchPlayerClass(const CPlayerClass* pClass) {
	m_iWeaponCount = pClass->numChooseableWeapons();

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
	Msg("Updating weapon to %s\n", pKit->m_pszWeaponName);
	UpdateImage(pKit);
	
	//update weapon name label
	g_pWeaponNameLabel->SetText(pKit->GetLocalizedName());
	g_pWeaponDescLabel->SetText(pKit->GetLocalizedDesc());

	//update the current ammo button
	CAmmoButton::UpdateToMatchClassAndWeapon(GetDisplayedClass(), pKit);

	//update displayed weapon stats!
	//we may have multiple weapons, make sure we get all of them!
	const CWeaponDef* pPrimary;
	const CWeaponDef* pSecondary;
	const CWeaponDef* pTertiary; //this is just here for forward-compatibility

	GetDisplayedClass()->getWeaponDef(m_iCurrentWeapon, &pPrimary, &pSecondary, &pTertiary);

	if (pPrimary)
		NClassWeaponStats::UpdateToMatchWeapon(pPrimary);
	if (pSecondary)
		NClassWeaponStats::UpdateToMatchWeapon(pSecondary);
	if (pTertiary)
		NClassWeaponStats::UpdateToMatchWeapon(pTertiary);
}

void CWeaponButton::UpdateImage(const CGunKit* pKit) {
	char buffer[128];
	Q_snprintf(buffer, ARRAYSIZE(buffer), "classmenu/weapons/%s", pKit->m_pszWeaponName);
	m_pCurrentImage = scheme()->GetImage(buffer, false);
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
	int g_iMeleeRangeWidth;
	int g_iMeleeDamageWidth;
	int g_iMeleeToleranceWidth;
	int g_iMeleeSpeedWidth;

	void UpdateToMatchClass(const CPlayerClass* pClass) {
		float relativeSpeed = FLerp(0.5f, 1.0f, SPEED_GRENADIER, SPEED_SKIRMISHER, pClass->m_flBaseSpeed);
		g_iClassSpeedWidth = g_iBarWidth * relativeSpeed;
	}

	void UpdateToMatchWeapon(const CWeaponDef* pWeapon) {
#define set(a,b) g_i##a##Width = g_iBarWidth * b

		const attackinfo* bullet = nullptr;
		const attackinfo* melee = nullptr;

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

			//calc accuracy
			float accuracy = 1 / Sqr(bullet->m_flStandAimStill);
			float maxAccuracy = 1 / Sqr(CWeaponDef::GetDefForWeapon("weapon_pennsylvania")->m_Attackinfos[0].m_flStandAimStill);

			accuracy = accuracy / maxAccuracy;
			accuracy = FLerp(0.3f, 1.0f, accuracy);
			set(Accuracy, accuracy);
			
			//calc bullet damage
			float damage = bullet->m_iDamage / 120.f;
			set(BulletDamage, damage);

			//calc reload speed
			float reload = 12.f - pWeapon->m_flApproximateReloadTime;
			reload /= 12.0f;
			set(ReloadSpeed, reload);
		}
		if (melee) {
			//calc melee range
			float meleeRange = melee->m_flRange / 100.f;
			set(MeleeRange, meleeRange);

			//calc melee damage
			float meleeDamage = melee->m_iDamage / 100.f;
			set(MeleeDamage, meleeDamage);

			//calc melee tolerance
			float meleeTolerance = acosf(melee->m_flCosAngleTolerance) + melee->m_flRetraceDuration;
			set(MeleeTolerance, meleeTolerance);

			float meleeSpeed = 0.3f / melee->m_flAttackrate;
			set(MeleeSpeed, meleeSpeed);
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
			g_pMeleeDamage		= new Label(pParent, "MeleeDamageLabel", "");
			g_pMeleeRange		= new Label(pParent, "MeleeRangeLabel", "");
			g_pMeleeTolerance	= new Label(pParent, "MeleeToleranceLabel", "");
			g_pMeleeSpeed		= new Label(pParent, "MeleeSpeedLabel", "");
		}
	}

	void CreateLayout() {
		int offset = yBase;

#define set(a) a->SetPos(xBase, offset); a->SetSize(g_iBarWidth, g_iBarHeight); offset += g_iRowSpacing;
		
		set(g_pClassSpeed);
		set(g_pAccuracy);
		set(g_pBulletDamage);
		set(g_pReloadSpeed);
		set(g_pMeleeRange);
		set(g_pMeleeDamage);
		set(g_pMeleeTolerance);
		set(g_pMeleeSpeed);
#undef set
		Localize();
	}

	void SetVisible(bool bVisible) {
#define set(a) a->SetVisible(bVisible);
		set(g_pClassSpeed)
		set(g_pAccuracy)
		set(g_pBulletDamage)
		set(g_pReloadSpeed)
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
	Label* g_pMeleeRange;
	Label* g_pMeleeDamage;
	Label* g_pMeleeTolerance; //COS tolerance + melee duration
	Label* g_pMeleeSpeed;
}


//-----------------------------------------------------------------------------
// Purpose: Class and weapon names and desc labels
//-----------------------------------------------------------------------------




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
	//nothing special to do
	PlayHoverSound();
}

void CAmmoButton::ManualPaint() {
	int x, y;
	GetPos(x, y);

	int w, h;
	GetSize(w, h);
	m_pCurrentImage->SetPos(x, y);
	m_pCurrentImage->SetSize(w, h);
	m_pCurrentImage->Paint();
}

void CAmmoButton::SetAsCurrent() {
	//change image of old button
	//g_pCurrentAmmoButton->m_pCurrentImage = g_pCurrentAmmoButton->m_pAvailableImage;

	//switch current button to us
	g_pCurrentAmmoButton = this;

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

	//we use checks instead of straight assignment to clamp out-of-range values
	if (pKit->m_bAllowBuckshot) {
		if (pClass->GetDefaultAmmo() == AMMO_KIT_BUCKSHOT) {
			pButtonToSelect = g_ppAmmoButtons[AMMO_KIT_BUCKSHOT];
		}
	}
	else {
		//only one ammo type, hide icons
		g_ppAmmoButtons[AMMO_KIT_BUCKSHOT]->SetVisible(false);
		g_ppAmmoButtons[AMMO_KIT_BALL]->SetVisible(false);
	}

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
	if (idealUniform >= pClass->m_iNumUniforms)
		idealUniform = pClass->m_iNumUniforms;
	if (idealUniform < 0)
		idealUniform = 0;

	//set selected button
	g_ppUniformButtons[idealUniform]->SetAsCurrent();

	//hide/show buttons based on uniform count
	char buffer[32];
	for (int i = 0; i < NUM_UNIFORM_BUTTONS; i++) {
		bool bVisible = i < pClass->m_iNumUniforms;
		CUniformButton* bt = g_ppUniformButtons[i];
		bt->SetVisible(bVisible);
		if (bVisible) {
			//update our image here
			Q_snprintf(buffer, sizeof(buffer), "classmenu/uniforms/%c/%s%i", charForTeam(pClass->m_iDefaultTeam), pClass->m_pszAbbreviation, bt->GetUniformIndex());
			bt->m_pCurrentImage = scheme()->GetImage(buffer, false);
		}
	}

	//don't show 1 button if there's only 1 uniform
	g_ppUniformButtons[0]->SetVisible(pClass->m_iNumUniforms > 1);
}


//-----------------------------------------------------------------------------
// Purpose: Go button sends us into the game.
//-----------------------------------------------------------------------------
void CGoButton::ApplySchemeSettings(IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);

	SetPaintBorderEnabled(false);
	SetPaintBackgroundEnabled(false);
	//SetPaintEnabled(false);
	SetVisible(true);
}

void CGoButton::OnMousePressed(MouseCode code) {
	Msg("Go button pressed!\n");
	g_pSelectedClassButton->SimulateMousePressed();
}

void CGoButton::ManualPaint() {
	int x, y, w, h;
	GetPos(x, y);
	GetSize(w, h);

	m_pCurrentImage->SetPos(x, y);
	m_pCurrentImage->SetSize(w, h);

	m_pCurrentImage->Paint();
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

	//Construct class buttons
	g_ppClassButtons = new CClassButton*[NUM_CLASS_BUTTONS];
	for (int i = 0; i < NUM_CLASS_BUTTONS; i++) {
		g_ppClassButtons[i] = new CClassButton(this, "ClassButton", "", i);
	}

	//Construct weapon button
	g_pWeaponButton = new CWeaponButton(this, "WeaponButton", "");

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

	g_pGoButton = new CGoButton(this, "GoButton", "");
	g_pGoButtonLabel = new CGoButton(this, "GoButtonLabel", "#BG3_Classmenu_go"); //the label is also a CGoButton, otherwise nothing happens when clicked

	//Construct labels
	g_pClassNameLabel = new Label(this, "ClassNameLabel", "");
	g_pWeaponNameLabel = new Label(this, "WeaponNameLabel", "");
	g_pClassDescLabel = new Label(this, "ClassDescLabel", "");
	g_pWeaponDescLabel = new Label(this, "ClassDescLabel", "");



	//Be 100% sure that the runtime data we need from the classes has been initialized
	//CPlayerClass::InitClientRunTimeData();


	//find images
	NClassWeaponStats::g_imgStatBar = scheme()->GetImage(MENU_STATBAR, false);
	NClassWeaponStats::g_imgStatBarOverlay = scheme()->GetImage(MENU_STATBARMETER, false);

	//Find selection and darken images
	g_pSelectionIcon = scheme()->GetImage(MENU_SELECTION_ICON, false);
	g_pDarkIcon = scheme()->GetImage(MENU_DARK_ICON, false);
	g_pNoneIcon = scheme()->GetImage(MENU_NONE_ICON, false);
	g_pGoButton->m_pCurrentImage = scheme()->GetImage(MENU_GO_ICON, false);

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
		g_ppClassButtons[i]->UpdateGamerulesData();
}

void CClassMenu::ShowPanel(bool bShow) {
	if (engine->IsPlayingDemo()) //Don't show up in demos -HairyPotter
		return;

	if (BaseClass::IsVisible() == bShow)
		return;

	if (bShow) {
		g_pTeamMenu->ShowPanel(false);
		CPlayerClass::InitClientRunTimeData();
		HideCommMenu();
		Activate();
		PerformLayout();

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


	/****************************************************
	* Begin manual painting!
	* bcs for some reason the images don't paint in the
	* built-in painting scheme, but they do here
	*****************************************************/
	int i;
	for (i = 0; i < NUM_CLASS_BUTTONS; i++)
		g_ppClassButtons[i]->ManualPaint();
	for (i = 0; i < NUM_UNIFORM_BUTTONS; i++)
		if (g_ppUniformButtons[i]->IsVisible())
			g_ppUniformButtons[i]->ManualPaint();
	for (i = 0; i < NUM_AMMO_BUTTONS; i++)
		if (g_ppAmmoButtons[i]->IsVisible())
			g_ppAmmoButtons[i]->ManualPaint();
	g_pWeaponButton->ManualPaint();

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

	g_pGoButton->ManualPaint();

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
	int index = code - KEY_1; //so that KEY_1 has index 0
	if (index >= 0 && index < NUM_CLASS_BUTTONS)
		g_ppClassButtons[index]->SimulateMousePressed();

	if (code == KEY_N)
		engine->ClientCmd("teammenu");
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
	
	g_pGoButton->MakeReadyForUse();
	g_pGoButtonLabel->SetText("#BG3_Classmenu_go");
	g_pGoButtonLabel->MakeReadyForUse();

	//Set fonts of the two titles!
	SetDefaultBG3FontScaled(pScheme, g_pClassNameLabel);
	SetDefaultBG3FontScaled(pScheme, g_pWeaponNameLabel);
	SetDefaultBG3FontScaled(pScheme, g_pGoButtonLabel);

#define set(a) a->SetFgColor(g_cBG3TextColor)
	set(g_pClassNameLabel);
	set(g_pWeaponNameLabel);
	set(g_pClassDescLabel);
	set(g_pWeaponDescLabel);
#undef set
}

void CClassMenu::PerformLayout() {
	BaseClass::PerformLayout();

	//Find default background image
	m_pBackground = scheme()->GetImage(MENU_BACKGROUND_A, false);

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
	int uniformh = iconsz * 2 / 3;
	for (i = 0; i < NUM_UNIFORM_BUTTONS; i++) {
		int x = xBase + i*iconsz;
		g_ppUniformButtons[i]->SetSize(iconsz, uniformh);
		g_ppUniformButtons[i]->SetPos(x, uniformy);
	}

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
	g_pGoButton->SetPos(gobuttonx, 0);
	g_pGoButton->SetSize(gobuttonw, gobuttonh);

	g_pGoButtonLabel->SetPos(gobuttonx + 18 * scale, 0);
	g_pGoButtonLabel->SetSize(gobuttonw, gobuttonh);


	//----------------------------------------------------------
	//GLOBAL TEXTS
	//----------------------------------------------------------
	int yInset = 150;
	int namey = yBase - yInset;
	int nameh = 65;
	g_pClassNameLabel->SetPos(xBase, namey);
	g_pClassNameLabel->SetSize(classw + iconsz, nameh);

	g_pWeaponNameLabel->SetPos(weaponx, namey);
	g_pWeaponNameLabel->SetSize(weaponw + iconsz, nameh);

	int desch = yInset - nameh;
	int descy = namey + nameh;
	g_pClassDescLabel->SetPos(xBase, descy);
	g_pClassDescLabel->SetSize(classw + iconsz, desch);

	g_pWeaponDescLabel->SetPos(weaponx, descy);
	g_pWeaponDescLabel->SetSize(weaponw, desch);
}

void CClassMenu::UpdateToMatchTeam(int iTeam) {
	g_iCurrentTeammenuTeam = iTeam; //just in case

	//don't bother updating if we're already showing this team
	if (m_iPreviouslyShownTeam == iTeam)
		return;

	//update class buttons
	for (int i = 0; i < NUM_CLASS_BUTTONS; i++) {
		const CPlayerClass* pClass = CPlayerClass::fromNums(iTeam, i);
		g_ppClassButtons[i]->UpdateToMatchClass(pClass);
	}

	//find a default button to select
	g_pSelectedClassButton = nullptr;
	CClassButton::FindAvailableButton()->Select();

	//update background image
	const char* pszImage = iTeam == TEAM_AMERICANS ? MENU_BACKGROUND_A : MENU_BACKGROUND_B;
	SetBackground(pszImage);
}

void CClassMenu::SetBackground(const char* pszImage) {
	m_pBackground = scheme()->GetImage(pszImage, false);
	m_pBackground->SetSize(ScreenWidth(), ScreenHeight());
	m_pBackground->SetPos(0, 0);

}

CClassMenu* g_pClassMenu = nullptr;

CON_COMMAND(classinfo, "") {
	CPlayerClass::InitClientRunTimeData();
	char buffer[256];

	//const CPlayerClass* const* lst = CPlayerClass::asList();
	//for (int i = 0; i < CPlayerClass::numClasses(); i++) {
		//const CPlayerClass* pClass = lst[i];
		
	wcstombs(buffer, g_pVGuiLocalize->Find("#BG3_ainf_desc"), sizeof(buffer));

		Msg("%s\n", buffer);



		Msg("%i", g_pVGuiLocalize->Find("#BG3_ainf_desc") == PlayerClasses::g_pAInfantry->m_pLocalizedDesc);
	//}
}

CON_COMMAND(classpics, "") {
	for (int i = 0; i < NUM_CLASS_BUTTONS; i++) {
		const CClassButton& cb = *g_ppClassButtons[i];
		Msg("%u\n", cb.m_pCurrentImage->GetID());
	}
}

CON_COMMAND(toggleteam, "") {
	g_iCurrentTeammenuTeam = g_iCurrentTeammenuTeam == TEAM_AMERICANS ? TEAM_BRITISH : TEAM_AMERICANS;
}
#endif