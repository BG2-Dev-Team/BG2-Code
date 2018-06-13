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
#include "hl2mp/hl2mp_player.h"
//#include "Math\static_initialize.h"
#include "../shared/bg3/bg3_buffs.h"
#include "bg2/vcomm.h"

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

PLAYER_COMMAND(kit) {
	if (args.ArgC() < 4)
		return;
	pPlayer->m_iGunKit = atoi(args[1]);
	pPlayer->m_iAmmoKit = atoi(args[2]);
	pPlayer->m_iClassSkin = atoi(args[3]);
}

PLAYER_COMMAND(class) {
	if (args.ArgC() < 3)
		return;
	int iClass = atoi(args[2]);

	//Eh.. it's a little messy, but it seems a bit more efficent to just use a switch here rather than comparing strings over and over.
	switch (atoi(args[1])) //Team
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
	BG3Buffs::RallyRequest(comm, pPlayer);
}

PLAYER_COMMAND(vcomm_halt) {
	uint8 comm = VCOMM2_HALT;
	pPlayer->HandleVoicecomm(comm);
	BG3Buffs::RallyRequest(comm, pPlayer);
}