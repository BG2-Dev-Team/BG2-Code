//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Combat behaviors for AIs in a relatively self-preservationist mode.
//			Lots of cover taking and attempted shots out of cover.
//
//=============================================================================//

#include "cbase.h"

#include "ai_hint.h"
#include "ai_node.h"
#include "ai_navigator.h"
#include "ai_tacticalservices.h"
#include "ai_behavior_standoff.h"
#include "ai_senses.h"
#include "ai_squad.h"
#include "ai_goalentity.h"
#include "ndebugoverlay.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
