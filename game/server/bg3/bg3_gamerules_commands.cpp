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
#include "hl2mp/hl2mp_gamerules.h"
#include "../shared/bg3/bg3_map_model.h"

void CSay(const char* pszFormat, ...) {
	va_list vl;
	va_start(vl, pszFormat);
	char buffer[128];
	V_vsnprintf(buffer, sizeof(buffer), pszFormat, vl);
	va_end(vl);


	char buffer2[140];
	Q_snprintf(buffer2, sizeof(buffer), "say %s\n", buffer);
	engine->ServerCommand(buffer2);
}

/*
The console commands here are shorthands for beginning different competitive matches
*/
CON_COMMAND(linebattle, "Starts a linebattle. With an integer argument provided, the round count is set to the given number and the competitive cfg is loaded. Otherwise the round system is disabled and then the public linebattle cfg is loaded. Modified CFGs can override the round system settings.") {
	if (!verifyMapModePermissions(__FUNCTION__))
		return;

	int count;
	if (args.ArgC() > 1)
		count = atoi(args[1]);
	else
		count = 10;
	mp_rounds.SetValue(count);
	engine->ServerCommand("exec linebattle.cfg\n");
	engine->ServerExecute();
}

CON_COMMAND(lms, "Starts an LMS match. With an integer argument provided, the round count is set to the given number and then lms.cfg is loaded. If no argument is provided, the round count is set to 0 and then the lms.cfg is loaded. Modifying lms.cfg can override the round system settings.") {
	if (!verifyMapModePermissions(__FUNCTION__))
		return;

	int count;
	if (args.ArgC() > 1)
		count = atoi(args[1]);
	else
		count = 10;
	mp_rounds.SetValue(count);
	engine->ServerCommand("exec lms.cfg\n");
	engine->ServerExecute();
}

CON_COMMAND(skirm, "Starts a 2-round competitive skirmish. An extra integer argument, if provided, sets the round duration in minutes, and then the skirm.cfg is loaded. If no argument is provided, a 20 minute round duration is assumed. Modify the skirm.cfg to change parameters.") {
	if (!verifyMapModePermissions(__FUNCTION__))
		return;

	int count;
	if (args.ArgC() > 1)
		count = atoi(args[1]);
	else
		count = 20;
	mp_roundtime.SetValue(60 * count);
	engine->ServerCommand("exec skirm.cfg\n");
	engine->ServerExecute();
}

CON_COMMAND(dm, "Starts a single-round deathmatch. An extra integer argument, if provided, specifies the number of rounds.") {
	if (!verifyMapModePermissions(__FUNCTION__))
		return;
	int roundCount = 1;
	if (args.ArgC() > 1)
		roundCount = atoi(args[1]);
	if (roundCount < 1)
		return;

	mp_rounds.SetValue(roundCount);
	engine->ServerCommand("exec dm.cfg\n");
	engine->ServerExecute();
}

CON_COMMAND(changemap, "Changes the server to the specified map") {
	if (!verifyMapModePermissions(__FUNCTION__) || args.ArgC() < 2)
		return;

	if (!CMapInfo::MapExists(args[1])) {
		CSay("The map %s does not exist", args[1]);
		return;
	}

	//after we've verified, just do changelevel
	nextlevel.SetValue(args[1]);
	((CHL2MPRules*) g_pGameRules)->ChangeLevel();
}

CON_COMMAND(csay, "Says a message in chat") {
	if (!verifyMapModePermissions(__FUNCTION__) || args.ArgC() < 2)
		return;
	int len = 7 + strlen(args[1]);
	char* buffer = new char[len];
	Q_snprintf(buffer, len, "say %s\n", args[1]);
	engine->ServerCommand(buffer);
	delete[] buffer;
}