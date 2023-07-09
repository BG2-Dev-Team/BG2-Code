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
//#include "Math\static_initialize.h"
#include "../shared/bg3/bg3_buffs.h"
#include "../shared/bg3/bg3_class_quota.h"
#include "Permissions/bg3_player_locator.h"
#include "bg2/vcomm.h"
#include "gameinterface.h"
#include "../shared/bg3/bg3_map_model.h"
#include "controls/bg3_rtv.h"
#include "controls/bg3_voting_system.h"
#include "controls/bg3_map_nomination.h"
#include "player_resource.h"


void CSay(const char* pszFormat, ...);
void MSayPlayer(CHL2MP_Player* pRecipient, const char* pszFormat, ...);
void Host_Say(edict_t *pEdict, const CCommand &args, bool teamonly, std::vector<CBasePlayer*>* recipients = NULL, bool bAdminOnly = false);

//Static map of from player-issued to command strings to their functions
CUtlDict<PlayerCommandFunc> CHL2MP_Player::s_mPlayerCommands;

//--------------------------------------------------------------------------------
// Purpose: This class and macro simplify the declaration of player commands
//--------------------------------------------------------------------------------
namespace {
	class CCommandDeclarator {
	public:
		CCommandDeclarator(const char* pszCommandName, PlayerCommandFunc f) {
			CHL2MP_Player::s_mPlayerCommands.Insert(pszCommandName, f);
		}
	};
}
#define PLAYER_COMMAND(name) \
	static void __player_command_##name(CHL2MP_Player*, const CCommand&);\
	static CCommandDeclarator __declare_command_##name(#name, &__player_command_##name); \
	static void __player_command_##name(CHL2MP_Player* pPlayer, const CCommand& args)
#define PLAYER_COMMAND_ALIAS(orig, name) \
	static CCommandDeclarator __declare_command_##name(#name, &__player_command_##orig); \

//--------------------------------------------------------------------------------
// Purpose: Player commands begin here
//--------------------------------------------------------------------------------
PLAYER_COMMAND(spectate) {
	if (pPlayer->ShouldRunRateLimitedCommand(args))
	{
		// instantly join spectators
		pPlayer->HandleCommand_JoinTeam(TEAM_SPECTATOR);
	}
}

//BG3 - Awesome - merged classkit command
PLAYER_COMMAND(classkit) {
	if (args.ArgC() < 6)
		return;
	int iTeam = atoi(args[1]);
	int iClass = atoi(args[2]);
	pPlayer->m_iGunKit = atoi(args[3]);
	pPlayer->m_iAmmoKit = atoi(args[4]);
	pPlayer->m_iClassSkin = atoi(args[5]);

	bool bKitChangeOnly = (iTeam == pPlayer->GetTeamNumber() && iClass == pPlayer->GetNextClass());
	if (!bKitChangeOnly) {
		switch (iTeam) //Team
		{
		case TEAM_BRITISH:
			if (iClass >= 0 && iClass < TOTAL_BRIT_CLASSES)
				pPlayer->AttemptJoin(TEAM_BRITISH, iClass);
			break;
		case TEAM_AMERICANS:
			if (iClass >= 0 && iClass < TOTAL_AMER_CLASSES)
				pPlayer->AttemptJoin(TEAM_AMERICANS, iClass);
			break;
		default:
			Msg("Team selection invalid. \n");
		}
	}
	else {
		pPlayer->CheckQuickRespawn();
	}
}

PLAYER_COMMAND(voicecomm) {
	if (args.ArgC() < 2)
		return;
	
	int comm = atoi(args[1]);
	pPlayer->HandleVoicecomm(comm);

	//actually, don't let these carefully chosen commands activate buffs
	//BG3Buffs::RallyRequest(comm, pPlayer);//BG3 - rallying ability
}

PLAYER_COMMAND(battlecry) {
	pPlayer->HandleVoicecomm(NUM_BATTLECRY);
}

//--------------------------------------------------------------------------------
// Purpose: Admin commands begin here
//--------------------------------------------------------------------------------
void PerPlayerCommand(CHL2MP_Player* pRequester, const char* pszPlayerSearchTerm, void(*pFunc)(CHL2MP_Player*)) {
	CHL2MP_Player** pPlayerList = new CHL2MP_Player*[gpGlobals->maxClients];
	memset(pPlayerList, 0, gpGlobals->maxClients * sizeof(CHL2MP_Player*));

	GetPlayersFromString(pPlayerList, pszPlayerSearchTerm, pRequester);
	int i = 0;
	while (pPlayerList[i]) {
		pFunc(pPlayerList[i]);
		i++;
	}

	delete[] pPlayerList;
}

static int g_banMinutes = 0;
PLAYER_COMMAND(rban) {
	if (!pPlayer->GetPermissions()->m_bPlayerManage)
		return;

	if (args[2][0] == '@' && strcmp(args[2], "@aim") != 0)
		return;
		
	if (args.ArgC() == 3) {
		g_banMinutes = atoi(args[1]); if (g_banMinutes < 0) return;
		PerPlayerCommand(pPlayer, args[2], [](CHL2MP_Player* pPlayer) {

			CSteamID sid;
			pPlayer->GetSteamID(&sid);
			char buffer[64];
			convertSteamIDToString(sid, buffer, sizeof(buffer));

			char buffer2[128];
			Q_snprintf(buffer2, sizeof(buffer2), "banid %i %s kick\n", g_banMinutes, buffer);
			engine->ServerCommand(buffer2);
			engine->ServerExecute();

			Q_snprintf(buffer2, sizeof(buffer2), "kick \"%s\"\n", pPlayer->GetPlayerName());
			engine->ServerCommand(buffer2);
		});
	}

}
PLAYER_COMMAND_ALIAS(rban, b);
/*PLAYER_COMMAND(rbanip) {
	if (!pPlayer->GetPermissions()->m_bPlayerManage)
		return;

	if (args.ArgC() == 3) {
		g_banMinutes = atoi(args[1]); if (g_banMinutes < 0) return;
		PerPlayerCommand(pPlayer, args[2], [](CHL2MP_Player* pPlayer) {

			pPlayer->GetSteamID()

			char buffer2[128];
			Q_snprintf(buffer2, sizeof(buffer2), "banid %i %s kick\n", g_banMinutes, buffer);
		});
	}

}
PLAYER_COMMAND_ALIAS(rbanip, bip);*/

PLAYER_COMMAND(rkick) {
	if (!pPlayer->GetPermissions()->m_bPlayerManage)
		return;
	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			char buffer[128];
			Q_snprintf(buffer, sizeof(buffer), "kick \"%s\"\n", pPlayer->GetPlayerName());
			engine->ServerCommand(buffer);
		});
	}
}
PLAYER_COMMAND_ALIAS(rkick, k);

PLAYER_COMMAND(slay) {
	if (!pPlayer->GetPermissions()->m_bPlayerManage)
		return;

	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			pPlayer->DontRemoveTicketOnNextRespawn();
			pPlayer->CommitSuicide();
			AdminSay("Slayed %s", pPlayer->GetPlayerName());
			MSayPlayer(pPlayer, "You were slain by admin");
		});
	}
}
PLAYER_COMMAND_ALIAS(slay, s);

PLAYER_COMMAND(mute) {
	if (!pPlayer->GetPermissions()->m_bPlayerManage)
		return;
	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			pPlayer->m_bMuted = true;
			AdminSay("Muted %s", pPlayer->GetPlayerName());
		});
	}
}
PLAYER_COMMAND_ALIAS(mute, m);
PLAYER_COMMAND(unmute) {
	if (!pPlayer->GetPermissions()->m_bPlayerManage)
		return;
	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			pPlayer->m_bMuted = false;
			AdminSay("Unmuted %s", pPlayer->GetPlayerName());
		});
	}
}
PLAYER_COMMAND_ALIAS(unmute, um);

PLAYER_COMMAND(gag) {
	if (!pPlayer->GetPermissions()->m_bPlayerManage)
		return;
	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			pPlayer->m_bGagged = true;
			AdminSay("Gagged %s", pPlayer->GetPlayerName());
		});
	}
}
PLAYER_COMMAND_ALIAS(gag, g);
PLAYER_COMMAND(ungag) {
	if (!pPlayer->GetPermissions()->m_bPlayerManage)
		return;
	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			pPlayer->m_bGagged = false;
			AdminSay("Ungagged %s", pPlayer->GetPlayerName());
		});
	}
}
PLAYER_COMMAND_ALIAS(ungag, ug);

PLAYER_COMMAND(silence) {
	if (!pPlayer->GetPermissions()->m_bPlayerManage)
		return;
	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			pPlayer->m_bGagged = true;
			pPlayer->m_bMuted = true;
			AdminSay("Silenced %s", pPlayer->GetPlayerName());
		});
	}
}
PLAYER_COMMAND_ALIAS(silence, sl);
PLAYER_COMMAND(unsilence) {
	if (!pPlayer->GetPermissions()->m_bPlayerManage)
		return;
	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			pPlayer->m_bGagged = false;
			pPlayer->m_bMuted = false;
			AdminSay("Unsilenced %s", pPlayer->GetPlayerName());
		});
	}
}
PLAYER_COMMAND_ALIAS(unsilence, usl);


PLAYER_COMMAND(oppress) {
	if (!pPlayer->GetPermissions()->m_bPlayerManage || mp_competitive.GetBool())
		return;
	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			pPlayer->m_bOppressed = true;
			pPlayer->m_bMuted = true;
			pPlayer->m_bGagged = true;
			AdminSay("Oppressed %s", pPlayer->GetPlayerName());
		});
	}
}
PLAYER_COMMAND_ALIAS(oppress, o);

PLAYER_COMMAND(unoppress) {
	if (!pPlayer->GetPermissions()->m_bPlayerManage)
		return;
	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			pPlayer->m_bOppressed = false;
			pPlayer->m_bMuted = false;
			pPlayer->m_bGagged = false;
			AdminSay("Unoppressed %s", pPlayer->GetPlayerName());
		});
	}
}
PLAYER_COMMAND_ALIAS(unoppress, uo);

PLAYER_COMMAND(ignite) {
	if (!pPlayer->GetPermissions()->m_bPlayerManage)
		return;
	if (args.ArgC() == 2) {
		engine->ClientCommand(pPlayer->edict(), "say \"Setting a player aflame\"");
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			if (pPlayer->IsAlive() && pPlayer->GetTeamNumber() > TEAM_SPECTATOR) {
				pPlayer->Ignite(999.f, false);
			}
		});
	}
	

}

PLAYER_COMMAND(oppressme) {
	pPlayer->m_bOppressed = true;
	pPlayer->m_bMuted = true;
}
PLAYER_COMMAND(muteme) {
	pPlayer->m_bMuted = true;
}

PLAYER_COMMAND(setname) {
	if (args.ArgC() > 1)
		pPlayer->SetPlayerName(args[1]);
}

/*PLAYER_COMMAND(spawn) {
	if (!pPlayer->GetPermissions()->m_bPlayerManage)
		return;

	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			CSay("Spawning %s via console command", pPlayer->GetPlayerName());
			pPlayer->Spawn();
		});
	}
}*/

//--------------------------------------------------------------------------
// LB Officer commands
//--------------------------------------------------------------------------
/*PLAYER_COMMAND(off) {
	if (!pPlayer->GetPermissions()->m_bPlayerManage && !(pPlayer->GetFlags() & FL_LBOFFICER))
		return;


}*/


//--------------------------------------------------------------------------
// Individual buff commands
//--------------------------------------------------------------------------
PLAYER_COMMAND(vcomm_advance) {	
	uint8 comm = VCOMM2_ADVANCE;
	pPlayer->HandleVoicecomm(comm);
	BG3Buffs::RallyRequest(comm, pPlayer);
}

PLAYER_COMMAND(vcomm_fire) {
	uint8 comm = VCOMM2_FIRE;
	pPlayer->HandleVoicecomm(comm);
	BG3Buffs::RallyRequest(comm, pPlayer);
}

PLAYER_COMMAND(vcomm_rally) {
	uint8 comm = VCOMM2_RALLY_ROUND;
	pPlayer->HandleVoicecomm(comm);
	BG3Buffs::RallyRequest(comm, pPlayer);
}

PLAYER_COMMAND(vcomm_retreat) {
	uint8 comm = VCOMM2_RETREAT;
	pPlayer->HandleVoicecomm(comm);
	BG3Buffs::RallyRequest(comm, pPlayer);
}

PLAYER_COMMAND(vcomm_follow) {
	uint8 comm = VCOMM_FOLLOW;
	pPlayer->HandleVoicecomm(comm);
	//BG3Buffs::RallyRequest(comm, pPlayer);
}

PLAYER_COMMAND(vcomm_halt) {
	uint8 comm = VCOMM2_HALT;
	pPlayer->HandleVoicecomm(comm);
	//BG3Buffs::RallyRequest(comm, pPlayer);
}

//Progression system setting changes
PLAYER_COMMAND(ps_checksum) {
	if (args.ArgC() == 2) {
		uint64 checksum = strtoull(args[1], NULL, 10);
		pPlayer->m_unlockableProfile.m_iUniqueIdChecksum = checksum;
		//Msg("Received checksum %llu\n", checksum);
	}
}

PLAYER_COMMAND(ps_settings) {
	if (args.ArgC() == 4) {
		uint64 checksum = strtoull(args[1], NULL, 10);

		if (checksum != pPlayer->m_unlockableProfile.m_iUniqueIdChecksum) {
			Warning("Invalid checksum from player %s. Are they trying to hack the progression system?", pPlayer->GetPlayerName());
			return;
		}

		uint64 key = NIntegrity::getEncryptKeyForPlayer(pPlayer);

		uint64 unlocked = key ^ strtoull(args[2], NULL, 10);
		uint64 activated = key ^ strtoull(args[3], NULL, 10);

		//Msg("Player setting bits %llu %llu, checksum %llu, key %llu\n", unlocked, activated, checksum, key);

		pPlayer->m_unlockableProfile.m_iUnlockedBits = unlocked;
		pPlayer->m_unlockableProfile.m_iActivatedBits = activated;
	}
}

PLAYER_COMMAND(rtv) {
	extern ConVar sv_rtv_enabled;
	//extern ConVar sv_rtv_nomination;
	//if (sv_rtv_enabled.GetBool()) {
		/*if (args.ArgC() == 2 && sv_rtv_nomination.GetBool()) {
			if (!CMapInfo::MapExists(args[1])) {
				MSayPlayer(pPlayer, "The map %s does not exist", args[1]);
				return;
			}
			RtvSystem::SetRtvMap(pPlayer, args[1]);
		}*/

		RtvSystem::ReceiveRtvFromPlayer(pPlayer);
	//}
}

//this command can do two things -- echo the current next map in chat, or set the next map
PLAYER_COMMAND(nextmap) {
	if (args.ArgC() == 1) {
		const char* next = nextlevel.GetString();
		if (next && next[0]) {
			CSay(nextlevel.GetString());
		}
		else {
			CSay("Next map not yet determined.");
		}
	}
	else {
		if (!verifyMapModePermissions(__FUNCTION__) || args.ArgC() < 2)
			return;

		if (!CMapInfo::MapExists(args[1])) {
			CSay("The map %s does not exist", args[1]);
			return;
		}

		//nextlevel is already built for this purpose
		nextlevel.SetValue(args[1]);
	}
}

PLAYER_COMMAND(vote) {
	if (pPlayer->GetTeamNumber() < TEAM_AMERICANS)
		return;

	//two possibilities - either we're submitting a vote, or we're creating a customized multi-choice election
	if (args.ArgC() == 2) {
		//absolutely ensure bounds
		int slot = atoi(args[1]) - 1;
		if (slot == -1) slot = 9; //0 serves as the tenth slot at index 9
		if (slot < 0 || slot > 9) return;

		CElectionSystem::GetActiveElectionSystemForTeam(pPlayer->GetTeamNumber())
			->SubmitPlayerVote(pPlayer, slot);
	}
	else if (pPlayer->GetPermissions()->m_bMapMode && args.ArgC() >= 4 && args.ArgC() <= 12) {
		//max is 12, because it's "vote" + "topic" + [<ten options>]
		CElectionSystem::GetElectionSystemForPlayer(pPlayer)
			->CreateActionlessMultichoiceElection(pPlayer, args);
	}
	else {
		MSayPlayer(pPlayer, "Invalid vote parameters");
	}
}

PLAYER_COMMAND(votekick) {
	if (pPlayer->GetTeamNumber() < TEAM_AMERICANS)
		return;

	if (args.ArgC() != 2)
		return;

	CElectionSystem::GetElectionSystemForPlayer(pPlayer)
		->CreateKickElection(pPlayer, args[1]);
}

PLAYER_COMMAND(votemute) {
	if (pPlayer->GetTeamNumber() < TEAM_AMERICANS)
		return;

	if (args.ArgC() != 2)
		return;

	CElectionSystem::GetElectionSystemForPlayer(pPlayer)
		->CreateMuteElection(pPlayer, args[1]);
}

PLAYER_COMMAND(votescramble) {
	if (pPlayer->GetTeamNumber() < TEAM_AMERICANS)
		return;

	CElectionSystem::GetElectionSystemForPlayer(pPlayer)
		->CreateScrambleTeamsElection(pPlayer);
}

PLAYER_COMMAND(votegungame) {
	if (pPlayer->GetTeamNumber() < TEAM_AMERICANS)
		return;

	CElectionSystem::GetElectionSystemForPlayer(pPlayer)
		->CreateGunGameElection(pPlayer);
}

PLAYER_COMMAND(votemap) {
	if (pPlayer->GetTeamNumber() < TEAM_AMERICANS)
		return;

	//two possibilities -- we're voting whether or not to vote for a specific map, or we're creating a multi-map choice vote
	CElectionSystem* system = CElectionSystem::GetElectionSystemForPlayer(pPlayer);

	
	if (args.ArgC() == 2) {

		if (Q_strcmp(args[1], "lb") == 0 && verifyMapModePermissions(__FUNCTION__)) {
			std::vector<std::string> options;
			NMapNomination::GetRandomLbMapList(options);
			system->CreateMultichoiceMapElection(pPlayer, options);
		}
		else if (Q_strcmp(args[1], "sg") == 0 && verifyMapModePermissions(__FUNCTION__)) {
			std::vector<std::string> options;
			NMapNomination::GetRandomSgMapList(options);
			system->CreateMultichoiceMapElection(pPlayer, options);
		}
		else if (Q_strcmp(args[1], "bg") == 0 && verifyMapModePermissions(__FUNCTION__)) {
			std::vector<std::string> options;
			NMapNomination::GetRandomBgMapList(options);
			system->CreateMultichoiceMapElection(pPlayer, options);
		}
		else if (Q_strcmp(args[1], "ctf") == 0 && verifyMapModePermissions(__FUNCTION__)) {
			std::vector<std::string> options;
			NMapNomination::GetRandomCtfMapList(options);
			system->CreateMultichoiceMapElection(pPlayer, options);
		}
		else {
			system->CreateMapChangeElection(pPlayer, args[1]);
		}
	}
	else if (args.ArgC() > 2 && verifyMapModePermissions(__FUNCTION__)) {
		system->CreateMultichoiceMapElection(pPlayer, args);
	}
}


CON_COMMAND(ff, "Reports friendly fire cvar statuses to chat") {
	CBasePlayer* pPlayer = UTIL_GetCommandClient();
	if (!pPlayer) 
		return;

	AdminSay("%s requesting FF status...", pPlayer->GetPlayerName());

	extern ConVar friendlyfire;
	extern ConVar mp_friendlyfire_grenades;
	extern ConVar mp_friendlyfire_swivel;
	std::string msg;
	if (friendlyfire.GetBool()) {
		msg = "Friendly fire is ON. ";
	}
	else {
		msg = "Friendly fire is OFF. ";
		if (mp_friendlyfire_grenades.GetBool()) {
			msg += "Grenade friendly fire is ON. ";
		}
		else {
			msg += "Grenade friendly fire is OFF. ";
		}
		if (mp_friendlyfire_swivel.GetBool()) {
			msg += "Swivel gun friendly fire is ON. ";
		}
		else {
			msg += "Swivel gun friendly fire is OFF. ";
		}
	}
	
	CSay(msg.c_str());
}

CON_COMMAND(currentmap, "Echoes in the chat the name of the current map.") {
	CBasePlayer* pPlayer = UTIL_GetCommandClient();
	if (!pPlayer)
		return;

	AdminSay("%s requesting map name...", pPlayer->GetPlayerName());
	CSay(HL2MPRules()->MapName());
}

CON_COMMAND(respawn, "Echoes the current respawn time to chat.") {
	CBasePlayer* pPlayer = UTIL_GetCommandClient();
	if (!pPlayer)
		return;

	AdminSay("%s requesting respawn time...", pPlayer->GetPlayerName());
	CSay("%i seconds between respawns", mp_respawntime.GetInt());
}

CON_COMMAND(pings, "Echoes into chat the average ping of each team.") {
	CBasePlayer* pPlayer = UTIL_GetCommandClient();
	if (!pPlayer)
		return;

	AdminSay("%s requesting average pings...", pPlayer->GetPlayerName());

	int amerTotalPing = 0;
	int britTotalPing = 0;
	int iNumBrit = 0;
	int iNumAmer = 0;
	for (int i = 1; i <= gpGlobals->maxClients; i++) {

		CBasePlayer *pPlayer = (CBasePlayer*)UTIL_PlayerByIndex(i);
		if (pPlayer && pPlayer->IsConnected() && !pPlayer->IsFakeClient())
		{
			int ping = g_pPlayerResource->GetPing(i);
			if (pPlayer->GetTeamNumber() == TEAM_BRITISH) {
				britTotalPing += ping;
				iNumBrit++;
			}
			else if (pPlayer->GetTeamNumber() == TEAM_AMERICANS) {
				amerTotalPing += ping;
				iNumAmer++;
			}
		}
	}
	//calc averages
	//no division by zero
	if (iNumAmer) amerTotalPing /= iNumAmer;
	if (iNumBrit) britTotalPing /= iNumBrit;

	CSay("Ave. American ping: %ims Ave. British ping: %ims", amerTotalPing, britTotalPing);
}

CON_COMMAND(nominate, "Lets player nominate map for map election") {
	CBasePlayer* pPlayer = UTIL_GetCommandClient();
	if (!pPlayer || args.ArgC() != 2)
		return;

	NMapNomination::NominateMap(args[1], (CHL2MP_Player*)pPlayer);
}

CON_COMMAND(admins, "Reports list of admins to chat") {
	CBasePlayer* pPlayer = UTIL_GetCommandClient();
	if (!pPlayer)
		return;

	AdminSay("%s requesting list of admins...", pPlayer->GetPlayerName());

	std::vector<CBasePlayer*> list;
	Permissions::GetAdminList(list);
	

	if (list.size() > 0) {
		string str;
		//assemble a string then spit it out
		str.get_allocator().allocate((list.size() + 1) * 32);
		str.append("Admins online: ");
		str.append(list[0]->GetPlayerName());
		
		for (size_t i = 1; i < list.size(); i++) {
			str.append(", ");
			str.append(list[i]->GetPlayerName());
		}
		CSay(str.c_str());
	}
	else {
		CSayPlayer((CHL2MP_Player*)pPlayer, "There are no admins online");
	}

	
	
}