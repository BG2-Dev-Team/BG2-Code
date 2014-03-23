//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Base class for many flying NPCs
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "ai_basenpc_physicsflyer.h"
#include "ai_route.h"
#include "ai_navigator.h"
#include "ai_motor.h"
#include "physics_saverestore.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
