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

#ifndef BG3_UNLOCKABLE_MENU_H
#define BG3_UNLOCKABLE_MENU_H

#include "cbase.h"
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

#include "hl2mp_gamerules.h"

#include "../shared/bg3/bg3_unlockable.h"
#include "bg3/vgui/fancy_button.h"


//Shared icon images
extern vgui::IImage* g_pUnlockabledLockedIcon;
extern vgui::IImage* g_pUnlockabledEnabledIcon;
extern vgui::IImage* g_pUnlockabledDisabledIcon;

//-----------------------------------------------------------------------------
// Button models a given unlockable
//-----------------------------------------------------------------------------
class CUnlockableMenu;
class CUnlockableButton : public vgui::Button {
public:
	CUnlockableButton(vgui::Panel* pParent, const char* buttonName, UnlockableBit ulk);

	void OnCursorEntered() override;
	void OnCursorExited() override;
	void OnMousePressed(vgui::MouseCode code) override;
	void ManualPaint();

	void UpdateUnlockableState();

	Unlockable* GetUnlockable() { return m_pUnlockable; }

private:
	//Our unlockable profile
	UnlockableProfile* m_pProfile;
	Unlockable* m_pUnlockable;

	bool m_bUnlocked;
	bool m_bEnabled;
	bool m_bMouseOver;

	vgui::IImage* m_pUnlockableImage;
};

extern CUnlockableButton* g_pHoveredUnlockable;
extern CUnlockableButton* g_pSelectedUnlockable;

//-----------------------------------------------------------------------------
// Purpose: Unlockable menu displays progression information and unlocked item buttons
//-----------------------------------------------------------------------------
extern CUnlockableMenu* g_pUnlockableMenu;
class CUnlockableMenu : public vgui::Panel {
public:
	DECLARE_CLASS_SIMPLE(CUnlockableMenu, vgui::Panel);

	CUnlockableMenu(vgui::Panel *parent, const char *panelName);

	virtual void	ApplySchemeSettings(vgui::IScheme *pScheme) override;
	void			Paint() override;
	void			PerformLayout() override;

	//Mouse hovered over child button -- update menu texts
	void			UnlockableButtonFocus(CUnlockableButton* pButton);
	void			UnlockButtonPressed();
	void			UpdateToMatchProfile();
	void			SetVisible(bool bVisible) override;
private:
	//Array of pointers to our unlockable buttons
	CUnlockableButton* m_aButtons[NUM_UNLOCKABLES];

	//Labels
	vgui::Label* m_pUnlockableTitle;
	vgui::Label* m_pUnlockableDesc;
	vgui::Label* m_pPointsRemainingCount;
	vgui::Label* m_pPointsRemainingText;

	//Our unlockable profile
	UnlockableProfile* m_pProfile;

	// 'Unlock' button will spend points to unlock an item
	//FancyButton* m_pUnlockButton;

	//Assets
	vgui::IImage* m_pBackground;
};

#endif //BG3_UNLOCKABLE_MENU_h