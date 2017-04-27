/*
The Battle Grounds 3 - A Source modification
Copyright (C) 2017, The Battle Grounds 2 Team and Contributors

The Battle Grounds 2 free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

The Battle Grounds 3 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Contact information:
Chel "Awesome" Trunk		mail, in reverse: com . gmail @ latrunkster

You may also contact the (future) team via the Battle Grounds website and/or forum at:
www.bg2mod.com

Note that because of the sheer volume of files in the Source SDK this
notice cannot be put in all of them, but merely the ones that have any
changes from the original SDK.
In order to facilitate easy searching, all changes are and must be
commented on the following form:

//BG3 - <name of contributer>[ - <small description>]
*/

#ifdef WIN32
#pragma once
#endif

#include "cbase.h"
#include "player.h"
//#include "sdk_player.h"
#include "in_buttons.h"
#include "movehelper_server.h"
#include "gameinterface.h"
#include "team.h"
#include "hl2mp_gamerules.h"
#include "hl2mp_player.h"
#include "../bg2/flag.h"
#include "../bg2/weapon_bg2base.h"
#include "bg3_bot_manager.h"
#include "iconvar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


ConVar bot_minplayers("bot_minplayers", "8", FCVAR_GAMEDLL, "");
ConVar bot_maxperteam("bot_maxperteam", "0", FCVAR_GAMEDLL, "Kicks bots to ensure that bots will not fill up a team to be larger than this");
ConVar bot_minplayers_map("bot_minplayers_map", "0", FCVAR_GAMEDLL, "Map-defined minimum bot players. Whether or not this is used depends on bot_minplayers_mode");
ConVar bot_minplayers_mode("bot_minplayers_mode", "0", FCVAR_GAMEDLL, "0 bots disabled, 1 use bot_minplayers, 2 let bot_minplayers_map override, 3 use only bot_minplayers_map");

