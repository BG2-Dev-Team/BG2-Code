//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "gamerules.h"
#include "npcevent.h"
#include "in_buttons.h"
#include "engine/IEngineSound.h"

#if defined( CLIENT_DLL )
	#include "c_hl2mp_player.h"
#else
	#include "hl2mp_player.h"
	#include "grenade_tripmine.h"
	#include "grenade_satchel.h"
	#include "entitylist.h"
	#include "eventqueue.h"
#endif

#include "hl2mp/weapon_slam.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

