/*
The Battle Grounds 3 - A Source modification
Copyright (C) 2017, The Battle Grounds 2 Team and Contributors

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

#ifdef WIN32
#pragma once
#endif


/*
Manages the bot population of the server.
*/
class CBotManager;

/*
Console Variables
*/
extern ConVar bot_minplayers;
extern ConVar bot_maxplayers; //Kicks bots to ensure that bots will not fill up a team to be larger than this
extern ConVar bot_minplayers_map; //potential override for bot_minplayers
extern ConVar bot_minplayers_mode; //0 bots disabled, 1 use bot_minplayers, 2 let bot_minplayers_map override, 3 use only bot_minplayers_map

class CBotManager {
private:
	/*
	Private helper variables
	*/
	static float s_flNextThink; //so that we don't have to the calculations every frame
	static float s_flThinkInterval; //amount of time between thinks

	/*
	Private helper functions
	*/
	static int	GetMinPlayers(); //looks at the svars to see minimum num players required
	static int	CountPlayersOfTeam(int iTeam);

	static int	CountBotsOfTeam(int iTeam);
	static void AddBotOfTeam(int iTeam);
	static void RemoveBotOfTeam(int iTeam);

public:
	/*
	Public procedures for manipulating the bot population
	*/
	static void Think(); //wraps this class's functionality into one function
	static void SetNextThink(float thinkTime) { s_flNextThink = thinkTime; }
	static void KickAllBots(); //removes all bots from the server
};