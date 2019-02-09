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
battlegrounds3.com

Note that because of the sheer volume of files in the Source SDK this
notice cannot be put in all of them, but merely the ones that have any
changes from the original SDK.
In order to facilitate easy searching, all changes are and must be
commented on the following form:

//BG3 - <name of contributer>[ - <small description>]
*/

#include "cbase.h"
#include "bg3_bot_navpoint.h"
#include "bg3_bot.h"
#include "bg3_player_search.h"


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC(CBotNavpoint)
	DEFINE_KEYFIELD( m_iszBritPoint, FIELD_STRING, "britPointName" ),
	DEFINE_KEYFIELD( m_iszAmerPoint, FIELD_STRING, "amerPointName" ),
	DEFINE_KEYFIELD( m_iszAltPoint, FIELD_STRING, "altPointName"),

	DEFINE_KEYFIELD( m_flBritAltChance, FIELD_FLOAT, "britAltChance" ),
	DEFINE_KEYFIELD( m_flAmerAltChance, FIELD_FLOAT, "amerAltChance" ),

	DEFINE_KEYFIELD( m_bForceBritAsForward, FIELD_BOOLEAN, "forceBritAsForward" ),

	DEFINE_KEYFIELD( m_flRadius, FIELD_FLOAT, "radius" ),
	DEFINE_KEYFIELD( m_eRange, FIELD_INTEGER, "enemyNoticeRange" ),
END_DATADESC()

LINK_ENTITY_TO_CLASS(bot_navpoint, CBotNavpoint);

void CBotNavpoint::Spawn() {
	CBaseEntity* pEnt = nullptr;
	while ((pEnt = gEntList.FindEntityByClassname(pEnt, "bot_navpoint")) != nullptr) {
		CBotNavpoint* pPoint = dynamic_cast<CBotNavpoint*>(pEnt);
		if (pPoint) {
			const char * szName = pPoint->GetEntityName().ToCStr();
			if (FStrEq(szName, m_iszBritPoint.ToCStr()))
				m_pBritPoint = pPoint;
			if (FStrEq(szName, m_iszAmerPoint.ToCStr()))
				m_pAmerPoint = pPoint;
			if (FStrEq(szName, m_iszAltPoint.ToCStr()))
				m_pAltPoint = pPoint;
		}
	}
	//check against unfounds
	if (*m_iszBritPoint.ToCStr() && !m_pBritPoint)
		Warning("bot_navpoint \"%s\" could not find \"%s\"! Tell the mapper!\n", this->GetEntityName(), m_iszBritPoint.ToCStr());
	//check against unfounds
	if (*m_iszAmerPoint.ToCStr() && !m_pAmerPoint)
		Warning("bot_navpoint \"%s\" could not find \"%s\"! Tell the mapper!\n", this->GetEntityName(), m_iszAmerPoint.ToCStr());
	//check against unfounds
	if (*m_iszAltPoint.ToCStr() && !m_pAltPoint)
		Warning("bot_navpoint \"%s\" could not find \"%s\"! Tell the mapper!\n", this->GetEntityName(), m_iszAltPoint.ToCStr());
}

CBotNavpoint* CBotNavpoint::ClosestPointToPlayer(CBasePlayer* pPlayer, bool bCheckVis) {
	vec_t minDist = FLT_MAX;
	CBaseEntity* pEnt = nullptr;
	CBotNavpoint* pClosestPoint = nullptr;

	//simple min-finding here
	while ((pEnt = gEntList.FindEntityByClassname(pEnt, "bot_navpoint")) != nullptr) {
		CBotNavpoint* pPoint = dynamic_cast<CBotNavpoint*>(pEnt);
		if (pPoint && (!bCheckVis || CPlayerSearch::IsInSight(pPoint->GetAbsOrigin() + Vector(0,0,16), pPlayer->Weapon_ShootPosition(), pPlayer))) {
			vec_t dist = EntDist(*pPoint, *pPlayer);
			if (dist < minDist) {
				minDist = dist;
				pClosestPoint = pPoint;
			}
		}
	}

	return pClosestPoint;
}

CBotNavpoint* CBotNavpoint::NextPointForPlayer(const CBasePlayer* pPlayer) {
	int iTeam = pPlayer->GetTeamNumber();

	//first check for alt branch
	if (m_pAltPoint) {
		float chance = iTeam == TEAM_AMERICANS ? m_flAmerAltChance : m_flBritAltChance;
		if (bot_randfloat() < chance) {
			return m_pAltPoint;
		}
	}

	CBotNavpoint* pNext;
	if (m_pBritPoint && m_bForceBritAsForward) {
		pNext = m_pBritPoint;
	} else {
		pNext = iTeam == TEAM_AMERICANS ? m_pAmerPoint : m_pBritPoint;
	}
	
	if (pNext)
		return pNext;
	
	//ensure we always return something that's not null
	if (m_pBritPoint)
		return m_pBritPoint;
	if (m_pAmerPoint)
		return m_pAmerPoint;
	if (m_pAltPoint)
		return m_pAltPoint;
	return this;
}

bool CBotNavpoint::PlayerWithinRadius(const CBasePlayer* pPlayer) const {
	//radius here is clamped to a minimum of 25
	return EntDist(*pPlayer, *this) < MAX(m_flRadius, 35);
}

Vector CBotNavpoint::GetLookTarget() const {
	//build a random point within a rectangle and then clamp it
	//this will lead to non-random tendency toward boundary points, but the math is easier
	Vector randomOffset(bot_randfloat(-m_flRadius, m_flRadius), bot_randfloat(-m_flRadius, m_flRadius), 60 + bot_randfloat(-10, 10));

	vec_t length = randomOffset.Length();
	if (length > m_flRadius)
		randomOffset *= (m_flRadius / length); //simple recipricol clamps length

	return GetAbsOrigin() + randomOffset;
}