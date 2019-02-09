//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef COMMMENU2_H
#define COMMMENU2_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui/IScheme.h>
#include <vgui/KeyCode.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ComboBox.h>

#include <game/client/iviewport.h>
#include "commmenu.h"

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

//-----------------------------------------------------------------------------
// Purpose: the bottom bar panel, this is a seperate panel because it
// wants mouse input and the main window doesn't
//----------------------------------------------------------------------------
class CCommMenu2 : public CCommBase
{
	DECLARE_CLASS_SIMPLE( CCommMenu2, CCommBase );

public:
	CCommMenu2( IViewPort *pViewPort );
	~CCommMenu2() {}

	virtual const char *GetName( void ) { return PANEL_COMM2; }
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

	void PlayVcommBySlot(int iSlot0);
private:
	// VGUI2 overrides
	virtual void OnKeyCodePressed(vgui::KeyCode code);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme){ BaseClass::ApplySchemeSettings(pScheme); }
	virtual void PerformLayout() {}

	void SetViewModeText( const char *text ) { }
	void SetPlayerFgColor( Color c1 ) { }

	vgui::Label	*m_pLabel;
};

extern CCommMenu2* g_pCommMenu2;

#endif // COMMMENU2_H
