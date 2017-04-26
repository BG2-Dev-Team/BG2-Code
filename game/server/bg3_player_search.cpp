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

//BG3 - player search class and associated global functions
#include "bg3_player_search.h"

CPlayerSearch::CPlayerSearch() {
	m_pOwner = nullptr;
}

void CPlayerSearch::Init(CBasePlayer* pOwner) {
	m_pOwner = pOwner;
	m_iOwnerTeam = pOwner->GetTeamNumber();
	m_iEnemyTeam = m_iOwnerTeam == TEAM_AMERICANS ? TEAM_BRITISH : TEAM_AMERICANS;
	UpdatePlayers();
	UpdateFlags();
	UpdateWaypointFirst();
}

//returns whether entity B is in sight of entity A
bool CPlayerSearch::IsInSight(CBaseEntity *pA, CBaseEntity *pB)
{
	trace_t tr;
	UTIL_TraceLine(pA->GetLocalOrigin() + Vector(0, 0, 36),
		pB->GetLocalOrigin() + Vector(0, 0, 36),
		MASK_SOLID, pA, COLLISION_GROUP_DEBRIS_TRIGGER, &tr);

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

void CPlayerSearch::UpdateWaypointFirst() {

}

void CPlayerSearch::UpdateOwnerLocation() {
	this->m_vOwnerLocation = m_pOwner->GetAbsOrigin();
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
				totalFriendDist += curDistance;
				if (curDistance < nearestFriendDist) {
					nearestFriend = curPlayer;
					nearestFriendDist = curDistance;
				}
			}//enemies
			else if (curPlayer->GetTeamNumber() == m_iEnemyTeam){
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
	m_flOutnumbered = totalFriendDist / (totalFriendDist + totalEnemyDist);
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
			if (curFlag->GetTeamNumber() == m_iOwnerTeam && (!pClosestEnemy || curDist < flClosestFriend)) {
				flClosestFriend = curDist;
				pClosestFriend = curFlag;
			}
			else if (curDist < flClosestEnemyVisible && IsInSight(m_pOwner, curFlag)) {
				flClosestEnemyVisible = curDist;
				pClosestEnemyVisible = curFlag;
			}
			else if (curDist < flClosestEnemy) {
				flClosestEnemy = curDist;
				pClosestEnemy = curFlag;
			}
		}
	}

	//now store these results for later usage
	m_pCloseEnemyFlag = pClosestEnemy;
	m_pCloseEnemyFlagVisible = pClosestEnemyVisible;
	m_pCloseFriendFlag = pClosestFriend;
}

void CPlayerSearch::UpdateWaypoint() {

}