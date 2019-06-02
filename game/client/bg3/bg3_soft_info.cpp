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
#include "bg3_soft_info.h"
#include "../shared/bg3/bg3_class.h"
#include "hl2mp/c_hl2mp_player.h"

namespace NSoftInfo {

	static int g_iAmericanClassCounts[TOTAL_AMER_CLASSES];
	static int g_iBritishClassCounts[TOTAL_BRIT_CLASSES];

	static float g_flNextUpdateTime = -FLT_MAX;

	void Reset() {
		g_flNextUpdateTime = -FLT_MAX;
		Update();
	}

	void UpdateClasses() {
		memset(g_iAmericanClassCounts, 0, sizeof(g_iAmericanClassCounts));
		memset(g_iBritishClassCounts, 0, sizeof(g_iBritishClassCounts));
		for (int i = 1; i <= gpGlobals->maxClients; i++) {
			CHL2MP_Player* pPlayer = ToHL2MPPlayer(UTIL_PlayerByIndex(i));
			if (pPlayer && pPlayer->GetNextClass() >= 0) {
				if (pPlayer->GetTeamNumber() == TEAM_AMERICANS) {
					g_iAmericanClassCounts[pPlayer->GetNextClass()]++;
				}
				else if (pPlayer->GetTeamNumber() == TEAM_BRITISH) {
					g_iBritishClassCounts[pPlayer->GetNextClass()]++;
				}
			}
		}
	}

	static const float UPDATE_INTERVAL = 1.f;
	void Update() {
		if (gpGlobals->curtime > g_flNextUpdateTime) {
			g_flNextUpdateTime = gpGlobals->curtime + UPDATE_INTERVAL;

			UpdateClasses();
		}
	}

	int GetNumPlayersOfClass(const CPlayerClass* pClass) {
		if (pClass->m_iDefaultTeam == TEAM_AMERICANS)
			return g_iAmericanClassCounts[pClass->m_iClassNumber];
		else
			return g_iBritishClassCounts[pClass->m_iClassNumber];
			
	}
}