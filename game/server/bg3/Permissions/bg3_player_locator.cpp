#include "cbase.h"
#include "bg3_player_locator.h"
#include "hl2mp/hl2mp_player.h"
#include "memdbgon.h"

using namespace std;

inline int StringMatchSimple(const char* pszFirst, const char* pszSecond) {
	int match = 0;
	while (pszFirst[match] && pszSecond[match] && pszFirst[match] == pszSecond[match]) {
		match++;
	}
	return match;
}

static int GetStringMatchLength(string str, string searchTerm, bool* bExactMatch, bool bIgnoreCase = false) {
	size_t maxMatchLength = 0;
	*bExactMatch = false;

	//only bother searching if it's feasible
	if (str.size() >= searchTerm.size() && searchTerm.size() > 0) {

		//ignore case if necessary
		if (bIgnoreCase) {
			for (size_t i = 0; i < str.size(); i++) {
				str[i] = tolower(str[i]);
			}
			for (size_t i = 0; i < searchTerm.size(); i++) {
				searchTerm[i] = tolower(searchTerm[i]);
			}
		}

		//find longest match
		for (size_t i = 0; i < str.size(); i++) {
			size_t match = StringMatchSimple(str.c_str() + i, searchTerm.c_str());
			if (match > maxMatchLength) {
				maxMatchLength = match;
			}
		}
		*bExactMatch = maxMatchLength == str.size();
	}
	return maxMatchLength;
}

CHL2MP_Player* FindPlayerByName(const char* pszName) {
	CHL2MP_Player* pResult = NULL;
	int maxMatchLength = 0;
	bool bExactMatch = false;

	//first search for players, not ignoring case
	for (int i = 1; i <= gpGlobals->maxClients; i++) {
		CHL2MP_Player* pPlayer = ToHL2MPPlayer(UTIL_PlayerByIndex(i));
		if (pPlayer) {

			//find longest match
			int matchLength = GetStringMatchLength(pPlayer->GetPlayerName(), pszName, &bExactMatch);
			//Msg("%i, %s == %s\n", matchLength, pPlayer->GetPlayerName(), pszName);
			if (matchLength >= maxMatchLength) {
				maxMatchLength = matchLength;
				pResult = pPlayer;
			}

			//if we've found exact match, quit loop now
			if (bExactMatch) {
				break;
			}
		}
	}

	//check if we need to more freely ignore case
	if (!bExactMatch && maxMatchLength < 4) {
		for (int i = 1; i <= gpGlobals->maxClients; i++) {
			CHL2MP_Player* pPlayer = ToHL2MPPlayer(UTIL_PlayerByIndex(i));
			if (pPlayer) {

				//find longest match, ignoring case
				int matchLength = GetStringMatchLength(pPlayer->GetPlayerName(), pszName, &bExactMatch, true);
				if (matchLength > maxMatchLength) {
					maxMatchLength = matchLength;
					pResult = pPlayer;
				}

				//if we've found exact match, quit loop now
				if (bExactMatch) {
					break;
				}
			}
		}
	}

	//if we don't have exact match and match is short, ignore it
	if (!bExactMatch && maxMatchLength < 4) {
		pResult = NULL;
	}

	return pResult;
}

void GetPlayersFromString(CHL2MP_Player** pPlayerList, const char* pszString, CHL2MP_Player* pRequester) {
	//two possibilities - keyword or name
	if (pszString[0] == '@') {
		//@me
		if (Q_strcmp(pszString, "@me") == 0 && pRequester) {
			pPlayerList[0] = pRequester;
		}
		else if (Q_strcmp(pszString, "@aim") == 0 && pRequester) {
			//trace from requester's aim
			QAngle angles = pRequester->EyeAngles();
			Vector direction;
			AngleVectors(angles, &direction);
			direction.NormalizeInPlace(); direction *= 2048;

			trace_t t;
			Vector start = pRequester->Weapon_ShootPosition();
			UTIL_TraceLine(start + direction / 100, start + direction, MASK_SHOT, pRequester, COLLISION_GROUP_PLAYER, &t);
			if (t.m_pEnt && t.m_pEnt->IsPlayer()) {
				pPlayerList[0] = ToHL2MPPlayer(t.m_pEnt);
			}
		}
		else {
			bool(*pFunc)(CHL2MP_Player*) = NULL;
			//@all
			if (Q_strcmp(pszString, "@all") == 0) {
				pFunc = [](CHL2MP_Player* pPlayer){ return true; };
			}

			//@amer
			else if (Q_strcmp(pszString, "@amer") == 0) {
				pFunc = [](CHL2MP_Player* pPlayer){ return pPlayer->GetTeamNumber() == TEAM_AMERICANS; };
			}

			//@brit
			else if (Q_strcmp(pszString, "@brit") == 0) {
				pFunc = [](CHL2MP_Player* pPlayer){ return pPlayer->GetTeamNumber() == TEAM_BRITISH; };
			}

			//@spec
			else if (Q_strcmp(pszString, "@spec") == 0) {
				pFunc = [](CHL2MP_Player* pPlayer){ return pPlayer->GetTeamNumber() < TEAM_AMERICANS; };
			}

			//@bot
			else if (Q_strcmp(pszString, "@bot") == 0) {
				pFunc = [](CHL2MP_Player* pPlayer){ return pPlayer->IsFakeClient(); };
			}

			//@human
			else if (Q_strcmp(pszString, "@human") == 0) {
				pFunc = [](CHL2MP_Player* pPlayer){ return !pPlayer->IsFakeClient(); };
			}

			//@dead
			else if (Q_strcmp(pszString, "@dead") == 0) {
				pFunc = [](CHL2MP_Player* pPlayer){ return pPlayer->IsDead(); };
			}

			//@alive
			else if (Q_strcmp(pszString, "@alive") == 0) {
				pFunc = [](CHL2MP_Player* pPlayer){ return pPlayer->IsAlive(); };
			}

			//add all players who pass the test
			if (pFunc) {
				int index = 0;
				for (int i = 1; i <= gpGlobals->maxClients; i++) {
					CBasePlayer* pPlayer = UTIL_PlayerByIndex(i);
					if (pPlayer) {
						CHL2MP_Player* pHL2MP_Player = ToHL2MPPlayer(pPlayer);
						if (pFunc(pHL2MP_Player)) {
							pPlayerList[index++] = pHL2MP_Player;
						}
					}
				}
			}
		}	
	}
	else {
		CHL2MP_Player* pFoundPlayer = NULL;

		//find player by name or by index
		if (pszString[0] != '\0') {
			pFoundPlayer = FindPlayerByName(pszString);
		}

		if (!pFoundPlayer && pszString[0] == '#') {
			int id = atoi(pszString + 1) - 1;
			pFoundPlayer = ToHL2MPPlayer(UTIL_PlayerByIndex(id));
		}
		pPlayerList[0] = pFoundPlayer;
	}
}

CON_COMMAND(psearch, "") {
	CHL2MP_Player** pPlayerList = new CHL2MP_Player*[gpGlobals->maxClients];
	memset(pPlayerList, 0, gpGlobals->maxClients * sizeof(CHL2MP_Player*));
	
	GetPlayersFromString(pPlayerList, args[1], NULL);
	int i = 0;
	while (pPlayerList[i]) {
		Msg("%s\n", pPlayerList[i]->GetPlayerName());
		i++;
	}

	delete[] pPlayerList;
}