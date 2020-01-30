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
//#include "classmenu.h"
#include "bg3/bg3_classmenu.h"
#include "bg3/vgui/bg3_vgui_shared.h"
#include "weapon_selection.h"
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
	teammenu = classmenu = commmenu = commmenu2 = admin = -1;

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
	CClassMenu *panel = g_pClassMenu;

	int iLastTrappedKey = code;	

	if( !m_pViewPort || !panel )
		return;

	if( iLastTrappedKey == slot10 )
		ShowPanel( false ); //This panel.

	else if( iLastTrappedKey == teammenu )
	{
		ShowPanel( false );
		panel->ShowPanel(true);

		return;
	}
	else if( iLastTrappedKey == classmenu )
	{
		ShowPanel( false );
		panel->ShowPanel(true);

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
	else if (iLastTrappedKey == admin)
	{
		m_pViewPort->ShowPanel(PANEL_COMM, false);
		m_pViewPort->ShowPanel(PANEL_COMM2, false);
		panel->ShowPanel(false); //Class menu

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
	/*#define getkey( index ) if( slot##index < 0 ) slot##index = gameuifuncs->GetButtonCodeForBind( "slot" #index )
		getkey( 1 );
		getkey( 2 );
		getkey( 3 );
		getkey( 4 );
		getkey( 5 );
		getkey( 6 );
		getkey( 7 );
		getkey( 8 );
		getkey( 9 );
		getkey( 10 );*/

	//BG3 - at least temporarily moving off the slot system, otherwise the buff command binds (ex. vcomm_advance) will conflict with these
#define s(n) slot##n = KEY_##n;
	s(1);
	s(2);
	s(3);
	s(4);
	s(5);
	s(6);
	s(7);
	s(8);
	s(9);
	slot10 = KEY_0;
#undef s
	
	
	if( teammenu < 0 ) teammenu = gameuifuncs->GetButtonCodeForBind( "teammenu" );
	if( classmenu < 0 ) classmenu = gameuifuncs->GetButtonCodeForBind( "classmenu" );
	if( commmenu < 0 ) commmenu = gameuifuncs->GetButtonCodeForBind( "commmenu" );
	if( commmenu2 < 0 ) commmenu2 = gameuifuncs->GetButtonCodeForBind( "commmenu2" );
	if (admin < 0) admin = gameuifuncs->GetButtonCodeForBind("admin");
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

	//BG3 - instead of getting input from menu code, getting it from weapon code lets us still press other keys...
	//SetKeyBoardInputEnabled( bShow );
}

//------------------------------------------------------------------------------------------------------------------------
// Begin first comm menu code.
//------------------------------------------------------------------------------------------------------------------------

CCommMenu* g_pCommMenu = NULL;
CCommMenu::CCommMenu( IViewPort *pViewPort ) : CCommBase()
{	
	g_pCommMenu = this;
	m_pViewPort = pViewPort;

	m_pLabel = new Label( this, "label", g_pVGuiLocalize->Find("#BG3_VoiceComm_Menu_A") );
	m_pLabel->SetPos( 50, 50 );
	m_pLabel->SizeToContents();
	SetKeyBoardInputEnabled(false);
}

void CCommMenu::PlayVcommBySlot(int iSlot) {
	char buffer[15];
	sprintf_s(buffer, "voicecomm %i", iSlot - 1);
	engine->ServerCmd(buffer);
	ShowPanel(false);
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
	else {
		//SimulateKeyCodePressed(code);
		BaseClass::OnKeyCodePressed(code);
	}
}

void CCommMenu::ShowPanel(bool bShow)
{
	BaseClass::ShowPanel( bShow );
	m_pLabel->SetVisible( bShow );
}

ConVar cl_forward_vcomm_binds_to_slots("cl_forward_vcomm_binds_to_slots", "1", FCVAR_ARCHIVE, 
	"If non-zero, then keys pressed for explicity bot-controlling vcomms will be forward to menus as slot keys. Ex. vcomm_advance -> slot1");

CON_COMMAND_F(vcomm_advance, "Fires off a \"Charge!\" voice command, influencing bots; or otherwise passed to menus as slot1 if cl_forward_vcomm_binds_to_slots is true", FCVAR_ARCHIVE) {
	if (cl_forward_vcomm_binds_to_slots.GetBool() && !GetHudWeaponSelection()->SelectSlot(1))
		engine->ServerCmd("vcomm_advance");
}

CON_COMMAND_F(vcomm_fire, "Fires off a \"Fire!\" voice command, influencing bots; or otherwise passed to menus as slot2 if cl_forward_vcomm_binds_to_slots is true", FCVAR_ARCHIVE) {
	if (cl_forward_vcomm_binds_to_slots.GetBool() && !GetHudWeaponSelection()->SelectSlot(2))
		engine->ServerCmd("vcomm_fire");
}

CON_COMMAND_F(vcomm_rally, "Fires off a \"Rally Round!\" voice command, influencing bots; or otherwise passed to menus as slot3 if cl_forward_vcomm_binds_to_slots is true", FCVAR_ARCHIVE) {
	if (cl_forward_vcomm_binds_to_slots.GetBool() && !GetHudWeaponSelection()->SelectSlot(3))
		engine->ServerCmd("vcomm_rally");
}

CON_COMMAND_F(vcomm_retreat, "Fires off a \"Retreat!\" voice command, influencing bots; or otherwise passed to menus as slot4 if cl_forward_vcomm_binds_to_slots is true", FCVAR_ARCHIVE) {
	if (cl_forward_vcomm_binds_to_slots.GetBool() && !GetHudWeaponSelection()->SelectSlot(4))
		engine->ServerCmd("vcomm_retreat");
}

CON_COMMAND_F(vcomm_follow, "Fires off a \"Follow!\" voice command, influencing bots; or otherwise passed to menus as slot5 if cl_forward_vcomm_binds_to_slots is true", FCVAR_ARCHIVE) {
	if (cl_forward_vcomm_binds_to_slots.GetBool() && !GetHudWeaponSelection()->SelectSlot(5))
		engine->ServerCmd("vcomm_follow");
}

CON_COMMAND_F(vcomm_halt, "Fires off a \"Halt!\" voice command, influencing bots; or otherwise passed to menus as slot6 if cl_forward_vcomm_binds_to_slots is true", FCVAR_ARCHIVE) {
	if (cl_forward_vcomm_binds_to_slots.GetBool() && !GetHudWeaponSelection()->SelectSlot(6))
		engine->ServerCmd("vcomm_halt");
}