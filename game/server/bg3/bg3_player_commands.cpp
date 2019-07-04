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
//#include "Math/static_initialize.h"
#include "../shared/bg3/bg3_buffs.h"
#include "../shared/bg3/bg3_class_quota.h"
#include "Permissions/bg3_player_locator.h"
#include "bg2/vcomm.h"
#include "gameinterface.h"

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
static void PerPlayerCommand(CHL2MP_Player* pRequester, const char* pszPlayerSearchTerm, void(*pFunc)(CHL2MP_Player*)) {
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

PLAYER_COMMAND(slay) {
	if (!pPlayer->m_pPermissions->m_bPlayerManage)
		return;

	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			pPlayer->CommitSuicide();
		});
	}
}

PLAYER_COMMAND(spawn) {
	if (!pPlayer->m_pPermissions->m_bPlayerManage)
		return;

	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			pPlayer->Spawn();
		});
	}
}

PLAYER_COMMAND(spec) {
	if (!pPlayer->m_pPermissions->m_bPlayerManage)
		return;

	if (args.ArgC() == 2 && pPlayer->ShouldRunRateLimitedCommand(args)) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			// instantly join spectators
			pPlayer->HandleCommand_JoinTeam(TEAM_SPECTATOR);
		});
	}
}

PLAYER_COMMAND(amer) {
	if (!pPlayer->m_pPermissions->m_bPlayerManage)
		return;

	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			if (pPlayer->GetTeamNumber() == TEAM_BRITISH) {
				const CPlayerClass* pClass = NClassQuota::FindInfiniteClassForTeam(TEAM_AMERICANS);
				pPlayer->ForceJoin(pClass, TEAM_AMERICANS, pClass->m_iClassNumber);
			}
		});
	}
}

PLAYER_COMMAND(brit) {
	if (!pPlayer->m_pPermissions->m_bPlayerManage)
		return;

	if (args.ArgC() == 2) {
		PerPlayerCommand(pPlayer, args[1], [](CHL2MP_Player* pPlayer) {
			if (pPlayer->GetTeamNumber() == TEAM_AMERICANS) {
				const CPlayerClass* pClass = NClassQuota::FindInfiniteClassForTeam(TEAM_BRITISH);
				pPlayer->ForceJoin(pClass, TEAM_BRITISH, pClass->m_iClassNumber);
			}
		});
	}
}

static const CPlayerClass* g_pNextLinebattleClass = NULL;
static void LinebattleSetClass(int iTeam, const char* pszAbbreviation) {
	if (!IsLinebattle())
		return;

	//First get pointer to player class
	g_pNextLinebattleClass = CPlayerClass::fromAbbreviation(iTeam, pszAbbreviation);

	if (g_pNextLinebattleClass) {
		const CPlayerClass& pc = *g_pNextLinebattleClass;
		const char* pszSelector = iTeam == TEAM_AMERICANS ? "@amer" : "@brit";

		//set all players of correct team to that class
		PerPlayerCommand(NULL, pszSelector, [](CHL2MP_Player* pPlayer){
			pPlayer->ForceJoin(g_pNextLinebattleClass, g_pNextLinebattleClass->m_iDefaultTeam, g_pNextLinebattleClass->m_iClassNumber);
		});

		//set all other limits on other classes to 0, set our limit to -1
		pc.m_pcvLimit_lrg->SetValue("-1");
		pc.m_pcvLimit_med->SetValue("-1");
		pc.m_pcvLimit_sml->SetValue("-1");
		for (int i = 0; i < CPlayerClass::numClassesForTeam(iTeam); i++) {
			const CPlayerClass* pOtherClass = CPlayerClass::fromNums(iTeam, i);
			if (pOtherClass != &pc) {
				pOtherClass->m_pcvLimit_lrg->SetValue(0);
				pOtherClass->m_pcvLimit_med->SetValue(0);
				pOtherClass->m_pcvLimit_sml->SetValue(0);
			}
		}
	}
}

PLAYER_COMMAND(aclass) {
	if (!pPlayer->m_pPermissions->m_bPlayerManage)
		return;

	//Don't let non-americans change kit
	if (pPlayer->GetTeamNumber() != TEAM_AMERICANS)
		return;
	if (args.ArgC() == 2) {
		LinebattleSetClass(TEAM_AMERICANS, args[1]);
	}
}
PLAYER_COMMAND(bclass) {
	if (!pPlayer->m_pPermissions->m_bPlayerManage)
		return;

	//Don't let non-brits change kit
	if (pPlayer->GetTeamNumber() != TEAM_BRITISH)
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
		if (pPlayer && pPlayer->GetTeamNumber() == iTeam) {
			Msg("Setting player kit\n");
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

PLAYER_COMMAND(akit) {
	//Don't let non-americans change kit
	if (pPlayer->GetTeamNumber() != TEAM_AMERICANS)
		return;

	if (!pPlayer->m_pPermissions->m_bPlayerManage)
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

PLAYER_COMMAND(bkit) {
	//Don't let non-brits change kit
	if (pPlayer->GetTeamNumber() != TEAM_BRITISH)
		return;

	if (!pPlayer->m_pPermissions->m_bPlayerManage)
		return;

	Msg("player bkit\n");

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

void Player_NoClip(CBasePlayer* pPlayer);
PLAYER_COMMAND(clip) {
	if (!pPlayer->m_pPermissions->m_bPlayerManage)
		return;

	const char* pszSelector = "@me";
	if (args.ArgC() > 1)
		pszSelector = args[1];

	PerPlayerCommand(pPlayer, pszSelector, [](CHL2MP_Player* pPlayer){
		if (pPlayer->GetTeamNumber() >= TEAM_AMERICANS) {
			Player_NoClip(pPlayer);
		}
	});
}

PLAYER_COMMAND(rc) {
	if (!pPlayer->m_pPermissions->m_bConsoleAccess)
		return;

	//build a second set of arguments
	char buffer[512];
	Q_snprintf(buffer, sizeof(buffer), "%s\n", args.GetCommandString() + 3);
	Msg(buffer);
	engine->ServerCommand(buffer);
	engine->ServerExecute();
}

//Let's alias some of the actual console commands
//--------------------------------------------------------------------------
// Bot commands
//--------------------------------------------------------------------------
/*PLAYER_COMMAND(bot_kick_all) {
	if (!pPlayer->m_pPermissions->m_bBotManage)
		return;

	//Don't reinvent the wheel, just send it to server console
	engine->ServerCommand("bot_kick_all 1\n");
}

PLAYER_COMMAND(bot_add) {
	if (!pPlayer->m_pPermissions->m_bBotManage)
		return;

	//Don't reinvent the wheel, just send it to server console
	//build a second set of arguments
	char buffer[16];
	Q_snprintf(buffer, sizeof(buffer), "%s\n", args.GetCommandString() + 3);
	engine->ServerCommand(buffer);
}
PLAYER_COMMAND(bot_add_a) {
	if (!pPlayer->m_pPermissions->m_bBotManage)
		return;

	char buffer[16];
	Q_snprintf(buffer, sizeof(buffer), "%s\n", args.GetCommandString() + 3);
	engine->ServerCommand(buffer);
}
PLAYER_COMMAND(bot_add_b) {
	if (!pPlayer->m_pPermissions->m_bBotManage)
		return;

	char buffer[16];
	Q_snprintf(buffer, sizeof(buffer), "%s\n", args.GetCommandString() + 3);
	engine->ServerCommand(buffer);
}
PLAYER_COMMAND(bot_minplayers_mode) {
	if (!pPlayer->m_pPermissions->m_bBotManage)
		return;

	char buffer[16];
	Q_snprintf(buffer, sizeof(buffer), "%s\n", args.GetCommandString() + 3);
	engine->ServerCommand(buffer);
}
PLAYER_COMMAND(bot_minplayers) {
	if (!pPlayer->m_pPermissions->m_bBotManage)
		return;

	char buffer[16];
	Q_snprintf(buffer, sizeof(buffer), "%s\n", args.GetCommandString() + 3);
	engine->ServerCommand(buffer);
}
PLAYER_COMMAND(bot_maxplayers) {
	if (!pPlayer->m_pPermissions->m_bBotManage)
		return;

	char buffer[16];
	Q_snprintf(buffer, sizeof(buffer), "%s\n", args.GetCommandString() + 3);
	engine->ServerCommand(buffer);
}
PLAYER_COMMAND(bot_difficulty) {
	if (!pPlayer->m_pPermissions->m_bBotManage)
		return;

	char buffer[16];
	Q_snprintf(buffer, sizeof(buffer), "%s\n", args.GetCommandString() + 3);
	engine->ServerCommand(buffer);
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
