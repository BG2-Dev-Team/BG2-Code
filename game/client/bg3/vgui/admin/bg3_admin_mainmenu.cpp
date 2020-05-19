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
#include "bg3_admin_submenu.h"
#include "../shared/hl2mp/hl2mp_gamerules.h"

const char* GetPlayerActionTitle(AdminMenuPlayerAction action) {
	switch (action)
	{
	case AdminMenuPlayerAction::Mute:
		return "Mute";
	case AdminMenuPlayerAction::Unmute:
		return "Unmute"; 
	case AdminMenuPlayerAction::Spawn:
		return "Spawn";
	case AdminMenuPlayerAction::Slay:
		return "Slay";
	case AdminMenuPlayerAction::Kick:
		return "Kick";
	case AdminMenuPlayerAction::Ban60:
		return "Ban 60 Minutes";
	case AdminMenuPlayerAction::BanIndef:
		return "Ban indefinitely";
	default:
		return "INVALID ACTION SELECTION";
	}
}

const char* GetPlayerActionCommand(AdminMenuPlayerAction action) {
	switch (action)
	{
	case AdminMenuPlayerAction::Mute:
		return "mute %s";
	case AdminMenuPlayerAction::Unmute:
		return "unmute %s";
	case AdminMenuPlayerAction::Spawn:
		return "spawn %s";
	case AdminMenuPlayerAction::Slay:
		return "slay %s";
	case AdminMenuPlayerAction::Kick:
		return "rkick %s";
	case AdminMenuPlayerAction::Ban60:
		return "rban %s 60";
	case AdminMenuPlayerAction::BanIndef:
		return "rban %s";
	default:
		return "INVALID ACTION SELECTION";
	}
}

CAdminSubMenu* CreateDynamicMenu(const std::vector<CAdminSubMenu*> &elements, int elementsPerPage, const char* menuTitle) {
	int numMaps = elements.size();
	int menuPagesCount = Ceil2Int((float)numMaps / (float)elementsPerPage);
	CAdminSubMenu** menuPages = new CAdminSubMenu*[menuPagesCount];
	for (int i = 0; i < menuPagesCount; i++) {
		menuPages[i] = new CAdminSubMenu;
		bool isLastPage = (i + 1 == menuPagesCount);
		int childCount = 0;
		if (isLastPage) {
			//We need the last remaining player +1 Menuentry for back
			int modulo = numMaps % elementsPerPage;
			//But only if the we not by chance got a number of players % PLAYER_PER_PAGE = 0
			if (modulo != 0){
				childCount = (numMaps % elementsPerPage) + 1;
			}
			else {
				childCount = elementsPerPage + 1;
			}
		}
		else {
			//Otherwise we have a full page of players plus 2 options for back and next page
			childCount = elementsPerPage + 2;
		}
		menuPages[i]->m_aChildren = new CAdminSubMenu*[childCount];
		menuPages[i]->m_iNumChildren = childCount;
		menuPages[i]->m_aChildren[0] = new CAdminSubMenu("#BG3_Adm_Back", "");
		menuPages[i]->m_aChildren[0]->m_pszFunc = [](uint8, CAdminSubMenu*)->bool { return true; };
		menuPages[i]->m_pszLineItemText = "#BG3_Adm_Next";
		menuPages[i]->m_pszTitle = menuTitle;
		int j = 0;
		while ((j < elementsPerPage) && ((i * elementsPerPage + j) < numMaps)){
			menuPages[i]->m_aChildren[j + 1] = elements.at(i * elementsPerPage + j);
			j++;
		}
	}
	for (int i = 0; i < menuPagesCount - 1; i++) {
		//All Fullpages get the next Page as their last Child
		menuPages[i]->m_aChildren[9] = menuPages[i + 1];
	}

	menuPages[0]->m_pszLineItemText = menuTitle;
	auto result = menuPages[0];
	delete[] menuPages;
	menuPages = NULL;
	return result;
}

CAdminPlayerSubMenu* CreatePlayerActionMenuEntry(AdminMenuPlayerAction action, const char* Playername, int PlayerID) {
	const char* command = GetPlayerActionCommand(action);
	CAdminPlayerSubMenu* result = new CAdminPlayerSubMenu(Playername, "");
	result->m_pszLineItemText = Playername;
	result->m_pszTitle = command;
	result->m_iPlayerID = PlayerID;
	result->m_pszFunc = [](uint8, CAdminSubMenu* pSelf) { 
		int playerId = static_cast<CAdminPlayerSubMenu*>(pSelf)->m_iPlayerID;
		char idBuffer[5]; //we have to put a # symbol in front of player id in order for the name matching to work
		Q_snprintf(idBuffer, sizeof(idBuffer), "#%i", playerId);
		SayServerCommand(pSelf->m_pszTitle, idBuffer);
		return false; 
	};
	return result;
}

CAdminSubMenu* CreatePlayerActionMenu(AdminMenuPlayerAction action, std::vector<int> &playerIDs) {
	std::vector<CAdminSubMenu*> playerItems;
	for (auto playerID = playerIDs.begin(); playerID != playerIDs.end(); ++playerID) {
		CAdminPlayerSubMenu* item = CreatePlayerActionMenuEntry(action, g_PR->GetPlayerName(*playerID), *playerID);
		playerItems.push_back(item);
	}
	return CreateDynamicMenu(playerItems, PLAYER_PER_PAGE, GetPlayerActionTitle(action));
}

void ClearMenu(CAdminSubMenu* subMenu, const int maxChildren) {
	if (subMenu->m_aChildren) {
		for (int i = 0; i < maxChildren; i++) {
			delete subMenu->m_aChildren[i];
		}
		delete[] subMenu->m_aChildren;
	}
}

void CreatePlayerActionTopMenu(CAdminSubMenu* pPlayerActionMenu) {
	const int NUM_CHILDREN = PLAYER_PER_PAGE;

	//delete any existing old data
	ClearMenu(pPlayerActionMenu, NUM_CHILDREN);

	std::vector <int> playerIDs;
	for (int i = 1; i <= gpGlobals->maxClients; i++) {
		if (g_PR->IsConnected(i)) {
			playerIDs.push_back(i);
		}
	}
	

	pPlayerActionMenu->m_iNumChildren = NUM_CHILDREN;
	pPlayerActionMenu->m_aChildren = new CAdminSubMenu*[pPlayerActionMenu->m_iNumChildren];
	pPlayerActionMenu->m_aChildren[0] = new CAdminSubMenu("#BG3_Adm_Back", "");
	pPlayerActionMenu->m_aChildren[0]->m_pszFunc = [](uint8, CAdminSubMenu*)->bool { return true; };
	pPlayerActionMenu->m_aChildren[1] = CreatePlayerActionMenu(AdminMenuPlayerAction::Mute, playerIDs);
	pPlayerActionMenu->m_aChildren[2] = CreatePlayerActionMenu(AdminMenuPlayerAction::Unmute, playerIDs);
	pPlayerActionMenu->m_aChildren[3] = CreatePlayerActionMenu(AdminMenuPlayerAction::Spawn, playerIDs);
	pPlayerActionMenu->m_aChildren[4] = CreatePlayerActionMenu(AdminMenuPlayerAction::Slay, playerIDs);
	pPlayerActionMenu->m_aChildren[5] = CreatePlayerActionMenu(AdminMenuPlayerAction::Kick, playerIDs);
	pPlayerActionMenu->m_aChildren[6] = CreatePlayerActionMenu(AdminMenuPlayerAction::Ban60, playerIDs);
	pPlayerActionMenu->m_aChildren[7] = CreatePlayerActionMenu(AdminMenuPlayerAction::BanIndef, playerIDs);
}

CAdminSubMenu* CreateMapMenuEntry(const char * mapName) {
	CAdminSubMenu* result = new CAdminSubMenu(mapName, "");
	result->m_pszLineItemText = mapName;
	result->m_pszFunc = [](uint8, CAdminSubMenu* pSelf)
	{ SayServerCommand("changemap %s", pSelf->m_pszLineItemText); 
	return false; };
	return result;
}

CAdminSubMenu* CreateMapMenu(const std::vector<const char*> &maps) {
	std::vector<CAdminSubMenu*> mapItems;
	for (auto map = maps.begin(); map != maps.end(); ++map) {
		CAdminSubMenu* item = CreateMapMenuEntry(*map);
		mapItems.push_back(item);
	}
	return CreateDynamicMenu(mapItems, MAPS_PER_PAGE, "Select Map");
}

//ADMIN MENU CONSTRUCTOR - THIS DEFINES THE MAIN STRUCTURE OF THE ENTIRE MENU
static CAdminMainMenu* g_pAdminMainMenu = NULL;
CAdminMainMenu::CAdminMainMenu() {
	m_pszTitle = "Admin Menu Version 0.2 Alpha";

	//placeholder menu does nothing but occupy a functionless spot
	//CAdminSubMenu* pPlaceholderMenu				= new CAdminSubMenu; 

	//general-purpose back button option
	CAdminSubMenu* pBackButton					= g_pAdminBackButton = new CAdminSubMenu("#BG3_Adm_Back", "");
	pBackButton->m_pszFunc = [](uint8, CAdminSubMenu*)->bool { return true; };

	//general-purpose cancel function does same as back button, just different label
	CAdminSubMenu* pCancelButton = new CAdminSubMenu("#BG3_Adm_Cancel", "");
	pBackButton->m_pszFunc = [](uint8, CAdminSubMenu*)->bool { return true; };

	//Admin menu children
	CAdminSubMenu* pModeMenu					= new CAdminSubMenu("Gamemode", "Choose Gamemode");
	CAdminSubMenu* pMapMenu						= new CAdminSubMenu("Change Map", "Select Map (default maps only)");
	CAdminSubMenu* pPlayerActionMenu			= new CAdminSubMenu("Player Actions", "Select Player Action");
	CAdminSubMenu* pOptionsMenu					= new CAdminSubMenu("#BG3_Adm_GameOptions", "#BG3_Adm_GameOptions");
	CAdminSubMenu* pCustomCFGMenu				= new CAdminSubMenu("Community CFGs", "Select Custom CFG");
	CAdminSubMenu* pTeamwideClassmenu			= new CAdminSubMenu("#BG3_Teamwide_Classmenu", "");
	pTeamwideClassmenu->m_pszFunc = [](uint8, CAdminSubMenu*)->bool { if (IsLinebattle()) engine->ClientCmd("teamkitmenu");  return false; };

	//CMapMenu* pMapMenu						= new CMapMenu;
	//CPlayerActionMenu* pPlayerActionMenu		= new CPlayerActionMenu;
	m_iNumChildren = 7;
	CAdminSubMenu** pChildren = m_aChildren		= new CAdminSubMenu*[m_iNumChildren]; {
		pChildren[static_cast<int>(AdminMenuOptionsPositons::Cancel)] = pCancelButton;
		pChildren[static_cast<int>(AdminMenuOptionsPositons::Mode)] = pModeMenu;
		pChildren[static_cast<int>(AdminMenuOptionsPositons::Map)] = pMapMenu;
		pChildren[static_cast<int>(AdminMenuOptionsPositons::PlayerAction)] = pPlayerActionMenu;
		pChildren[static_cast<int>(AdminMenuOptionsPositons::Options)] = pOptionsMenu;
		pChildren[static_cast<int>(AdminMenuOptionsPositons::CustomCFG)] = pCustomCFGMenu;
		pChildren[static_cast<int>(AdminMenuOptionsPositons::Class)] = pTeamwideClassmenu;
	};

#define set(i, lineItemText, title) \
	pChildren[i] = new CAdminSubMenu(lineItemText, title);\
	pChildren[i]->m_pszFunc = [](uint8 slot, CAdminSubMenu*)->bool

	//MAP MENU

	//MODE MENU
	pModeMenu->m_iNumChildren = 9;
	pChildren = pModeMenu->m_aChildren = new CAdminSubMenu*[pModeMenu->m_iNumChildren]; {
		//back
		pChildren[0] = pBackButton;

		//linebattle
		set(1, "Linebattle 10 Rounds", "")
		{ SayServerCommand("linebattle 10"); return false; };

		set(2, "Linebattle 30 Rounds", "")
		{ SayServerCommand("linebattle 30"); return false; };

		//skirm
		set(3, "Skirmish 20 Minutes", "")
		{ SayServerCommand("skirm 20"); return false; };

		set(4, "Skirmish 30 Minutes", "")
		{ SayServerCommand("skirm 30"); return false; };

		//lms
		set(5, "LMS 10 Rounds", "")
		{ SayServerCommand("lms 10"); return false; };

		set(6, "LMS 30 Rounds", "")
		{ SayServerCommand("lms 10"); return false; };

		//dm
		set(7, "Deathmatch 10 Rounds", "")
		{ SayServerCommand("dm 10"); return false; };

		set(8, "Deathmatch 30 Rounds", "")
		{ SayServerCommand("dm 30"); return false; };
	}

	//Player Action Menu


	//OPTIONS MENU
	pOptionsMenu->m_iNumChildren = 10;
	pChildren = pOptionsMenu->m_aChildren = new CAdminSubMenu*[pOptionsMenu->m_iNumChildren]; {

		//Back
		pChildren[0] = pBackButton;

		set(1, "Toggle Friendly Fire", "") { 
			extern ConVar mp_friendlyfire;
			SayServerCommand("mp_friendlyfire_toggle");
			return false;
		};

		set(2, "Toggle Alltalk", "") { 
			extern ConVar sv_alltalk;
			bool val = !sv_alltalk.GetBool();
			SayServerCommand("rc sv_alltalk %i", val);
			const char* pszMessage = val == false ? "msay #BG3_Adm_AT_Off" : "msay #BG3_Adm_AT_On";
			engine->ServerCmd(pszMessage);
			return false;
		};

		set(3, "Toggle Bot Population Manager", "") {
			extern ConVar bot_minplayers_mode;
			int val = bot_minplayers_mode.GetBool() ? 0 : 3;
			SayServerCommand("rc bot_minplayers_mode %i", val);
			return false;
		};

		set(4, "Add 2 Bots", "") {
			SayServerCommand("bot_add_a 1");
			SayServerCommand("bot_add_b 1");
			return false;
		};

		set(5, "Remove All Bots", "") {
			SayServerCommand("bot_kick_all");
			return false;
		};

		set(6, "Toggle Bot Voice Communications", "") {
			extern ConVar bot_vcomms;
			int origVal = bot_vcomms.GetInt();
			int newVal = !origVal;
			Msg("Changing from %i to %i\n", origVal, newVal);
			SayServerCommand("rc bot_vcomms %i", !bot_vcomms.GetBool());
			return false;
		};

		set(7, "Restart Round", "") {
			SayServerCommand("rc sv_restartround 1");
			return false;
		};

		set(8, "Run default CFG for current map", "") {
			Msg("Level name is %s", engine->GetLevelName());
			//HACK HACK - get map name from GetLevelName()
			char buffer[128];
			strcpy_s(buffer, engine->GetLevelName() + 5); //the +5 gets us past the maps/ prefix
			//find the . and make that null
			for (int i = 0; i < 128; i++) {
				if (buffer[i] == '.') {
					buffer[i] = '\0';
					break;
				}
			}
			SayServerCommand("rc exec %s", buffer);
			return false;
		};

		//GAME OPTIONS PAGE 2
		pChildren[9] = new CAdminSubMenu("#BG3_Adm_Next", "#BG3_Adm_GameOptions");
		pChildren[9]->m_iNumChildren = 6;
		pChildren = pChildren[9]->m_aChildren = new CAdminSubMenu*[pChildren[9]->m_iNumChildren]; {

			pChildren[0] = pBackButton;

			set(1, "Swap Teams", "") {
				SayServerCommand("swap_teams");
				return false;
			};

			set(2, "Toggle Moon Gravity", "") {
				extern ConVar sv_gravity;
				int val = sv_gravity.GetInt() == 600 ? 100 : 600;
				SayServerCommand("rc sv_gravity %i", val);
				engine->ServerCmd("msay #BG3_Adm_GravityChanged");
				return false;
			};

			set(3, "Remove All Class Limits (limits reset on map change)", "") {
				SayServerCommand("remove_class_limits");
				return false;
			};

			set(4, "Restart Game/Match/Map", "") {
				SayServerCommand("rc sv_restartmap 1");
				return false;
			};

			set(5, "Toggle Team Autobalance", "") {
				extern ConVar mp_autobalanceteams;
				SayServerCommand("rc mp_autobalanceteams %i", (int) !mp_autobalanceteams.GetBool());
				return false;
			};
		}
	}

	pCustomCFGMenu->m_iNumChildren = 9;
	pChildren = pCustomCFGMenu->m_aChildren = new CAdminSubMenu*[pCustomCFGMenu->m_iNumChildren]; {
		pChildren[0] = pBackButton;

		set(1, "1v1 Match", "") {
			SayServerCommand("rc exec 1v1"); return false;
		};
		set(2, "2v2 Match", "") {
			SayServerCommand("rc exec 2v2"); return false;
		};
		set(3, "Public Linebattle", "") {
			SayServerCommand("rc exec PubLB"); return false;
		};
		set(4, "Competitive Linebattle", "") {
			SayServerCommand("rc exec CompLB"); return false;
		};
		set(5, "Competitive Siege", "") {
			SayServerCommand("rc exec CompSG"); return false;
		};
		set(6, "Competitive Skirmish", "") {
			SayServerCommand("rc exec CompSk"); return false;
		};
		set(7, "Close Server", "") {
			SayServerCommand("rc exec CloseServer"); return false;
		};
		set(8, "Open Server", "") {
			SayServerCommand("rc exec OpenServer"); return false;
		};
	}
#undef set
}

CAdminMainMenu* CAdminMainMenu::Get() {
	if (!g_pAdminMainMenu) {
		g_pAdminMainMenu = new CAdminMainMenu();
	}
	CreatePlayerActionTopMenu(g_pAdminMainMenu->m_aChildren[static_cast<int>(AdminMenuOptionsPositons::PlayerAction)]);

	ClearMenu(g_pAdminMainMenu->m_aChildren[static_cast<int>(AdminMenuOptionsPositons::Map)], MAPS_PER_PAGE);
	std::vector<const char*> mapList;
	mapList.push_back("bg_ambush");
	mapList.push_back("bg_freemans_farm");
	mapList.push_back("bg_maricopa");
	mapList.push_back("bg_plateau");
	mapList.push_back("bg_townguard");
	mapList.push_back("bg_trenton");
	mapList.push_back("bg_woodland");
	mapList.push_back("ctf_mill_creek");
	mapList.push_back("ctf_road");
	mapList.push_back("ctf_stonefort");
	mapList.push_back("lb_alpinepass");
	mapList.push_back("lb_battleofconcord");
	mapList.push_back("lb_battleofvalcourisland");
	mapList.push_back("lb_continental");
	mapList.push_back("lb_nativevalley");
	mapList.push_back("sg_fall");
	mapList.push_back("sg_siege");
	mapList.push_back("sq");

	g_pAdminMainMenu->m_aChildren[static_cast<int>(AdminMenuOptionsPositons::Map)] = CreateMapMenu(mapList);
	return g_pAdminMainMenu;
}
