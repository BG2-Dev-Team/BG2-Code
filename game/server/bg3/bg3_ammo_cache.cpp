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
#include "bg3_ammo_cache.h"
#include "../shared/bg3/bg3_math_shared.h"

BEGIN_DATADESC(CAmmoCache)
	DEFINE_KEYFIELD(m_iForTeam, FIELD_CHARACTER, "forTeam"),
	DEFINE_KEYFIELD(m_flRadius, FIELD_FLOAT, "radius"),
END_DATADESC()

LINK_ENTITY_TO_CLASS(info_ammo_cache, CAmmoCache);

ConVar mp_ammo_cache_refresh_time("mp_ammo_cache_refresh_time", "30", FCVAR_GAMEDLL);
ConVar mp_disable_ammo_cache("mp_disable_ammo_cache_on_restart", "0", FCVAR_GAMEDLL);

void CAmmoCache::Spawn() {
	ThinkSet((BASEPTR) &CAmmoCache::Think);
	
	if (mp_disable_ammo_cache.GetBool()) {
		SetNextThink(FLT_MAX);
	}
	else {
		SetNextThink(gpGlobals->curtime + 1.0f);
	}
	

	//clamp our radius to a minimum
	Clamp(m_flRadius, 0.f, m_flRadius);
}

void CAmmoCache::Think() {
	//we need to iterate through all players with a loop,
	//but we can actually be more efficient if we use two loops
	//with goto
	vec_t minPlayerDistance = FLT_MAX;
	int i;
	CHL2MP_Player* pPlayer;
	int activatorTeam;

	for (i = 0; i < gpGlobals->maxClients; i++) {
		pPlayer = static_cast<CHL2MP_Player*>(UTIL_PlayerByIndex(i));
		if (pPlayer && (m_iForTeam == TEAM_ANY || pPlayer->GetTeamNumber() == m_iForTeam)) {
			vec_t dist = EntDist(*this, *pPlayer);
			if (dist < minPlayerDistance) {
				if (dist < m_flRadius) {
					activatorTeam = pPlayer->GetTeamNumber();
					goto giveammoloop;
				}
				else {
					minPlayerDistance = dist;
				}
			}
		}
	}
	//we didn't encounter any players within our radius
	//set next think based on distance to nearest player
	minPlayerDistance -= m_flRadius;
	float nextThink = gpGlobals->curtime + minPlayerDistance / 230;
	nextThink = min(nextThink, gpGlobals->curtime + 30);
	SetNextThink(nextThink); //the time here is an estimation of the time it takes for players to run to us.
	return;

giveammoloop:
	//index stays the same as before
	for (i; i < gpGlobals->maxClients; i++) {
		pPlayer = static_cast<CHL2MP_Player*>(UTIL_PlayerByIndex(i));
		if (pPlayer 
			&& (m_iForTeam == TEAM_ANY || pPlayer->GetTeamNumber() == m_iForTeam)
			&& (IsLinebattle() || EntDist(*this, *pPlayer) < m_flRadius)
			&& pPlayer->GetTeamNumber() == activatorTeam
			&& pPlayer->m_flNextAmmoRefill < gpGlobals->curtime) {
			pPlayer->m_flNextAmmoRefill = gpGlobals->curtime + mp_ammo_cache_refresh_time.GetFloat(); //30 seconds intervals
			pPlayer->SetDefaultAmmoFull(true);
		}
	}
	//Set next think to 0.5s later
	SetNextThink(gpGlobals->curtime + 0.5f);
}
