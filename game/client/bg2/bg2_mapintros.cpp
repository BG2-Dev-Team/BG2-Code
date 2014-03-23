//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "hud.h"
#include "iclientmode.h"
#include "view.h"
#include <vgui_controls/Controls.h>
#include "vgui/ISurface.h"
#include "ivrenderview.h"
#include "hudelement.h"
#include <vgui_controls/Panel.h>
#include <vgui/IVGui.h>
#include "choreoscene.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CBG2MapPreview : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CBG2MapPreview, vgui::Panel );
public:
	CBG2MapPreview( const char *pElementName );

	bool			ShouldDraw();

protected:
	virtual void	ApplySchemeSettings( vgui::IScheme *scheme );
	virtual void	Paint();
	virtual void	OnTick( void );

private:
	// Crosshair sprite and colors
	CHudTexture		*m_pCrosshair;
	CHudTexture		*m_pDefaultCrosshair;
	Color			m_clrCrosshair;
	QAngle			m_vecCrossHairOffsetAngle;

	QAngle			m_curViewAngles;
	Vector			m_curViewOrigin;
};

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

int ScreenTransform( const Vector& point, Vector& screen );

DECLARE_HUDELEMENT( CBG2MapPreview );

CBG2MapPreview::CBG2MapPreview( const char *pElementName ) :
  CHudElement( pElementName ), BaseClass( NULL, "HudMenu" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
	
	vgui::ivgui()->AddTickSignal( GetVPanel() );
}

void CBG2MapPreview::ApplySchemeSettings( IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme );

	SetPaintBackgroundEnabled( false );

    SetSize( ScreenWidth(), ScreenHeight() );
}

//-----------------------------------------------------------------------------
// Purpose: Save CPU cycles by letting the HUD system early cull
// costly traversal.  Called per frame, return true if thinking and 
// painting need to occur.
//-----------------------------------------------------------------------------
bool CBG2MapPreview::ShouldDraw( void )
{
	return true;
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBG2MapPreview::OnTick()
{
	if ( engine->IsDrawingLoadingImage()  ) //This should suffice.
	{
		//const char *mapname = engine->GetLevelName();//IGameSystem::MapName();

		//if ( mapname )
		//	Error("Drawing loading screen for %s. This is a good thing. \n", mapname);
	}
}
void CBG2MapPreview::Paint( void )
{
	if ( !IsCurrentViewAccessAllowed() )
		return;

	float x, y;
	x = ScreenWidth();
	y = ScreenHeight();
}