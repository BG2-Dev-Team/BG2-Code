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
#include "bg3_bot.h"
#include "hl2mp_player.h"
#include "../bg2/flag.h"
#include "../bg2/weapon_bg2base.h"

//bot manager manages bot population
#include "bg3_bot_manager.h"
#include "bg3_bot_navpoint.h"
#include "bg3_bot_vcomms.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

Vector LerpVector(const Vector& v0, const Vector& v1, float lerp) {
	return (v1 * lerp) + (v0 * (1 - lerp));
}

class CSDKBot;
void Bot_Think(CSDKBot *pBot);

//static ConVar bot_mimic("bot_mimic", "0", 0, "Bot uses usercmd of player by index.");
static ConVar bot_mimic_yaw_offset("bot_mimic_yaw_offset", "0", 0, "Offsets the bot yaw.");
ConVar bot_pause("bot_pause", "0", 0, "Stops the bot thinking cycle entirely.");


//ConVar bot_sendcmd( "bot_sendcmd", "", 0, "Forces bots to send the specified command." );

//So what if these are global? Want to fight about it?
CSDKBot	gBots[MAX_PLAYERS];
bool g_bServerReady = false; //Are we ready to use the temp int below?


const Vector downToChest(0.f, 0.f, -25.f);
const Vector downToHead(0.f, 0.f, -10.f);


//Bot difficulties - the meanings of these numbers are
//						 (aimAngle,	reload,	earlyFire,	meleeTurn,	brave,	meleeRangeOffset,	meleeReaction)
BotDifficulty BotDiffEasy(12.f,		0.4f,	0.8f,		0.3f,		0.3f,	30.f,				0.25f);
BotDifficulty BotDiffNorm(7.f,		0.1f,	0.65f,		0.45f,		0.5f,	15.f,				0.35f);
BotDifficulty BotDiffHard(5.f,		0.03f,	0.45f,		0.55f,		0.8f,	8.f,				0.40f);

#define DEF_THINKER(name, interval) \
	BotThinker name (&CSDKBot::Think##name##_Begin, &CSDKBot::Think##name##_Check, &CSDKBot::Think##name, &CSDKBot::Think##name##_End, interval, #name)
//Bot Thinkers - artificial BRAINS these are
//the floats are the times between thinks
namespace BotThinkers{
	BotThinker Waypoint	(&CSDKBot::ThinkWaypoint_Begin,		&CSDKBot::ThinkWaypoint_Check,	&CSDKBot::ThinkWaypoint,	&CSDKBot::ThinkWaypoint_End,	0.3f, "Waypoint Think");
	BotThinker Flag		(&CSDKBot::ThinkFlag_Begin,			&CSDKBot::ThinkFlag_Check,		&CSDKBot::ThinkFlag,		&CSDKBot::ThinkFlag_End,		0.4f, "Flag Think");
	BotThinker LongRange(&CSDKBot::ThinkLongRange_Begin,	&CSDKBot::ThinkLongRange_Check, &CSDKBot::ThinkLongRange,	&CSDKBot::ThinkLongRange_End,	0.4f, "Long Range Think");
	BotThinker MedRange	(&CSDKBot::ThinkMedRange_Begin,		&CSDKBot::ThinkMedRange_Check,	&CSDKBot::ThinkMedRange,	&CSDKBot::ThinkMedRange_End,	0.18f, "Med Range Think");
	BotThinker Melee	(&CSDKBot::ThinkMelee_Begin,		&CSDKBot::ThinkMelee_Check,		&CSDKBot::ThinkMelee,		&CSDKBot::ThinkMelee_End,		0.07f, "Melee Think");
	BotThinker Retreat	(&CSDKBot::ThinkRetreat_Begin,		&CSDKBot::ThinkRetreat_Check,	&CSDKBot::ThinkRetreat,		&CSDKBot::ThinkRetreat_End,		0.3f, "Retreat Think");
	BotThinker PointBlank(&CSDKBot::ThinkPointBlank_Begin,	&CSDKBot::ThinkPointBlank_Check,&CSDKBot::ThinkPointBlank,	&CSDKBot::ThinkPointBlank_End,	0.1f, "PointBlank Think");
	DEF_THINKER(Reload, 0.2f);
	BotThinker Death	(&CSDKBot::ThinkDeath_Begin,		&CSDKBot::ThinkDeath_Check,		&CSDKBot::ThinkDeath,		&CSDKBot::ThinkDeath_End,		1.0f, "Death Think");
}
//Default thinker is death
BotThinker* CSDKBot::s_pDefaultThinker = &BotThinkers::Death;

//BG3 - this function is here because it is inline
//-------------------------------------------------------------------------------------------------
// Purpose: Checks whether or not the musket is capable of firing at this moment. Does not look for FF
//-------------------------------------------------------------------------------------------------
bool CSDKBot::CanFire() {
	bool canFire = false;
	m_pWeapon = static_cast<CBaseBG2Weapon*>(m_pPlayer->GetActiveWeapon());
	if (m_pWeapon) {
		canFire = m_pWeapon->Clip1() > 0;
	}

	return canFire;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Checks if the bot can and could reasonably reload
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ShouldReload() {
	m_pWeapon = (m_pPlayer->GetActiveWeapon());
	return m_pWeapon && m_pWeapon->Clip1() == 0 && m_PlayerSearchInfo.CloseEnemyDist() > 800;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Checks if we're dead, and if so, schedules the Death BotThinker
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkCheckDeath() {
	if (m_pPlayer->IsDead()) {
		ScheduleThinker(&BotThinkers::Death, 1.0);
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Checks if we our m_flNextFireTime tells us to start point-blanking
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkCheckPointBlank() {
	if (CanFire() && m_flNextFireTime < gpGlobals->curtime
		&& m_PlayerSearchInfo.CloseEnemy()) {
		ScheduleThinker(&BotThinkers::PointBlank, 0.05f);
		return false;
	} else if (m_flNextFireTime < gpGlobals->curtime)
		m_flNextFireTime = FLT_MAX;

	return true;
}


//-------------------------------------------------------------------------------------------------
// Purpose: Checks if immediately close to the enemy, and if so, schedules the Melee BotThinker
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkCheckMelee() {
	//check for emergency close-quarters combat
	if (m_PlayerSearchInfo.CloseEnemyDist() < MELEE_RANGE_START) {
		ScheduleThinker(&BotThinkers::Melee, BotThinkers::Melee.m_flThinkDelay);
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Returns us to the last thinker if melee is over
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkCheckExitMelee() {
	BotThinker* prevThinker = m_pPrevThinker;

	//Retreat also returns us to last thinker and we don't want infinite state change loops!
	if (prevThinker == &BotThinkers::Retreat)
		prevThinker = &BotThinkers::MedRange;

	if (m_PlayerSearchInfo.CloseEnemyDist() > MELEE_RANGE_START) {
		ScheduleThinker(prevThinker, prevThinker->m_flThinkDelay);
		return false;
	}
	return true;
}


//-------------------------------------------------------------------------------------------------
// Purpose: Checks if we should RETREAT! If so, schedules the retreat BotThinker
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkCheckRetreat() {
	//check for emergency close-quarters combat
	if (m_PlayerSearchInfo.CloseEnemyDist() < 800 
		&& m_PlayerSearchInfo.CloseEnemySecondDist() < 800 
		&& m_PlayerSearchInfo.OutNumbered() > 0.57 
		&& m_PlayerSearchInfo.CloseFriendDist() < 800
		&& m_PlayerSearchInfo.CloseFriendDist() > RETREAT_STOP_RANGE
		&& m_flForceNoRetreatTime < gpGlobals->curtime) {
		ScheduleThinker(&BotThinkers::Retreat, BotThinkers::Retreat.m_flThinkDelay);
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Checks if we should stop retreating
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkCheckRetreatEnd() {
	bool nearFriend = false;
	if (m_PlayerSearchInfo.CloseFriend() && m_PlayerSearchInfo.CloseFriendDist() < RETREAT_STOP_RANGE)
		nearFriend = true;
	
	CBasePlayer* pEnemy = m_PlayerSearchInfo.CloseEnemy();
	if (!pEnemy || nearFriend || m_flEndRetreatTime < gpGlobals->curtime) {
		ScheduleThinker(m_pPrevThinker, 0.1f);
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Checks if we have a visible flag to go to
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkCheckFlag() {
	if (m_PlayerSearchInfo.CloseEnemyFlagVisible()) {
		ScheduleThinker(&BotThinkers::Flag, BotThinkers::Flag.m_flThinkDelay);
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Enters waypoint or combat if no enemy flags are visible
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkCheckFlagExit() {
	if (!m_PlayerSearchInfo.CloseEnemyFlagVisible()) {
		if (m_PlayerSearchInfo.CloseEnemyDist() < MED_RANGE_START)
			ScheduleThinker(&BotThinkers::MedRange, 0.05f);
		else
			ScheduleThinker(&BotThinkers::Waypoint, 0.2f);

		return false;
	}
	return true;
}


//-------------------------------------------------------------------------------------------------
// Purpose: Checks if we should fight nearest enemy or run to flag, if we're already in a mid-range engagement
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkCheckFlagOrFight() {
	CFlag* pFlag = m_PlayerSearchInfo.CloseEnemyFlagVisible();
	CBasePlayer* pEnemy = m_PlayerSearchInfo.CloseEnemy();
	if (pFlag && pEnemy) {
		float enemyDistanceToFlag = (pEnemy->GetAbsOrigin() - pFlag->GetAbsOrigin()).Length();
		float ourDistanceToFlag = (m_PlayerSearchInfo.OwnerOrigin() - pFlag->GetAbsOrigin()).Length();
		if (m_pCurThinker != &BotThinkers::Flag /*No self-scheduling!*/ 
			&& enemyDistanceToFlag > 100 //don't bother if they're on the flag
			&& enemyDistanceToFlag > ourDistanceToFlag && m_PlayerSearchInfo.CloseEnemyDist() > MELEE_RANGE_START) {
			m_bTowardFlag = true;
			ScheduleThinker(&BotThinkers::Flag, 0.1f);
			return false;
		}

		//also check if we should exit flag to med-range
		if (m_pCurThinker == &BotThinkers::Flag && m_PlayerSearchInfo.CloseEnemyDist() < ourDistanceToFlag) {
			ScheduleThinker(&BotThinkers::MedRange, 0.1f);
			return false;
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Checks if we should enter long-range combat
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkCheckEnterLongRangeCombat() {
	if (m_PlayerSearchInfo.CloseEnemyDist() < LONG_RANGE_START && m_PlayerSearchInfo.CloseEnemyDist() > MED_RANGE_START
		&& EnemyNoticeRange() >= LONG_RANGE
		&& !IsCapturingEnemyFlag() && !m_bTowardFlag) {
		ScheduleThinker(&BotThinkers::LongRange, 0.2f);
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Checks if we should enter med-range combat from short-range or long-range combat
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkCheckEnterMedRangeCombat() {
	if (m_PlayerSearchInfo.CloseEnemyDist() < MED_RANGE_START && m_PlayerSearchInfo.CloseEnemyDist() > MELEE_RANGE_START
		&& EnemyNoticeRange() >= MED_RANGE
		&& !IsCapturingEnemyFlag() && !m_bTowardFlag && !WantsToRetreat()) {
		ScheduleThinker(&BotThinkers::MedRange, 0.1f);
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Checks if we should stop worrying about enemies and go back to waypoints
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkCheckExitCombat() {
	if ( m_PlayerSearchInfo.CloseEnemyDist() > LONG_RANGE_START) {
		ScheduleThinker(&BotThinkers::Waypoint, 0.3f);
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Master think function - uses the current thinker to check for state changes or otherwise think
//-------------------------------------------------------------------------------------------------
#if 0
#define ThinkMsg(a) Msg(a)
#define ThinkMsgF(a,b) Msg((a),(b))
#else
#define ThinkMsgF(a,b)
#define ThinkMsg(a)
#endif
void CSDKBot::Think() {
	bool didThink = false;
	if (gpGlobals->curtime > m_flNextThink) {
		didThink = true;
		ThinkMsg(m_pPlayer->GetPlayerName());
		//ThinkMsg(" Updating Players...");
		m_PlayerSearchInfo.UpdateOwnerLocation();
		m_PlayerSearchInfo.UpdatePlayers();

		
		if (m_bLastThinkWasStateChange) {
			m_bLastThinkWasStateChange = false;
			PostStateChangeThink();
		}

		//ThinkMsg("Flags...");
		if (m_bUpdateFlags)
			m_PlayerSearchInfo.UpdateFlags();
		ThinkMsgF("%s ", m_pCurThinker->m_ppszThinkerName);
		ThinkMsg("Check...\n");
		if ((this->*(m_pCurThinker->m_pThinkCheck))()) {
			ThinkMsgF("Think...", m_pCurThinker->m_ppszThinkerName);
			//wonderful syntax here right?
			(this->*(m_pCurThinker->m_pThink))();
			m_flNextThink = gpGlobals->curtime + m_pCurThinker->m_flThinkDelay;
		} else {
			m_bLastThinkWasStateChange = true;
			ThinkMsg("Change...");
		}
			
		m_LastCmd = m_curCmd;
		//ThinkMsg("Buttons...");
		ButtonThink();
	}
	if (didThink) {
		//ThinkMsg("Move...");
	}
	RunPlayerMove(m_pPlayer, m_curCmd, gpGlobals->frametime);
	if (didThink) {
		ThinkMsg("Finished!\n");
	}
}

//-------------------------------------------------------------------------------------------------
// Purpose: Button think function translate movement buttons into movement floats
//-------------------------------------------------------------------------------------------------
void CSDKBot::ButtonThink() {
	int& buttons = m_curCmd.buttons;
	if (buttons & IN_FORWARD)
		m_curCmd.forwardmove = 1000.f;
	else if (buttons & IN_BACK)
		m_curCmd.forwardmove = -1000.f;
	else
		m_curCmd.forwardmove = 0.0f;

	if (buttons & IN_RIGHT)
		m_curCmd.sidemove = 600.f;
	else if (buttons & IN_LEFT)
		m_curCmd.sidemove = -600.f;
	else
		m_curCmd.sidemove = 0.0f;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Provides common functionalities and changes for 1 think after any state change
//-------------------------------------------------------------------------------------------------
void CSDKBot::PostStateChangeThink() {
	if (m_pWeapon && m_pWeapon->IsIronsighted() && m_pCurThinker != &BotThinkers::PointBlank) {
		m_curCmd.buttons &= ~IN_ZOOM; //disable ironsights
	}
	//Msg("%s\tchanged state from \"%s\" to \"%s\"\n", m_pPlayer->GetPlayerName(), m_pPrevThinker->m_ppszThinkerName, m_pCurThinker->m_ppszThinkerName);
}


//-------------------------------------------------------------------------------------------------
// Purpose: 
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkWaypoint_Begin() {
	SetUpdateFlags(true);
	SendBotVcommContext(CLEAR);
	m_curCmd.buttons = IN_FORWARD;
	m_PlayerSearchInfo.UpdateNavpointFirst();
	return true;
}

bool CSDKBot::ThinkWaypoint_Check() {
	//we could do a whole bunch of nested ifs, but short-circuiting works as well.
	return ThinkCheckDeath() && ThinkCheckEnterLongRangeCombat() && ThinkCheckEnterMedRangeCombat()
		&& ThinkCheckMelee() && ThinkCheckFlag();
}

bool CSDKBot::ThinkWaypoint() {
	MoveToWaypoint();
	return true;
}

bool CSDKBot::ThinkWaypoint_End() {
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Flag think sets the bot to move towards enemy flag until flag is captured or enemy gets too close
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkFlag_Begin() {
	if (bot_randfloat() < 0.4f)
		m_flNextFireTime = gpGlobals->curtime + bot_randfloat(2.0f, 8.0f);
	SendBotVcommContext(OBJECTIVE);
	SetUpdateFlags(true);
	m_bTowardFlag = true;
	return true;
}

bool CSDKBot::ThinkFlag_Check() {
	return ThinkCheckDeath() && ThinkCheckRetreat() && ThinkCheckFlagOrFight()
		&& ThinkCheckMelee() && ThinkCheckPointBlank() && ThinkCheckFlagExit();
}

const Vector g_vUpFromFlag = Vector(0, 0, 40);

bool CSDKBot::ThinkFlag() {
	CFlag* pFlag = m_PlayerSearchInfo.CloseEnemyFlagVisible();

	if (IsCapturingEnemyFlagAttempt()) {
		if (!m_bLastThinkWasInFlag) {
			m_bLastThinkWasInFlag = true;
			if (bot_randfloat() < 0.4f && ShouldReload()) {
				m_curCmd.buttons |= IN_RELOAD;
			}
			m_flNextStrafeTime = gpGlobals->curtime;
		}

		CBasePlayer* pEnemy = m_PlayerSearchInfo.CloseEnemy();
		if (pEnemy) {
			LookAt(pEnemy->Weapon_ShootPosition(), 0.7f, 3);
			if (pFlag)
				DanceAround(pFlag->GetAbsOrigin(), pFlag->m_flCaptureRadius);
			if (bot_randfloat() < 0.3f)
				m_flNextFireTime = gpGlobals->curtime;
		}
		else {
			StopMoving();
		}
	} else {
		m_bLastThinkWasInFlag = false;

		if (pFlag) {
			m_curCmd.buttons |= IN_FORWARD;
			LookAt(pFlag->GetAbsOrigin() + g_vUpFromFlag, 0.8f, 5);
		}
		TeammateGoAround(true);
	}
	return true;
}

bool CSDKBot::ThinkFlag_End() {
	m_curCmd.buttons = 0;
	m_bTowardFlag = false;
	m_bLastThinkWasInFlag = false;
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Long-range combat
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkLongRange_Begin() {
	SendBotVcommContext(ADVANCE);
	SetUpdateFlags(true);
	if (CanFire()) {
		float random = bot_randfloat();
		if (random < m_pDifficult->m_flLongRangeFire) {
			m_flNextFireTime = gpGlobals->curtime + RandomFloat(0.5f, 4.2f);
		}
	}
	return true;
}

bool CSDKBot::ThinkLongRange_Check() {
	return ThinkCheckDeath() && ThinkCheckPointBlank() && ThinkCheckEnterMedRangeCombat()
		&& ThinkCheckMelee() && ThinkCheckExitCombat();
	return true;
}

bool CSDKBot::ThinkLongRange() {
	m_curCmd.buttons |= IN_FORWARD;
	Vector lookTarget;
	CBasePlayer* pEnemy = m_PlayerSearchInfo.CloseEnemy();
	CFlag* pFlag = m_PlayerSearchInfo.CloseEnemyFlagVisible();
	//if there's a flag move towards the flag as well
	if (pEnemy && pFlag) {
		lookTarget = LerpVector(pFlag->GetAbsOrigin(), pEnemy->Weapon_ShootPosition(), 0.3f);
	} else if (pEnemy) {
		lookTarget = pEnemy->Weapon_ShootPosition();
	}
	LookAt(lookTarget, 0.6f, 5.f);
	TeammateGoAround(true);
	return true;
}

bool CSDKBot::ThinkLongRange_End() {
	m_flNextFireTime = FLT_MAX;
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Medium-range combat
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkMedRange_Begin() {
	SendBotVcommContext(DEFAULT);
	SetUpdateFlags(true);
	if (CanFire()) {
		float random = bot_randfloat();
		//if we're already hurt, we're more desperate
		if (m_pPlayer->GetHealth() < 70)
			random -= 0.1;

		//if our enemy is at a flag, it's more important to stop him!
		CBasePlayer* pEnemy = m_PlayerSearchInfo.CloseEnemy();
		if (pEnemy) {
			CFlag* pOurFlag = m_PlayerSearchInfo.CloseFriendFlag();
			CFlag* pTheirFlag = m_PlayerSearchInfo.CloseEnemyFlag();
			if (pOurFlag && EntDist(*pOurFlag, *pEnemy) < pOurFlag->m_flCaptureRadius)
				random -= 0.1;
			else if (pTheirFlag && EntDist(*pTheirFlag, *pEnemy) < pTheirFlag->m_flCaptureRadius)
				random -= 0.2;
		}
		if (random < 0.9) {
			m_flNextFireTime = gpGlobals->curtime + bot_randfloat(0.1f, 3.2f);
		}
	}
	m_curCmd.buttons |= IN_FORWARD;
	m_flNextStrafeTime = gpGlobals->curtime + bot_randfloat(0.5f, 1.0f);
	return true;
}

bool CSDKBot::ThinkMedRange_Check() {
	return ThinkCheckDeath() && ThinkCheckEnterLongRangeCombat() && ThinkCheckPointBlank()
		&& ThinkCheckMelee() && ThinkCheckRetreat() && ThinkCheckFlagOrFight() && ThinkCheckExitCombat();
}

bool CSDKBot::ThinkMedRange() {
	CBasePlayer* pEnemy = m_PlayerSearchInfo.CloseEnemy();
	CBasePlayer* pEnemySecond = m_PlayerSearchInfo.CloseEnemySecond();
	bool bForceForward = false;

	if (pEnemy && pEnemySecond) {
		Vector lookTarget;
		lookTarget = LerpVector(pEnemy->Weapon_ShootPosition(), pEnemySecond->Weapon_ShootPosition(), 0.3);
		LookAt(lookTarget, m_pDifficult->m_flMeleeTurnLerp - 0.1, 5.f);
		//move more aggressively forward if the second enemy is far away
		if (m_PlayerSearchInfo.CloseEnemyDist() + 400 < m_PlayerSearchInfo.CloseEnemySecondDist()) {
			bForceForward = true;
			m_curCmd.buttons |= IN_FORWARD;
		}
	} else if (pEnemy) {
		Vector lookTarget;
		lookTarget = pEnemy->Weapon_ShootPosition();
		LookAt(lookTarget, m_pDifficult->m_flMeleeTurnLerp - 0.1, 5.f);

		CBasePlayer* pFriend = m_PlayerSearchInfo.CloseFriend();
		if (pFriend) {
			vec_t friendCombatRange = (pFriend->GetAbsOrigin() - pEnemy->GetAbsOrigin()).Length();
			if (friendCombatRange < HELP_START_RANGE)
				bForceForward = true;
		}
	}
	
	if (gpGlobals->curtime > m_flNextStrafeTime) {
		float randomStrafe = bot_randfloat();
		if (randomStrafe < 0.4f) {
			m_curCmd.buttons |= IN_LEFT;
			m_curCmd.buttons &= ~IN_RIGHT;
		} else if (randomStrafe < 0.8f) {
			m_curCmd.buttons &= ~IN_LEFT;
			m_curCmd.buttons |= IN_RIGHT;
		} else {
			m_curCmd.buttons &= ~IN_LEFT;
			m_curCmd.buttons &= ~IN_RIGHT;
		}
		//we're more likely to go forward if we're being forced to
		if (bot_randfloat() < 0.4f || (bForceForward && bot_randfloat() < 0.8f)) {
			m_curCmd.buttons |= IN_FORWARD;
		} else if (!bForceForward) {
			m_curCmd.buttons &= ~IN_FORWARD;
		}

		m_flNextStrafeTime += RandomFloat(0.1f, 1.4f);
	}
	return true;
}

bool CSDKBot::ThinkMedRange_End() {
	m_curCmd.buttons = 0;
	m_flNextFireTime = FLT_MAX;
	m_flNextStrafeTime = FLT_MAX;
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Simulates close-quarters combat
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkMelee_Begin() {
	SendBotVcommContext(DEFAULT);
	SetUpdateFlags(false);
	//m_curCmd.buttons |= IN_FORWARD;
	m_flNextStrafeTime = gpGlobals->curtime + RandomFloat(0.5f, 1.0f);
	if (CanFire()) {
		float random = RandomFloat();
		if (m_pPlayer->GetHealth() < 70)
			random += 0.2;
		if (random > 0.5) {
			m_flNextFireTime = gpGlobals->curtime + RandomFloat(0.5f, 1.2f);
		}
	}

	float randomStrafe = bot_randfloat();
	if (randomStrafe < 0.45f) {
		m_curCmd.buttons |= IN_LEFT;
		m_curCmd.buttons &= ~IN_RIGHT;
	} else if (randomStrafe < 0.9f) {
		m_curCmd.buttons &= ~IN_LEFT;
		m_curCmd.buttons |= IN_RIGHT;
	} else {
		m_curCmd.buttons &= ~IN_LEFT;
		m_curCmd.buttons &= ~IN_RIGHT;
	}

	float randomForward = bot_randfloat();
	//don't move forward if we're too close
	if ( m_PlayerSearchInfo.CloseEnemyDist() < m_flMeleeRange) {
		m_curCmd.buttons &= ~IN_FORWARD;
		m_curCmd.buttons |= IN_BACK;
	} else if (randomForward < 0.5f){
		m_curCmd.buttons |= IN_FORWARD;
		m_curCmd.buttons &= ~IN_BACK;
	} else {
		m_curCmd.buttons &= ~IN_FORWARD;
		m_curCmd.buttons &= ~IN_BACK;
	}

	return true;
}

bool CSDKBot::ThinkMelee_Check() {
	return ThinkCheckDeath() && ThinkCheckExitMelee() && ThinkCheckExitCombat();
}

bool CSDKBot::ThinkMelee() {
	CBasePlayer* pEnemy = m_PlayerSearchInfo.CloseEnemy();
	if (pEnemy) {
		LookAt(pEnemy->Weapon_ShootPosition() + downToHead, m_pDifficult->m_flMeleeTurnLerp, m_pDifficult->m_flRandomAim);
	}
	if (m_flNextFireTime < gpGlobals->curtime && !IsAimingAtTeammate(m_PlayerSearchInfo.CloseEnemyDist())) {
		m_curCmd.buttons |= IN_ATTACK;
	}
	DoMelee();
	if (gpGlobals->curtime > m_flNextStrafeTime) {
		float randomStrafe = bot_randfloat();
		if (randomStrafe < 0.45f) {
			m_curCmd.buttons |= IN_LEFT;
			m_curCmd.buttons &= ~IN_RIGHT;
		} else if (randomStrafe < 0.9f) {
			m_curCmd.buttons &= ~IN_LEFT;
			m_curCmd.buttons |= IN_RIGHT;
		} else {
			m_curCmd.buttons &= ~IN_LEFT;
			m_curCmd.buttons &= ~IN_RIGHT;
		}

		float randomForward = bot_randfloat();
		//don't move forward if we're too close
		if (m_PlayerSearchInfo.CloseEnemyDist() < m_flAdjustedMeleeRange) {
			m_curCmd.buttons &= ~IN_FORWARD;
			m_curCmd.buttons |= IN_BACK;
		} else if (randomForward < 0.5f){
			m_curCmd.buttons |= IN_FORWARD;
			m_curCmd.buttons &= ~IN_BACK;
		} else {
			m_curCmd.buttons &= ~IN_FORWARD;
			m_curCmd.buttons &= ~IN_BACK;
		}

		m_flNextStrafeTime += RandomFloat(0.05f, 0.2f);
	}
	return true;
}

bool CSDKBot::ThinkMelee_End() {
	m_curCmd.buttons &= ~IN_ATTACK2;
	m_flNextFireTime = FLT_MAX;
	m_flNextStrafeTime = FLT_MAX;
	m_curCmd.buttons = 0;
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: 
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkRetreat_Begin() {
	SendBotVcommContext(RETREAT);
	SetUpdateFlags(true);
	m_flNextFireTime = FLT_MAX;
	m_flEndRetreatTime = gpGlobals->curtime + bot_randfloat(1.5, 7.0) * (1 - m_pDifficult->m_flBravery);
	return true;
}

bool CSDKBot::ThinkRetreat_Check() {
	return ThinkCheckDeath() && ThinkCheckRetreatEnd()
		&& ThinkCheckMelee() && ThinkCheckExitCombat();
}

bool CSDKBot::ThinkRetreat() {
	CBasePlayer* pEnemy = m_PlayerSearchInfo.CloseEnemy();
	CBasePlayer* pFriend = m_PlayerSearchInfo.CloseFriend();

	//Prefer to move towards friend while looking at friend,
	//otherwise look backwards from enemy
	if (pEnemy || pFriend) {
		if (pFriend) {
			MoveToNearestTeammate();
		} else if (pEnemy){
			MoveAwayFromEnemy();
		}
	}
	return true;
}

bool CSDKBot::ThinkRetreat_End() {
	m_curCmd.buttons = 0;
	m_flEndRetreatTime = FLT_MAX;
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Aims from hip at enemy and shoots with proper timing. Will last no longer than ~.5s
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkPointBlank_Begin() {
	SendBotVcommContext(FIRE);
	SetUpdateFlags(false);
	m_curCmd.buttons = 0;
	//random crouching
	

	bool bZoom = m_PlayerSearchInfo.CloseEnemyDist() > 400 && bot_randfloat() < 0.7f;
	if (!bZoom)
		bZoom = m_PlayerSearchInfo.CloseEnemyDist() > 1100 && bot_randfloat() < 0.9f;

	if (bZoom) {
		m_curCmd.buttons |= IN_ZOOM;
	}

	m_flNextFireTime = gpGlobals->curtime + bot_randfloat(0.4f, 0.6f);
	if (m_PlayerSearchInfo.CloseEnemyDist() < 400) {
		m_flNextFireTime -= 0.2f; //shoot faster at nearby enemies
	}

	if (bot_randfloat() < 0.3f) {
		m_curCmd.buttons |= IN_DUCK;
		//give them a little bit of extra time to crouch
		m_flNextFireTime += 0.2f;
	}
	return true;
}

bool CSDKBot::ThinkPointBlank_Check() {
	if (!(ThinkCheckDeath() && ThinkCheckMelee() && ThinkCheckExitCombat()))
		return false;

	//don't stop too quicky... or else we'll start moving before the bullet's fired.
	if (m_flNextFireTime + 0.8f < gpGlobals->curtime && !CanFire()) {
		//we've just shot!
		if (bot_randfloat() < 0.7f && ShouldReload()) {
			ScheduleThinker(&BotThinkers::Reload, 0.4f);
		} else {
			ScheduleThinker(&BotThinkers::MedRange, 0.2f);
		}

		return false;
	}

	//check if we've been sitting too long because the enemy died or something
	if (m_flNextFireTime + 1.0f < gpGlobals->curtime) {
		ScheduleThinker(&BotThinkers::LongRange, 0.15f);
		return false;
	}
	return true;
}

bool CSDKBot::ThinkPointBlank() {
	CBasePlayer* pEnemy = m_PlayerSearchInfo.CloseEnemy();
	if (pEnemy) {
		LookAt(pEnemy->Weapon_ShootPosition() + downToChest * bot_randfloat(0.5, 2.0), 0.9f, m_pDifficult->m_flRandomAim / 6);
	}
	if (CanFire() && m_flNextFireTime < gpGlobals->curtime && !IsAimingAtTeammate(m_PlayerSearchInfo.CloseEnemyDist())) {
		m_curCmd.buttons |= IN_ATTACK;
		//ScheduleThinker(m_pPrevThinker, 0);
		//return false;
	}

	return true;
}

bool CSDKBot::ThinkPointBlank_End() {
	m_curCmd.buttons &= ~IN_ATTACK;
	m_curCmd.buttons &= ~IN_DUCK;
	m_curCmd.buttons &= ~IN_ZOOM;
	
	m_flNextFireTime = FLT_MAX;
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: This runs while reloading. Bot either retreats to teammate, retreats from enemy, or follows navpoints
//		Or stands still if at flag
//		@updates m_flEndRetreatTime - to ensure we don't leave before we've started reloading
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkReload_Begin() {
	SendBotVcommContext(BotContext::DEFAULT);
	m_curCmd.buttons |= IN_RELOAD;
	StopMoving();
	m_PlayerSearchInfo.UpdateNavpointFirst();

	//this is used to switch between looking away/toward enemy
	m_flNextStrafeTime = gpGlobals->curtime;

	//give us some time before we could even finish reloading
	m_flEndRetreatTime = gpGlobals->curtime + 3.0f;

	return true;
}

bool CSDKBot::ThinkReload_Check() {
	if (!ThinkCheckDeath())
		return false;

	if (m_flEndRetreatTime < gpGlobals->curtime && !m_pPlayer->Weapon_InReload()) {
		//hey! we've finished reloading!
		if (m_PlayerSearchInfo.CloseEnemyDist() < 400)
			ScheduleThinker(&BotThinkers::PointBlank, 0.01f);
		else if (m_PlayerSearchInfo.CloseEnemyDist() < MED_RANGE_START)
			ScheduleThinker(&BotThinkers::MedRange, 0.2f);
		else
			ScheduleThinker(&BotThinkers::Waypoint, 0.3f);

		return false;
	}

	return true;
}

bool CSDKBot::ThinkReload() {
	//Spam the reload button - the reload only happens when the button changes
	ToggleButton(IN_RELOAD);

	//if we're capturing a flag, don't do anything
	if (m_PlayerSearchInfo.CloseFriend() && IsCapturingEnemyFlagAttempt()) {
		StopMoving();
	} else if (m_PlayerSearchInfo.CloseEnemy() && m_PlayerSearchInfo.OutNumbered() > 0.35) {
		MoveAwayFromEnemy();
	} else if (m_PlayerSearchInfo.CloseFriend() && !m_PlayerSearchInfo.CloseFriend()->Weapon_InReload()) {
		MoveToNearestTeammate();
	} else {
		MoveToWaypoint();
	}
	return true;
}

bool CSDKBot::ThinkReload_End() {
	m_curCmd.buttons &= ~IN_RELOAD;
	m_flEndRetreatTime = FLT_MAX;
	m_flNextStrafeTime = FLT_MAX;
	StopMoving();
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Manages death/spawn switches, otherwise idles
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkDeath_Begin() {
	SetUpdateFlags(false);
	m_flNextFireTime = FLT_MAX;
	m_curCmd.buttons = 0;
	return true;
}

bool CSDKBot::ThinkDeath_Check() {
	if (m_pPlayer->IsAlive()) {
		ScheduleThinker(&BotThinkers::Waypoint, 0.1f);
		return false;
	}
	return true;
}

bool CSDKBot::ThinkDeath() { return true; }

bool CSDKBot::ThinkDeath_End() {
	m_curCmd.buttons = 0;
	//SetUpdateFlags(true); //we technically don't have to do this, because if we're alive, our next thinker is always waypoint think, which also sets it to true
	m_flNextFireTime = FLT_MAX;
	//m_flEndRetreatTime;
	m_pWeapon = (m_pPlayer->GetActiveWeapon());
	if (m_pWeapon) {
		m_flMeleeRange = m_pWeapon->Def()->m_Attackinfos[1].m_flRange;
		m_flAdjustedMeleeRange = m_flMeleeRange - m_pDifficult->m_flMeleeRangeOffset;
	}

	//update our searcher's team info in case we switched teams
	m_PlayerSearchInfo.UpdateOwnerTeamInfo();

	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Function group for navigating between waypoints when no other objectives can be found
//-------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose: Run through all the Bots in the game and let them think.
//-----------------------------------------------------------------------------
void Bot_RunAll(void)
{
	/*if (g_iWaitingAmount && g_bServerReady) //Kind of a shitty hack. But this will allow people to spawn a certain amount of bots in 
	{										  //the server.cfg. Anyway, the server is ready, so do it.
		BotPutInServer(g_iWaitingAmount, false);
		g_iWaitingAmount = 0; //Make sure we've reset the waiting count.
	}*/

	//bot manager handles bot population via the svars
	CBotManager::Think();

	//let the vcomm managers do their thing
	g_BotAmerComms.Think();
	g_BotBritComms.Think();

	if (bot_pause.GetBool()) //If we're true, just don't run the thinking cycle. Effectively "pausing" the bots.
		return;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CSDKPlayer *pPlayer = UTIL_PlayerByIndex(i);// );
		
		if (pPlayer && (pPlayer->GetFlags() & FL_FAKECLIENT))
		{
			CSDKBot *pBot = CSDKBot::ToBot(pPlayer);
			if (pBot) //Do most of the "filtering" here.
				pBot->Think();
		}
	}
}



//returns whether entity B is in sight of entity A
/*bool IsInSight(CBaseEntity *pA, CBaseEntity *pB)
{
	trace_t tr;
	UTIL_TraceLine(pA->GetLocalOrigin() + Vector(0, 0, 36),
		pB->GetLocalOrigin() + Vector(0, 0, 36),
		MASK_SOLID, pA, COLLISION_GROUP_DEBRIS_TRIGGER, &tr);

	return !tr.DidHitWorld();
}*/

/*bool Bot_RunMimicCommand(CUserCmd& cmd)
{
	if (bot_mimic.GetInt() <= 0)
		return false;

	if (bot_mimic.GetInt() > gpGlobals->maxClients)
		return false;


	CBasePlayer *pPlayer = UTIL_PlayerByIndex(bot_mimic.GetInt());
	if (!pPlayer)
		return false;

	if (!pPlayer->GetLastUserCommand())
		return false;

	cmd = *pPlayer->GetLastUserCommand();
	cmd.viewangles[YAW] += bot_mimic_yaw_offset.GetFloat();

	return true;
}*/

//-----------------------------------------------------------------------------
// Purpose: Simulates a single frame of movement for a player
// Input  : *fakeclient - 
//			*viewangles - 
//			forwardmove - 
//			m_flSideMove - 
//			upmove - 
//			buttons - 
//			impulse - 
//			msec - 
// Output : 	virtual void
//-----------------------------------------------------------------------------
static void RunPlayerMove(CSDKPlayer *fakeclient, CUserCmd &cmd, float frametime)
{
	//if ( !fakeclient )
	//	return;

	// Store off the globals.. they're gonna get whacked
	float flOldFrametime = gpGlobals->frametime;
	float flOldCurtime = gpGlobals->curtime;

	float flTimeBase = gpGlobals->curtime + gpGlobals->frametime - frametime;
	fakeclient->SetTimeBase(flTimeBase);

	MoveHelperServer()->SetHost(fakeclient);
	fakeclient->PlayerRunCommand(&cmd, MoveHelperServer());

	// save off the last good usercmd
	fakeclient->SetLastUserCommand(cmd);

	// Clear out any fixangle that has been set
	fakeclient->pl.fixangle = FIXANGLE_NONE;

	// Restore the globals..
	gpGlobals->frametime = flOldFrametime;
	gpGlobals->curtime = flOldCurtime;
	MoveHelperServer()->SetHost(NULL);
}

/*void Bot_FlipOut(CSDKBot *pBot, CUserCmd &cmd, bool mayAttack, bool mayReload)
{
	if (bot_flipout.GetInt() > 0 /*&& pBot->m_pPlayer->IsAlive()*/ /*)
	{
		if (pBot->attack++ >= 20)//(RandomFloat(0.0,1.0) > 0.5) )
		{
			//cmd.buttons |= bot_forceattack2.GetBool() ? IN_ATTACK2 : IN_ATTACK;
			if (mayAttack)
				cmd.buttons |= IN_ATTACK;

			pBot->attack = 0;
		}

		if (pBot->attack2++ >= 13)
		{
			if (mayAttack)
				cmd.buttons |= IN_ATTACK2;

			pBot->attack2 = 0;
		}

		/*if( RandomFloat( 0, 1 ) > 0.9 )
		cmd.buttons |= IN_RELOAD;*/
		/*if (pBot->reload++ >= 40)
		{
			if (mayReload)
				cmd.buttons |= IN_RELOAD;

			pBot->reload = 0;
		}

		if (bot_flipout.GetInt() >= 2)
		{
			QAngle angOffset = RandomAngle(-1, 1);

			pBot->m_LastAngles += angOffset;

			for (int i = 0; i < 2; i++)
			{
				if (fabs(pBot->m_LastAngles[i] - pBot->m_ForwardAngle[i]) > 15.0f)
				{
					if (pBot->m_LastAngles[i] > pBot->m_ForwardAngle[i])
					{
						pBot->m_LastAngles[i] = pBot->m_ForwardAngle[i] + 15;
					}
					else
					{
						pBot->m_LastAngles[i] = pBot->m_ForwardAngle[i] - 15;
					}
				}
			}

			pBot->m_LastAngles[2] = 0;

			pBot->m_pPlayer->SetLocalAngles(pBot->m_LastAngles);
		}
	}
}*/

/**
* A simple lagged Fibonacchi generator.
* We need this because something's wrong with Valve's PRNG.
* Both RandomInt() and random->RandomInt() seem to eventually "run of out randomness".
* Also, Valve decided to replace rand() with a call to random->RandomInt() for some reason.
*/
int a = 9158152, b = 14257153;
int bot_rand()
{
	a += b + (a >> 5);
	b += a + (b >> 11);

	return b;
}

int bot_rand(int min, int max) {
	return RoundFloatToInt(bot_randfloat(min, max));
}

float bot_randfloat() {
	float result = (1.0f * bot_rand()) / INT_MAX;
	if (result > 0)
		return result;
	else
		return -result;
}

float bot_randfloat(float min, float max) {
	float result = (1.0f * bot_rand()) / INT_MAX;
	if (result < 0)
		result = -result;
	result = min + result*(max-min);
	return result;
}

/*void Bot_UpdateStrafing(CSDKBot *pBot, CUserCmd &cmd)
{
	if (gpGlobals->curtime >= pBot->m_flNextStrafeTime)
	{
		pBot->m_flNextStrafeTime = gpGlobals->curtime + RandomFloat(0.7f, 1.5f);

		if (bot_rand() % 5 == 0)
		{
			pBot->m_flSideMove = -600.0f + (bot_rand() % 1200);
		}
		else
		{
			pBot->m_flSideMove = 0;
		}

		if (bot_rand() % 20 == 0)
		{
			pBot->m_bBackwards = true;
		}
		else
		{
			pBot->m_bBackwards = false;
		}
	}
	else
		pBot->m_flSideMove *= 0.9f;

	cmd.sidemove = pBot->m_flSideMove;
}*/

/**
* Traces in front of the bot to see if something's in the way.
* Used for basic obstacle avoidance.
*/
bool Bot_TraceAhead(CSDKBot *pBot, CUserCmd &cmd)
{
	trace_t tr;
	Vector forward;

	AngleVectors(pBot->m_curCmd.viewangles, &forward);

	for (int ofs = -40; ofs < 56; ofs += 8)
	{
		UTIL_TraceLine(pBot->m_pPlayer->Weapon_ShootPosition() + Vector(0, 0, ofs),
			pBot->m_pPlayer->Weapon_ShootPosition() + Vector(0, 0, ofs) + forward * 56,
			MASK_SOLID, pBot->m_pPlayer, COLLISION_GROUP_NONE, &tr);

		if (tr.fraction < 1.0)
			return true;
	}

	return false;
}


