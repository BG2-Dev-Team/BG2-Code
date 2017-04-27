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



struct CPlayerSearch {
	
private:
	CBasePlayer* m_pOwner;
	int			 m_iOwnerTeam; //so we don't have to access the pointer over and over
	int			 m_iEnemyTeam;
	Vector		 m_vOwnerLocation;

	CBasePlayer* m_pCloseEnemy;
	CBasePlayer* m_pCloseEnemySecond; //second closest enemy used for more complex movement
	CBasePlayer* m_pCloseFriend;

	float m_flCloseEnemyDist;
	float m_flCloseEnemySecondDist;
	float m_flCloseFriendDist;

	CFlag* m_pCloseFriendFlag;
	CFlag* m_pCloseEnemyFlag; //can also be neutral flag
	CFlag* m_pCloseEnemyFlagVisible;

	//Waypoints are map-placed entities used for bot pathing before reaching enemy or flag
	//CBaseEntity*	m_pNextWaypoint;

	//weighted ratio for how close teammates are relative to enemies.
	//1.0 is enemies are close, 0.0 is friends are close
	//those extreme values would never happen but you get the idea
	float m_flOutnumbered;

	//returns whether entity B is in sight of entity A
	static bool IsInSight(CBaseEntity *pA, CBaseEntity*pb);

	static CFlag* FindClosestFlagToSpot(CBasePlayer* pPlayer, bool insight, bool checkTeam, Vector vSpot);

	//Finds our first waypoint, called on construction
	void UpdateWaypointFirst();

public:
	/*
	Constructors
	*/
	CPlayerSearch();

	void Init(CBasePlayer* pOwner);

	/*
	Accessors
	*/
	inline CBasePlayer* CloseEnemy()		const { return m_pCloseEnemy; }
	inline CBasePlayer* CloseEnemySecond()	const { return m_pCloseEnemySecond; }
	inline CBasePlayer* CloseFriend()		const { return m_pCloseFriend; }

	inline float CloseEnemyDist()			const { return m_flCloseEnemyDist; }
	inline float CloseEnemySecondDist()		const { return m_flCloseEnemySecondDist; }
	inline float CloseFriendDist()			const { return m_flCloseFriendDist; }

	inline CFlag* CloseEnemyFlag()			const { return m_pCloseEnemyFlag; }
	inline CFlag* CloseEnemyFlagVisible()	const { return m_pCloseEnemyFlagVisible; }
	inline CFlag* CloseFriendFlag()			const { return m_pCloseFriendFlag; }

	//above 0.5 is enemies are closer, below 0.5 is friends are closer
	inline float OutNumbered() const { return m_flOutnumbered; }

	inline Vector OwnerOrigin() const { return m_vOwnerLocation; }

	//inline CBaseEntity* m_pNextWaypoint() const { return m_pNextWaypoint; }

	/*
	Updaters
	*/
	void UpdateOwnerLocation();
	void UpdatePlayers();
	void UpdateFlags();
	void UpdateWaypoint();

};

