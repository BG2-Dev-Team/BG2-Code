//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include <cdll_client_int.h>
#include <globalvars_base.h>
#include <cdll_util.h>
#include <KeyValues.h>

#include <vgui/ILocalize.h>

#include <stdio.h> // _snprintf define

//#include "spectatorgui.h"
#include "commmenu2.h"
#include "c_hl2mp_player.h"
#include "../server/bg2/vcomm.h"
#include "vguicenterprint.h"

#include <game/client/iviewport.h>
#include "commandmenu.h"
#include "hltvcamera.h"

#include "IGameUIFuncs.h" // for key bindings
#include <shareddefs.h>
#include <igameresources.h>

//BG2 - HairyPotter
#include "classmenu.h"
//

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern IGameUIFuncs *gameuifuncs; // for key binding details

using namespace vgui;

// EnforceOfficerForCommenu2
//  checks if the local player is an officer, and prints an error message if so is not the case
/*bool EnforceOfficerForCommenu2( void )
{
	C_HL2MP_Player *pPlayer = dynamic_cast<C_HL2MP_Player*> (C_HL2MP_Player::GetLocalPlayer());

	if( pPlayer && pPlayer->GetClass() == CLASS_OFFICER )
	{
		return true;
	}
	else
	{
		internalCenterPrint->Print( vgui::localize()->Find( "#BG3_VoiceComm2_Officer_Only" ) );
		return false;
	}
}*/
CCommMenu2* g_pCommMenu2 = NULL;

CCommMenu2::CCommMenu2( IViewPort *pViewPort )
{	
	g_pCommMenu2 = this;
	m_pViewPort = pViewPort;

	m_pLabel = new Label( this, "label", g_pVGuiLocalize->Find("#BG3_VoiceComm_Menu_B") );
	m_pLabel->SetPos( 50, 50 );
	m_pLabel->SizeToContents();
	SetKeyBoardInputEnabled(false);
}

void CCommMenu2::PlayVcommBySlot(int iSlot) {
	char buffer[16];
	sprintf_s(buffer, "voicecomm %i", iSlot + 8);
	engine->ServerCmd(buffer);
	ShowPanel(false);
}

void CCommMenu2::OnKeyCodePressed(KeyCode code)
{
	// we can't compare the keycode to a known code, because translation from bound keys
	// to vgui key codes is not 1:1. Get the engine version of the key for the binding
	// and the actual pressed key, and compare those..
	int iLastTappedKey = code;	// the enginekey version of the code param

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if ( !pPlayer )
		return;

#define ifkey( index, index2 ) if( iLastTappedKey == slot##index ) { engine->ServerCmd( "voicecomm " #index2 ); ShowPanel( false ); }
			ifkey( 1, 9 )
	else	ifkey( 2, 10 )
	else	ifkey( 3, 11 )
	else	ifkey( 4, 12 )
	else	ifkey( 5, 13 )
	else	ifkey( 6, 14 )
	else	ifkey( 7, 15 )
	else	ifkey( 8, 16 )
	else	ifkey( 9, 17 )
	else
		BaseClass::OnKeyCodePressed( code );
}

void CCommMenu2::ShowPanel(bool bShow)
{
	BaseClass::ShowPanel( bShow );
	m_pLabel->SetVisible( bShow );
}


