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
class CAdminMenu : public vgui::Frame, public IViewPortPanel
{
	DECLARE_CLASS_SIMPLE(CAdminMenu, vgui::Frame);

public:
	CAdminMenu(IViewPort *pViewPort);
	~CAdminMenu() {}

	virtual const char *GetName(void) { return PANEL_ADMIN; }

	IViewPort *m_pViewPort;

	int	slot1,
		slot2,
		slot3,
		slot4,
		slot5,
		slot6,
		slot7,
		slot8,
		slot9,
		slot10,

		teammenu,
		classmenu,
		commmenu,
		commmenu2,
		weaponmenu;

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void OnKeyCodePressed(vgui::KeyCode code);
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	vgui::VPANEL GetVPanel(void) { return BaseClass::GetVPanel(); }
	virtual void SetParent(vgui::VPANEL parent) { BaseClass::SetParent(parent); }
	virtual void Update(void);
	virtual void ShowPanel(bool bShow);
	virtual void SetData(KeyValues *data) {};
	virtual void Reset(void) { }
	virtual bool NeedsUpdate(void) { return false; }
	virtual bool HasInputElements(void) { return false; }


	void SetViewModeText(const char *text) { }
	void SetPlayerFgColor(Color c1) { }
	virtual void PerformLayout() {}

	vgui::Label	*m_pLabel;

	//And here is the actual important BG3/Admin menu stuff
	CAdminSubMenu* m_pCurrentMenu;
	void forwardKeyToSubmenu(uint8 slot);
	void updateLabelToMatchCurrentMenu();
};

extern CAdminMenu* g_pAdminMenu;

#endif // ADMINMENU_H
