/*
The Battle Grounds 3 - A Source modification
Copyright (C) 2017, The Battle Grounds 3 Team and Contributors

The Battle Grounds 3 free software; you can redistribute it and/or
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

#include "cbase.h"
#include "entitylist.h"
#include "hl2mp/hl2mp_player.h"

/*
The purpose of this file is to provide utilities used strictly for development-only purposes
*/

static CBaseEntity* g_pAdjustedEntity = nullptr;

CON_COMMAND_F(ent_adjust_select, "Selects entity to move about via console commands.", FCVAR_CHEAT) {
	if (args.ArgC() == 2) {
		CBaseEntity* pEnt = gEntList.FindEntityByName(g_pAdjustedEntity, args[1]);
		if (pEnt)
			g_pAdjustedEntity = pEnt;
		else
			Warning("Could not find entity named %s !\n", args[1]);
	}
}

CON_COMMAND_F(ent_adjust_xyz, "Adds to the position of adjusted entity by the given vector", FCVAR_CHEAT)  {
	if (g_pAdjustedEntity && args.ArgC() == 4) {
		Vector adjust = Vector(atoi(args[1]), atoi(args[2]), atoi(args[3]));
		g_pAdjustedEntity->SetAbsOrigin(g_pAdjustedEntity->GetAbsOrigin() + adjust);
	}
}

CON_COMMAND_F(ent_adjust_angle, "Sets angle of adjusted entity given an xyz direction", FCVAR_CHEAT) {
	if (g_pAdjustedEntity && args.ArgC() == 4) {
		Vector vangle = Vector(atoi(args[1]), atoi(args[2]), atoi(args[3]));
		QAngle angle;
		VectorAngles(vangle, angle);
		g_pAdjustedEntity->SetAbsAngles(angle);
	}
}