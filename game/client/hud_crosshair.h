//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef HUD_CROSSHAIR_H
#define HUD_CROSSHAIR_H
#ifdef _WIN32
#pragma once
#endif

#include "hudelement.h"
#include <vgui_controls/Panel.h>

class C_BaseBG2Weapon;

namespace vgui
{
	class IScheme;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------

class CHudCrosshair : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHudCrosshair, vgui::Panel );
public:
	CHudCrosshair( const char *pElementName );
	virtual ~CHudCrosshair();

	virtual void	SetCrosshairAngle( const QAngle& angle );
	virtual void	SetCrosshair( CHudTexture *texture, const Color& clr );
	static CHudCrosshair* GetCrosshair() { return g_pCrosshair; }
	virtual void	ResetCrosshair();
	virtual void	DrawCrosshair( void ) {}
  	virtual bool	HasCrosshair( void ) { return ( m_pCrosshair != NULL ); }
	virtual bool	ShouldDraw();

	void			Reset() override {
		m_flMeleeScanEndTime = -FLT_MAX;
		m_flMeleeScanStartTime = -FLT_MAX;
	}

	// any UI element that wants to be at the aim point can use this to figure out where to draw
	static	void	GetDrawPosition ( float *pX, float *pY, bool *pbBehindCamera, QAngle angleCrosshairOffset = vec3_angle );

	//This is called when a melee attack starts on the client side
	void			RegisterMeleeSwing(C_BaseBG2Weapon* pWeapon, int iAttack);

	void			SetCrosshairTextureIndex(int i);

protected:
	static CHudCrosshair* g_pCrosshair;

	virtual void	ApplySchemeSettings( vgui::IScheme *scheme );
	virtual void	IronPaint();//Bg2 - Commented -Hairypotter
	virtual void	Paint();
	
	// Crosshair sprite and colors
	CHudTexture		*m_pCrosshair;
	int				m_iCrosshairTextureIndex;
	CHudTexture		*m_pDefaultCrosshair;
	Color			m_clrCrosshair;
	QAngle			m_vecCrossHairOffsetAngle;

	float			m_flMeleeScanStartTime;
	float			m_flMeleeScanEndTime;

	CPanelAnimationVar( bool, m_bHideCrosshair, "never_draw", "false" );

};


// Enable/disable crosshair rendering.
extern ConVar crosshair;


#endif // HUD_CROSSHAIR_H
