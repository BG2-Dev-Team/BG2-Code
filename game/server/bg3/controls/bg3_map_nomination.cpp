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
#include "hl2mp/hl2mp_player.h"
#include "player_resource.h"
#include "../shared/bg3/controls/bg3_voting_system_shared.h"
#include "../shared/bg3/bg3_map_model.h"
#include "../shared/bg3/bg3_math_shared.h"
#include "filesystem.h"
#include <vector>
#include <string>

using namespace std;

ConVar sv_vote_mapchoice_medium_threshold("sv_vote_mapchoice_medium_threshold", "10", FCVAR_GAMEDLL | FCVAR_NOTIFY, "This number of clients or above uses the public_medium keyvalues in the mapelections.res");
ConVar sv_vote_mapchoice_large_threshold("sv_vote_mapchoice_large_threshold", "22", FCVAR_GAMEDLL | FCVAR_NOTIFY, "This number of clients or above uses the public_large keyvalues in the mapelections.res");

template<class T>
bool VectorContainsValue(vector<T>& vec, const T& value) {
	for (size_t i = 0; i < vec.size(); i++) {
		if (vec[i] == value) {
			return true;
		}
	}
	return false;
}

void GetCurrentMapNameSanitized(std::string& currentMapName) {
	const char* psz = HL2MPRules()->MapName();
	if (psz[0] != 'm') {
		Warning(__FUNCTION__ " could not parse map name %s\n", psz);
		Warning(__FUNCTION__ " could not parse map name %s\n", psz);
		Warning(__FUNCTION__ " could not parse map name %s\n", psz);
	}
	else {
		char buffer[MAX_MAP_NAME];
		strcpy_s(buffer, psz + 5); //the +5 skips the maps/ prefix

		//find the .bsp extension and replace the . with null character
		int index = 0;
		char* c = &buffer[0];
		while (*c && index < MAX_MAP_NAME) {
			if (*c == '.') {
				*c = '\0';
				break;
			}
			c++;
			index++;
		}

		//now ready to give it to string constructor
		currentMapName = buffer;
	}
}

namespace NMapNomination {
	//Eight lists
	// 1. Complete list of official maps
	// 2. List of bg_ maps for votemap bg
	// 3. List of ctf_ maps for votemap ctf
	// 4. List of sg_ maps for votemap sg
	// 5. List of lb_ maps for votemap lb
	// 6. List of large maps for large player counts
	// 7. List of medium maps for medium player counts
	// 8. List of small maps for small player counts

	vector<string> g_completeOfficialMapList = {
		BG3_OFFICIAL_MAP_LIST
	};

	vector<string> g_bgMapList = {
		"bg_abbey",
		"bg_ambush",
		"bg_blackswamp",
		"bg_canal",
		"bg_fort_stmathieu",
		"bg_freemans_farm",
		"bg_horseshoe",
		"bg_maricopa",
		"bg_plateau",
		"bg_snowlake",
		"bg_townguard",
		"bg_trenton",
		"bg_winterisle",
		"bg_woodland",
		"bg_wrecked"
	};

	vector<string> g_ctfMapList = {
		"ctf_mill_creek",
		"ctf_stonefort",
		"ctf_river",
		"ctf_road",
	};

	vector<string> g_sgMapList = {
		"sg_boston",
		"sg_deroga",
		"sg_fall",
		"sg_ravine",
		"sg_siege"
	};

	vector<string> g_lbMapList = {
		"lb_alpinepass",
		"lb_autumn",
		"lb_battleofconcord",
		"lb_battleofvalcourisland",
		"lb_continental",
		"lb_nativevalley",
		"lb_princeton"
	};

	vector<string> g_largeOfficialMapList = {
		"bg_abbey",
		"bg_blackswamp",
		"bg_canal",
		"bg_fort_stmathieu",
		"bg_freemans_farm",
		"bg_maricopa",
		"bg_plateau",
		"bg_snowlake",
		"bg_townguard",
		"bg_woodland",
		"ctf_mill_creek",
		"ctf_stonefort",
		"sg_boston",
		"sg_deroga",
		"sg_fall",
		"sg_ravine",
		"sg_siege"
	};

	static vector<string> g_mediumOfficialMapList = {
		"bg_abbey",
		"bg_blackswamp",
		"bg_canal",
		"bg_fort_stmathieu",
		"bg_horseshoe",
		"bg_maricopa",
		"bg_townguard",
		"bg_trenton",
		"bg_winterisle",
		"bg_woodland",
		"bg_wrecked",
		"ctf_mill_creek",
		"ctf_river",
		"ctf_road",
		"ctf_stonefort",
		"sg_boston",
		"sg_fall",
		"sg_ravine",
		"sg_siege"
	};

	static vector<string> g_smallOfficialMapList = {
		"bg_ambush",
		"bg_horseshoe",
		"bg_winterisle",
		"bg_wrecked",
		"ctf_river",
		"ctf_road",
		//"sq",
	};

	struct SMapNomination {
		string m_mapName;
		CSteamID m_player;
	};

	static vector<SMapNomination> g_mapNominations;

	//list of every map that's on the server that could ever possibly appear in map elections
	//includes official maps and maps included in mapelections.res
	//excludes all other maps, ex. sq, training maps, test maps, etc
	static vector<string> g_completeNominatableMapList; 
	

	bool MapIsOfficial(const char* pszMapName) {
		return VectorContainsValue<string>(g_completeOfficialMapList, pszMapName);
	}

	bool MapIsNominatable(const char* pszMapName) {
		return VectorContainsValue<string>(g_completeNominatableMapList, pszMapName);
	}

	bool NominateMap(const char* pszMapName, CHL2MP_Player* pNominator) {
		//assume true, then check otherwise
		bool validNomination = true;
		string failMessage = "";
		string currentMapName;
		SMapNomination* pOldNomination = NULL;

		SMapNomination nom = { pszMapName, CSteamID() };
		pNominator->GetSteamID(&nom.m_player);
		

		//one step at a time
		//first verify that the map exists
		if (!CMapInfo::MapExists(pszMapName)) {
			validNomination = false;
			failMessage = "That map does not exist";
			goto end;
		}

		 //look for old nomination from the same player
		//verify that this person or the map hasn't nominated yet
		for (size_t i = 0; i < g_mapNominations.size(); i++) {
			SMapNomination& n = g_mapNominations[i];
			if (n.m_player == nom.m_player) {
				pOldNomination = &n;
			}
			if (n.m_mapName == nom.m_mapName) {
				validNomination = false;
				failMessage = "That map is already nominated";
				goto end;
			}
		}

		//verify that the given map is in our nominatable list, so as to exclude private training and test maps
		if (!VectorContainsValue(g_completeNominatableMapList, nom.m_mapName)) {
			validNomination = false;
			failMessage = "This map exists but is not nominatable";
			goto end;
		}

		GetCurrentMapNameSanitized(currentMapName);
		if (nom.m_mapName == currentMapName) {
			validNomination = false;
			failMessage = "The current map cannot be nominated";
			goto end;
		}

	end:
		if (validNomination) {
			//if old nomination exists, modify that one instead
			if (pOldNomination) {
				pOldNomination->m_mapName = nom.m_mapName;
				CSay("%s changed their nomination to %s", pNominator->GetPlayerName(), pszMapName);
			}
			else {
				g_mapNominations.emplace_back(nom);
				CSay("%s has nominated %s", pNominator->GetPlayerName(), pszMapName);
			}
		}
		else {
			CSayPlayer(pNominator, failMessage.c_str());
		}

		return validNomination;
	}

	void ClearNominations() {
		g_mapNominations.clear();
	}

	void GetFilteredMapElectionOptions(vector<string>& resultList, vector<string>& filter, bool bFilterNominations) {
		resultList.clear();

		bool bIsLbElection = &filter == &g_lbMapList || IsLinebattle();
		bool bServerHasAdmin = Permissions::ServerHasAdmin();
		bool bAllowLbNominations = bIsLbElection || bServerHasAdmin;

		string currentMapName;
		GetCurrentMapNameSanitized(currentMapName);

		//first get any nominated maps in this category
		for (size_t i = 0; i < g_mapNominations.size(); i++) {

			string& name = g_mapNominations[i].m_mapName;

			bool isLbMap = name.length() >= 2 && name[0] == 'l' && name[1] == 'b';
			if (isLbMap && !bAllowLbNominations)
				continue;

			//no voting for the current map
			if (currentMapName == name)
				continue;

			if (!bFilterNominations || VectorContainsValue<string>(filter, name)) {
				Msg("Adding nomination %s\n", name.c_str());
				resultList.emplace_back(name);
			}
		}

		int remainingMaps = filter.size() - resultList.size();

		bool bSkippedCurrentMap = false;

		//while loop inserts up to 6 maps into list
		int loopCount = 0; //sanity checker to prevent any infinite loop situations that may have slipped through
		while (resultList.size() < 6 && remainingMaps > 0 && loopCount < 2000) {
			//find an index we haven't used yet
			int index = RndInt(0, filter.size() - 1);
			while (VectorContainsValue(resultList, filter[index])) {
				index = RndInt(0, filter.size() - 1);
			}

			//edge case -- if we randomly pick the same map as what's already on the server,
			//then do NOT add it to the list of results. However, we still have to decrement
			//remaininMaps or else the above loop could run infinitely (when the number of nominated maps + number of maps in filter = 6)
			//however, we also have make sure that in the case of skipping the current map, it is only decremented ONCE, or else
			//this algorithm will think there are fewer maps available in the filter than there actually are.
			//we use bSkippedCurrentMap to keep track of whether or not we decremented remainingMaps for the currentMapName
			if (filter[index] == currentMapName) {
				if (!bSkippedCurrentMap) {
					remainingMaps--;
					bSkippedCurrentMap = true;
				}
				//if we picked the current map name again, do nothing
			}
			else {
				resultList.emplace_back(filter[index]);
				remainingMaps--;
			}
			loopCount++; //sanity checker to prevent infinite loops in case I missed any such situations
		}

		ClearNominations();
		CSay("Generated %i map options", (int)resultList.size());
	}

	void GetRandomLbMapList(vector<string>& list) {
		CSay("Finding lb maps...");
		GetFilteredMapElectionOptions(list, g_lbMapList, true);
	}

	void GetRandomBgMapList(vector<string>& list) {
		CSay("Finding bg maps...");
		GetFilteredMapElectionOptions(list, g_bgMapList, true);
	}

	void GetRandomSgMapList(vector<string>& list) {
		CSay("Finding sg maps...");
		GetFilteredMapElectionOptions(list, g_sgMapList, true);
	}

	void GetRandomCtfMapList(vector<string>& list) {
		CSay("Finding ctf maps...");
		GetFilteredMapElectionOptions(list, g_ctfMapList, true);
	}

	void GetRandomPublicMapList(vector<string>& list) {
		//choose map list to pick from based on player count and gamemode
		int num = g_pPlayerResource->GetNumNonSpectators();

		if (num >= 18 && IsLinebattle()) {
			GetFilteredMapElectionOptions(list, g_lbMapList, false);
		}
		else if (num >= sv_vote_mapchoice_large_threshold.GetInt()) {
			GetFilteredMapElectionOptions(list, g_largeOfficialMapList, false);
		}
		else if (num >= sv_vote_mapchoice_medium_threshold.GetInt()) {
			GetFilteredMapElectionOptions(list, g_mediumOfficialMapList, false);
		}
		else {
			GetFilteredMapElectionOptions(list, g_smallOfficialMapList, false);
		}
	}

	void LoadCustomMapKvsToVector(KeyValues* kv, vector<string>& list) {
		for (KeyValues *pKey = kv->GetFirstSubKey(); pKey; pKey = pKey->GetNextKey()) {
			if (!CMapInfo::MapExists(pKey->GetName())) {
				Msg("\tERROR could not add map %s to nomination list because it does not exist, skipping. Check spelling\n", pKey->GetName());
				continue;
			}

			//enforce map election fairness by making sure a map does not appear more than once in the list
			//also improves future performance
			string mapName = pKey->GetName();
			if (VectorContainsValue(list, mapName)) {
				Msg("\tSKIPPING custom map %s because it is already in the nomination list for its category\n", pKey->GetName());
			}
			else {
				Msg("\tAdding custom map %s to nomination list\n", pKey->GetName());
				list.emplace_back(pKey->GetName());
			}
		}

		//add everything in this list to the global list of map nominations
		for (size_t i = 0; i < list.size(); i++) {
			if (!VectorContainsValue(g_completeNominatableMapList, list[i])) {
				g_completeNominatableMapList.emplace_back(list[i]);
			}
		}
	}

	void LoadCustomMapOptions() {
		
		KeyValues* kv = new KeyValues("kv");
		kv->Clear();
		kv->LoadFromFile(filesystem, "cfg/mapelections.res");
		
		KeyValues* kvList = kv->FindKey("lb");
		if (kvList) {
			LoadCustomMapKvsToVector(kvList, g_lbMapList);
		}
		kvList = kv->FindKey("bg");
		if (kvList) {
			LoadCustomMapKvsToVector(kvList, g_bgMapList);
		}
		kvList = kv->FindKey("sg");
		if (kvList) {
			LoadCustomMapKvsToVector(kvList, g_sgMapList);
		}
		kvList = kv->FindKey("ctf");
		if (kvList) {
			LoadCustomMapKvsToVector(kvList, g_ctfMapList);
		}
		kvList = kv->FindKey("public_large");
		if (kvList) {
			LoadCustomMapKvsToVector(kvList, g_largeOfficialMapList);
		}
		kvList = kv->FindKey("public_medium");
		if (kvList) {
			LoadCustomMapKvsToVector(kvList, g_mediumOfficialMapList);
		}
		kvList = kv->FindKey("public_small");
		if (kvList) {
			LoadCustomMapKvsToVector(kvList, g_smallOfficialMapList);
		}

		kv->deleteThis();
	}
}
