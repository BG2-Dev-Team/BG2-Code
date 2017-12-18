/*
The Battle Grounds 3 - A Source modification
Copyright (C) 2017, The Battle Grounds 2 Team and Contributors

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
#include "bg3_bot_influencer.h"
#include "bg3\static_initialize.h"
#include "bg3\bg3_player_shared.h"
//#include "memory"

#define DEF_EXECUTOR(name) \
	NBotInfluencer::pfExecutor NBotInfluencer::s_pf##name = NBotInfluencer::Execute_##name; \
	void NBotInfluencer::Execute_##name(CSDKBot* pBot)

using namespace BotThinkers;

DEF_EXECUTOR(Fire) {
	//we could just modify m_flNextFireTime, but that wouldn't guarantee that we'd fire immediately
	if (pBot->CanFire()
		&& pBot->m_PlayerSearchInfo.CloseEnemy()) {
		pBot->ScheduleThinker(&BotThinkers::PointBlank, bot_randfloat(0.0f, 0.2f));
	}
}

DEF_EXECUTOR(Retreat) {
	//This is copied from ThinkCheckRetreat() but has more lenient conditions
	if (pBot->m_PlayerSearchInfo.CloseEnemyDist() < 1000 && pBot->m_PlayerSearchInfo.CloseFriendDist() < 1000
		&& pBot->m_PlayerSearchInfo.CloseFriendDist() > CSDKBot::RETREAT_STOP_RANGE) {
		pBot->ScheduleThinker(&BotThinkers::Retreat, BotThinkers::Retreat.m_flThinkDelay);
	}
}

DEF_EXECUTOR(Advance) {
	//most of the time bots are already moving forward because they're more efficient than human players
	if (pBot->m_PlayerSearchInfo.CloseFriend()
		&& (pBot->m_pCurThinker == &MedRange || pBot->m_pCurThinker == &Melee)) {

		pBot->m_curCmd.buttons |= IN_FORWARD;
		pBot->m_curCmd.buttons &= ~IN_BACK;

		//set next strafe time so we'll be moving forward for a little while longer
		pBot->m_flNextStrafeTime = gpGlobals->curtime + bot_randfloat(3.0f, 5.0f);
	}
}

DEF_EXECUTOR(Halt) {
	//If we're retreating or waypoint navigating, we'll say NO
	if (pBot->m_pCurThinker == &Retreat || pBot->m_pCurThinker == &Waypoint) {
		pBot->SendBotVcommContext(BotContext::NEGATE);
	}
	else if (pBot->m_pCurThinker == &LongRange || pBot->m_pCurThinker == &MedRange) {
		//stop moving
		pBot->StopMoving();

		//set next strafe time so that we'll stand still for a while
		pBot->m_flNextStrafeTime = gpGlobals->curtime + 4.0f;
	}
}

DEF_EXECUTOR(Rally_Round) {

}

//for unique_ptr
using namespace std;

void NBotInfluencer::InitMap(CUtlMap<comm_t, NBotInfluencer::pfExecutor>* pMap) {
	pMap->Insert(VCOMM2_FIRE,		s_pfFire);
	pMap->Insert(VCOMM2_ADVANCE,	s_pfAdvance);
	pMap->Insert(VCOMM2_HALT,		s_pfHalt);
	pMap->Insert(VCOMM2_RETREAT,	s_pfRetreat);
}

//Sets up map of voice command codes to influencing functions
CUtlMap<comm_t, NBotInfluencer::pfExecutor>* NBotInfluencer::CreateMap() {
	CUtlMap<comm_t, pfExecutor>* pMap = new CUtlMap<comm_t, pfExecutor>();
	
	SetDefLessFunc(*pMap);

	pMap->Insert(VCOMM2_FIRE,		s_pfFire);
	pMap->Insert(VCOMM2_ADVANCE,	s_pfAdvance);
	pMap->Insert(VCOMM2_HALT,		s_pfHalt);
	pMap->Insert(VCOMM2_RETREAT,	s_pfRetreat);

	return pMap;
}

//Global map of voice command codes to bot influencing functions
CUtlMap<comm_t, NBotInfluencer::pfExecutor>* NBotInfluencer::g_pExecutorMap = CreateMap();

//Valve's map class is devoid of any common-sense "contains" function so we'll use this instead
//Some day I should reimplement those utility classes myself
bool IsExecutableVcomm(comm_t iComm) {
	return iComm == VCOMM2_FIRE || iComm == VCOMM2_ADVANCE || iComm == VCOMM2_RETREAT || iComm == VCOMM2_HALT;
}

void NBotInfluencer::DispatchCommand(CBasePlayer* pRequester, vec_t vRange, comm_t iComm) {
	//Get our executor we'll send to the bots
	unsigned short pfIndex = g_pExecutorMap->Find(iComm);
	pfExecutor pExecutor = g_pExecutorMap->Element(pfIndex);

	for (int i = 1; i <= gpGlobals->maxClients; i++) {
		CBasePlayer* pPlayer = UTIL_PlayerByIndex(i);
		if (pPlayer && pPlayer->IsFakeClient() && pPlayer->IsAlive() && EntDist(*pRequester, *pPlayer) < vRange) {

			//here we have a valid bot who's close enough to us
			CSDKBot* pBot = CSDKBot::ToBot(pPlayer);

			//Some bots may be retreating or meleeing
			if (ShouldIgnore(pBot))
				continue;

			//Call the executor onto the bot
			pExecutor(pBot);
		}
	}
}

//We won't let players harass & confuse the bots by using these
//TODO make these for individual bots instead of whole teams,
//but this should work for now.
float g_flNextAmericanExecutor	= -1.f;
float g_flNextBritishExecutor	= -1.f;
const float g_flExecutorInterval = 2.0f;

void NBotInfluencer::NotifyCommand(CBasePlayer* pPlayer, comm_t iComm) {
	if (!IsExecutableVcomm(iComm))
		return;

	//assuming pPlayer is non-null, this is next allowed time for our team
	float& nextCommand = pPlayer->GetTeamNumber() == TEAM_AMERICANS ? g_flNextAmericanExecutor : g_flNextBritishExecutor;

	if (gpGlobals->curtime > nextCommand) {
		//Set next time for parsing command
		nextCommand = gpGlobals->curtime + g_flExecutorInterval;

		//Dispatch the command to the bots
		DispatchCommand(pPlayer, GetExecutorCommandRange(), iComm);
	}
}