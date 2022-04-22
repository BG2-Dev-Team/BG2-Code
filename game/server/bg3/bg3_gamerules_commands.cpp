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
#include "client.h"
#include "hl2mp/hl2mp_gamerules.h"
#include "../shared/bg3/bg3_map_model.h"
#include "../shared/bg3/Math/bg3_rand.h"
#include "../shared/bg3/bg3_class_quota.h"
#include "../shared/bg3/bg3_buffs.h"
#include "controls/bg3_voting_system.h"
#include "player_resource.h"
#include "team.h"

void PerPlayerCommand(CHL2MP_Player* pRequester, const char* pszPlayerSearchTerm, void(*pFunc)(CHL2MP_Player*));

ConVar mp_comp_notifications = ConVar("mp_comp_notifications", "1", FCVAR_GAMEDLL);

//Prints in chat
void CSay(const char* pszFormat, ...) {
	va_list vl;
	va_start(vl, pszFormat);
	char buffer[128];
	V_vsnprintf(buffer, sizeof(buffer), pszFormat, vl);
	va_end(vl);


	char buffer2[140];
	Q_snprintf(buffer2, sizeof(buffer), "say %s\n", buffer);
	engine->ServerCommand(buffer2);
}

void CSayPlayer(CHL2MP_Player* pRecipient, const char* pszFormat, ...) {
	va_list vl;
	va_start(vl, pszFormat);
	char buffer[128];
	V_vsnprintf(buffer, sizeof(buffer), pszFormat, vl);
	va_end(vl);
	CCommand args;
	args.Tokenize(buffer);

	std::vector<CBasePlayer*> recipients;
	recipients.emplace_back(pRecipient);
	Host_Say(NULL, args, false, &recipients);
}

void AdminSay(const char* pszFormat, ...) {
	va_list vl;
	va_start(vl, pszFormat);
	char buffer[128];
	V_vsnprintf(buffer, sizeof(buffer), pszFormat, vl);
	va_end(vl);
	CCommand args;
	args.Tokenize(buffer);

	std::vector<CBasePlayer*> admins;
	Permissions::GetAdminList(admins);
	Host_Say(NULL, args, false, &admins);
}

//Prints on screen below scoreboard
void MSay(const char* pszFormat, ...) {
	va_list vl;
	va_start(vl, pszFormat);
	char buffer[256];
	V_vsnprintf(buffer, sizeof(buffer), pszFormat, vl);

	const char* pszMessage = buffer;
	if (strlen(pszMessage) > 244) return; //avoid out-of-bounds strings

	CReliableBroadcastRecipientFilter recpfilter;
	UserMessageBegin(recpfilter, "GameMsg");
	WRITE_SHORT(strlen(pszMessage)); //length of message
	WRITE_SHORT(260); //no player injected into localized string, out-of-bounds player index will work
	WRITE_STRING(pszMessage);
	MessageEnd();
}

//Prints on screen below scoreboard
void MSayPlayer(CHL2MP_Player* pRecipient, const char* pszFormat, ...) {
	va_list vl;
	va_start(vl, pszFormat);
	char buffer[256];
	V_vsnprintf(buffer, sizeof(buffer), pszFormat, vl);

	const char* pszMessage = buffer;
	if (strlen(pszMessage) > 244) return; //avoid out-of-bounds strings

	CSingleUserRecipientFilter recpfilter(pRecipient);
	UserMessageBegin(recpfilter, "GameMsg");
	WRITE_SHORT(strlen(pszMessage)); //length of message
	WRITE_SHORT(260); //no player injected into localized string, out-of-bounds player index will work
	WRITE_STRING(pszMessage);
	MessageEnd();
}

/*
The console commands here are shorthands for beginning different competitive matches
*/
CON_COMMAND(linebattle, "Starts a linebattle. With an integer argument provided, the round count is set to the given number and the competitive cfg is loaded. Otherwise the round system is disabled and then the public linebattle cfg is loaded. Modified CFGs can override the round system settings.") {
	if (!verifyMapModePermissions(__FUNCTION__))
		return;

	int count;
	if (args.ArgC() > 1)
		count = atoi(args[1]);
	else
		count = 10;
	mp_rounds.SetValue(count);
	engine->ServerCommand("exec linebattle.cfg\n");
	engine->ServerExecute();

	if (RndFloat() < 0.02f)
		MSay("#BG3_Winebottle_Started");
	else
		MSay("#BG3_Linebattle_Started");
}

CON_COMMAND(lms, "Starts an LMS match. With an integer argument provided, the round count is set to the given number and then lms.cfg is loaded. If no argument is provided, the round count is set to 0 and then the lms.cfg is loaded. Modifying lms.cfg can override the round system settings.") {
	if (!verifyMapModePermissions(__FUNCTION__))
		return;

	int count;
	if (args.ArgC() > 1)
		count = atoi(args[1]);
	else
		count = 10;
	mp_rounds.SetValue(count);
	engine->ServerCommand("exec lms.cfg\n");
	engine->ServerExecute();
	MSay("#BG3_LMS_Started");
}

CON_COMMAND(skirm, "Starts a 2-round competitive skirmish. An extra integer argument, if provided, sets the round duration in minutes, and then the skirm.cfg is loaded. If no argument is provided, a 20 minute round duration is assumed. Modify the skirm.cfg to change parameters.") {
	if (!verifyMapModePermissions(__FUNCTION__))
		return;

	int count;
	if (args.ArgC() > 1)
		count = atoi(args[1]);
	else
		count = 20;
	mp_roundtime.SetValue(60 * count);
	engine->ServerCommand("exec skirm.cfg\n");
	engine->ServerExecute();
	MSay("#BG3_Skirmish_Started");
}

CON_COMMAND(dm, "Starts a single-round deathmatch. An extra integer argument, if provided, specifies the number of rounds.") {
	if (!verifyMapModePermissions(__FUNCTION__))
		return;
	int roundCount = 1;
	if (args.ArgC() > 1)
		roundCount = atoi(args[1]);
	if (roundCount < 1)
		return;

	mp_rounds.SetValue(roundCount);
	engine->ServerCommand("exec dm.cfg\n");
	engine->ServerExecute();
	MSay("#BG3_DM_Started");
}

CON_COMMAND(changemap, "Changes the server to the specified map") {
	if (!verifyMapModePermissions(__FUNCTION__) || args.ArgC() < 2)
		return;

	if (!CMapInfo::MapExists(args[1])) {
		CSay("The map %s does not exist", args[1]);
		return;
	}


	CElectionSystem::CancelAllElections();
	//after we've verified, just do changelevel
	nextlevel.SetValue(args[1]);
	((CHL2MPRules*) g_pGameRules)->ChangeMapDelayed(5.f);
}

CON_COMMAND(csay, "Says a message in chat") {
	if (!verifyMapModePermissions(__FUNCTION__) || args.ArgC() < 2)
		return;
	//Msg("%s executing csay command", );
	int len = 7 + strlen(args[1]);
	char* buffer = new char[len];
	Q_snprintf(buffer, len, "say %s\n", args[1]);
	engine->ServerCommand(buffer);
	delete[] buffer;
}

CON_COMMAND(remove_class_limits, "Removes all class limit and weapon restrictions") {
	if (!verifyMapModePermissions(__FUNCTION__))
		return;

	extern ConVar lb_enforce_weapon_amer, lb_enforce_weapon_brit;
	lb_enforce_weapon_amer.Revert();
	lb_enforce_weapon_brit.Revert();
	CPlayerClass::RemoveClassLimits();
	MSay("#BG3_Adm_Limits_Removed");
}

CON_COMMAND(mp_friendlyfire_toggle, "Toggles friendly fire setting") {
	if (!verifyMapModePermissions(__FUNCTION__))
		return;

	extern ConVar friendlyfire;
	friendlyfire.SetValue(!friendlyfire.GetBool());
	const char* pszMessage = friendlyfire.GetBool() ? "#BG3_Adm_FF_On" : "#BG3_Adm_FF_Off";
	MSay(pszMessage);
}

CON_COMMAND(swap_teams, "Swaps teams") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	HL2MPRules()->SwapTeams();
	MSay("#BG3_Adm_TeamsSwapped");
}

CHL2MP_Player* g_pMicSoloPlayer = NULL;

CON_COMMAND(solo, "Gives solo permission to player who sent the command, or toggles solo permission if the player already has it. Does nothing when entered from server console.") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;
	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	if (!pPlayer || !pPlayer->GetPermissions()->m_bPlayerManage)
		return;
	if (pPlayer == g_pMicSoloPlayer) {
		g_pMicSoloPlayer = NULL;
		CSay("Solo power taken away - everyone can speak.");
	}
		
	else
	{
		g_pMicSoloPlayer = pPlayer;
		CSay("Giving Solo power to %s", pPlayer->GetPlayerName());
	}
		

	
}

CON_COMMAND(unsolo, "Turns off all solo permissions.") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;
	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	if (!pPlayer || !pPlayer->GetPermissions()->m_bPlayerManage)
		return;

	g_pMicSoloPlayer = NULL;
	CSay("Solo power taken away - everyone can speak.");
}


CON_COMMAND(spawn, "Spawns specified player(s)") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL

	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			if (mp_comp_notifications.GetBool()) CSay("Spawning %s via console command", pPlayer->GetPlayerName());
			pPlayer->Spawn();
		});
	}
}

CON_COMMAND(heal, "Heals specified player(s)") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL

	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			if (mp_comp_notifications.GetBool()) CSay("Healing %s via console command", pPlayer->GetPlayerName());
			if (pPlayer->IsAlive() && pPlayer->GetHealth() != 100) {
				pPlayer->SetHealth(100);
				pPlayer->EmitSound("AmmoCrate.Give");
			}
		});
	}
}

static int g_iHealHealth = 0;
CON_COMMAND(health, "Adds HP to specified player(s)") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL

	if (args.ArgC() == 3) {
		g_iHealHealth = atoi(args[1]);
		PerPlayerCommand(pPlayer, args[2], [](CHL2MP_Player* pPlayer) {
			if (mp_comp_notifications.GetBool()) MSay("Giving %s extra health", pPlayer->GetPlayerName());
			if (pPlayer->IsAlive()) {
				pPlayer->SetHealth(pPlayer->GetHealth() + g_iHealHealth);
				pPlayer->EmitSound("AmmoCrate.Give");
			}
		});
	}
}

CON_COMMAND(shrooms, "Speeds up a player's movement") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL

	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			if (mp_comp_notifications.GetBool()) CSay("Speeding up %s", pPlayer->GetPlayerName());
			if (pPlayer->IsAlive()) {
				pPlayer->AddSpeedModifier(127, ESpeedModID::Weapon);
				pPlayer->EmitSound("AmmoCrate.Give");
			}
		});
	}
}

CON_COMMAND(freeze, "Freezes player") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL

	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			if (mp_comp_notifications.GetBool()) CSay("Freezing %s", pPlayer->GetPlayerName());
			pPlayer->AddFlag(FL_FROZEN);
		});
	}
}
CON_COMMAND(unfreeze, "Unfreezes player") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL

	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			if (mp_comp_notifications.GetBool()) CSay("Freezing %s", pPlayer->GetPlayerName());
			pPlayer->RemoveFlag(FL_FROZEN);
		});
	}
}

CON_COMMAND(spec, "Switches the specifies player(s) to spectate") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL

	if (args.ArgC() == 2 /*&& pPlayer->ShouldRunRateLimitedCommand(args)*/) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			// instantly join spectators
			pPlayer->HandleCommand_JoinTeam(TEAM_SPECTATOR);
		});
	}
}

CON_COMMAND(amer, "Switches specified player(s) to American team") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL

	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			if (pPlayer->GetTeamNumber() == TEAM_BRITISH) {
				const CPlayerClass* pClass = NClassQuota::FindInfiniteClassForTeam(TEAM_AMERICANS);
				pPlayer->ForceJoin(pClass, TEAM_AMERICANS, pClass->m_iClassNumber);
			}
		});
	}
}

CON_COMMAND(brit, "Switches specified player(s) to British team") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL

	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			if (pPlayer->GetTeamNumber() == TEAM_AMERICANS) {
				const CPlayerClass* pClass = NClassQuota::FindInfiniteClassForTeam(TEAM_BRITISH);
				pPlayer->ForceJoin(pClass, TEAM_BRITISH, pClass->m_iClassNumber);
			}
		});
	}
}

void ScrambleTeams() {
	const int SIZE = gpGlobals->maxClients;

	//create list, then randomly swap them around
	CHL2MP_Player** pPlayerList = new CHL2MP_Player*[SIZE];
	memset(pPlayerList, 0, SIZE * sizeof(CHL2MP_Player*));

	for (int i = 0; i < SIZE; i++) {
		CHL2MP_Player* pPlayer = ToHL2MPPlayer(UTIL_PlayerByIndex(i));
		if (pPlayer && pPlayer->GetTeamNumber() >= TEAM_AMERICANS) {
			pPlayerList[i] = pPlayer;
		}
		
	}

	auto swap = [&](int i1, int i2) {
		CHL2MP_Player* temp = pPlayerList[i1];
		pPlayerList[i1] = pPlayerList[i2];
		pPlayerList[i2] = temp;
	};

	//randomly move things around in the list
	for (int i = 0; i < SIZE; i++) {
		swap(RndInt(0, SIZE - 1), RndInt(0, SIZE - 1));
	}

	auto swapPlayerToTeam = [](int* counter, int iTeam, CHL2MP_Player* pPlayer) {
		(*counter)++;
		if (pPlayer->GetTeamNumber() != iTeam) {
			const CPlayerClass* pClass = NClassQuota::FindInfiniteClassForTeam(iTeam);
			pPlayer->ForceJoin(pClass, iTeam, pClass->m_iClassNumber);
		}
	};

	int numAmer, numBrit;
	numAmer = numBrit = 0;
	for (int i = 0; i < SIZE; i++) {
		CHL2MP_Player* pPlayer = pPlayerList[i];

		if (!pPlayer)
			continue;

		if (numAmer < numBrit) {
			swapPlayerToTeam(&numAmer, TEAM_AMERICANS, pPlayer);
		}
		else {
			swapPlayerToTeam(&numBrit, TEAM_BRITISH, pPlayer);
		}
	}

	//free memory
	delete[] pPlayerList;

	MSay("#BG3_Adm_TeamsScrambled");
}

CON_COMMAND(scramble_teams, "Scrambles the teams") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	ScrambleTeams();
}

static const CPlayerClass* g_pNextLinebattleClass = NULL;
static void LinebattleSetClass(int iTeam, const char* pszAbbreviation, bool instant = true) {
	if (!IsLinebattle())
		return;

	//First get pointer to player class
	g_pNextLinebattleClass = CPlayerClass::fromAbbreviation(iTeam, pszAbbreviation);

	if (g_pNextLinebattleClass) {
		const CPlayerClass& pc = *g_pNextLinebattleClass;

		if (instant) {
			const char* pszSelector = iTeam == TEAM_AMERICANS ? "@amer" : "@brit";
			//set all players of correct team to that class
			PerPlayerCommand(NULL, pszSelector, [](CHL2MP_Player* pPlayer){
				//don't change players who are on unique classes
				if (pPlayer->GetPlayerClass()->GetLimitSml() >= 0)
					return;

				pPlayer->ForceJoin(g_pNextLinebattleClass, g_pNextLinebattleClass->m_iDefaultTeam, g_pNextLinebattleClass->m_iClassNumber);
			});
		}

		//set all other limits on other classes to 0, set our limit to -1
		pc.m_pcvLimit_lrg->SetValue("-1");
		pc.m_pcvLimit_med->SetValue("-1");
		pc.m_pcvLimit_sml->SetValue("-1");
		ConVar * officerClass = iTeam == TEAM_AMERICANS ? &lb_officer_classoverride_a : &lb_officer_classoverride_b;
		int iOfficerClass = officerClass->GetInt();
		for (int i = 0; i < CPlayerClass::numClassesForTeam(iTeam); i++) {
			const CPlayerClass* pOtherClass = CPlayerClass::fromNums(iTeam, i);
			if (pOtherClass != &pc) {
				if (pOtherClass->m_iClassNumber == iOfficerClass) {
					pOtherClass->m_pcvLimit_lrg->SetValue(1);
					pOtherClass->m_pcvLimit_med->SetValue(1);
					pOtherClass->m_pcvLimit_sml->SetValue(1);
				}
				else {
					pOtherClass->m_pcvLimit_lrg->SetValue(0);
					pOtherClass->m_pcvLimit_med->SetValue(0);
					pOtherClass->m_pcvLimit_sml->SetValue(0);
				}
			}
		}
	}
}

CON_COMMAND(aclass, "Sets class for american team from its abbreviation. Linebattle only. Works only if player is on American team or if command is sent through server console.") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL
	
	//Don't let non-americans change kit
	if (pPlayer && pPlayer->GetTeamNumber() != TEAM_AMERICANS && !pPlayer->GetPermissions()->m_bConsoleAccess)
		return;
	if (args.ArgC() == 2) {
		LinebattleSetClass(TEAM_AMERICANS, args[1]);
	}
}
CON_COMMAND(bclass, "Sets class for british team from its abbreviation. Linebattle only. Works only if player is on American team or if command is sent through server console.") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL

	//Don't let non-brits change kit
	if (pPlayer && pPlayer->GetTeamNumber() != TEAM_BRITISH && !pPlayer->GetPermissions()->m_bConsoleAccess)
		return;
	if (args.ArgC() == 2) {
		LinebattleSetClass(TEAM_BRITISH, args[1]);
	}
}



static void LinebattleSetKit(int iTeam, int iWeapon, int iUniform, int iAmmo) {
	if (!IsLinebattle())
		return;

	//tell all players on team to change and spawn with new kit
	for (int i = 1; i <= gpGlobals->maxClients; i++) {
		CHL2MP_Player* pPlayer = ToHL2MPPlayer(UTIL_PlayerByIndex(i));
		if (pPlayer && pPlayer->GetTeamNumber() == iTeam && 
			pPlayer->GetPlayerClass()->GetLimitSml() < 0) {
			pPlayer->m_iClassSkin = iUniform;
			pPlayer->m_iGunKit = iWeapon;
			pPlayer->m_iAmmoKit = iAmmo;

			//let the respawn code fix clamp the out-of-range values
			pPlayer->CheckQuickRespawn();

			//set the weapon forcer to force joining players to a specific kit
			ConVar* pWeaponKitEnforcer = pPlayer->GetTeamNumber() == TEAM_AMERICANS ? &lb_enforce_weapon_amer : &lb_enforce_weapon_brit;
			pWeaponKitEnforcer->SetValue(iWeapon + 1);
		}
	}
}

void OnOfficerClassOverrideChange(IConVar* cvar, const char* pszOldValue, float flOldValue) {
	//Only do special stuff in linebattle mode
	if (!IsLinebattle())
		return;

	int iTeam = (cvar == (IConVar*)&lb_officer_classoverride_a) ? TEAM_AMERICANS : TEAM_BRITISH;
	int iOldClass = atoi(pszOldValue);
	int iNewClass = ((ConVar*)cvar)->GetInt();
	const CPlayerClass* pNewClass = CPlayerClass::fromNums(iTeam, iNewClass);

	//switch any officers from old class to new class
	for (int i = 0; i <= gpGlobals->maxClients; i++) {
		CHL2MP_Player* pPlayer = ToHL2MPPlayer(UTIL_PlayerByIndex(i));
		if (pPlayer && pPlayer->GetTeamNumber() == iTeam && pPlayer->GetNextClass() == iOldClass) {
			pPlayer->ForceJoin(pNewClass, iTeam, iNewClass);
		}
	}
}

CON_COMMAND(akit, "Sets the weapon, uniform, and ammo of American players") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL


	//Don't let non-americans change kit
	if (pPlayer && pPlayer->GetTeamNumber() != TEAM_AMERICANS)
		return;


	if (args.ArgC() == 2) {
		LinebattleSetKit(TEAM_AMERICANS, atoi(args[1]), 0, 0);
	}
	else if (args.ArgC() == 3) {
		LinebattleSetKit(TEAM_AMERICANS, atoi(args[1]), atoi(args[2]), 0);
	}
	else if (args.ArgC() == 4) {
		LinebattleSetKit(TEAM_AMERICANS, atoi(args[1]), atoi(args[2]), atoi(args[3]));
	}
}

CON_COMMAND(bkit, "") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL

	//Don't let non-brits change kit
	if (pPlayer && pPlayer->GetTeamNumber() != TEAM_BRITISH)
		return;

	if (args.ArgC() == 2) {
		LinebattleSetKit(TEAM_BRITISH, atoi(args[1]), 0, 0);
	}
	else if (args.ArgC() == 3) {
		LinebattleSetKit(TEAM_BRITISH, atoi(args[1]), atoi(args[2]), 0);
	}
	else if (args.ArgC() == 4) {
		LinebattleSetKit(TEAM_BRITISH, atoi(args[1]), atoi(args[2]), atoi(args[3]));
	}
}

CON_COMMAND(aclasskit, "") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL

	//Don't let non-americans change kit
	if (pPlayer && pPlayer->GetTeamNumber() != TEAM_AMERICANS && !pPlayer->GetPermissions()->m_bConsoleAccess)
		return;
	if (args.ArgC() == 5) {
		LinebattleSetClass(TEAM_AMERICANS, args[1], true);
		LinebattleSetKit(TEAM_AMERICANS, atoi(args[2]), atoi(args[3]), atoi(args[4]));
	}
}
CON_COMMAND(bclasskit, "") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL

	//Don't let non-british change kit
	if (pPlayer && pPlayer->GetTeamNumber() != TEAM_BRITISH && !pPlayer->GetPermissions()->m_bConsoleAccess)
		return;
	if (args.ArgC() == 5) {
		LinebattleSetClass(TEAM_BRITISH, args[1], true);
		LinebattleSetKit(TEAM_BRITISH, atoi(args[2]), atoi(args[3]), atoi(args[4]));
	}
}

void Player_NoClip(CBasePlayer* pPlayer);
CON_COMMAND(clip, "") {
	if (!verifyPlayerPermissions(__FUNCTION__))
		return;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	//Player MIGHT BE NULL

	const char* pszSelector = "";
	if (pPlayer && args.ArgC() == 1) {
		pszSelector = "@me";
	}
	else if (args.ArgC() > 1) {
		pszSelector = args[1];
	}

	PerPlayerCommand(pPlayer, pszSelector, [](CHL2MP_Player* pPlayer){
		if (pPlayer->GetTeamNumber() >= TEAM_AMERICANS) {
			Player_NoClip(pPlayer);
		}
	});
}

CON_COMMAND(rc, "") {
	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	if (!pPlayer || !pPlayer->GetPermissions()->m_bConsoleAccess)
		return;

	//build a second set of arguments
	char buffer[512];
	Q_snprintf(buffer, sizeof(buffer), "%s\n", args.GetCommandString() + 3);
	Msg(buffer);
	engine->ServerCommand(buffer);
	engine->ServerExecute();
}

void ClientPrintAll(const char *str, bool printfordeadplayers, bool forcenextclientprintall);

CON_COMMAND(msay, "") {
	if (args.ArgC() < 2 || !verifyPlayerPermissions(__FUNCTION__))
		return;

	const char* pszMessage = args.GetCommandString() + 5;
	if (strlen(pszMessage) > 244) return; //avoid out-of-bounds strings

	CReliableBroadcastRecipientFilter recpfilter;
	UserMessageBegin(recpfilter, "GameMsg");
	WRITE_SHORT(strlen(pszMessage)); //length of message
	WRITE_SHORT(260); //no player injected into localized string, out-of-bounds player index will work
	WRITE_STRING(pszMessage);
	MessageEnd();
}

CON_COMMAND(asay, "") {
	if (args.ArgC() < 2 || !verifyPlayerPermissions(__FUNCTION__) || !UTIL_GetCommandClient())
		return;

	const char* pszMessage = args.GetCommandString() + 5;
	if (strlen(pszMessage) > 244) return; //avoid out-of-bounds strings

	CCommand args2;
	args2.Tokenize(pszMessage);

	std::vector<CBasePlayer*> admins;
	Permissions::GetAdminList(admins, UTIL_GetCommandClient());
	Host_Say(UTIL_GetCommandClient()->edict(), args2, false, &admins, true);
}

CON_COMMAND(mp_tickets_a_adjust, "Adjusts American ticket amount by given amout, positive or negative") {
	if (args.ArgC() < 2 || !verifyMapModePermissions(__FUNCTION__))
		return;
	int amount = atoi(args[1]);

	if (mp_comp_notifications.GetBool()) CSay("Changing American team ticket amount.");
	if (amount > 0) MSay("Americans receive reinforcements!");
	if (amount < 0) MSay("American morale depleted!");
	CTeam *pAmericans = g_Teams[TEAM_AMERICANS];
	pAmericans->ChangeTickets(atoi(args[1]));
}

CON_COMMAND(mp_tickets_b_adjust, "Adjusts British ticket amount by given amout, positive or negative") {
	if (args.ArgC() < 2 || !verifyMapModePermissions(__FUNCTION__))
		return;
	int amount = atoi(args[1]);

	if (mp_comp_notifications.GetBool()) CSay("Changing British team ticket amount.");
	if (amount > 0) MSay("British receive reinforcements!");
	if (amount < 0) MSay("British morale depleted!");
	CTeam *pBritish = g_Teams[TEAM_BRITISH];
	pBritish->ChangeTickets(amount);
}

