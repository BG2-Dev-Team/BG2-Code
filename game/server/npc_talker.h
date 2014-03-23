//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//

#ifndef TALKNPC_H
#define TALKNPC_H

#ifdef POSIX
#undef time
#include <time.h>
#endif

#ifndef _XBOX
#undef min
#undef max
#pragma warning(push)
#include <set>
#pragma warning(pop)
#endif

#ifdef _WIN32
#pragma once
#endif

// the include <set> monkey's with the MAX() define, unbreak it
#undef MINMAX_H
#include "minmax.h"

#include "ai_playerally.h"

#include "soundflags.h"

#include "ai_task.h"
#include "ai_schedule.h"
#include "ai_default.h"
#include "ai_speech.h"
#include "ai_basenpc.h"
#include "ai_behavior.h"
#include "ai_behavior_follow.h"

#include "tier0/memdbgon.h"

#include "tier0/memdbgoff.h"

#endif		//TALKNPC_H
