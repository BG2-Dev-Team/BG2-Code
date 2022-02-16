//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "usermessages.h"
#include "shake.h"
#include "voice_gamemgr.h"

// NVNT include to register in haptic user messages
#include "haptics/haptic_msgs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

void RegisterUserMessages( void )
{
	//usermessages->Register( "Geiger", 1 ); //BG2 - Geiger Removed from BG2. -HairyPotter
	usermessages->Register( "Train", 1 );
	usermessages->Register( "HudText", -1 );
	usermessages->Register( "SayText", -1 );
	usermessages->Register( "SayText2", -1 );
	usermessages->Register( "TextMsg", -1 );
	usermessages->Register( "HudMsg", -1 );
	usermessages->Register( "ResetHUD", 1);		// called every respawn
	usermessages->Register( "GameTitle", 0 );
	usermessages->Register( "ItemPickup", -1 );
	usermessages->Register( "ShowMenu", -1 );
	usermessages->Register( "Shake", 13 );
	usermessages->Register( "Fade", 10 );
	usermessages->Register( "VGUIMenu", -1 );	// Show VGUI menu
	usermessages->Register( "Rumble", 3 );	// Send a rumble to a controller
	usermessages->Register( "Battery", 2 );
	usermessages->Register( "Damage", 18 );		// BUG: floats are sent for coords, no variable bitfields in hud & fixed size Msg
	usermessages->Register( "VoiceMask", VOICE_MAX_PLAYERS_DW*4 * 2 + 1 );
	usermessages->Register( "RequestState", 0 );
	usermessages->Register( "CloseCaption", -1 ); // Show a caption (by string id number)(duration in 10th of a second)
	usermessages->Register( "HintText", -1 );	// Displays hint text display
	usermessages->Register( "KeyHintText", -1 );	// Displays hint text display
	usermessages->Register( "SquadMemberDied", 0 );
	usermessages->Register( "AmmoDenied", 2 );
	usermessages->Register( "CreditsMsg", 1 );
	usermessages->Register( "LogoTimeMsg", 4 );
	usermessages->Register( "AchievementEvent", -1 );
	//usermessages->Register( "UpdateJalopyRadar", -1 );
	//BG2 - Draco - stamina message to HUD
	//usermessages->Register( "Stamina", 2 );
	//usermessages->Register( "classreward", -1 );
	//usermessages->Register( "teamreward", -1 );
	//usermessages->Register( "flagstatus", -1 );
	//BG2 - Tjoppen - usermessages
	usermessages->Register("VoiceComm", 3);
	usermessages->Register("GameMsg", -1); //player message string (varies)
	usermessages->Register("HitVerif", 5);	//XXX hit YYY in ZZZ for WWW damage
	usermessages->Register("ExpEvent", 8); //checksum and event type
	usermessages->Register("BG3Election", -1); //info for BG3's custom election/vote system
	usermessages->Register("ServerBlood", -1);	//pos(varies), normal(varies), amount(2)
	usermessages->Register("WinMusic", 1);		//byte = team ID. let client decide how to interpret
	usermessages->Register("CaptureSounds", -1);	//Position of flag (varies). -HairyPotter
	usermessages->Register("VCommSounds", -1); //Position Of Player (varies). -HairyPotter
	//usermessages->Register( "MapName", -1 ); //Name of map that we're changng to (varies). -HairyPotter //Not used yet.
	usermessages->Register("BG2Events", -1); //BG2 - Used for localization. -HairyPotter
	//

#ifndef _X360
	// NVNT register haptic user messages
	RegisterHapticMessages();
#endif
}