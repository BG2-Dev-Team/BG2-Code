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
#include "bg3_scorepreserve.h"
#include "player_resource.h"
#include "vector"

using namespace std;

namespace NScorePreserve {
	ConVar sv_preserve_score_time("sv_preserve_score_time", "240", FCVAR_GAMEDLL | FCVAR_ARCHIVE | FCVAR_NOTIFY, "How long to remember scores of disconnected players, in seconds");

	//Score preservation data type
	struct scoreinfo_t {
		AccountID_t m_id;
		short m_score;
		short m_damage;
		float m_flRemovalTime;

		scoreinfo_t(AccountID_t id, short score, short damage, float removalTime) : m_id(id), m_score(score), m_damage(damage), m_flRemovalTime(removalTime) {}
	};

	//global list of scores to preserve
	vector<scoreinfo_t> g_scores;

	/***********************************************************
	* Removes old players after their set time
	***********************************************************/
	void Think() {
		//we're using it as a queue really
		while (g_scores.size() > 0 && g_scores[0].m_flRemovalTime < gpGlobals->curtime) {
			g_scores.erase(g_scores.begin());
		}
	}

	/***********************************************************
	* Restores player's score if such information is available
	***********************************************************/
	void NotifyConnected(int iPlayerIndex) {
		CBasePlayer* pPlayer = UTIL_PlayerByIndex(iPlayerIndex);
		if (pPlayer) {
			CSteamID newID;
			if (pPlayer->GetSteamID(&newID)) {
				//we have a valid ID from the new player

				//see if we have his ID in the score list somewhere
				int foundIndex = -1;
				uint i = 0;
				while (foundIndex == -1 && i < g_scores.size()) {
					if (g_scores[i].m_id == newID.GetAccountID()) {
						foundIndex = i;
					}
					i++;
				}

				if (foundIndex != -1) {
					//alright we have his score, now just move it in
					pPlayer->AddPoints(g_scores[foundIndex].m_score, false);
					pPlayer->SetDeathCount(g_scores[foundIndex].m_damage);

					g_scores.erase(g_scores.begin() + foundIndex);
				}
			}
		}
	}

	/***********************************************************
	* Stores the player's score for potential later restoration
	***********************************************************/
	void NotifyDisconnected(int iPlayerIndex) {
		CBasePlayer* pPlayer = UTIL_PlayerByIndex(iPlayerIndex);
		if (pPlayer) {
			CSteamID id;

			if (pPlayer->GetSteamID(&id)) {
				short score = pPlayer->FragCount();
				short damage = pPlayer->DeathCount();
				float removalTime = gpGlobals->curtime + sv_preserve_score_time.GetFloat();

				//construct his info and put it onto the end
				g_scores.emplace_back(scoreinfo_t(id.GetAccountID(), score, damage, removalTime));
			}
		}
	}

	/***********************************************************
	* Makes server forget all the score information (for map start)
	***********************************************************/
	void Flush() {
		g_scores.clear();
	}

	CON_COMMAND(score_preserve_report, "Displays info on preserved scores\n") {
		Msg("(%i) scores available\n", g_scores.size());
		Msg("SteamID\t: score, damage\n");
		for (uint i = 0; i < g_scores.size(); i++) {
			scoreinfo_t& s = g_scores[i];
			Msg("%i\t:%i,%i\n", s.m_id, s.m_score, s.m_damage);
		}
	}
}