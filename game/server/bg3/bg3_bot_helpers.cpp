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

//BG2 - <name of contributer>[ - <small description>]
*/
/*
Contrib by HairyPotter: These bots could do soo much more, and also be soo much more efficient as far as CPU goes.
Maybe I'll remove some code and do a few teaks here and there. Then again it's not like these will be used much anyway.
*/
/*
Contrib by Awesome: Overhauled bots to use an intelligent strategy pattern. Also added additional functionalities
in separate files for bot communication, map navigation, etc.
*/

#include "cbase.h"
#include "player.h"
//#include "sdk_player.h"
#include "in_buttons.h"
#include "movehelper_server.h"
#include "gameinterface.h"
#include "team.h"
#include "hl2mp_gamerules.h"
#include "../sdk/bg3_bot.h"
#include "hl2mp_player.h"
#include "../bg2/flag.h"
#include "../bg2/weapon_bg2base.h"

//bot manager manages bot population
#include "bg3_bot_manager.h"
#include "bg3_bot_navpoint.h"
#include "bg3_bot_vcomms.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar bot_difficulty("bot_difficulty", "3", FCVAR_GAMEDLL, "Bot skill level. 0,1,2,3 are easy, norm, hard, and random, respectively.");

BotDifficulty* CSDKBot::GetDifficulty() {
	//determine bot skill level based on cvar
	BotDifficulty* result;
	switch (bot_difficulty.GetInt()) {
	case 0: result = &BotDiffEasy; break;
	case 1: result = &BotDiffNorm; break;
	case 2: result = &BotDiffHard; break;
	default: result = BotDifficulty::Random();
	}
	return result;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Dispatches thinker start-end functions and switches to next Thinker, with a given delay
//-------------------------------------------------------------------------------------------------
void CSDKBot::ScheduleThinker(BotThinker* pNextThinker, float delay) {
	(this->*(m_pCurThinker->m_pPostThink))(); //wonderful syntax here right?
	(this->*(pNextThinker->m_pPreThink))();
	m_pPrevThinker = m_pCurThinker;
	m_pCurThinker = pNextThinker;
	m_flNextThink = gpGlobals->curtime + delay;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Returns the bot controller the given player, if there is one.
//-------------------------------------------------------------------------------------------------
CSDKBot* CSDKBot::ToBot(CBasePlayer* pPlayer) {
	return &gBots[pPlayer->GetClientIndex()];
}

//-------------------------------------------------------------------------------------------------
// Purpose: Checks enemy position, current weapon, a little randomness etc. to decide to stab
//-------------------------------------------------------------------------------------------------
const Vector downToStomach(0, 0, -35);
bool CSDKBot::DoMelee() {
	bool shouldMelee = false;
	if (bot_randfloat() < m_pDifficult->m_flMeleeReaction) {
		bool didMelee = false;
		if (!IsAimingAtTeammate(m_flMeleeRange)) {
			if (m_pWeapon) {
				//add a little randomness
				float detectedRange = m_PlayerSearchInfo.CloseEnemyDist() + RandomFloat(-2.f, 2.f);

				//for some reason our bots are too short-sighted so let's decrease this
				detectedRange -= 10;

				//take into consideration the melee range of the current bayonet
				shouldMelee = (m_flMeleeRange + bot_randfloat(-m_pDifficult->m_flMeleeRangeOffset, m_pDifficult->m_flMeleeRangeOffset) > detectedRange);
			}

			if (shouldMelee) {
				CBasePlayer* pEnemy = m_PlayerSearchInfo.CloseEnemy();
				if (pEnemy) {
					LookAt(pEnemy->Weapon_ShootPosition() + (downToStomach * bot_randfloat()), m_pDifficult->m_flMeleeTurnLerp, m_pDifficult->m_flRandomAim);
					didMelee = true;
				}
			}
		}
		if (didMelee) {
			m_curCmd.buttons |= IN_ATTACK2;
		} else {
			m_curCmd.buttons &= ~IN_ATTACK2;
		}
	}
	return shouldMelee;
}



//-------------------------------------------------------------------------------------------------
// Purpose: Checks if the player is looking at a teammate within the specified range
//-------------------------------------------------------------------------------------------------
bool CSDKBot::IsAimingAtTeammate(vec_t range) const {
	trace_t tr;
	Vector forward;

	AngleVectors(m_curCmd.viewangles, &forward);

	UTIL_TraceLine(m_pPlayer->Weapon_ShootPosition(),
				   m_pPlayer->Weapon_ShootPosition() + forward * range,
				   MASK_SOLID, m_pPlayer, COLLISION_GROUP_NONE, &tr);
	if (tr.m_pEnt) {
		CBasePlayer* pPlayer = dynamic_cast<CBasePlayer*>(tr.m_pEnt);
		if (pPlayer && pPlayer->GetTeamNumber() == m_pPlayer->GetTeamNumber())
			return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Checks if the player is capturing or attempting to capture an enemy flag
//-------------------------------------------------------------------------------------------------
bool CSDKBot::IsCapturingEnemyFlag() const {
	bool nearFlag = IsCapturingEnemyFlagAttempt();

	//this extra check if necessary, or else you could have two bots
	//on opposite teams trying to capture a flag while stareing at
	//each other.
	if (nearFlag) {
		CBasePlayer* pEnemy = m_PlayerSearchInfo.CloseEnemy();
		CFlag*		 pFlag = m_PlayerSearchInfo.CloseEnemyFlag();
		if (pEnemy && pFlag) {
			vec_t enemyDistance = (pEnemy->GetAbsOrigin() - pFlag->GetAbsOrigin()).Length();
			if (enemyDistance < pFlag->m_flCaptureRadius)
				nearFlag = false;
		}
	}

	return nearFlag;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Checks if a player is within a enemy's flag's radius, regardless of enemy presence
//-------------------------------------------------------------------------------------------------
bool CSDKBot::IsCapturingEnemyFlagAttempt() const {
	CFlag* pFlag = m_PlayerSearchInfo.CloseEnemyFlagVisible();

	if (pFlag && m_pPlayer && (m_pPlayer->GetAbsOrigin() - pFlag->GetAbsOrigin()).Length() < pFlag->m_flCaptureRadius)
		return true;

	return false;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Retrieves the enumerated distance a bot using navpoints should start noticing enemies
//-------------------------------------------------------------------------------------------------
ENavPointRange CSDKBot::EnemyNoticeRange() const {
	ENavPointRange eRange = ENavPointRange::LONG_RANGE;
	if (m_pCurThinker == &BotThinkers::Waypoint && m_PlayerSearchInfo.CurNavpoint()) {
		eRange = m_PlayerSearchInfo.CurNavpoint()->GetEnemyNoticeRange();
	}
	return eRange;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Modifies the angles of m_curCmd to look at the given location
//-------------------------------------------------------------------------------------------------
void CSDKBot::LookAt(Vector location, float lerp, vec_t randomOffset) {
	//create a new set of angles to compute
	QAngle angles;

	//build vector from our head to the given location
	Vector forward = location - m_pPlayer->Weapon_ShootPosition();
	VectorAngles(forward, angles);

	//apply some randomness
	angles += RandomAngle(-randomOffset, randomOffset);

	Quaternion qAngles(angles);
	QuaternionSlerp(m_LastAngles, qAngles, lerp, qAngles);
	angles = RadianEuler(qAngles).ToQAngle();

	//apply the linear interpolation from the last angle
	//angles = (lerp * angles) + (1 - lerp) * m_LastAngles;

	m_pPlayer->SetLocalAngles(angles);
	m_curCmd.viewangles = angles;
	m_LastAngles = qAngles;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Sends a context to our vcomm manager for interpretation
//-------------------------------------------------------------------------------------------------
void CSDKBot::SendBotVcommContext(BotContext context) {
	CBotComManager* pComms = CBotComManager::GetBotCommsOfPlayer(m_pPlayer);
	pComms->ReceiveContext(m_pPlayer, context);
}

//-------------------------------------------------------------------------------------------------
// Purpose: Moves the bot to its nearest teammate
//	@requires Teammate in m_PlayerSearchInfo is non-null
//-------------------------------------------------------------------------------------------------
void CSDKBot::MoveToNearestTeammate() {
	CBasePlayer* pFriend = m_PlayerSearchInfo.CloseFriend();
	m_curCmd.buttons |= IN_FORWARD;
	m_curCmd.buttons &= ~IN_BACK;
	LookAt(pFriend->Weapon_ShootPosition(), 0.8f, 4.f);
}

//-------------------------------------------------------------------------------------------------
// Purpose: Moves to nearest waypoint and handles some navigation logic.
//		Putting it here so it can be used by different Thinks as necessary
//-------------------------------------------------------------------------------------------------
void CSDKBot::MoveToWaypoint() {
	CBotNavpoint* pPoint = m_PlayerSearchInfo.CurNavpoint();
	if (pPoint) {
		m_PlayerSearchInfo.UpdateNavpoint();
		pPoint = m_PlayerSearchInfo.CurNavpoint();
		//a navpoint will always return a reference to a navpoint, so we don't have to check it again
		float lerp = pPoint->GetRadius() < 50 ? 0.9f : 0.6f;
		LookAt(pPoint->GetLookTarget(), lerp, 5);
	} else {
		m_PlayerSearchInfo.UpdateNavpointFirst();
	}
}

//-------------------------------------------------------------------------------------------------
// Purpose: Looks away/toward from the nearest enemy and runs away
// @requires ClosestEnemy in m_PlayerSearchInfo is non-null
// @updates m_flNextStrafeTime - for looking away/toward enemy
//-------------------------------------------------------------------------------------------------
void CSDKBot::MoveAwayFromEnemy() {
	if (gpGlobals->curtime > m_flNextStrafeTime) {
		m_flNextStrafeTime += bot_randfloat(1.0f, 2.0f);

		CBasePlayer* pEnemy = m_PlayerSearchInfo.CloseEnemy();
		Vector lookTarget;

		//Either look away from enemies...
		if (bot_randfloat() < 0.5f) {
			//this math is wierd but actually works... it gets displacement from them to us and adds it to our own position
			lookTarget = 2 * m_pPlayer->GetAbsOrigin() - pEnemy->Weapon_ShootPosition();

			//Get displacement from enemy to us
			Vector disp = m_pPlayer->GetAbsOrigin() - pEnemy->GetAbsOrigin();

			m_curCmd.buttons |= IN_FORWARD;
			m_curCmd.buttons &= ~IN_BACK;
		}
		//Or look at them...
		else {
			lookTarget = pEnemy->Weapon_ShootPosition();
			m_curCmd.buttons |= IN_BACK;
			m_curCmd.buttons &= ~IN_FORWARD;
		}
		LookAt(lookTarget, 0.7f, 5.f);
	}
}

//-------------------------------------------------------------------------------------------------
// Purpose: Modifies button flags to stop all motion
//-------------------------------------------------------------------------------------------------
int g_iZeroMovement = ~(IN_FORWARD | IN_LEFT | IN_RIGHT | IN_BACK);
void CSDKBot::StopMoving() {
	m_curCmd.buttons &= g_iZeroMovement;
}