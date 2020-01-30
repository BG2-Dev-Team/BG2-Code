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

void CSay(const char* pszFormat, ...);
void Host_Say(edict_t *pEdict, const CCommand &args, bool teamonly);

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

/*PLAYER_COMMAND(kit) {
	if (args.ArgC() < 4)
		return;
	pPlayer->m_iGunKit = atoi(args[1]);
	pPlayer->m_iAmmoKit = atoi(args[2]);
	pPlayer->m_iClassSkin = atoi(args[3]);
	pPlayer->CheckQuickRespawn();
}*/

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
			pPlayer->CommitSuicide();
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
			Msg("Muted %s\n", pPlayer->GetPlayerName());
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
			Msg("Unmuted %s\n", pPlayer->GetPlayerName());
		});
	}
}
PLAYER_COMMAND_ALIAS(unmute, um);


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


/* GAME INFO COMMANDS */
//float g_flNextServerChatMessage = -FLT_MAX;
/*PLAYER_COMMAND(ff) {
	engine->ServerExecute();
	if (friendlyfire.GetBool())
		engine->ServerCommand("say Friendly Fire is ON\n");
	else
		engine->ServerCommand("say Friendly Fire is OFF\n");
	engine->ServerExecute();
}*/
/*
PLAYER_COMMAND(currentmap) {
	CSay(g_pGameRules->MapName());
}
*/
CON_COMMAND(nextmap, "Changes the server to the specified map") {
	if (!verifyMapModePermissions(__FUNCTION__) || args.ArgC() < 2)
		return;

	if (!CMapInfo::MapExists(args[1])) {
		CSay("The map %s does not exist", args[1]);
		return;
	}

	//nextlevel is already built for this purpose
	nextlevel.SetValue(args[1]);
}