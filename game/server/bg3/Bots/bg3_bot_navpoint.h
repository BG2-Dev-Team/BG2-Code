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
// #ifdef WIN32
#pragma once
// #endif

#include "cbase.h"
#include "baseentity.h"

//enumator for at what distance the bot using this waypoint should
//stop using this waypoint and fight enemies instead
enum ENavPointRange {
	MELEE, //use for closed, tight spaces
	MED_RANGE, //use for semi-open areas
	LONG_RANGE, //use for open areas
};

class CBotNavpoint : public CServerOnlyPointEntity {

	DECLARE_CLASS(CBotNavpoint, CServerOnlyPointEntity);
public:
	DECLARE_DATADESC();

	CBotNavpoint() {}
	void	Spawn(void) override;

protected:
	string_t m_iszBritPoint; //next point for british team
	string_t m_iszAmerPoint; //next point for american team
	string_t m_iszAltPoint; //branch point that either team can use

	CBotNavpoint* m_pBritPoint; //these are parsed on spawn
	CBotNavpoint* m_pAmerPoint;
	CBotNavpoint* m_pAltPoint;

	float m_flBritAltChance; //chance that a brit will branch onto the alt path
	float m_flAmerAltChance;

	bool m_bForceBritAsForward; //for one-way paths that either team can use


	float			m_flRadius;

	//at what range should a bot using this point start looking at enemies instead?
	ENavPointRange	m_eRange;

public:
	static CBotNavpoint* ClosestPointToPlayer(CBasePlayer* pPlayer, bool bCheckVis = false);

	CBotNavpoint*		NextPointForPlayer(const CBasePlayer* pPlayer);
	bool				PlayerWithinRadius(const CBasePlayer* pPlayer) const;

	inline float		GetRadius() const { return m_flRadius; }

	Vector				GetLookTarget() const;
	ENavPointRange		GetEnemyNoticeRange() const { return m_eRange; }
};
