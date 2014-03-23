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
		Tomas "Tjoppen" Härdin		mail, in reverse: se . gamedev @ tjoppen

	You may also contact the (future) team via the Battle Grounds website and/or forum at:
		www.bgmod.com

	 Note that because of the sheer volume of files in the Source SDK this
	notice cannot be put in all of them, but merely the ones that have any
	changes from the original SDK.
	 In order to facilitate easy searching, all changes are and must be
	commented on the following form:

	//BG2 - <name of contributer>[ - <small description>]
*/

//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
//
// Health.cpp
//
// implementation of CHudHealth class
//
#include "cbase.h"
#include "hud.h"
#include "hud_macros.h"
#include "view.h"

#include "iclientmode.h"

#include <KeyValues.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/AnimationController.h>

#include <vgui/ILocalize.h>

using namespace vgui;

#include "hudelement.h"
#include "hud_numericdisplay.h"

#include "convar.h"

//BG2 - Tjoppen - #includes
#include "c_hl2mp_player.h"
//

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define INIT_HEALTH -1

//-----------------------------------------------------------------------------
// Purpose: Health panel
//-----------------------------------------------------------------------------
class CHudStamina : public CHudElement, public CHudNumericDisplay
{
	DECLARE_CLASS_SIMPLE( CHudStamina, CHudNumericDisplay );

public:
	CHudStamina( const char *pElementName );
	virtual void Init( void );
	virtual void VidInit( void );
	virtual void Reset( void );
	virtual void OnThink();
	//void MsgFunc_Stamina( bf_read &msg );

private:
	// old variables
	int		m_iStamina;
};	

DECLARE_HUDELEMENT( CHudStamina );
//DECLARE_HUD_MESSAGE( CHudStamina, Stamina );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudStamina::CHudStamina( const char *pElementName ) : CHudElement( pElementName ), CHudNumericDisplay(NULL, "HudStamina")
{
	SetHiddenBits( HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudStamina::Init()
{
	//HOOK_HUD_MESSAGE( CHudStamina, Stamina );
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudStamina::Reset()
{
	m_iStamina		= 100;

	//BG2 - Tjoppen - default to "Health", not "HEALTH" in HUD
	SetLabelText(L"Stamina");

	SetDisplayValue(m_iStamina);
	SetFgColor( Color( 255, 220, 200, 150 ) );
	SetBgColor( Color( 0, 0, 0, 0 ) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudStamina::VidInit()
{
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudStamina::OnThink()
{
	//feck off dammit
	SetPaintEnabled(false);
	SetPaintBackgroundEnabled(false);
	return;
	/*
	C_HL2MP_Player *local = (C_HL2MP_Player*)C_HL2MP_Player::GetLocalPlayer();

	if( local )
		SetDisplayValue( local->m_iStamina );
	else
		SetDisplayValue( -1 );
	//SetDisplayValue(m_iStamina);*/
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
/*void CHudStamina::MsgFunc_Stamina( bf_read &msg )
{
	m_iStamina = msg.ReadShort();
}*/