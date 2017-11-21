/*
The Battle Grounds 3 - A Source modification
Copyright (C) 2017, The Battle Grounds 2 Team and Contributors

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
www.bg2mod.com

Note that because of the sheer volume of files in the Source SDK this
notice cannot be put in all of them, but merely the ones that have any
changes from the original SDK.
In order to facilitate easy searching, all changes are and must be
commented on the following form:

//BG2 - <name of contributer>[ - <small description>]
*/

#ifdef WIN32
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
#include "hl2mp_player.h"
#include "../bg2/flag.h"
#include "../bg2/weapon_bg2base.h"
#include "../shared/bg3/bg3_math_shared.h"

//BG3 - player search class and associated global functions
#include "bg3_player_search.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


CPlayerSearch::CPlayerSearch() {
	m_pOwner = nullptr;
}

void CPlayerSearch::Init(CBasePlayer* pOwner) {
	m_pOwner = pOwner;
	m_iOwnerTeam = pOwner->GetTeamNumber();
	m_iEnemyTeam = m_iOwnerTeam == TEAM_AMERICANS ? TEAM_BRITISH : TEAM_AMERICANS;
	UpdatePlayers();
	UpdateFlags();
	//UpdateWaypointFirst();
}

//returns whether entity B is in sight of entity A
bool CPlayerSearch::IsInSight(CBaseEntity *pA, CBaseEntity *pB)
{
	trace_t tr;
	UTIL_TraceLine(pA->GetLocalOrigin() + Vector(0, 0, 36),
		pB->GetLocalOrigin() + Vector(0, 0, 36),
		MASK_BLOCKLOS, pA, COLLISION_GROUP_DEBRIS_TRIGGER, &tr);

	return !tr.DidHitWorld();
}

//returns whether these two world locations can see each other
bool CPlayerSearch::IsInSight(Vector v1, Vector v2, CBaseEntity* pIgnore) {
	trace_t tr;
	UTIL_TraceLine(v1,
				   v2,
				   MASK_BLOCKLOS, pIgnore, COLLISION_GROUP_NPC, &tr);

	return !tr.DidHitWorld();
}

CFlag* CPlayerSearch::FindClosestFlagToSpot(CBasePlayer* pPlayer, bool insight, bool checkTeam, Vector vSpot) {
	//if checkTeam, ignore flags that belong to the bot's team
	int team = pPlayer->GetTeam()->GetTeamNumber();
	CFlag *pClosest = NULL;
	vec_t dist = 1000000000;
	CBaseEntity *pEntity = NULL;

	while ((pEntity = gEntList.FindEntityByClassname(pEntity, "flag")) != NULL)
	{
		CFlag *pFlag = dynamic_cast<CFlag*>(pEntity);

		if (!pFlag || (checkTeam && pFlag->GetTeamNumber() == team))
			continue;

		if (insight)
		{
			//check to make sure flag is in sight
			if (!IsInSight(pPlayer, pFlag))
				continue;
		}

		vec_t dist2 = pFlag->GetLocalOrigin().DistToSqr(vSpot);

		if (!pClosest || dist2 < dist)
		{
			pClosest = pFlag;
			dist = dist2;
		}
	}

	return pClosest;
}

CBasePlayer* CPlayerSearch::FriendlyBotNearestTo(CBasePlayer* pOtherPlayer) {
	CBasePlayer* pFriendlyBot = nullptr;
	CBasePlayer* pCurPlayer = nullptr;
	vec_t		 flMinDist = FLT_MAX;
	int			 iTeam = pOtherPlayer->GetTeamNumber();

	//iterate through all players to find closest fake client
	for (int i = 0; i <= gpGlobals->maxClients; i++) {
		pCurPlayer = UTIL_PlayerByIndex(i);
		if (pCurPlayer && pCurPlayer->IsFakeClient() && pCurPlayer->GetTeamNumber() == iTeam
			&& pCurPlayer->IsAlive()) {
			vec_t flDist = EntDist(*pCurPlayer, *pOtherPlayer);
			if (flDist < flMinDist) {
				flMinDist = flDist;
				pFriendlyBot = pCurPlayer;
			}
		}
	}
	return pFriendlyBot;
}


void CPlayerSearch::UpdateOwnerLocation() {
	this->m_vOwnerLocation = m_pOwner->GetAbsOrigin();
}

void CPlayerSearch::UpdateOwnerTeamInfo() {
	m_iOwnerTeam = m_pOwner->GetTeamNumber();
	m_iEnemyTeam = m_iOwnerTeam == TEAM_AMERICANS ? TEAM_BRITISH : TEAM_AMERICANS;
}

void CPlayerSearch::UpdatePlayers() {
	CBasePlayer* curPlayer = nullptr;
	float		 curDistance;
	CBasePlayer* nearestEnemy = nullptr;
	CBasePlayer* nearestEnemySecond = nullptr;
	CBasePlayer* nearestFriend = nullptr;
	float nearestEnemyDist = FLT_MAX;
	float nearestEnemySecondDist = FLT_MAX;
	float nearestFriendDist = FLT_MAX;

	int iNumFriendly = 0;
	int iNumEnemy = 0;

	float totalEnemyDist = 0;
	float totalFriendDist = 0;

	//simple maxes and mins here
	for (int i = 1; i <= gpGlobals->maxClients; i++) {
		curPlayer = UTIL_PlayerByIndex(i);
		//check only valid alive players
		if (curPlayer && curPlayer != m_pOwner && curPlayer->IsAlive() && IsInSight(m_pOwner, curPlayer)) {
			curDistance = (m_vOwnerLocation - curPlayer->GetAbsOrigin()).Length();
			//friendlies
			if (curPlayer->GetTeamNumber() == m_iOwnerTeam) {
				iNumFriendly++;
				totalFriendDist += curDistance;
				if (curDistance < nearestFriendDist) {
					nearestFriend = curPlayer;
					nearestFriendDist = curDistance;
				}
			}//enemies
			else if (curPlayer->GetTeamNumber() == m_iEnemyTeam){
				iNumEnemy++;
				totalEnemyDist += curDistance;
				if (curDistance < nearestEnemyDist) {
					nearestEnemy = curPlayer;
					nearestEnemyDist = curDistance;
					if (!nearestEnemySecond) {
						nearestEnemySecond = curPlayer;
						nearestEnemySecondDist = curDistance;
					}
				}
				else if (curDistance < nearestEnemySecondDist) {
					nearestEnemySecond = curPlayer;
					nearestEnemySecondDist = curDistance;
				}
			}
		}
	}

	//now remember the search data by storing it in the member variables
	m_pCloseEnemy				= nearestEnemy;
	m_pCloseEnemySecond			= nearestEnemySecond;
	m_pCloseFriend				= nearestFriend;
	m_flCloseEnemyDist			= nearestEnemyDist;
	m_flCloseEnemySecondDist	= nearestEnemySecondDist;
	m_flCloseFriendDist			= nearestFriendDist;

	//calculated outnumberedness - as friends get closer the value decreases
	if (iNumEnemy != 0 && iNumFriendly != 0) {
		totalFriendDist /= iNumFriendly;
		totalEnemyDist /= iNumEnemy;
		m_flOutnumbered = totalFriendDist / (totalFriendDist + totalEnemyDist);
	} else
		m_flOutnumbered = 0.5f;
}

void CPlayerSearch::UpdateFlags() {
	CFlag* pClosestFriend = nullptr; vec_t flClosestFriend = FLT_MAX;
	CFlag* pClosestEnemy = nullptr; vec_t flClosestEnemy = FLT_MAX;
	CFlag* pClosestEnemyVisible = nullptr; vec_t flClosestEnemyVisible = FLT_MAX;
	CBaseEntity *pEntity = nullptr;

	while ((pEntity = gEntList.FindEntityByClassname(pEntity, "flag")) != nullptr) {
		CFlag *curFlag = dynamic_cast<CFlag*>(pEntity);
		if (curFlag) {
			vec_t curDist = (m_vOwnerLocation - curFlag->GetAbsOrigin()).Length();
			if (curFlag->GetTeamNumber() == m_iOwnerTeam) {
				if (curDist < flClosestFriend) {
					flClosestFriend = curDist;
					pClosestFriend = curFlag;
				} else {
					continue;
				}
			} 
			else if (curDist < curFlag->m_flBotNoticeRange){
				if (curDist < flClosestEnemyVisible && IsInSight(m_pOwner, curFlag)) {
					flClosestEnemyVisible = curDist;
					pClosestEnemyVisible = curFlag;
				} else if (curDist < flClosestEnemy) {
					flClosestEnemy = curDist;
					pClosestEnemy = curFlag;
				}
			}
		}
	}

	//now store these results for later usage
	m_pCloseEnemyFlag = pClosestEnemy;
	m_pCloseEnemyFlagVisible = pClosestEnemyVisible;
	m_pCloseFriendFlag = pClosestFriend;
}

void CPlayerSearch::UpdateNavpoint() {
	if (m_pCurNavpoint->PlayerWithinRadius(m_pOwner)) {
		m_pCurNavpoint = m_pCurNavpoint->NextPointForPlayer(m_pOwner);
	}
}

void CPlayerSearch::UpdateNavpointFirst() {
	//first look for visible waypoint, then invisible one
	m_pCurNavpoint = CBotNavpoint::ClosestPointToPlayer(m_pOwner, true);

	//give a warning to the mapper if the bot couldn't find one
	if (!m_pCurNavpoint) {
		Warning("\"%s\" couldn't find a visible bot_navpoint! Tell the mapper!\n", m_pOwner->GetPlayerName());
		m_pCurNavpoint = CBotNavpoint::ClosestPointToPlayer(m_pOwner, false);
	}
}
