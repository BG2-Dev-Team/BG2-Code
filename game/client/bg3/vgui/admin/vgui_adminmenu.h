//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef ADMINMENU_H
#define ADMINMENU_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui/IScheme.h>
#include <vgui/KeyCode.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ComboBox.h>

#include <game/client/iviewport.h>
#include <../../shared/bg3/bg3_player_shared.h>
#include "bg3_admin_submenu.h"
#include <vector>

class KeyValues;

namespace vgui
{
	class TextEntry;
	class Button;
	class Panel;
	class ImagePanel;
	class ComboBox;
}

class IBaseFileSystem;

//------------------------------------------------------------------------------------------------------------------------
// Begin comm menu base code.
//------------------------------------------------------------------------------------------------------------------------
class CAdminMenu : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CAdminMenu, vgui::Panel);

public:
	CAdminMenu(vgui::Panel* parent);
	~CAdminMenu() {}

	virtual const char *GetName(void) { return PANEL_ADMIN; }

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme) override;
	virtual bool IsVisible() override { return BaseClass::IsVisible(); }
	vgui::VPANEL GetVPanel(void) override { return BaseClass::GetVPanel(); }
	virtual void SetParent(vgui::VPANEL parent) override { BaseClass::SetParent(parent); }

	vgui::Label*	m_pTitleLabel;
	vgui::Label*	m_pLabels[NUM_ADMIN_MENU_LABELS];
	void			SetFontsAllLabels(vgui::HFont font, vgui::HFont titleFont);

	//And here is the actual important BG3/Admin menu stuff
	CAdminSubMenu*					m_pCurrentMenu;
	std::vector<CAdminSubMenu*>		m_menuStack; //so we can go back with the back button
	void ResetToMainMenu();
	void ForwardKeyToSubmenu(uint8 slot); //handles input. 0 = slot1, 1 = slot2, etc. 0 = slot 10
	void UpdateLabelsToMatchCurrentMenu();
	void SetVisible(bool bVisible) override;
};

extern CAdminMenu* g_pAdminMenu;

#endif // ADMINMENU_H
