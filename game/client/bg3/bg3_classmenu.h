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

#ifndef BG3_CLASSMENU_H
#define BG3_CLASSMENU_H

#include <vgui/IScheme.h>
#include <vgui/KeyCode.h>
#include <vgui/MouseCode.h>
#include <vgui_controls/HTML.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/ToggleButton.h>
#include <vgui_controls/CheckButton.h>
#include "vgui_bitmapimage.h"
#include "vgui_bitmapbutton.h"
#include <vgui_controls/ImagePanel.h>
#include <imagemouseoverbutton.h>
#include "bg3/vgui//bg3_button_close.h"

#include <game/client/iviewport.h>

#include "hl2mp_gamerules.h" // BG2 - VisualMelon - moved from classmenu.cpp so we can get the ammo counts

class CClassButton;
class CWeaponButton;
class CAmmoButton;
class CUniformButton;
class CTeamToggleButton;
class GlowyArrowForPlayersWhoDontRealizeYouCanSwitchWeapons;

extern CClassButton*	g_pDisplayedClassButton;	//which class is currently displayed?
extern CClassButton*	g_pSelectedClassButton;		//what was the last class clicked on?
extern CWeaponButton*	g_pWeaponButton;		//we only have one weapon button, which cycles through the possible weapons
extern CAmmoButton*		g_pCurrentAmmoButton;
extern CUniformButton*	g_pCurrentUniformButton;

extern vgui::IImage*			g_pSelectionIcon; //these are overlays
extern vgui::IImage*			g_pDarkIcon;
extern vgui::IImage*			g_pNoneIcon;

//image file paths
#define MENU_GO_ICON			"classmenu/go"
#define MENU_SELECTION_ICON		"classmenu/select"
#define MENU_HOVER_ICON			"classmenu/hover"
#define MENU_DARK_ICON			"classmenu/darken"
#define MENU_NONE_ICON			"classmenu/none"
#define MENU_STATBAR			"classmenu/statbar"
#define MENU_STATBARMETER		"classmenu/statbarmeter"
#define MENU_BACKGROUND_A		"classmenu/background_a"
#define MENU_BACKGROUND_B		"classmenu/background_b"	

#define MENU_AMMO_BALL			"classmenu/ammo/ball"
#define MENU_AMMO_BUCKSHOT		"classmenu/ammo/buckshot"

#define MENU_ARROW_NORMAL		"classmenu/weapons/arrow_normal"
#define MENU_ARROW_HOVER		"classmenu/weapons/arrow_hover"

#define MENU_TEAM_SWITCH_ICON	"classmenu/team_switch"
#define MENU_CLOSE_ICON			"classmenu/close"

//-----------------------------------------------------------------------------
// Purpose: These buttons store and link all of the class-related menu data,
//			as well as enable switching between classes
//-----------------------------------------------------------------------------
class CClassButton : public vgui::Button {
public:
	DECLARE_CLASS_SIMPLE(CClassButton, vgui::Button);

	CClassButton(Panel *parent, const char *panelName, const char *text, int index) : Button(parent, panelName, text), m_iIndex(index) { }
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

	inline void SimulateMousePressed() { OnMousePressed(MOUSE_LEFT); }
	void OnMousePressed(vgui::MouseCode code);
	void OnCursorEntered(void);
	void OnCursorExited(void);

	void	ManualPaint(); //we'll paint directly from class menu to ensure proper layering
	//const wchar* GetPlayerClassName(void); //gets name of class from localization

	static CClassButton* FindAvailableButton();
	void			UpdateGamerulesData(); //looks at gamerules to decide if we're selectable or not, etc.
	void			UpdateImage();
	static void		UpdateDisplayedButton(CClassButton* pNext);
	void			Select();
	inline bool		IsSelected()	const { return g_pSelectedClassButton == this; }
	inline bool		IsAvailable()	{ return m_eAvailable != CLASS_FULL && m_eAvailable != CLASS_UNAVAILABLE && IsVisible(); }
	int				GetIndex()		const { return m_iIndex; }

	void	UpdateToMatchClass(const CPlayerClass* pClass);
	const CPlayerClass* m_pPlayerClass; //what class are we representing?
	

	static int s_iSize;
private:
	bool m_bMouseOver = false;

	int		m_iIndex;
	float	m_flLastClickTime = -FLT_MAX;
	vgui::IImage* m_pCurrentImage;
	vgui::IImage* m_pClassArt;

	//vgui::IImage* m_pSelectedImage;		//currently selected
	vgui::IImage* m_pAvailableImage;	//selectable, free
	//vgui::IImage* m_pFullImage;		//currently taken, not free
	//vgui::IImage* m_pNoneImage;			//limit is 0, not allowed for this map/gamemode

	EClassAvailability	m_eAvailable;
};
inline const CPlayerClass* GetDisplayedClass() { return g_pDisplayedClassButton->m_pPlayerClass; }
void SimulateButtonPress(vgui::Button* pButton);
void JoinRequest(int iGun, int iAmmo, int iSkin, int iTeam, int iClass);

//-----------------------------------------------------------------------------
// Purpose: This single weapon button displays the currently selected weapon
//			for the class. Pressing it cycles through the possible weapons.
//-----------------------------------------------------------------------------
class CWeaponButton : public vgui::Button {
public:
	DECLARE_CLASS_SIMPLE(CWeaponButton, Button);

	CWeaponButton(Panel *parent, const char *panelName, const char *text) : Button(parent, panelName, text) { }
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

	inline void SimulateMousePressed() { OnMousePressed(MOUSE_LEFT); }
	void OnMousePressed(vgui::MouseCode code);
	void OnCursorEntered(void);
	void OnCursorExited(void);

	void	ManualPaint();
	void UpdateToMatchPlayerClass(const CPlayerClass* pClass);
	void SetCurrentGunKit(int iKit);
	uint8  GetCurrentGunKit() const { return m_iCurrentWeapon; }
	uint8  GetNumWeapons() const { return m_iWeaponCount; }

private:
	bool m_bMouseOver = false;
	uint8 m_iWeaponCount; //how many weapons do we cycle through?
	uint8 m_iCurrentWeapon; //which weapon are we currently displaying?
	vgui::IImage*			m_pCurrentImage;

	void			UpdateImage(const CGunKit* pKit);
};

//-----------------------------------------------------------------------------
// Purpose: Arrow that glows and pulses to let players know they can switch
// weapon kits
//-----------------------------------------------------------------------------
class GlowyArrowForPlayersWhoDontRealizeYouCanSwitchWeapons {
public:

	GlowyArrowForPlayersWhoDontRealizeYouCanSwitchWeapons();

			void PaintToScreen();
	inline	void SetPosition(uint16 x, uint16 y)	{ m_iX = x; m_iY = y; }
	inline	void SetSize(uint16 wh)					{ m_iWH = wh; }
	inline  void SetPulseSize(uint8 sz)				{ m_iPulseSize = sz; }
	inline	void SetVisible(bool bVisible)			{ m_bVisible = bVisible; }
	inline  bool IsVisible() const					{ return m_bVisible; }
	inline	void SetImage(const char* pszImgName);

private:
	vgui::IImage* m_pImage;
	uint16	m_iX;
	uint16	m_iY;
	uint16	m_iWH;
	uint8	m_iPulseSize;
	bool	m_bVisible;
};

//-----------------------------------------------------------------------------
// Purpose: Displays Stats for the currently selected class, weapon, and ammo
//-----------------------------------------------------------------------------
namespace NClassWeaponStats {
	
	void UpdateToMatchClass(const CPlayerClass* pClass);
	void UpdateWeaponStatsAllCurrent();
	void UpdateToMatchWeapon(const CWeaponDef* pWeapon);

	void Create(vgui::Panel* pParent);
	void CreateLayout();
	void SetVisible(bool bVisible);

	void ManualPaint(); //this must be called manually because we're not actually an object or anything
					//paints within the context of the calling vgui::Panel
	void Localize();

#define dec(a) extern vgui::Label* a
	//Text labels for class stats
	dec(g_pClassSpeed);
	//dec(g_pClassNotes);	//comments on strengths/weaknesses/abilities of class

	//Text labels for names weapon stats
	dec(g_pAccuracy);
	dec(g_pBulletDamage);
	dec(g_pReloadSpeed);
	dec(g_pMeleeRange);
	dec(g_pMeleeDamage);
	dec(g_pMeleeTolerance); //COS Tolerance + retrace duration
	dec(g_pMeleeSpeed);

	//number labels for weapon stats
	dec(g_pnClassSpeed);
	dec(g_pnBulletDamage);
	dec(g_pnReloadSpeed);
	dec(g_pnMeleeRange);
	dec(g_pnMeleeDamage);
	dec(g_pnMeleeSpeed);

#undef dec
}

//-----------------------------------------------------------------------------
// Purpose: Allows for ammo selection
//-----------------------------------------------------------------------------
class CAmmoButton : public vgui::Button {
public:
	DECLARE_CLASS_SIMPLE(CAmmoButton, Button);

	CAmmoButton(Panel *parent, const char *panelName, const char *text, int index);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

	inline void SimulateMousePressed() { OnMousePressed(MOUSE_LEFT); }
	void OnMousePressed(vgui::MouseCode code);
	void OnCursorEntered(void);
	void OnCursorExited(void);

	void	ManualPaint(); //we'll paint directly from class menu
	void	SetAsCurrent(); //selects button, no sound
	static void	UpdateToMatchClassAndWeapon(const CPlayerClass* pClass, const CGunKit* pKit);
	int		GetAmmoIndex() const { return m_iIndex; } //gets this ammo's number

private:
	bool m_bMouseOver = false;
	static const CGunKit* s_pGunKit;
	int m_iIndex;
	vgui::IImage* m_pCurrentImage;
};

//-----------------------------------------------------------------------------
// Purpose: Allows for uniform selection
//-----------------------------------------------------------------------------
class CUniformButton : public vgui::Button {
public:
	DECLARE_CLASS_SIMPLE(CUniformButton, Button);

	CUniformButton(Panel *parent, const char *panelName, const char *text, int index) : Button(parent, panelName, text), m_iIndex(index) { }
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

	inline void SimulateMousePressed() { OnMousePressed(MOUSE_LEFT); }
	void OnMousePressed(vgui::MouseCode code);
	void OnCursorEntered(void);
	//void OnCursorExited(void);

	void	ManualPaint(); //we'll paint directly from class menu

	void	SetAsCurrent();
	static void	UpdateToMatchClass(const CPlayerClass* pClass);
	int		GetUniformIndex() const { return m_iIndex; } //gets this ammo's number

private:
	int m_iIndex;
	vgui::IImage* m_pCurrentImage;
};

//-----------------------------------------------------------------------------
// Purpose: Switches classmenu between teams without having to enter Teammenu
//-----------------------------------------------------------------------------
class CTeamToggleButton : public vgui::Button {
public:
	CTeamToggleButton(const char* pszImgName);

	void OnMousePressed(vgui::MouseCode code) override;
	void Paint() override;
	void OnCursorEntered() override;
	void OnCursorExited() override { m_bMouseOver = false; }
private:
	vgui::IImage* m_pImage;
	bool m_bMouseOver;
};

//-----------------------------------------------------------------------------
// Purpose: Wraps everything into a handy menu
//-----------------------------------------------------------------------------
class CClassMenu : public vgui::Frame, public IViewPortPanel
{
	friend class CTeamToggleButton;
	DECLARE_CLASS_SIMPLE(CClassMenu, vgui::Frame);

public:
	CClassMenu(vgui::VPANEL pParent);
	~CClassMenu();

	virtual const char *GetName(void) { return PANEL_CLASSES; }
	virtual void SetData(KeyValues *data){}
	virtual void Reset(void) { }//m_pPlayerList->DeleteAllItems(); }
	virtual void Update(void);
	virtual bool NeedsUpdate(void) { return m_flNextGameRulesUpdate < gpGlobals->curtime; }
	virtual bool HasInputElements(void) { return true; }
	virtual void ShowPanel(bool bShow) override;
	virtual void Paint(void);


	// both vgui::Frame and IViewPortPanel define these, so explicitly define them here as passthroughs to vgui
	vgui::VPANEL GetVPanel(void) { return BaseClass::GetVPanel(); }
	virtual void SetParent(vgui::VPANEL parent) { BaseClass::SetParent(parent); }
	void OnThink();

	//these disambiguate our multiple inheritance
	void SetVisible(bool bVisible) override;
	bool IsVisible() override { return BaseClass::IsVisible(); }

private:
	// VGUI2 overrides
	virtual void	OnKeyCodePressed(vgui::KeyCode code);
	virtual void	OnKeyCodeTyped(vgui::KeyCode code); //for ESCAPE key
	virtual void	ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void	PerformLayout();
	void			UpdateToMatchTeam(int iTeam);
	void			SetBackground(const char* pszBackground);

	//used so that all the buttons don't have to be updated if they don't have to be
public:
	int m_iPreviouslyShownTeam = TEAM_INVALID;
private:

	float				m_flNextGameRulesUpdate = 0.0f;

	vgui::IImage*		m_pBackground;

	vgui::CCloseButton*	m_pCloseButton;
	CTeamToggleButton*	m_pTeamToggleButton;
};

extern CClassMenu* g_pClassMenu;

inline void HideCommMenu() {
	gViewPortInterface->ShowPanel(PANEL_COMM, false);
	gViewPortInterface->ShowPanel(PANEL_COMM2, false);
}
#endif //BG3_CLASSMENU_H