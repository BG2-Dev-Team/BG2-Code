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


ConVar bot_forcefireweapon("bot_forcefireweapon", "", 0, "Force bots with the specified weapon to fire.");
ConVar bot_forceattack2("bot_forceattack2", "0", 0, "When firing, use attack2.");
ConVar bot_forceattackon("bot_forceattackon", "0", 0, "When firing, don't tap fire, hold it down.");
ConVar bot_flipout("bot_flipout", "1", 0, "When on, all bots fire their guns.");
ConVar bot_limitclass("bot_limitclass", "1", 0, "Force bots to conform to class regulations.");
ConVar bot_forceclass("bot_forceclass", "0", FCVAR_GAMEDLL | FCVAR_HIDDEN, "Force bots to spawn as given class. 0 = Infantry, 1 = Officer, 2 = Sniper, 3 = Skirmisher");
static ConVar bot_mimic("bot_mimic", "0", 0, "Bot uses usercmd of player by index.");
static ConVar bot_mimic_yaw_offset("bot_mimic_yaw_offset", "0", 0, "Offsets the bot yaw.");
ConVar bot_pause("bot_pause", "0", 0, "Stops the bot thinking cycle entirely.");
ConVar bot_difficulty("bot_difficulty", "3", FCVAR_GAMEDLL, "Bot skill level. 0,1,2,3 are easy, norm, hard, and random, respectively.", true, 0.f, true, 1.f);

//ConVar bot_sendcmd( "bot_sendcmd", "", 0, "Forces bots to send the specified command." );

//So what if these are global? Want to fight about it?
CSDKBot	gBots[MAX_PLAYERS];
bool g_bServerReady = false; //Are we ready to use the temp int below?
int  g_iWaitingAmount = 0; //This is just a temp int really.
int  g_iNextBotTeam = TEAM_BRITISH;

static const Vector downToChest(0.f, 0.f, -10.f);

const char* g_ppszEasyBotPrefixes[] = { "Pte.", "Pte.", "LCpl.", };

const char* g_ppszMedBotPrefixes[] = { "Pte.", "Pte.", "LCpl.", "LCpl.", "Cpl.", };

const char* g_ppszHardBotPrefixes[] = { "Cpl.", "Cpl.", "Sjt.", "Sjt.", };

const char* g_ppszRareBotPrefixes[] = { "Lt.", "Capt.", "Maj.", "LtCol.", };

const char* g_ppszBotNames[] = { 
	"Martin", "Smith", "Williams", "Jones", "Coleman", "Jenkins", "Adams", "Clark",
	"Hall", "Nelson", "Campbell", "Cox", "Cook", "Washington", "Cooper", "Harris", "Rogers",
	"Torres", "Ward", "Murphy", "Long", "Alexander", "Thomas", "Wright", "Bell", "Richardson",
	"Evans", "Butler", "Kelly", "Lee", "Rivera", "Miller", "Davis", "Garcia", "Griffin",
	"Bailey", "Powell", "Ross", "Wood", "Reed", "Lewis", "Taylor", "Perry", "Young",
};

const char* g_ppszBotRareNames[] = { 
	"Karpinsky", "Allen", "Hale", "Revere", "Tallmadge", "Gates", "Morgan", "Ziegler",
	"Arnold", "Greene", "Howe", "Prescott",

};

//Bot difficulties - the meanings of these numbers are
//						 (aimAngle,	reload,	earlyFire,	meleeTurn,	brave,	meleeRangeOffset,	meleeReaction)
BotDifficulty BotDiffEasy(6.f,		0.4f,	0.8f,		0.3f,		0.3f,	30.f,				0.35f);
BotDifficulty BotDiffNorm(4.f,		0.1f,	0.55f,		0.55f,		0.5f,	15.f,				0.4f);
BotDifficulty BotDiffHard(2.f,		0.03f,	0.25f,		0.7f,		0.8f,	8.f,				0.45f);

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
	BotThinker Death	(&CSDKBot::ThinkDeath_Begin,		&CSDKBot::ThinkDeath_Check,		&CSDKBot::ThinkDeath,		&CSDKBot::ThinkDeath_End,		1.0f, "Death Think");
}
//Default thinker is death
BotThinker* CSDKBot::s_pDefaultThinker = &BotThinkers::Death;


//-----------------------------------------------------------------------------
// Purpose: Create a new Bot and put it in the game.
// Output : Pointer to the new Bot, or NULL if there's no free clients.
//-----------------------------------------------------------------------------
CBasePlayer *BotPutInServer(int iAmount, bool bFrozen)
{
	int i = 1;
	while (i <= iAmount)
	{
		char botname[32];
		const char * pRank;
		const char * pName;

		//First choose a difficulty
		BotDifficulty* pDifficulty = CSDKBot::GetDifficulty();

		//randomly get a name
		if (pDifficulty == &BotDiffEasy) {
			pRank = g_ppszEasyBotPrefixes[RandomInt(0, ARRAYSIZE(g_ppszEasyBotPrefixes) - 1)];
			pName = g_ppszBotNames[RandomInt(0, ARRAYSIZE(g_ppszBotNames) - 1)];
		} else if (pDifficulty == &BotDiffNorm) {
			pRank = g_ppszMedBotPrefixes[RandomInt(0, ARRAYSIZE(g_ppszMedBotPrefixes) - 1)];
			pName = g_ppszBotNames[RandomInt(0, ARRAYSIZE(g_ppszBotNames) - 1)];
		} else {
			if (RandomFloat() > 0.95f) {
				pRank = g_ppszRareBotPrefixes[RandomInt(0, ARRAYSIZE(g_ppszRareBotPrefixes) - 1)];
				pName = g_ppszBotRareNames[RandomInt(0, ARRAYSIZE(g_ppszBotRareNames) - 1)];
			} else {
				pRank = g_ppszHardBotPrefixes[RandomInt(0, ARRAYSIZE(g_ppszHardBotPrefixes) - 1)];
				pName = g_ppszBotNames[RandomInt(0, ARRAYSIZE(g_ppszBotNames) - 1)];
			}
		}

		Q_snprintf(botname, sizeof(botname), "%s %s", pRank, pName);

		// This trick lets us create a CSDKBot for this client instead of the CSDKPlayer
		// that we would normally get when ClientPutInServer is called.
		//ClientPutInServerOverride( &CBotManager::ClientPutInServerOverride_Bot );
		edict_t *pEdict = engine->CreateFakeClient(botname);
		//ClientPutInServerOverride( NULL );

		if (!pEdict)
		{
			Msg("Failed to create Bot(%s).\n", botname);
			return NULL;
		}

		// Allocate a player entity for the bot, and call spawn
		CSDKPlayer *pPlayer = ((CSDKPlayer *)CBaseEntity::Instance(pEdict));

		if (!pPlayer)
		{
			Msg("Couldn't cast bot to player\n");
			return NULL;
		}

		//initialize the bot's AI, class, and team
		CSDKBot::Init(pPlayer, pDifficulty);

		i++;
		//return pPlayer;
	}
	return NULL;
}

//ConCommand  cc_Bot( "bot_add", BotAdd_f, "Add a bot", FCVAR_CHEAT );
CON_COMMAND_F(bot_add, "Creates bot(s)in the server. <Bot Count>", FCVAR_GAMEDLL)
{
	int m_iCount = 0;
	if (args.ArgC() > 1)
		m_iCount = atoi(args[1]); //Spawn given number of bots.
	else
		m_iCount = 1; //Just spawn 1 bot.
	g_iNextBotTeam = g_iNextBotTeam == TEAM_AMERICANS ? TEAM_BRITISH : TEAM_AMERICANS;

	if (g_bServerReady) //Server is already loaded, just do it.
		BotPutInServer(m_iCount, false);
	else				  //Server just exec'd the server.cfg, but isn't ready to handle players. Just wait a while.
		g_iWaitingAmount = m_iCount;
}

//called when game server is created
CSDKBot::CSDKBot() {
	m_bLastThinkWasStateChange = false;
	m_bUpdateFlags = false;
	m_bTowardFlag = false;
	m_pPlayer = NULL;
	m_flNextFireTime = FLT_MAX;
	m_flNextStrafeTime = FLT_MAX;
	m_flEndRetreatTime = FLT_MAX;

	m_pCurThinker = m_pPrevThinker = s_pDefaultThinker;

}

//called when a bot is put in the server
void CSDKBot::Init(CBasePlayer* pPlayer, BotDifficulty* pDifficulty) {
	CSDKBot& curBot = gBots[pPlayer->GetClientIndex()];
	curBot.m_pPlayer = pPlayer;
	curBot.m_flNextStrafeTime = 0;

	//default to death thinker or else bad things happen!
	curBot.m_pPrevThinker = &BotThinkers::Death;
	curBot.m_pCurThinker = &BotThinkers::Death;

	curBot.m_pDifficult = pDifficulty;
	curBot.m_flNextThink = gpGlobals->curtime + 0.1f;
	pPlayer->ClearFlags();

	//lastClass = (lastClass + 1) % 4; //4 classes now.
	int lastClass = RandomInt(0, 3);

	pPlayer->ChangeTeam(g_iNextBotTeam);

	//BG2 - Obey Class Limits now. -HairyPotter
	if (bot_forceclass.GetInt() > -1) //Force the bot to spawn as the given class.
		((CHL2MP_Player*)pPlayer)->SetNextClass(bot_forceclass.GetInt());

	else if (bot_limitclass.GetBool() && bot_forceclass.GetInt() < 0) //Just make sure we're not trying to force a class that has a limit.
	{
		int limit = HL2MPRules()->GetLimitTeamClass(g_iNextBotTeam, lastClass);
		if (limit >= 0 && g_Teams[g_iNextBotTeam]->GetNumOfNextClass(lastClass) >= limit)
		{
			Msg("Tried to spawn too much of class %i. Spawning as infantry. \n", lastClass);
			((CHL2MP_Player*)pPlayer)->SetNextClass(0); //Infantry by default
		} else
			((CHL2MP_Player*)pPlayer)->SetNextClass(lastClass);
	} else //Otherwise just spawn as whatever RandomInt() turns up.
		((CHL2MP_Player*)pPlayer)->SetNextClass(lastClass);

	pPlayer->AddFlag(FL_CLIENT | FL_FAKECLIENT);
	pPlayer->Spawn();
	curBot.m_PlayerSearchInfo.Init(pPlayer);
}

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
CSDKBot* ToBot(CBasePlayer* pPlayer) {
	return &gBots[pPlayer->GetClientIndex()];
}

//-------------------------------------------------------------------------------------------------
// Purpose: Checks enemy position, current weapon, a little randomness etc. to decide to stab
//-------------------------------------------------------------------------------------------------
bool CSDKBot::DoMelee() {
	bool shouldMelee = false;
	if (bot_randfloat() < m_pDifficult->m_flMeleeReaction) {
		bool didMelee = false;
		static const Vector downToStomach(0, 0, -15);
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
// Purpose: Checks whether or not the musket is capable of firing at this moment. Does not look for FF
//-------------------------------------------------------------------------------------------------
bool CSDKBot::CanFire() const {
	bool canFire = false;

	if (m_pWeapon) {
		canFire = m_pWeapon->Clip1() > 0;
	}

	return canFire;
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
	if (m_PlayerSearchInfo.CloseEnemyDist() < 800 && m_PlayerSearchInfo.OutNumbered() > 0.57 && m_PlayerSearchInfo.CloseFriendDist() < 800
		&& m_PlayerSearchInfo.CloseFriendDist() > RETREAT_STOP_RANGE) {
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
	CBotNavpoint* pPoint = m_PlayerSearchInfo.CurNavpoint();
	if (pPoint) {
		m_PlayerSearchInfo.UpdateNavpoint();
		pPoint = m_PlayerSearchInfo.CurNavpoint();
		//a navpoint will always return a reference to a navpoint, so we don't have to check it again
		LookAt(pPoint->GetLookTarget(), 0.6f, 5);

	} else {
		m_PlayerSearchInfo.UpdateNavpointFirst();
	}
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
	m_bUpdateFlags = true;
	m_bTowardFlag = true;
	return true;
}

bool CSDKBot::ThinkFlag_Check() {
	return ThinkCheckDeath() && ThinkCheckRetreat() && ThinkCheckFlagOrFight()
		&& ThinkCheckMelee() && ThinkCheckPointBlank() && ThinkCheckFlagExit();
}

bool CSDKBot::ThinkFlag() {
	if (IsCapturingEnemyFlagAttempt()) {
		m_curCmd.buttons = 0;
		CBasePlayer* pEnemy = m_PlayerSearchInfo.CloseEnemy();
		if (pEnemy) {
			LookAt(pEnemy->Weapon_ShootPosition(), 0.5f, 3);
			if (bot_randfloat() < 0.3f)
				m_flNextFireTime = gpGlobals->curtime;
		}
	} else {
		CFlag* pFlag = m_PlayerSearchInfo.CloseEnemyFlagVisible();
		/*if (!pFlag) {
			pFlag = m_PlayerSearchInfo.CloseEnemyFlag();
		}*/

		if (pFlag) {
			m_curCmd.buttons |= IN_FORWARD;
			LookAt(pFlag->GetAbsOrigin() + Vector(0, 0, 40), 0.8f, 5);
		}

	}
	return true;
}

bool CSDKBot::ThinkFlag_End() {
	m_curCmd.buttons = 0;
	m_bTowardFlag = false;
	return true;
}

//-------------------------------------------------------------------------------------------------
// Purpose: Long-range combat
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkLongRange_Begin() {
	SendBotVcommContext(ADVANCE);
	m_bUpdateFlags = true;
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
	m_bUpdateFlags = true;
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
		if (random < 0.7) {
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
	m_bUpdateFlags = false;
	m_curCmd.buttons |= IN_FORWARD;
	m_flNextStrafeTime = gpGlobals->curtime + RandomFloat(0.5f, 1.0f);
	if (CanFire()) {
		float random = RandomFloat();
		if (m_pPlayer->GetHealth() < 70)
			random += 0.2;
		if (random > 0.5) {
			m_flNextFireTime = gpGlobals->curtime + RandomFloat(0.5f, 1.2f);
		}
	}
	return true;
}

bool CSDKBot::ThinkMelee_Check() {
	return ThinkCheckDeath() && ThinkCheckExitMelee() && ThinkCheckExitCombat();
}

bool CSDKBot::ThinkMelee() {
	CBasePlayer* pEnemy = m_PlayerSearchInfo.CloseEnemy();
	if (pEnemy) {
		LookAt(pEnemy->Weapon_ShootPosition() + downToChest, m_pDifficult->m_flMeleeTurnLerp, m_pDifficult->m_flRandomAim);
	}
	if (m_flNextFireTime < gpGlobals->curtime && !IsAimingAtTeammate(m_PlayerSearchInfo.CloseEnemyDist())) {
		m_curCmd.buttons |= IN_ATTACK;
	}
	DoMelee();
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

		float randomForward = bot_randfloat();
		//don't move forward if we're too close
		if (randomForward < 0.3f && m_PlayerSearchInfo.CloseEnemyDist() < 70) {
			m_curCmd.buttons |= IN_FORWARD;
			m_curCmd.buttons &= ~IN_BACK;
		} else if (randomForward < 0.5f){
			m_curCmd.buttons &= ~IN_FORWARD;
			m_curCmd.buttons |= IN_BACK;
		} else {
			m_curCmd.buttons &= ~IN_FORWARD;
			m_curCmd.buttons &= ~IN_BACK;
		}

		m_flNextStrafeTime += RandomFloat(0.05f, 0.3f);
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
	m_bUpdateFlags = true;
	m_flNextFireTime = FLT_MAX;
	m_flEndRetreatTime = gpGlobals->curtime + RandomFloat(1.5, 5.0) * (1 - m_pDifficult->m_flBravery);
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
		Vector lookTarget;
		if (pFriend) {
			lookTarget = pFriend->Weapon_ShootPosition();
			m_curCmd.buttons |= IN_FORWARD;
			m_curCmd.buttons &= ~IN_BACK;
		} else if (pEnemy){
			lookTarget = pEnemy->Weapon_ShootPosition();
			m_curCmd.buttons |= IN_BACK;
			m_curCmd.buttons &= ~IN_FORWARD;
		}
		LookAt(lookTarget, 0.8f, 4.f);
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
	m_bUpdateFlags = false;
	m_curCmd.buttons = 0;
	//random crouching
	

	bool bZoom = m_PlayerSearchInfo.CloseEnemyDist() > 400 && bot_randfloat() < 0.5f;
	if (!bZoom)
		bZoom = m_PlayerSearchInfo.CloseEnemyDist() > 1100 && bot_randfloat() < 0.8f;

	if (bZoom) {
		m_curCmd.buttons |= IN_ZOOM;
	}

	m_flNextFireTime = gpGlobals->curtime + bot_randfloat(0.3f, 0.6f);

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
	//check if we've been sitting too long because the enemy died or something
	if (m_flNextFireTime + 0.5f < gpGlobals->curtime) {
		ScheduleThinker(&BotThinkers::LongRange, 0.15f);
		return false;
	}
	return true;
}

bool CSDKBot::ThinkPointBlank() {
	CBasePlayer* pEnemy = m_PlayerSearchInfo.CloseEnemy();
	if (pEnemy) {
		LookAt(pEnemy->Weapon_ShootPosition() + downToChest, 0.8f, m_pDifficult->m_flRandomAim / 3);
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
// Purpose: Manages death/spawn switches, otherwise idles
//-------------------------------------------------------------------------------------------------
bool CSDKBot::ThinkDeath_Begin() {
	m_bUpdateFlags = false;
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
	m_bUpdateFlags = true;
	m_flNextFireTime = FLT_MAX;
	m_flEndRetreatTime;
	m_pWeapon = dynamic_cast<CBaseBG2Weapon*>(m_pPlayer->GetActiveWeapon());
	if (m_pWeapon)
		m_flMeleeRange = m_pWeapon->m_Attackinfos[1].m_flRange;
	m_flNextVoice = gpGlobals->curtime + RandomFloat(3.0f, 30.0f);

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
	if (g_iWaitingAmount && g_bServerReady) //Kind of a shitty hack. But this will allow people to spawn a certain amount of bots in 
	{										  //the server.cfg. Anyway, the server is ready, so do it.
		BotPutInServer(g_iWaitingAmount, false);
		g_iWaitingAmount = 0; //Make sure we've reset the waiting count.
	}

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
			CSDKBot *pBot = &gBots[pPlayer->GetClientIndex()];
			if (pBot) //Do most of the "filtering" here.
				pBot->Think();
		}
	}
}

void CSDKBot::ResetAllBots() {
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CSDKPlayer *pPlayer = UTIL_PlayerByIndex(i);// );

		if (pPlayer && (pPlayer->GetFlags() & FL_FAKECLIENT))
		{
			CSDKBot *pBot = &gBots[pPlayer->GetClientIndex()];
			if (pBot) //Do most of the "filtering" here.
				pBot->ScheduleThinker(&BotThinkers::Death, 1.0f);
		}
	}
}

//returns whether entity B is in sight of entity A
bool IsInSight(CBaseEntity *pA, CBaseEntity *pB)
{
	trace_t tr;
	UTIL_TraceLine(pA->GetLocalOrigin() + Vector(0, 0, 36),
		pB->GetLocalOrigin() + Vector(0, 0, 36),
		MASK_SOLID, pA, COLLISION_GROUP_DEBRIS_TRIGGER, &tr);

	return !tr.DidHitWorld();
}

bool Bot_RunMimicCommand(CUserCmd& cmd)
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
}

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
int bot_rand()
{
	static int a = 9158152, b = 14257153;

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

//-----------------------------------------------------------------------------
// Run this Bot's AI for one frame.
//-----------------------------------------------------------------------------
/*char vcmd[512];
void Bot_Think(CSDKBot *pBot)
{

	/*if( !pBot ) //These are figured out by bot_runall anyway.
	return;
	if( !pBot->m_bInuse || !pBot->m_pPlayer )
	return;*/

	// Make sure we stay being a bot
	//pBot->m_pPlayer->AddFlag( FL_FAKECLIENT );

	/*CUserCmd cmd;
	Q_memset(&cmd, 0, sizeof(cmd));


	//For testing Voicecomms -HairyPotter
	if (bot_voicecomms.GetInt() && pBot->m_flNextVoice < gpGlobals->curtime)
	{
		Q_snprintf(vcmd, 512, "voicecomm %i\n", RandomInt(1, 7));
		CBaseEntity *ent = dynamic_cast< CBaseEntity * >(pBot->m_pPlayer);
		engine->ClientCommand(ent->edict(), vcmd);
		//engine->ClientCommand( pBot->m_pPlayer->edict(), UTIL_VarArgs( "voicecomm %i\n", RandomInt(1,7) ) );
		Msg(vcmd);
		pBot->m_flNextVoice = gpGlobals->curtime + 20.0f;
	}

	// Finally, override all this stuff if the bot is being forced to mimic a player.
	if (!Bot_RunMimicCommand(cmd))
	{
		Bot_UpdateDirectionAndSpeed(pBot, cmd);

		// Handle console settings.
		Bot_ForceFireWeapon(pBot, cmd);
		//Bot_HandleSendCmd( pBot );

		// Fix up the m_fEffects flags
		//pBot->m_pPlayer->PostClientMessagesSent();
	}
	//cmd.upmove = 0;
	//cmd.impulse = 0;

	pBot->m_LastCmd = cmd;

	RunPlayerMove(pBot->m_pPlayer, cmd, gpGlobals->frametime);
}*/


