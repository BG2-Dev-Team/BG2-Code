/*
	The Battle Grounds 2 - A Source modification
	Copyright (C) 2005, The Battle Grounds 2 Team and Contributors

	The Battle Grounds 2 free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The Battle Grounds 2 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

	Contact information:
		Tomas "Tjoppen" Härdin		tjoppen@gamedev.se

	You may also contact the (future) team via the Battle Grounds website and/or forum at:
		www.bgmod.com

	 Note that because of the sheer volume of files in the Source SDK this
	notice cannot be put in all of them, but merely the ones that have any
	changes from the original SDK.
	 In order to facilitate easy searching, all changes are and must be
	commented on the following form:

	//BG2 - <name of contributer>[ - <small description>]
*/
 
#ifdef _WIN32
#pragma once
#endif
 
#include "cbase.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "c_playerresource.h"
#include "clientmode_hl2mpnormal.h"
#include <vgui_controls/Controls.h>
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <KeyValues.h>
#include "c_baseplayer.h"
#include "c_hl2mp_player.h"
#include "bg2_hud_pickup.h"


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


//using namespace vgui;
using namespace vgui;

DECLARE_HUDELEMENT( CHudPickup );
DECLARE_HUD_MESSAGE( CHudPickup, WeaponPickup );
DECLARE_HUD_MESSAGE( CHudPickup, PickupDelay );

CHudPickup *CHudPickup::s_pInstance = NULL;
CHudPickup *CHudPickup::GetInstance()
{
	return CHudPickup::s_pInstance;
}

//==============================================
// Constructor
//==============================================
CHudPickup::CHudPickup( const char *pElementName ) :
	CHudElement( pElementName ), BaseClass( NULL, "HudPickup" )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
	SetAlpha( 255 );

	SetPaintBackgroundEnabled( false );

	SetHiddenBits( HIDEHUD_PLAYERDEAD );
}

void CHudPickup::ApplySchemeSettings( IScheme *scheme )
{
	//BaseClass::ApplySchemeSettings( scheme );
	//SetPaintBackgroundEnabled( false );
}

void CHudPickup::Init()
{
	HOOK_HUD_MESSAGE( CHudPickup, WeaponPickup );
	HOOK_HUD_MESSAGE( CHudPickup, PickupDelay );
}
void CHudPickup::VidInit()
{
	m_PickupIcon = gHUD.GetIcon( "death_brownbess_nobayo" );
	m_PickupProgress = gHUD.GetIcon( "death_brownbess_nobayo" );
}
//-----------------------------------------------------------
// Purpose: If pPlayer (me) is on team Combine, display Iraq icon
// Purpose+: else if i'm on team Rebels display British icon.
//-----------------------------------------------------------
void CHudPickup::Paint()
{  
	Color iconColor(125,125,125,125);

	int iconWide = 0;
	int iconTall = 0;
	int xOffset = 0;
	int yOffset = 0;

	if ( m_fPickEnd > gpGlobals->curtime && m_PickupProgress )
	{
		iconWide = (int)m_PickupProgress->Width();
		iconTall = (int)m_PickupProgress->Height();
		xOffset = (ScreenWidth()-iconWide)/2;
		yOffset = (ScreenHeight()*5)/16;

		int crop;

		crop = iconWide * (int)(3.0f - ( gpGlobals->curtime - m_fPickStart )) / 3;

		m_PickupProgress->DrawSelfCropped( xOffset, yOffset, 0, 0, iconWide - crop, iconTall, iconColor);
	}
	else if ( m_fTouchExpire > gpGlobals->curtime && m_PickupIcon )
	{
		iconWide = (int)m_PickupIcon->Width();
		iconTall = (int)m_PickupIcon->Height();
		xOffset = (ScreenWidth()-iconWide)/2;
		yOffset = (ScreenHeight()*5)/16;

		m_PickupIcon->DrawSelf( xOffset, yOffset, iconWide, iconTall, iconColor );;
	}

	
    
      SetPaintBorderEnabled(false);
	  BaseClass::PaintBackground();
}
void CHudPickup::MsgFunc_PickupDelay( bf_read &msg )
{
	m_fPickStart = gpGlobals->curtime;
	m_fPickEnd = gpGlobals->curtime + 3.0f;
}
void CHudPickup::MsgFunc_WeaponPickup( bf_read &msg )
{
	msg.ReadOneBit(); //BG2 - Awesome - changed this from "bool loaded = msg.ReadOneBit();" to just "msg.ReadOneBit();"
	char weapon[128];
	char texture[128];

	msg.ReadString( weapon, sizeof(weapon) );	

	Q_snprintf( texture, sizeof(texture), "death_%s", weapon );

	m_PickupIcon = gHUD.GetIcon( texture );
	m_fTouchExpire = gpGlobals->curtime + 0.2f;
}
