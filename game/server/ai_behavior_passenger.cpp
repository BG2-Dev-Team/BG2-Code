//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Behavior for NPCs riding in cars (with boys)
//
//=============================================================================

#include "cbase.h"
#include "ai_playerally.h"
#include "ai_motor.h"
#include "bone_setup.h"
#include "vehicle_base.h"
#include "entityblocker.h"
#include "ai_behavior_passenger.h"
#include "ai_pathfinder.h"
#include "ai_network.h"
#include "ai_node.h"
#include "ai_moveprobe.h"
#include "env_debughistory.h"
