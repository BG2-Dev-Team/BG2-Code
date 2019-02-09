/*
The Battle Grounds 3 - A Source modification
Copyright (C) 2017, The Battle Grounds 3 Team and Contributors

The Battle Grounds 2 free software; you can redistribute it and/or
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
#include "bg3_bot_debug.h"

#include "bg3_bot.h"

//Global reference to currently followed bot
static CSDKBot* g_pDebugBot = nullptr;

ConVar bot_mimic("bot_mimic", "0", FCVAR_CHEAT | FCVAR_GAMEDLL);
bool Bot_RunMimicCommand()
{
	if (bot_mimic.GetInt() > gpGlobals->maxClients)
		return false;


	CBasePlayer *pPlayer = UTIL_PlayerByIndex(bot_mimic.GetInt());
	if (!pPlayer)
		return false;

	if (!pPlayer->GetLastUserCommand())
		return false;

	CUserCmd cmd = *pPlayer->GetLastUserCommand();
	//cmd.viewangles[YAW] += bot_mimic_yaw_offset.GetFloat();
	RunPlayerMove(g_pDebugBot->m_pPlayer, cmd, gpGlobals->frametime);

	return true;
}

extern ConVar bot_pause;
CON_COMMAND_F(bot_pause_toggle, "Toggles value of bot_pause", FCVAR_CHEAT | FCVAR_GAMEDLL) {
	bot_pause.SetValue(!bot_pause.GetBool());
}

//Command sets which bot to follow
CON_COMMAND_F(bot_debug_pick, "Local server only: given a client index, follows the bot controlling the player and displays its navigation information in the world.", FCVAR_CHEAT | FCVAR_GAMEDLL) {
	if (args.ArgC() == 2) {
		CBasePlayer* pPlayer = UTIL_PlayerByIndex(atoi(args[1]));
		if (pPlayer && pPlayer->IsFakeClient()) {
			CSDKBot* pBot = CSDKBot::ToBot(pPlayer);
			if (pBot) g_pDebugBot = pBot;
		}

	}
}

ConVar bot_debug_follow("bot_debug_follow", "0", FCVAR_CHEAT | FCVAR_GAMEDLL);

static int g_iDebugBotWeapon = 0;
CON_COMMAND_F(bot_debug_switch, "Tells the currently controlled bot to switch its weapon.", FCVAR_CHEAT | FCVAR_GAMEDLL) {
	if (!g_pDebugBot)
		return;
	g_iDebugBotWeapon++;
	if (g_iDebugBotWeapon >= g_pDebugBot->m_pPlayer->WeaponCount())
		g_iDebugBotWeapon = 0;

	CBaseCombatWeapon* next = g_pDebugBot->m_pPlayer->GetWeapon(g_iDebugBotWeapon);
	if (next)
		g_pDebugBot->m_pPlayer->Weapon_Switch(next);
}

CON_COMMAND_F(bot_debug_say, "Tells the selected bot to say the given text to chat.", FCVAR_CHEAT | FCVAR_GAMEDLL) {
	if (!g_pDebugBot || args.ArgC() != 2)
		return;

	char textBuffer[128];
	Q_snprintf(textBuffer, sizeof(textBuffer), "\"%s\"", args[1]);
	CCommand sayCommand;
	sayCommand.Tokenize(textBuffer, CCommand::DefaultBreakSet());

	Host_Say(g_pDebugBot->m_pPlayer->edict(), sayCommand, false);
	
	
}

void BotDebugThink() {
	if (g_pDebugBot && g_pDebugBot->m_pPlayer->IsAlive()) {
		if (bot_mimic.GetBool())
			Bot_RunMimicCommand();
		if (bot_debug_follow.GetBool()) {
			CPlayerSearch& ps = g_pDebugBot->m_PlayerSearchInfo;


			CBasePlayer* pEnemy = ps.CloseEnemy();
			CBasePlayer* pSecond = ps.CloseEnemySecond();
			CBasePlayer* pFriend = ps.CloseFriend();

			CBaseEntity* pFlag = ps.CloseEnemyFlagVisible();
			CBaseEntity* pFriendFlag = ps.CloseFriendFlag();


			Color color = COLOR_WHITE;

			if (pEnemy) {
				if (g_pDebugBot->m_pCurThinker == &BotThinkers::LongRange) {
					color = Color(255, 255, 0);
				}
				else if (g_pDebugBot->m_pCurThinker == &BotThinkers::MedRange) {
					color = COLOR_ORANGE;
				}
				else if (g_pDebugBot->m_pCurThinker == &BotThinkers::Melee) {
					color = COLOR_RED;
				}

				NDebugOverlay::LineForced(g_pDebugBot->m_pPlayer->Weapon_ShootPosition(), pEnemy->Weapon_ShootPosition(), color.r(), color.g(), color.b(), true, 0.1f);
			}
			if (pSecond) {
				if (g_pDebugBot->m_pCurThinker == &BotThinkers::LongRange) {
					color = Color(255, 255, 0);
				}
				else if (g_pDebugBot->m_pCurThinker == &BotThinkers::MedRange) {
					color = COLOR_ORANGE;
				}
				else if (g_pDebugBot->m_pCurThinker == &BotThinkers::Melee) {
					color = COLOR_RED;
				}

				NDebugOverlay::LineForced(g_pDebugBot->m_pPlayer->Weapon_ShootPosition(), pSecond->Weapon_ShootPosition(), color.r(), color.g(), color.b(), true, 0.1f);
			}
			if (pFriend) {
				NDebugOverlay::LineForced(g_pDebugBot->m_pPlayer->Weapon_ShootPosition(), pFriend->Weapon_ShootPosition(), 0, 255, 0, true, 0.1f);
			}
			if (pFlag) {
				NDebugOverlay::LineForced(g_pDebugBot->m_pPlayer->Weapon_ShootPosition(), pFlag->GetAbsOrigin(), 255, 255, 0, true, 0.1f);
			}
			if (pFriendFlag) {
				NDebugOverlay::LineForced(g_pDebugBot->m_pPlayer->Weapon_ShootPosition(), pFriendFlag->GetAbsOrigin(), 0, 255, 0, true, 0.1f);
			}
			if (g_pDebugBot->m_pCurThinker == &BotThinkers::Waypoint) {
				CBaseEntity* pWaypoint = ps.CurNavpoint();
				if (pWaypoint) {
					NDebugOverlay::LineForced(g_pDebugBot->m_pPlayer->Weapon_ShootPosition(), pWaypoint->GetAbsOrigin(), 255, 255, 255, true, 0.1f);
				}
			}
		}
	}
}