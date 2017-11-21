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

#include "bg3_bot.h"
#include "bg3_bot_vcomms.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


ConVar bot_minplayers("bot_minplayers", "8", FCVAR_GAMEDLL, "");
ConVar bot_maxplayers("bot_maxplayers", "0", FCVAR_GAMEDLL, "Kicks bots to ensure that bot population will not inflate player count beyond this number");
ConVar bot_minplayers_map("bot_minplayers_map", "0", FCVAR_GAMEDLL, "Map-defined minimum bot players. Whether or not this is used depends on bot_minplayers_mode");
ConVar bot_minplayers_mode("bot_minplayers_mode", "0", FCVAR_GAMEDLL, "0 bots disabled, 1 use bot_minplayers, 2 let bot_minplayers_map override, 3 use only bot_minplayers_map");


extern bool g_bServerReady;
extern CSDKBot gBots[];

//-------------------------------------------------------------------------------------------------
// Purpose: Initial values of private static helper variables
//-------------------------------------------------------------------------------------------------
float CBotManager::s_flNextThink = 0.f;
float CBotManager::s_flThinkInterval = 10.0f;


//-------------------------------------------------------------------------------------------------
// Purpose: Private helper functions
//-------------------------------------------------------------------------------------------------
int CBotManager::GetMinPlayers() {
	int minPlayers = 0;
	switch (bot_minplayers_mode.GetInt()) {
	case 1:
		minPlayers = bot_minplayers.GetInt();
		break;
	case 2:
		if (bot_minplayers_map.GetBool())
			minPlayers = bot_minplayers_map.GetInt();
		else
			minPlayers = bot_minplayers.GetInt();
		break;
	case 3:
		if (bot_minplayers_map.GetBool())
			minPlayers = bot_minplayers_map.GetInt();
		break;

	//otherwise do nothing
	case 0:
	default:
		break;
	}

	//clamp the value to avoid bad, bad things!
	minPlayers = Clamp(minPlayers, 0, gpGlobals->maxClients);

	return minPlayers;
}

int CBotManager::CountPlayersOfTeam(int iTeam) {
	int count = 0;
	CBasePlayer * curPlayer = nullptr;
	for (int i = 1; i <= gpGlobals->maxClients; i++) {
		curPlayer = UTIL_PlayerByIndex(i);
		if (curPlayer && curPlayer->GetTeamNumber() == iTeam) {
			count++;
		}
	}
	return count;
}

int CBotManager::CountBotsOfTeam(int iTeam) {
	int count = 0;
	CBasePlayer * curPlayer = nullptr;
	for (int i = 1; i <= gpGlobals->maxClients; i++) {
		curPlayer = UTIL_PlayerByIndex(i);
		if (curPlayer && curPlayer->GetFlags() & FL_FAKECLIENT && curPlayer->GetTeamNumber() == iTeam) {
			count++;
		}
	}
	return count;
}


//Adds bot of team to server. Requires that the server be ready.
void CBotManager::AddBotOfTeam(int iTeam, int count) {
	extern int g_iNextBotTeam;
	extern CBasePlayer* BotPutInServer(int iAmount, bool bFrozen);
	g_iNextBotTeam = iTeam;
	BotPutInServer(count, false);

	//reset the bot's vcomm manager
	CBotComManager* pComms;
	if (g_iNextBotTeam == TEAM_BRITISH)
		pComms = &g_BotBritComms;
	else
		pComms = &g_BotAmerComms;
	pComms->ResetThinkTime(bot_randfloat(4.0, 8.0));
}

void CBotManager::RemoveBotOfTeam(int iTeam) {
	CBasePlayer* pPlayer = nullptr;
	bool found = false;
	int i = 0;
	while (!found && i <= gpGlobals->maxClients) {
		pPlayer = UTIL_PlayerByIndex(i);
		if (pPlayer && pPlayer->GetFlags() & FL_FAKECLIENT && pPlayer->GetTeamNumber() == iTeam) {
			found = true;
		}
		i++;
	}

	if (pPlayer && found) {
		//assert that the bot's m_pPlayer is set to null
		gBots[pPlayer->GetClientIndex()].m_pPlayer = nullptr;

		//construct a server command to kick the bot
		//maximum length of bot name is 32 so we'll add a bit more
		char kickCommand[40];
		Q_snprintf(kickCommand, sizeof(kickCommand), "kick \"%s\"\n", pPlayer->GetPlayerName());
		
		//now actually kick them
		engine->ServerCommand(kickCommand);
	}
}


//-------------------------------------------------------------------------------------------------
// Purpose: Master think function manages bot population
//-------------------------------------------------------------------------------------------------
void CBotManager::Think() {
	if (g_bServerReady && s_flNextThink < gpGlobals->curtime) {

		//first check if we need to add
		int iAmerPlayers = CountPlayersOfTeam(TEAM_AMERICANS);
		int iBritPlayers = CountPlayersOfTeam(TEAM_BRITISH);

		int iTotalPlayers = iAmerPlayers + iBritPlayers;
		int iMinPlayers = GetMinPlayers();
		int iPlayersToAdd = iMinPlayers - iTotalPlayers;

		//clamp the value of maxplayers to minplayers
		if (bot_maxplayers.GetBool() && bot_maxplayers.GetInt() < iMinPlayers)
			bot_maxplayers.SetValue(iMinPlayers);

		//check if we need to add bots
		if (iPlayersToAdd > 0) {
			for (iPlayersToAdd; iPlayersToAdd > 0; iPlayersToAdd--) {
				//update the values on each iteration
				iAmerPlayers = CountPlayersOfTeam(TEAM_AMERICANS);
				iBritPlayers = CountPlayersOfTeam(TEAM_BRITISH);

				int iTeam = iAmerPlayers < iBritPlayers ? TEAM_AMERICANS : TEAM_BRITISH;
				AddBotOfTeam(iTeam);
			}
		} 
		//check if we need to remove bots
		else if (bot_maxplayers.GetBool() && bot_maxplayers.GetInt() < iTotalPlayers) {
			int iBritBots = CountBotsOfTeam(TEAM_BRITISH);
			int iAmerBots = CountBotsOfTeam(TEAM_AMERICANS);
			//int iTotalBots = iAmerBots + iBritBots;
			//okay, we have to kick a bot now
			if ((iBritBots || iAmerBots) ) {
				int iTeam;
				//if we have bots on both teams, then decide team based on which team has more
				if (iBritBots && iAmerBots) {
					iTeam = iAmerPlayers > iBritPlayers ? TEAM_AMERICANS : TEAM_BRITISH;
				} else {
					iTeam = iAmerBots > iBritBots ? TEAM_AMERICANS : TEAM_BRITISH;
				}
				RemoveBotOfTeam(iTeam);
			}
		}

		s_flNextThink += s_flThinkInterval;
	}
}

void CBotManager::KickAllBots() {
	CBasePlayer* pPlayer = nullptr;
	for (int i = 1; i <= gpGlobals->maxClients; i++) {
		pPlayer = UTIL_PlayerByIndex(i);
		if (pPlayer && pPlayer->GetFlags() & FL_FAKECLIENT) {
			char kickCommand[40];
			Q_snprintf(kickCommand, sizeof(kickCommand), "kick \"%s\"\n", pPlayer->GetPlayerName());

			//now actually kick them
			engine->ServerCommand(kickCommand);
		}
	}
}

CON_COMMAND_F(bot_kick_all, "Removes all bots from the server and sets bot_minplayers_mode to 0", FCVAR_GAMEDLL) {
	CBotManager::KickAllBots();
	bot_minplayers_mode.SetValue("0");
}

CON_COMMAND_F(bot_debug_report, "Gives status information on all bots, useful for debugging.", FCVAR_GAMEDLL) {
	CHL2MP_Player* pPlayer = nullptr;
	for (int i = 1; i <= gpGlobals->maxClients; i++) {
		pPlayer = ToHL2MPPlayer(UTIL_PlayerByIndex(i));
		if (pPlayer && pPlayer->IsFakeClient()) {
			Msg("%i. %20s : %s\n", i, pPlayer->GetPlayerName(), gBots[i].m_pCurThinker->m_ppszThinkerName);
		}
	}
}