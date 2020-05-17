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

CAdminPlayerSubMenu* CreatePlayerActionMenuEntry(AdminMenuPlayerAction action, const char* Playername, int PlayerID) {
	const char* command = GetPlayerActionCommand(action);
	CAdminPlayerSubMenu* result = new CAdminPlayerSubMenu(Playername, "");
	result->m_pszLineItemText = Playername;
	result->m_pszTitle = command;
	result->m_iPlayerID = PlayerID;
	result->m_pszFunc = [](uint8, CAdminSubMenu* pSelf)
	{ return true; };
	return result;
}

CAdminSubMenu* CreatePlayerActionMenu(AdminMenuPlayerAction action) {
	int numClients = 0;
	for (int i = 1; i <= gpGlobals->maxClients; i++) {
		//Trick to get the real Playercount not the maximal possible children
		if (g_PR->IsConnected(i)) {
			numClients++;
		}
	}
	int* clientIDs;
	clientIDs = new int[numClients];
	int j = 0;
	for (int i = 1; i <= gpGlobals->maxClients; i++) {
		if (g_PR->IsConnected(i)) {
			clientIDs[j] = i;
			j++;
		}
	}
	int menuPagesCount = Ceil2Int((float)numClients / (float)PLAYER_PER_PAGE);
	CAdminSubMenu** playerPages = new CAdminSubMenu*[menuPagesCount];
	const char* actionName = GetPlayerActionTitle(action);
	for (int i = 0; i < menuPagesCount; i++) {
		playerPages[i] = new CAdminSubMenu;
		bool isLastPage = (i + 1 == menuPagesCount);
		int childCount = 0;
		if (isLastPage) {
			 //We need the last remaining player +1 Menuentry for back
			int modulo = numClients % PLAYER_PER_PAGE;
			//But only if the we not by chance got a number of players % PLAYER_PER_PAGE = 0
			if (modulo != 0){
				childCount = (numClients % PLAYER_PER_PAGE) + 1;
			}
			else {
				childCount = PLAYER_PER_PAGE + 1;
			}
		} else {
			//Otherwise we have a full page of players plus 2 options for back and next page
			childCount = PLAYER_PER_PAGE + 2;
		}
		playerPages[i]->m_aChildren = new CAdminSubMenu*[childCount];
		playerPages[i]->m_iNumChildren = childCount;
		playerPages[i]->m_aChildren[0] = g_pAdminBackButton;
		playerPages[i]->m_pszLineItemText = "#BG3_Adm_Next";
		playerPages[i]->m_pszTitle = actionName;
		int j = 0;
		while ((j < PLAYER_PER_PAGE) && ((i * PLAYER_PER_PAGE + j) < numClients)){
			int playerID = clientIDs[i * PLAYER_PER_PAGE + j];
			const char* playerName = g_PR->GetPlayerName(playerID);
			playerPages[i]->m_aChildren[j + 1] = CreatePlayerActionMenuEntry(action, playerName, playerID);
			j++;
		}
	}
	delete[] clientIDs;
	clientIDs = NULL;
	for (int i = 0; i < menuPagesCount-1; i++) {
		//All Fullpages get the next Page as their last Child
		playerPages[i]->m_aChildren[9] = playerPages[i + 1];
	}

	playerPages[0]->m_pszLineItemText = actionName;
	return playerPages[0];
	delete[] playerPages;
	playerPages = NULL;
}

void CreatePlayerActionTopMenu(CAdminSubMenu* pPlayerActionMenu) {
	pPlayerActionMenu->m_iNumChildren = 8;
	pPlayerActionMenu->m_aChildren = new CAdminSubMenu*[pPlayerActionMenu->m_iNumChildren];
	pPlayerActionMenu->m_aChildren[0] = new CAdminSubMenu("#BG3_Adm_Back", "");
	pPlayerActionMenu->m_aChildren[0]->m_pszFunc = [](uint8, CAdminSubMenu*)->bool { return true; };
	pPlayerActionMenu->m_aChildren[1] = CreatePlayerActionMenu(AdminMenuPlayerAction::Mute);
	pPlayerActionMenu->m_aChildren[2] = CreatePlayerActionMenu(AdminMenuPlayerAction::Unmute);
	pPlayerActionMenu->m_aChildren[3] = CreatePlayerActionMenu(AdminMenuPlayerAction::Spawn);
	pPlayerActionMenu->m_aChildren[4] = CreatePlayerActionMenu(AdminMenuPlayerAction::Slay);
	pPlayerActionMenu->m_aChildren[5] = CreatePlayerActionMenu(AdminMenuPlayerAction::Kick);
	pPlayerActionMenu->m_aChildren[6] = CreatePlayerActionMenu(AdminMenuPlayerAction::Ban60);
	pPlayerActionMenu->m_aChildren[7] = CreatePlayerActionMenu(AdminMenuPlayerAction::BanIndef);
}

//ADMIN MENU CONSTRUCTOR - THIS DEFINES THE MAIN STRUCTURE OF THE ENTIRE MENU
static CAdminMainMenu* g_pAdminMainMenu = NULL;
CAdminMainMenu::CAdminMainMenu() {
	m_pszTitle = "Admin Menu Version 0.1 Alpha";

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
		pChildren[0] = pModeMenu;
		pChildren[1] = pMapMenu;
		pChildren[2] = pPlayerActionMenu;
		pChildren[3] = pOptionsMenu;
		pChildren[4] = pCustomCFGMenu;
		pChildren[5] = pTeamwideClassmenu;
		pChildren[6] = pCancelButton;
	};

#define set(i, lineItemText, title) \
	pChildren[i] = new CAdminSubMenu(lineItemText, title);\
	pChildren[i]->m_pszFunc = [](uint8 slot, CAdminSubMenu*)->bool

	//MAP MENU
	auto mapChangeFunc = [](uint8, CAdminSubMenu* pSelf){ SayServerCommand("changemap %s", pSelf->m_pszLineItemText); return false; };
	pMapMenu->m_iNumChildren = 10;
	pChildren = pMapMenu->m_aChildren = new CAdminSubMenu*[pMapMenu->m_iNumChildren]; {
		pChildren[0] = pBackButton;

#define setmap(i, mapName) \
	pChildren[i] = new CAdminSubMenu(mapName, ""); \
	pChildren[i]->m_pszFunc = mapChangeFunc

		setmap(1, "bg_ambush");
		setmap(2, "bg_freemans_farm");
		setmap(3, "bg_maricopa");
		setmap(4, "bg_plateau");
		setmap(5, "bg_townguard");
		setmap(6, "bg_trenton");
		setmap(7, "bg_woodland");
		setmap(8, "ctf_mill_creek");
		
		pChildren[9] = new CAdminSubMenu("#BG3_Adm_Next", "Select Map (default maps only)");
		pChildren[9]->m_iNumChildren = 10;
		pChildren = pChildren[9]->m_aChildren = new CAdminSubMenu*[pChildren[9]->m_iNumChildren]; {

			pChildren[0] = pBackButton;
			setmap(1, "ctf_road");
			setmap(2, "ctf_stonefort");
			setmap(3, "lb_alpinepass");
			setmap(4, "lb_battleofconcord");
			setmap(5, "lb_battleofvalcourisland");
			setmap(6, "lb_continental");
			setmap(7, "lb_nativevalley");
			setmap(8, "sg_fall");

			pChildren[9] = new CAdminSubMenu("#BG3_Adm_Next", "Select Map (default maps only)");
			pChildren[9]->m_iNumChildren = 3;
			pChildren = pChildren[9]->m_aChildren = new CAdminSubMenu*[pChildren[9]->m_iNumChildren]; {
				pChildren[0] = pBackButton;
				setmap(1, "sg_siege");
				setmap(2, "sq");
			}
		}
	}

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
	CreatePlayerActionTopMenu(g_pAdminMainMenu->m_aChildren[2]);
	return g_pAdminMainMenu;
}
