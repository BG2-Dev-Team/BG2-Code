//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef COMMMENU_H
#define COMMMENU_H
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
class CCommBase : public vgui::Frame, public IViewPortPanel
{
	DECLARE_CLASS_SIMPLE( CCommBase, vgui::Frame );

public:
	CCommBase();
	~CCommBase() {}

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
		admin;

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void OnKeyCodePressed(vgui::KeyCode code);
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	vgui::VPANEL GetVPanel( void ) { return BaseClass::GetVPanel(); }
	virtual void SetParent(vgui::VPANEL parent) { BaseClass::SetParent(parent); }
	virtual void Update( void );
	virtual void ShowPanel( bool bShow );
};

//-----------------------------------------------------------------------------------------------------------------------
// Begin first comm menu code.
//-----------------------------------------------------------------------------------------------------------------------
class CCommMenu : public CCommBase
{
	DECLARE_CLASS_SIMPLE( CCommMenu, CCommBase );

public:
	CCommMenu( IViewPort *pViewPort );
	~CCommMenu() {}

	virtual const char *GetName( void ) { return PANEL_COMM; }
	virtual void SetData(KeyValues *data) {};
	virtual void Reset( void ) { }
	virtual void Update( void ) { BaseClass::Update(); }
	virtual bool NeedsUpdate( void ) { return false; }
	virtual bool HasInputElements( void ) { return false; }
	virtual void ShowPanel( bool bShow );

	// both vgui::Frame and IViewPortPanel define these, so explicitly define them here as passthroughs to vgui
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	vgui::VPANEL GetVPanel( void ) { return BaseClass::GetVPanel(); }
	virtual void SetParent(vgui::VPANEL parent) { BaseClass::SetParent(parent); }

	void PlayVcommBySlot(int iSlot);

private:
	// VGUI2 overrides
	virtual void OnKeyCodePressed(vgui::KeyCode code);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme){ BaseClass::ApplySchemeSettings(pScheme); }
	virtual void PerformLayout() {}

	void SetViewModeText( const char *text ) { }
	void SetPlayerFgColor( Color c1 ) { }

	vgui::Label	*m_pLabel;
};

extern CCommMenu* g_pCommMenu;

#endif // COMMMENU_H
