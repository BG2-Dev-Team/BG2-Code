//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef BASEGRENADE_SHARED_H
#define BASEGRENADE_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#include "baseprojectile.h"

#if defined( CLIENT_DLL )

#define CBaseGrenade C_BaseGrenade

#include "c_basecombatcharacter.h"

#else

#include "basecombatcharacter.h"
#include "player_pickup.h"

#endif

#endif // BASEGRENADE_SHARED_H
