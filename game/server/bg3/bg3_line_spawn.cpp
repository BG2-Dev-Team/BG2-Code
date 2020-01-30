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
#include "bg3_line_spawn.h"
#include "hl2mp_gamerules.h"
#include "Bots\bg3_bot.h"

BEGIN_DATADESC(CLineSpawn)
	DEFINE_KEYFIELD(m_iForTeam, FIELD_INTEGER, "forTeam"),
	DEFINE_KEYFIELD(m_flWeight, FIELD_FLOAT, "weight"),
	DEFINE_KEYFIELD(m_flSeparationDistance, FIELD_FLOAT, "separationDistance"),
	DEFINE_KEYFIELD(m_flRaycastHeight, FIELD_FLOAT, "raycastHeight"),
	DEFINE_KEYFIELD(m_flGroundSeparation, FIELD_FLOAT, "groundSeparation"),
	DEFINE_KEYFIELD(m_iszSpawnDirection, FIELD_STRING, "spawnDirection"),
	DEFINE_FIELD(m_hSpawnDirection, FIELD_EHANDLE),
END_DATADESC()

LINK_ENTITY_TO_CLASS(info_player_line, CLineSpawn);

void CLineSpawn::CalculateSpawnLocations() {
	//only do this if we can & have to
	if (m_hSpawnDirection && m_aSpawnLocations.Count() != gpGlobals->maxClients) {

		//calc offset between spawn locations
		m_vSpawnDirection = m_hSpawnDirection->GetAbsOrigin() - GetAbsOrigin();
		m_vSpawnDirection.z = 0; //zero out Z direction to simplify slopes
		m_vSpawnDirection.NormalizeInPlace();
		m_vSpawnDirection *= m_flSeparationDistance;
		

		//setup variables and such
		m_aSpawnLocations.RemoveAll();
		Vector nextBaseLocation = GetAbsOrigin();
		Vector nextLocation;
		trace_t tr;
		const Vector VERTICAL_OFFSET(0, 0, m_flRaycastHeight);
		const Vector LITTLE_UP(0, 0, m_flGroundSeparation); //so players don't actually spawn in the ground
		//Keep going 
		for (int i = 0; i < gpGlobals->maxClients; i++) {
			nextLocation = nextBaseLocation + VERTICAL_OFFSET;
			UTIL_TraceLine(nextLocation, nextBaseLocation, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr);
			DebugDrawLine(nextLocation, tr.endpos, 255, 0, 0, false, 99999);
			nextLocation = tr.endpos + LITTLE_UP;
			

			m_aSpawnLocations.AddToTail(nextLocation);
			
			nextBaseLocation += m_vSpawnDirection;
		}
	}
}

struct weighted_spawn_t {
public:
	CLineSpawn*		m_pSpawn;
	float			m_flAccumulatedWeight;
};

CUtlVector<weighted_spawn_t> g_americanSpawns;
CUtlVector<weighted_spawn_t> g_britishSpawns;

float		g_flAmericanWeight = 0.0f;
float		g_flBritishWeight = 0.0f;

CLineSpawn* CLineSpawn::ForceFindSpawnForTeam(int iTeam, CLineSpawn* pExclude) {
	CUtlVector<weighted_spawn_t>* pSpawns = iTeam == TEAM_AMERICANS ? &g_americanSpawns : &g_britishSpawns;

	CLineSpawn* pResult = pExclude;

	//keep going until we run out or we find a unique one
	int i = 0;
	while (i < pSpawns->Count() && pResult == pExclude) {
		pResult = (*pSpawns)[i].m_pSpawn;
		i++;
	}

	//double check that we are not reusing a spawn point
	return pResult == pExclude ? NULL : pResult;
}

void CLineSpawn::RecalculateSpawns(void) {
	g_americanSpawns.RemoveAll();
	g_britishSpawns.RemoveAll();

	g_flAmericanWeight		= 0.0f;
	g_flBritishWeight		= 0.0f;

	CLineSpawn* pSpawn = NULL;
	weighted_spawn_t ws;
	
	while ((pSpawn = static_cast<CLineSpawn*>(gEntList.FindEntityByClassname(pSpawn, "info_player_line"))) != NULL) {
		DevMsg("Recalculating spawns for info_line_spawn\n");
		pSpawn->m_hSpawnDirection = gEntList.FindEntityByName(NULL, pSpawn->m_iszSpawnDirection);
		if (!pSpawn->m_hSpawnDirection) {
			Warning("Failed to find spawn direction \"%s\" for info_line_spawn for team %i\n", pSpawn->m_iszSpawnDirection.ToCStr(), pSpawn->m_iForTeam);
		}
		pSpawn->CalculateSpawnLocations();
		if (pSpawn->IsValid()) {
			ws.m_pSpawn = pSpawn;

			//For British Spawns
			if (pSpawn->m_iForTeam != TEAM_AMERICANS) {
				g_flBritishWeight += pSpawn->m_flWeight;
				ws.m_flAccumulatedWeight = g_flBritishWeight;
				g_britishSpawns.AddToTail(ws);
			}
			//For American Spawns
			if (pSpawn->m_iForTeam != TEAM_BRITISH) {
				g_flAmericanWeight += pSpawn->m_flWeight;
				ws.m_flAccumulatedWeight = g_flAmericanWeight;
				g_americanSpawns.AddToTail(ws);
			}
		}
		else {
			Warning("Failed to validate info_line_spawn for team %i\n", pSpawn->m_iForTeam);
		}
	}
}

CLineSpawn* CLineSpawn::RandomSpawnForTeam(int iTeam) {
	float globalWeight = iTeam == TEAM_AMERICANS ? g_flAmericanWeight : g_flBritishWeight;
	float randomWeight = bot_randfloat(0.0f, globalWeight);

	//iterate through list of spawn points until we find one of the proper accumulated weight
	CUtlVector<weighted_spawn_t>* pSpawns = iTeam == TEAM_AMERICANS ? &g_americanSpawns : &g_britishSpawns;
	CLineSpawn* pResult = NULL;
	float currentWeight = FLT_MAX;

	for (int i = pSpawns->Count() - 1; i >= 0 && currentWeight > randomWeight; i--) {
		pResult = (*pSpawns)[i].m_pSpawn;
		currentWeight = pResult->m_flWeight;
	}

	return pResult;
}

void CLineSpawn::SpawnTeam(int iTeam, CLineSpawn* pSpawn) {
	if (!pSpawn->IsValid()) {
		CLineSpawn::RecalculateSpawns();
		if (!pSpawn->IsValid()) {
			Warning("Aborting spawn from invalid spawn point for team %i\n", iTeam);
			Warning("(%i, %i == %i)\n", pSpawn->m_hSpawnDirection.Get(), pSpawn->m_aSpawnLocations.Count(), gpGlobals->maxClients);
			return;
		}
	}

	int loc = 0;
	QAngle angles;
	VectorAngles(-pSpawn->m_vSpawnDirection, angles);

	for (int i = 1; i < gpGlobals->maxClients; i++) {
		CBasePlayer* pPlayer = UTIL_PlayerByIndex(i);
		if (pPlayer && pPlayer->GetTeamNumber() == iTeam) {
			pPlayer->SetAbsOrigin(pSpawn->m_aSpawnLocations[loc]);
			pPlayer->SetLocalAngles(angles);
			pPlayer->SnapEyeAngles(angles);
			loc++;
		}
	}
}

bool CLineSpawn::SpawnBothTeams() {
	//RecalculateSpawns();
	//we'll make it first-come first-served, but to ensure
	//that the any-team spawnpoints aren't biased toward one team,
	//we randomly decide which team can pick first.

	CLineSpawn* pAmericanSpawn;
	CLineSpawn* pBritishSpawn;

	CLineSpawn**	ppFirstTeamSpawn;
	CLineSpawn**	ppSecondTeamSpawn;
	int				secondTeamNumber;
	if (bot_randfloat() < 0.5f) {
		pAmericanSpawn = RandomSpawnForTeam(TEAM_AMERICANS);
		ppFirstTeamSpawn = &pAmericanSpawn;

		ppSecondTeamSpawn = &pBritishSpawn;
		secondTeamNumber = TEAM_BRITISH;
	}
	else {
		pBritishSpawn = RandomSpawnForTeam(TEAM_BRITISH);
		ppFirstTeamSpawn = &pBritishSpawn;

		ppSecondTeamSpawn = &pAmericanSpawn;
		secondTeamNumber = TEAM_AMERICANS;
	}

	(*ppSecondTeamSpawn) = RandomSpawnForTeam(secondTeamNumber);

	//we need to ensure that the spawns aren't the same
	//also use a counter to ensure an infinite loop doesn't occur when there's only 1 spawn point on the map or something
	int i = 0;
	const int MAX_ITERATIONS = 5;
	while ((*ppSecondTeamSpawn) == (*ppFirstTeamSpawn) && i < MAX_ITERATIONS)  {
		(*ppSecondTeamSpawn) = RandomSpawnForTeam(secondTeamNumber);
		i++;
	}

	if (i == MAX_ITERATIONS) {
		//randomness wasn't enough, so force the finding of unique one if it exists anywhere
		(*ppSecondTeamSpawn) = ForceFindSpawnForTeam(secondTeamNumber, *ppFirstTeamSpawn);
	}

	if (pAmericanSpawn)
		SpawnTeam(TEAM_AMERICANS, pAmericanSpawn);
	else
		Warning("No valid info_player_line found for American team! Tell the mapper!\n");

	if (pBritishSpawn)
		SpawnTeam(TEAM_BRITISH, pBritishSpawn);
	else
		Warning("No valid info_player_line found for British team! Tell the mapper!\n");

	return pBritishSpawn && pAmericanSpawn;
}
