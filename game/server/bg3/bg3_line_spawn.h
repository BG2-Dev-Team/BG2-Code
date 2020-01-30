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
#ifdef WIN32
#pragma once
#endif

#ifndef BG3_LINE_SPAWN_H
#define BG3_LINE_SPAWN_H

#include "cbase.h"
#include "baseentity.h"

class CLineSpawn : public CServerOnlyPointEntity {
	DECLARE_CLASS(CLineSpawn, CServerOnlyPointEntity);
	DECLARE_DATADESC();
public:
	CLineSpawn() {}

private:
	

	CLineSpawn(const CLineSpawn& rhs);
	CLineSpawn& operator=(const CLineSpawn& rhs);

	int		m_iForTeam				= 0; //which team can use this spawn?
	float	m_flWeight				= 1.0f;//what is the relative likelihood of using this spawn?
	float	m_flSeparationDistance	= 64.0f; //how far to separate spawned players?
	float	m_flRaycastHeight		= 128.0f; //vertical height from which to calculate spawn points?
	float	m_flGroundSeparation	= 5.0f;
	string_t m_iszSpawnDirection;
	EHANDLE m_hSpawnDirection;
	Vector	m_vSpawnDirection;

	CUtlVector<Vector> m_aSpawnLocations;

	void CalculateSpawnLocations(void);

	static	CLineSpawn* ForceFindSpawnForTeam(int iTeam, CLineSpawn* pExclude); //for when randomness isn't enough
	
public:
	static	void		RecalculateSpawns(void); //ensures we're ready to call these other spawning function
	static	CLineSpawn* RandomSpawnForTeam(int iTeam);
	static	void		SpawnTeam(int iTeam, CLineSpawn* pSpawn);
	static	bool		SpawnBothTeams(void); //we could put this in gamerules, but those files are cluttered enough
	
	inline bool	IsValid() { return m_hSpawnDirection && m_aSpawnLocations.Count() == gpGlobals->maxClients; }
};

#endif //BG3_LINE_SPAWN_H