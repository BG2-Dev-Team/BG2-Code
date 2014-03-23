//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Base class for helicopters & helicopter-type vehicles
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "ai_network.h"
#include "ai_default.h"
#include "ai_schedule.h"
#include "ai_hull.h"
#include "ai_node.h"
#include "ai_task.h"
#include "ai_senses.h"
#include "ai_memory.h"
#include "entitylist.h"
#include "soundenvelope.h"
#include "gamerules.h"
#include "grenade_homer.h"
#include "ndebugoverlay.h"
#include "cbasehelicopter.h"
#include "soundflags.h"
#include "rope.h"
#include "saverestore_utlvector.h"
#include "collisionutils.h"
#include "coordsize.h"
#include "effects.h"
#include "rotorwash.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
