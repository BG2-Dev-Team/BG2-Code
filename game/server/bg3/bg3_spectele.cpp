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
battlegrounds3.com

Note that because of the sheer volume of files in the Source SDK this
notice cannot be put in all of them, but merely the ones that have any
changes from the original SDK.
In order to facilitate easy searching, all changes are and must be
commented on the following form:

//BG3 - <name of contributer>[ - <small description>]
*/

#include "cbase.h"



ConVar teleplayer("teleplayer", "-1", FCVAR_GAMEDLL, "Sets by client index which player is to be affected by teleport commands. Player must be spectator.");

CON_COMMAND_F(tele, "Teleports the player with the client index set by \"teleplayer\" to the given (x,y,z) coordinates. Player must be spectator", FCVAR_GAMEDLL) {
	if (args.ArgC() == 4) {

		CBasePlayer* pPlayer = UTIL_PlayerByIndex(teleplayer.GetInt());
		if (pPlayer && pPlayer->GetTeamNumber() == TEAM_SPECTATOR) {
			int x = atoi(args[1]);
			int y = atoi(args[2]);
			int z = atoi(args[3]);

			pPlayer->SetAbsOrigin(Vector(x, y, z));
		}
	}
}

CON_COMMAND_F(telelook, "Given an (x,y,z) local offset from the \"teleplayer\" player, sets the player's view angles in the direction of the offset.", FCVAR_GAMEDLL) {
	if (args.ArgC() == 4) {

		CBasePlayer* pPlayer = UTIL_PlayerByIndex(teleplayer.GetInt());
		if (pPlayer && pPlayer->GetTeamNumber() == TEAM_SPECTATOR) {
			int x = atoi(args[1]);
			int y = atoi(args[2]);
			int z = atoi(args[3]);

			Vector offset(x, y, z);
			QAngle angles;
			VectorAngles(offset, angles);
			pPlayer->SetAbsAngles(angles);
		}
	}
}

