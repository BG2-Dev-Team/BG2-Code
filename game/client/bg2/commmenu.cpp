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
#include "commmenu.h"
#include "../game/server/bg2/vcomm.h"
#include "vguicenterprint.h"

#include <game/client/iviewport.h>
#include "commandmenu.h"
#include "hltvcamera.h"

#include "IGameUIFuncs.h" // for key bindings

//BG2 - HairyPotter
#include "classmenu.h"
//

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern IGameUIFuncs *gameuifuncs; // for key binding details

using namespace vgui;

//------------------------------------------------------------------------------------------------------------------------
// Base code for comm menus.
//------------------------------------------------------------------------------------------------------------------------

CCommBase::CCommBase() : Frame( NULL, PANEL_COMM )
{
	slot1 = slot2 = slot3 = slot4 = slot5 = slot6 = slot7 = slot8 = slot9 = slot10 = -1;
	teammenu = classmenu = commmenu = commmenu2 = weaponmenu = -1;

	SetMouseInputEnabled( false );
	SetKeyBoardInputEnabled( true );
	SetTitleBarVisible( false ); // don't draw a title bar
	SetMoveable( false );
	SetSizeable( false );
	SetProportional(true);

	SetScheme("ClientScheme");

	SetSize( 640, 480 );

	int w,h;
	surface()->GetScreenSize(w, h);

	SetPos( 50, h - GetTall() );
	SetAlpha( 0 );
	SetPaintBackgroundEnabled( false );
	SetBorder( NULL );
}

void CCommBase::OnKeyCodePressed(KeyCode code)
{
	CClassMenu *panel = static_cast<CClassMenu*>(gViewPortInterface->FindPanelByName( PANEL_CLASSES ) );

	int iLastTrappedKey = code;	

	if( !m_pViewPort || !panel )
		return;

	if( iLastTrappedKey == slot10 )
		ShowPanel( false ); //This panel.

	else if( iLastTrappedKey == teammenu )
	{
		ShowPanel( false );
		panel->SetScreen( 1, true );

		return;
	}
	else if( iLastTrappedKey == classmenu )
	{
		ShowPanel( false );
		panel->SetScreen( 2, true );

		return;
	}
	else if( iLastTrappedKey == weaponmenu )
	{
		ShowPanel( false );
		panel->SetScreen( 3, true );

		return;
	}
	else if( iLastTrappedKey == commmenu )
	{
		m_pViewPort->ShowPanel( PANEL_COMM2, false );
		m_pViewPort->ShowPanel( PANEL_COMM, !IsVisible() );
		panel->ShowPanel( false ); //Class menu
		
		return;
	}
	else if( iLastTrappedKey == commmenu2 )
	{
		m_pViewPort->ShowPanel( PANEL_COMM, false );
		m_pViewPort->ShowPanel( PANEL_COMM2, !IsVisible() );
		panel->ShowPanel( false ); //Class menu
		
		return;
	}
	else
		BaseClass::OnKeyCodePressed( code );
}

void CCommBase::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	int w,h;
	surface()->GetScreenSize(w, h);

	SetPos( 50, h - GetTall() );
}

void CCommBase::Update()
{
	#define getkey( index ) if( slot##index < 0 ) slot##index = gameuifuncs->GetButtonCodeForBind( "slot" #index )
		getkey( 1 );
		getkey( 2 );
		getkey( 3 );
		getkey( 4 );
		getkey( 5 );
		getkey( 6 );
		getkey( 7 );
		getkey( 8 );
		getkey( 9 );
		getkey( 10 );

	if( teammenu < 0 ) teammenu = gameuifuncs->GetButtonCodeForBind( "teammenu" );
	if( classmenu < 0 ) classmenu = gameuifuncs->GetButtonCodeForBind( "classmenu" );
	if( commmenu < 0 ) commmenu = gameuifuncs->GetButtonCodeForBind( "commmenu" );
	if( commmenu2 < 0 ) commmenu2 = gameuifuncs->GetButtonCodeForBind( "commmenu2" );
	if( weaponmenu < 0 ) weaponmenu = gameuifuncs->GetButtonCodeForBind( "weaponmenu" );
}

void CCommBase::ShowPanel( bool bShow )
{
	if ( IsVisible() == bShow )
		return;

	if ( engine->IsPlayingDemo() ) //Don't display when playing demos. -HairyPotter
		return;

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if ( !pPlayer ) 
		return;

	if ( bShow )
	{
		//Make sure the player is alive to use voicecomms. -HairyPotter
		if ( pPlayer->GetTeamNumber() <= TEAM_SPECTATOR || !pPlayer->IsAlive() )
			return;

		Activate();
	}
	else
		SetVisible( false );

	SetKeyBoardInputEnabled( bShow );
}

//------------------------------------------------------------------------------------------------------------------------
// Begin first comm menu code.
//------------------------------------------------------------------------------------------------------------------------

CCommMenu::CCommMenu( IViewPort *pViewPort ) : CCommBase()
{	
	m_pViewPort = pViewPort;

	m_pLabel = new Label( this, "label", g_pVGuiLocalize->Find("#BG2_VoiceComm_Menu_A") );
	m_pLabel->SetPos( 50, 50 );
	m_pLabel->SizeToContents();
}

void CCommMenu::OnKeyCodePressed(KeyCode code)
{
	// we can't compare the keycode to a known code, because translation from bound keys
	// to vgui key codes is not 1:1. Get the engine version of the key for the binding
	// and the actual pressed key, and compare those..
	int iLastTrappedKey = code;	

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if ( !pPlayer )
		return;

#define ifkey( index, index2 ) if( iLastTrappedKey == slot##index ) { engine->ServerCmd( "voicecomm " #index2 ); ShowPanel( false ); }
			ifkey( 1, 0 )
	else	ifkey( 2, 1 )
	else	ifkey( 3, 2 )
	else	ifkey( 4, 3 )
	else	ifkey( 5, 4 )
	else	ifkey( 6, 5 )
	else	ifkey( 7, 6 )
	else	ifkey( 8, 7 )
	else	ifkey( 9, 8 )
	else
		BaseClass::OnKeyCodePressed( code );
}

void CCommMenu::ShowPanel(bool bShow)
{
	BaseClass::ShowPanel( bShow );
	m_pLabel->SetVisible( bShow );
}
