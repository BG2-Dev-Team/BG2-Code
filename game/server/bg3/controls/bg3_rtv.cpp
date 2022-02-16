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
#include "hl2mp/hl2mp_player.h"
#include "player_resource.h"
#include "bg3_voting_system.h"

ConVar sv_rtv_enabled("sv_rtv_enabled", "1", FCVAR_GAMEDLL | FCVAR_NOTIFY, "Whether or not the native RTV system is enabled.");
//ConVar sv_rtv_nomination("sv_rtv_enabled", "0", FCVAR_GAMEDLL | FCVAR_NOTIFY, "Whether or not the first player to RTV can select a map.");
ConVar sv_rtv_threshold("sv_rtv_threshold", "0.55", FCVAR_GAMEDLL | FCVAR_NOTIFY, "What ratio of players entering RTV is required to change the map.");
ConVar sv_rtv_delay("sv_rtv_delay", "120", FCVAR_GAMEDLL | FCVAR_NOTIFY, "How many seconds after map change or RTV cancellation can another RTV begin.");


namespace RtvSystem {
	CUtlVector<CSteamID> g_rtvs;
	bool g_bRtvTempDisable = false; //for the ime between a succesful RTV amd map change
	float g_flNextRtvTime = FLT_MAX; //when can players start RTVing again?

	bool RtvDisabled(CHL2MP_Player* pRequester) {
		if (!sv_rtv_enabled.GetBool()) {
			CSayPlayer(pRequester, "RTV disabled on server, see sv_rtv_enabled");
			return true;
		}
		if (g_bRtvTempDisable || nextlevel.GetString()[0]) {
			CSayPlayer(pRequester, "RTV not available at this time.");
			return true;
		}
		if (g_flNextRtvTime > gpGlobals->curtime) {
			CSayPlayer(pRequester, "Too soon after map change to RTV.");
			return true;
		}
		if (g_pPlayerResource->GetNumNonSpectators() < 3) {
			CSayPlayer(pRequester, "Too few players for RTV.");
			return true;
		}

		return false;
	}


	static ConVar sv_repeat_rtvs("sv_repeat_rtvs", "0", FCVAR_HIDDEN);
	void ReceiveRtvFromPlayer(CHL2MP_Player* pPlayer) {
		//if previous RTV was successful and we're still changing map, do nothing
		if (RtvDisabled(pPlayer))
			return;

		//get their steam id
		CSteamID id;
		pPlayer->GetSteamID(&id);

		//Only do anything with it if we haven't already received an RTV from this player
		if (!g_rtvs.HasElement(id) || sv_repeat_rtvs.GetBool()) {
			g_rtvs.AddToTail(id);

			//get these numbers as ints we can report
			int count = g_rtvs.Count();
			int required = sv_rtv_threshold.GetFloat() * g_pPlayerResource->GetNumNonSpectators();
			if (required < 2) required = 2;

			CSay("%s wants to rock the vote (%i/%i)", pPlayer->GetPlayerName(), count, required);

			//if we have enough, create a map election
			if (count >= required) {
				CSay("RTV successful! Creating election...");
				g_bRtvTempDisable = true;	

				g_pBritishVotingSystem->FlagForRtvMapElection(true);
				g_pBritishVotingSystem->AutoMapchoiceElectionThink(); //check immediately, tho if another vote is already ongoing it will happen later instead
				
			}
		}
	}

	void ClearRtvs() {
		g_rtvs.Purge();
		g_bRtvTempDisable = false;
		g_flNextRtvTime = gpGlobals->curtime + sv_rtv_delay.GetInt();
	}
}

CON_COMMAND_F(rtvcancel, "Cancels the ongoing RTV", FCVAR_GAMEDLL) {
	if (RtvSystem::g_rtvs.Count() > 0) {
		RtvSystem::ClearRtvs();
		CSay("Cancelled the RTV");
	}
	else {
		CSay("There is no RTV to cancel.");
	}
}