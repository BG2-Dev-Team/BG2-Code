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

You should have received a copy of the GNU Le							sser General Public
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
#include <string>

//Commands in this dictionary will be forwarded to console if they appear as the first word in chat
CUtlVector<std::string> g_chatCommands;

void InitChatCommandList() {
	g_chatCommands.Purge();
	g_chatCommands.AddToTail("ff");
	g_chatCommands.AddToTail("currentmap");
	g_chatCommands.AddToTail("nextmap");
	g_chatCommands.AddToTail("rtv");
	g_chatCommands.AddToTail("pings");
	g_chatCommands.AddToTail("admins");
	g_chatCommands.AddToTail("nominate");
	g_chatCommands.AddToTail("respawn");
	g_chatCommands.AddToTail("vote");
	g_chatCommands.AddToTail("votekick");
	g_chatCommands.AddToTail("votemute");
	g_chatCommands.AddToTail("votemap");
	g_chatCommands.AddToTail("votescramble");
	g_chatCommands.AddToTail("votegungame");
	g_chatCommands.AddToTail("votecancel");
	g_chatCommands.AddToTail("revote");
	g_chatCommands.AddToTail("asay");
}

//Checks if given chat text is a command (ex. "ff") and executes it
//if applicable. Does modify the given chat string.
bool ChatStringShortCommand(char* command, int bufLen, bool& bOutExecute) {
	//assume false, then check otherwise
	bOutExecute = false;
	bool result = false;

	extern ConVar sv_chatcommands;
	if (sv_chatcommands.GetInt() == 0) {
		return result;
	}

	char* firstSpace = NULL;
	
	int i = 0;
	for (; i < bufLen && command[i] != '\0'; i++) {
		if (command[i] == ' ') {
			firstSpace = command + i;
			break;
		}
	}

	//switch the space to a null character for std::string parsing
	if (firstSpace) {
		firstSpace[0] = '\0';
	}

	
	if (firstSpace || (i < bufLen && command[i] == '\0')) {
		//Msg("passed null character check, searching for %s\n", command);

		//we now have our clean string, check if it's in the list
		if (g_chatCommands.HasElement(command)) {
			if (Q_strcmp(command, "asay") == 0) {
				result = true;
			}
			//Msg("Command found in list\n");

			bOutExecute = true;
		}

		if (firstSpace) {
			//Msg("Putting the space back\n");
			firstSpace[0] = ' ';
		}
	}

	return result;
}
