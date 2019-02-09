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

#include "cbase.h"
#include "bg3_stat_tracker.h"


#ifndef CLIENT_DLL
#include "hl2mp/hl2mp_player.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifndef CLIENT_DLL

//Resets the stat tracker to a clean, ready state
void PlayerStatTracker::Reset()
{
	//Reset the per round stats
	m_bSurivivedHeadshot = false;
	m_iMaxNumAttackers = 0; 
	m_flMaxDamageDealt = 0;
	m_iMaxEnemiesKilled = 0;
	m_iMaxNumHeadshots = 0; 
	m_iMaxLifetime = 0;
	m_iNumRespawns = 0;
}

//Called when the player who owns this stat tracker respawns
void PlayerStatTracker::OnPlayerRespawn(const CHL2MP_Player* pPlayer)
{
	//Reset the per life stats
	m_iDamageDealt = 0;
	m_iEnemiesKilled = 0;
	m_iNumHeadshots = 0;

	//Add +1 to the player's respawn count
	++m_iNumRespawns;
}
uint8 attackers;
//Separate the death/round end event from the respawn
void PlayerStatTracker::OnPlayerEndControl(CHL2MP_Player* pPlayer)
{
	//Check to see if any of the current held stats are greater than the current max stats
	if (m_iDamageDealt > m_flMaxDamageDealt)
	{
		m_flMaxDamageDealt = m_iDamageDealt; //Set the current max damage dealt to a player's greater damage dealt
	}
	if (m_iNumHeadshots > m_iMaxNumHeadshots)
	{
		m_iMaxNumHeadshots = m_iNumHeadshots; //Set the current max number of headshots to a player's greater number of headshots
	}
	if (m_iEnemiesKilled > m_iMaxEnemiesKilled)
	{
		m_iMaxEnemiesKilled = m_iEnemiesKilled; //Set the current max number of enemies killed to a player's greater number of kills
	}
	uint16 iLifetime = gpGlobals->curtime - pPlayer->m_fLastRespawn; //A lifetime is the current time minus the time the player last respawned
	if (iLifetime > m_iMaxLifetime)
	{
		m_iMaxLifetime = iLifetime; //Set the current max lifetime of a player to a player's greater lifetime
	}
	
	attackers = m_aAttackers.Size();
	if (attackers > m_iMaxNumAttackers)
	{
		m_iMaxNumAttackers = attackers; //Set the current max number of attackers to a player's greater number of attackers
	}
}

//Called when our player deals damage to another player
void PlayerStatTracker::OnPlayerDamageDealt(const CTakeDamageInfo& info)
{
	m_iDamageDealt += info.GetDamage(); //Get damage amount dealt by any weapon the player used

	//Check to see if the damage dealt was fatal
	if (info.GetDamage() > info.GetInflictor()->GetHealth()) //If the damage dealt by our player is greater than the victim player's current health...
	{
		++m_iEnemiesKilled; //Pre-increment enemies killed amount by +1
	}

	//Check to see if the damage dealt was a fatal headshot
	if (info.GetHitGroup() == HITGROUP_HEAD) //If the damage dealt by our player is a headshot...
	{
		++m_iNumHeadshots; //Pre-increment number of headshots amount by +1
	}
}

//Called when our player receives damage from another player
void PlayerStatTracker::OnPlayerDamageReceived(const CTakeDamageInfo& info)
{
	uint16 eindex = info.GetAttacker()->entindex(); //Get the index of the attacker
	if (!m_aAttackers.HasElement(eindex)) //Does the list already have that index
	{
		m_aAttackers.AddToTail(eindex); //If not, add it to the list
	}

	if (info.GetHitGroup() == HITGROUP_HEAD && info.GetAttacker()->GetDamage() < info.GetInflictor()->GetHealth()) /*If the player was hit in the head
	and the damage dealt by the attacker was less than the player's current health...*/
	{
		m_bSurivivedHeadshot = true; //Set survived a headshot to true
	}
}

//Called to determine how "interesting" this player's most interesting stat is; 
//this is used to determine whether or not this player's stat tracker is used 
//to generate the message. Returns a score, and based on what stat was most
//interesting, updates the ETrackedEvent pointed to by the given pointer.
uint16 PlayerStatTracker::CalculateInterestScore(ETrackedEvent* pOutEvent)
{
	uint16 resultScore = 0;

	//Lambda function to assist us with comparing scores
	auto checkScore = [&](uint16 score, ETrackedEvent e)
	{
		if (score > resultScore)
		{
			resultScore = score;
			*pOutEvent = e;
		}
	};

	//First, base our score on m_iMaxNumAttackers
	checkScore(m_iMaxNumAttackers, EVENT_NUM_ATTACKERS);

	//Check if we survived a headshot
	checkScore(m_bSurivivedHeadshot * 10, EVENT_SURVIVED_HEADSHOT);

	//Base our score on m_flMaxDamageDealt
	checkScore(m_flMaxDamageDealt / 1000, EVENT_DAMAGE_DEALT);

	//Base our score on m_iMaxEnemiesKilled
	checkScore(m_iMaxEnemiesKilled, EVENT_ENEMIES_KILLED);

	//Base our score on m_iMaxNumHeadShots
	checkScore(m_iMaxNumHeadshots, EVENT_NUM_HEADSHOTS);

	//Base our score on m_iMaxLifetime
	checkScore(m_iMaxLifetime / 100, EVENT_LIFETIME);

	return resultScore;
}
#endif //CLIENT_DLL