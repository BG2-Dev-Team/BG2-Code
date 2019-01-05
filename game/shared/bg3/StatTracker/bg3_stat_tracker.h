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
#ifndef BG3_STAT_TRACKER_H
#define BG3_STAT_TRACKER_H

#ifndef CLIENT_DLL
#include "cbase.h"

//Forward declarations
class CHL2MP_Player;
class CTakeDamageInfo;
#endif

//------------------------------------------------------------------------------------
// Purpose - on server, stores player-specific game data which is used to generate
//			 humorous and interesting round end messages. On client, this class
//			 is never instantiated and is instead just used as a static function 
//			 library for generating messages.
//
//			Any given PlayerStatTracker does not know which player it belongs to;
//			via "hollywood" pattern, it does not need to know.
//			Every player object will have a member variable which is a 
//			PlayerStatTracker
//------------------------------------------------------------------------------------
class PlayerStatTracker {
public:

	//To be used for indexing into arrays of strings and for keeping track
	//of interesting events
	enum ETrackedEvent {
		EVENT_SURVIVED_HEADSHOT = 0,
		EVENT_NUM_ATTACKERS,
		EVENT_DAMAGE_DEALT,
		EVENT_ENEMIES_KILLED,
		EVENT_NUM_HEADSHOTS,
		EVENT_LIFETIME,

		NUM_EVENTS // <-- useful, tells us how many event there are
	};

	//disable instatiation on client
#ifdef CLIENT_DLL
private:
	PlayerStatTracker() {}

	//Given an ETrackedEvent, returns the language-independent string 
	// token which is used to generate the message as seen by client
	static const char* GetMessageTokenForEvent(ETrackedEvent);

#else
	//--------------------------------------------------------------------------------
	// Member variables
	//--------------------------------------------------------------------------------
	//Per-life stats; reset these on respawn
	CUtlVector<uint16>	m_aAttackers; //array of indexes of players who have hit us, used to keep track of how many have hit us
	uint16				m_flDamageDealt; //how much damage have we done to enemies?
	uint16				m_iEnemiesKilled; //how many enemies have we killed?
	uint16				m_iNumHeadshots; //how many times have we killed enemies by hitting them in the head with bullet or blade?

	//Per-round stats, reset these on reset
	bool				m_bSurivivedHeadshot; //have we been hit in the head by a bullet and survived?
	uint8				m_iMaxNumAttackers; //among all of our lives, what was the highest number of enemies who have hit us?
	uint16				m_flMaxDamageDealt; //among all of our lives, what was the most damage we dealt in a single life?
	uint16				m_iMaxNumHeadshots; //among all of our lives, what was the highest number of lethal headshots or headstabs we have delivered?
	uint16				m_iMaxLifetime; //Among all our lives, what was our longest life time, in seconds?
	uint16				m_iNumRespawns; //How many times have we respawned? Could be useful info in ticket mode

	//--------------------------------------------------------------------------------
	// Member functions
	//--------------------------------------------------------------------------------

	//Constructor initializes variables
	PlayerStatTracker() { Reset(); }

	//Resets the stat tracker to a clean, ready state
	void Reset();

	//Called when the player who owns this stat tracker respawns
	void OnPlayerRespawn(const CHL2MP_Player* pPlayer);

	//Called when our player deals damage to another player
	void OnPlayerDamageDealt(const CTakeDamageInfo& info);

	//Called when our receives damage from another player
	void OnPlayerDamageReceived(const CTakeDamageInfo& info);

	//Called to determine how "interesting" this player's most interesting stat is; 
	//this is used to determine whether or not this player's stat tracker is used 
	//to generate the message. Returns a score, and based on what stat was most
	//interesting, updates the ETrackedEvent pointed to by the given pointer.
	uint16 CalculateInterestScore(ETrackedEvent* pOutEvent);

	//For the given ETrackedEvent, returns the relevant data
	//ex. for EVENT_NUM_ATTACKERS, return a copy of m_iMaxNumAttackers
	uint16 GetMessageDataForEvent(ETrackedEvent);

#endif //CLIENT_DLL
};

#endif //BG3_STAT_TRACKER_H