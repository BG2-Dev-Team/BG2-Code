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
#include "hl2mp/hl2mp_gamerules.h"

/*
The console commands here are shorthands for beginning different competitive matches
*/
CON_COMMAND_F(linebattle, "Starts a linebattle. With an integer argument provided, the round count is set to the given number and the competitive cfg is loaded. Otherwise the round system is disabled and then the public linebattle cfg is loaded. Modified CFGs can override the round system settings.", FCVAR_GAMEDLL) {
	if (args.ArgC() == 2) {
		int roundCount = atoi(args[1]);
		mp_rounds.SetValue(roundCount);
		engine->ServerCommand("exec linebattle.cfg\n");
	}
	else {
		engine->ServerCommand("exec linebattle_public.cfg\n");
	}
	engine->ServerExecute();
}

CON_COMMAND_F(lms, "Starts an LMS match. With an integer argument provided, the round count is set to the given number and then lms.cfg is loaded. If no argument is provided, the round count is set to 0 and then the lms.cfg is loaded. Modifying lms.cfg can override the round system settings.", FCVAR_GAMEDLL) {
	int roundCount = args.ArgC() == 2 ? atoi(args[1]) : 0;
	mp_rounds.SetValue(roundCount);
	engine->ServerCommand("exec lms.cfg\n");
	engine->ServerExecute();
}

CON_COMMAND_F(skirm, "Starts a 2-round competitive skirmish. An extra integer argument, if provided, sets the round duration in minutes, and then the skirm.cfg is loaded. If no argument is provided, a 20 minute round duration is assumed. Modify the skirm.cfg to change parameters.", FCVAR_GAMEDLL) {
	int minuteCount = args.ArgC() == 2 ? atoi(args[1]) : 20;
	mp_roundtime.SetValue(minuteCount * 60);
	engine->ServerCommand("exec skirm.cfg\n");
	engine->ServerExecute();
}