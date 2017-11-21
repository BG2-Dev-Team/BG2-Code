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

#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "player.h"
//#include "sdk_player.h"
#include "in_buttons.h"
#include "movehelper_server.h"
#include "gameinterface.h"
#include "team.h"
#include "hl2mp_gamerules.h"
#include "../bg2/flag.h"
#include "../bg2/weapon_bg2base.h"
#include "bg3_bot_manager.h"
#include "iconvar.h"

#include "bg3_bot.h"
#include "bg3_bot_vcomms.h"
#include "bg3_player_search.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
//#include "hl2mp_gamerules.h"

/*
Singletons - handle bots for a given team.
*/

ConVar bot_vcomms("bot_vcomms", "1", FCVAR_GAMEDLL, "Whether or not to allow bots to use voice commands");

CBotComManager g_BotBritComms(TEAM_BRITISH);
CBotComManager g_BotAmerComms(TEAM_AMERICANS);

/*
Enumerated Bot Comms - these arrays are used to parse the client commands sent to players
the numbers are converted into their characters in a one-time operation in the manager's constructor
*/
comm_t g_vclAdvance[]	{VCOMM2_ADVANCE, VCOMM2_RALLY_ROUND, VCOMM_FOLLOW, VCOMM_ENEMY_AHEAD, 0};
comm_t g_vclRetreat[]	{VCOMM2_RETREAT, VCOMM_ON_THE_FLANKS, VCOMM_ENEMY_AHEAD, 0};
comm_t g_vclObjective[]	{VCOMM2_OBJECTIVE, VCOMM2_RALLY_ROUND, 0};
comm_t g_vclClear[]		{VCOMM_AREA_SECURE, 0};
comm_t g_vclFire[]		{VCOMM2_FIRE, VCOMM2_FIRE, VCOMM2_PRESENT, VCOMM2_MAKE_READY, 0};
comm_t g_vclAffirm[]	{VCOMM_AYE, 0};
comm_t g_vclNegate[]	{VCOMM_NO, 0};
comm_t g_vclDefault[]	{VCOMM_BATTLECRY, 0};

int strlen(comm_t* cCom) {
	int count = 0;
	while (*cCom) {
		count++;
		cCom++;
	}
	return count;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Constructors and Initializers
//-------------------------------------------------------------------------------------------------

CBotComManager::CBotComManager(int iTeam) {
	m_iTeam = iTeam;
	m_eContext = CONTEXT_NONE;
	m_flNextComm = FLT_MAX;
	m_pContextPlayer = nullptr;
	m_eForcedContext = CONTEXT_NONE;
}

void CBotComManager::ResetThinkTime(float flDelay) {
	m_flNextComm = gpGlobals->curtime + flDelay;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Private helper functions
//-------------------------------------------------------------------------------------------------
comm_t CBotComManager::ParseContext(BotContext context) {
	comm_t* commArray = g_vclDefault;
	switch (context)
	{
	case ADVANCE:
		commArray = g_vclAdvance;
		break;
	case RETREAT:
		commArray = g_vclRetreat;
		break;
	case OBJECTIVE:
		commArray = g_vclObjective;
		break;
	case CLEAR:
		commArray = g_vclClear;
		break;
	case FIRE:
		commArray = g_vclFire;
		break;
	case AFFIRM:
		commArray = g_vclAffirm;
		break;
	case NEGATE:
		commArray = g_vclNegate;
		break;
	}
	comm_t result = *commArray;
	int length = strlen(commArray);
	if (length > 0)
		result = *(commArray + bot_rand(0, length - 1));
	return result;
}

void CBotComManager::DispatchVCommToBot() {
	if (m_pContextPlayer) {
		if (m_eForcedContext) {
			m_eContext = m_eForcedContext;
			m_eForcedContext = CONTEXT_NONE;
			m_pContextPlayer = CPlayerSearch::FriendlyBotNearestTo(m_pContextPlayer);
		}
		//nearest bot might be null so check it again
		if (m_pContextPlayer && m_pContextPlayer->IsFakeClient()) {
			
			//build a vcomm
			static char vcommTemplate[64];
			Q_snprintf(vcommTemplate, sizeof vcommTemplate, "voicecomm %i\n", ParseContext(m_eContext));

			static CCommand vcommCmd;
			vcommCmd.Tokenize(vcommTemplate);
			m_pContextPlayer->ClientCommand(vcommCmd);
		}
	}
}

//-------------------------------------------------------------------------------------------------
// Purpose: Public interface implementation
//-------------------------------------------------------------------------------------------------
void CBotComManager::Think() {
	if (bot_vcomms.GetBool() && m_flNextComm < gpGlobals->curtime && !IsLinebattle()) {
		DispatchVCommToBot();
		m_flNextComm = gpGlobals->curtime + bot_randfloat(3.0, 8.0);
	}
}

void CBotComManager::ReceiveContext(CBasePlayer* pDispatcher, BotContext eContext, bool bSoundAlreadyPlayed) {
	//don't always respond to other contexts
	if (bSoundAlreadyPlayed && bot_randfloat() < 0.35f) {
		if (eContext == RETREAT || eContext == NEGATE)
			m_eForcedContext = NEGATE;
		else if (eContext == ADVANCE && bot_randfloat() < 0.6f)
			m_eForcedContext = ADVANCE;
		else
			m_eForcedContext = AFFIRM;
		ResetThinkTime(bot_randfloat(0.75f, 1.75f));
	} else if (!bSoundAlreadyPlayed) {
		m_eContext = eContext;
	}
	m_pContextPlayer = pDispatcher;
}

BotContext CBotComManager::ParseIntToContext(int iCom) {
	BotContext result = DEFAULT;
	switch (iCom) {
	case VCOMM2_ADVANCE:
		result = ADVANCE; break;
	case VCOMM2_RETREAT:
		result = RETREAT; break;
	case VCOMM_NO:
		result = NEGATE; break;
	}
	return result;
}

CBotComManager* CBotComManager::GetBotCommsOfPlayer(CBasePlayer* pPlayer) {
	if (pPlayer->GetTeamNumber() == TEAM_BRITISH)
		return &g_BotBritComms;
	else
		return &g_BotAmerComms;
}

