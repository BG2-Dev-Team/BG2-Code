/*
The Battle Grounds 3 - A Source modification
Copyright (C) 2017, The Battle Grounds 3 Team and Contributors

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

//BG3 - <name of contributer>[ - <small description>]
*/
#ifndef SDK_BOT_TEMP_H
#define SDK_BOT_TEMP_H
#ifdef _WIN32
#pragma once
#endif

#include "bg3_player_search.h"
#include "bg3_bot_navpoint.h"
#include "bg2/weapon_bg2base.h"

//BG2 - Tjoppen - HACKHACK
#define CSDKPlayer CBasePlayer

//linear interpolation from v0 to v1
inline Vector LerpVector(const Vector& v0, const Vector& v1, float lerp);

class CSDKBot;

/*
BotThinker contexts are used for bot's vcomms
These are dispatched to the vcomm manager by thinks and state changes
*/
enum BotContext {
	CONTEXT_NONE = 0,
	ADVANCE,
	RETREAT,
	OBJECTIVE,
	CLEAR,
	FIRE,
	AFFIRM, //yes
	NEGATE, //no
	DEFAULT, //battlecry
	NUM_CONTEXTS //useful, don't change ordering
};



struct BotThinker {
	bool (CSDKBot::* m_pPreThink)(); //called when this think is first scheduled
	bool (CSDKBot::* m_pThinkCheck)(); //checks if a different thinker should be scheduled
	bool (CSDKBot::* m_pThink)(); //called after every m_flThinkDelay
	bool (CSDKBot::* m_pPostThink)(); //called when another event is scheduled
	float m_flThinkDelay; //delay between each think
	BotContext m_eContext;
	const char * m_ppszThinkerName;
	

	BotThinker(bool (CSDKBot::* preThinker)(),
			   bool (CSDKBot::* thinkChecker)(), 
			   bool (CSDKBot::* thinker)(),  
			   bool (CSDKBot::* postThinker)(), 
			   float delay,
			   const char * thinkerName) {
		m_pPreThink = preThinker;
		m_pThink = thinker;
		m_pThinkCheck = thinkChecker;
		m_pPostThink = postThinker;
		m_flThinkDelay = delay;
		m_ppszThinkerName = thinkerName;
	}
};

//Forward declerations of BotThinkers
namespace BotThinkers {
	extern BotThinker Death;
	extern BotThinker Waypoint;
	extern BotThinker Retreat;
	extern BotThinker Melee;
	extern BotThinker PointBlank;
	extern BotThinker MedRange;
	extern BotThinker LongRange;
	extern BotThinker Follow;
}


struct BotDifficulty;
extern BotDifficulty BotDiffEasy;
extern BotDifficulty BotDiffNorm;
extern BotDifficulty BotDiffHard;

struct BotDifficulty {
	vec_t m_flRandomAim; //magninute of random angle from target, in degrees
	float m_flMidRangeReload; //chance of reloading after firing in mid-range combat
	float m_flLongRangeFire; //chance of firing early in long-range
	float m_flMeleeTurnLerp; //higher value allow for faster turning in melee
	float m_flBravery; //0.0-1.0, tendency to run away when outnumbered
	float m_flMeleeRangeOffset; //reduction of melee range
	float m_flMeleeReaction;

	BotDifficulty(vec_t aimInnaccuracy, float midRangeReload, float longRangeFire, float meleeTurnLerp, float bravery, float meleeRangeOffset, float meleeReaction) {
		m_flRandomAim = aimInnaccuracy;
		m_flMidRangeReload = midRangeReload;
		m_flLongRangeFire = longRangeFire;
		m_flMeleeTurnLerp = meleeTurnLerp;
		m_flBravery = bravery;
		m_flMeleeRangeOffset = meleeRangeOffset;
		m_flMeleeReaction = meleeReaction;
	}

	static BotDifficulty* Random() {
		BotDifficulty* result;
		float flRand = RandomFloat();
		if (flRand < 0.2f)
			result = &BotDiffEasy;
		else if (flRand < 0.8f)
			result = &BotDiffNorm;
		else
			result = &BotDiffHard;
		return result;
	}
};

// This is our bot class.
class CSDKBot // : public CSDKPlayer
{
	friend class NBotInfluencer;
public:
	CPlayerSearch	m_PlayerSearchInfo;
	BotThinker*		m_pPrevThinker;
	BotThinker*		m_pCurThinker;
	static BotThinker* s_pDefaultThinker;
	float			m_flNextThink;
	BotDifficulty*	m_pDifficult;

	bool			m_bLastThinkWasStateChange;

	bool			m_bUpdateFlags; //we can save a little performance by not updating the flags if we don't need to
	void			SetUpdateFlags(bool bUpdate); //we use this setter to also check for LMS in which case we should actually ignore flags
	bool			m_bTowardFlag; //Whether or not to ignore enemies and move to the nearest flag

	float			m_flNextFireTime;
	float			m_flFireEndTime; //it takes .135s for the bullet to come out, so don't mess up accuracy beforehand

	bool			m_bLastThinkWasInFlag; //used by flag think to do one-time events on entering flag
	//bool			m_bBackwards, m_bLastTurnToRight;

	float			m_flNextStrafeTime;
	float			m_flForceNoRetreatTime;

	QAngle			m_ForwardAngle;
	Quaternion		m_LastAngles;

	

	CSDKPlayer		*m_pPlayer;
	CBaseCombatWeapon	*m_pWeapon;
	EHANDLE			m_hFollowedPlayer;
	float			m_flLastFollowTime;
	float			m_flMeleeRange; //calculated whenever spawned
	float			m_flAdjustedMeleeRange; //calculated whenever spawned

	CUserCmd		m_curCmd;
	CUserCmd		m_LastCmd;
	inline void		ToggleButton(int button) { m_curCmd.buttons ^= button; }

	/*
	Constructors and Initializers
	*/
	CSDKBot(); //called when game server is created
	static void Init(CBasePlayer* pPlayer, BotDifficulty* pDifficulty); //called when a bot is put in the server

	static BotDifficulty* GetDifficulty();

	/*
	Helper Functions common to many of the thinker function groups
	*/
	void ScheduleThinker(BotThinker* pNextThinker, float delay);

	static CSDKBot*	ToBot(CBasePlayer* pPlayer);
	bool		DoMelee(); //based on difficulty, range, current weapon, a little randomness
	inline bool CanFire(); //whether or not the bot's current weapon is capable of firing
	inline bool ShouldReload(); //whether or not we can and could reload
	bool		IsAimingAtTeammate(vec_t range) const; //whether or not we're aiming at a teammate
	bool		IsCapturingEnemyFlag() const;
	bool		IsCapturingEnemyFlagAttempt() const;
	inline bool WantsToRetreat() const { return m_flEndRetreatTime < FLT_MAX - 1; }
	
	ENavPointRange EnemyNoticeRange() const;

	void		LookAt(Vector location, float lerp, vec_t randomOffset = 0.0f); //Modifies the angles of m_curCmd to look at given target

	void		SendBotVcommContext(BotContext context); //sends context to vcomm manager

	void MoveToNearestTeammate();
	void MoveToWaypoint();
	void MoveAwayFromEnemy();
	void MoveTowardPointNoTurn(const Vector& vLocation); // moves bot toward point without changing look target
	void DanceAround(const Vector& vLocation, vec_t flMaxRange); //for dancing around a flag or other point
	void StopMoving();
	void TeammateGoAround(bool bStopSideStep);

	/*
	These thinks checks return false if they scheduled a new BotThinker, true otherwise
	These checks are prioritized and ordered in the thinker's individual think checks
	*/
	bool ThinkCheckDeath(); //checks if we're dead
	bool ThinkCheckPointBlank(); //Checks if m_flNextFireTime tells us to poibt-blank
	bool ThinkCheckMelee(); //checks if we should immediately enter close-quarters melee
	bool ThinkCheckExitMelee(); //returns us to the last thinker if melee is over
	bool ThinkCheckRetreat(); //Checks if we're outnumbered
	bool ThinkCheckRetreatEnd(); //Checks if we've retreated long enough or that friends are close
	bool ThinkCheckFlag(); //Looks for a flag, if an enemy is not too close
	bool ThinkCheckFlagExit(); //exits to waypoint or med-range if there are no flags visible
	bool ThinkCheckFlagOrFight(); //In mid-range combat, checks if we should fight or run to flag
	bool ThinkCheckEnterLongRangeCombat();
	bool ThinkCheckEnterMedRangeCombat();
	bool ThinkCheckStopFollow();
	bool ThinkCheckExitCombat();

	static const int LONG_RANGE_START = 1680;
	static const int MED_RANGE_START = 850;
	static const int MELEE_RANGE_START = 140;
	static const int RETREAT_STOP_RANGE = 125; //retreat stops when we are this close to friend
	static const int HELP_START_RANGE = 200; //teammates will rush forward to other teammates who are this close to the enemy

	/*
	Master think functions
	*/
	void Think();
	void ButtonThink(); //translate movement button presses into the movement floats
	void PostStateChangeThink(); //called one think after any state change
	static void ResetAllBots();

	/*
	BotThinker Function groups
	*/
	//For navigating between waypoints when no other objective can be found
	bool ThinkWaypoint_Begin();
	bool ThinkWaypoint_Check(); //check for flag or enemy
	bool ThinkWaypoint(); //move towards next waypoint
	bool ThinkWaypoint_End();

	//For navigating to undefended flags
	bool ThinkFlag_Begin();
	bool ThinkFlag_Check(); //if enemy is closer than next flag, point-blank if applicable
	bool ThinkFlag(); //move toward flag or stand on it, dodge enemy bullets
	bool ThinkFlag_End();

	//For long-range musket combat
	bool ThinkLongRange_Begin(); //checks if we want to fire based on several factors, including difficulty
	bool ThinkLongRange_Check(); //Enemy death, enter medium range, flag becomes closer
	bool ThinkLongRange(); //Move towards enemy, not in a straight line
	bool ThinkLongRange_End();

	//For medium-range point-blank standoffish combat
	bool ThinkMedRange_Begin(); //Decide if and when we want to fire
	bool ThinkMedRange_Check(); //Decide if we point-blank, decide if we want to retreat or attack, enemy death, or flag becomes closer
	bool ThinkMedRange(); //Dance in a general direction toward the enemy, depending
	bool ThinkMedRange_End();

	bool ThinkMelee_Begin(); //whether or not we shoot, how aggressive we want to be, etc.
	bool ThinkMelee_Check(); //If we're dead or if we should leave close-range combat
	bool ThinkMelee(); //aim your stab and stab if you can, otherwise run around
	bool ThinkMelee_End();

	bool ThinkRetreat_Begin(); //Decide how long we retreat for
	bool ThinkRetreat_Check(); //When to stop retreating, after amount of time, reach flag, reach teammate
	bool ThinkRetreat(); //Run away from enemy and toward teammate/flag
	bool ThinkRetreat_End();
	float m_flEndRetreatTime;

	bool ThinkPointBlank_Begin(); //decide when we stop aiming if we can't find a shot, etc.
	bool ThinkPointBlank_Check(); //after m_flEndFireTime, we're dead, target already dead, etc.
	bool ThinkPointBlank(); //aim and fire, but don't shoot teammate!
	bool ThinkPointBlank_End();

	bool ThinkReload_Begin(); //Press reload button
	bool ThinkReload_Check(); //Finished reloading? -> waypoint or shoot?
	bool ThinkReload(); //stand still, spin, retreat, or move to waypoint?
	bool ThinkReload_End(); //Un-press reload button

	bool ThinkDeath_Begin(); //WE DEAD OH NOES
	bool ThinkDeath_Check(); //Back to life?!
	bool ThinkDeath(); //do nothing
	bool ThinkDeath_End(); //do spawn-related stuff here

	bool ThinkFollow_Begin(); //stop updating flags, reset next follow time
	bool ThinkFollow_Check(); //does our followed player still exist and still alive? Have we followed long enough?
	bool ThinkFollow(); //follow player, shoot sometimes
	bool ThinkFollow_End();
};

extern CSDKBot	gBots[MAX_PLAYERS];
extern bool g_bServerReady;
extern int g_iWaitingAmount;

// If iTeam or iClass is -1, then a team or class is randomly chosen.
CBasePlayer *BotPutInServer(int iAmount, bool bFrozen);
void Bot_RunAll();
void RunPlayerMove(CSDKPlayer *fakeclient, CUserCmd &cmd, float frametime);

int	  bot_rand();
int   bot_rand(int min, int max);
float bot_randfloat();
float bot_randfloat(float min, float max);
#endif // SDK_BOT_TEMP_Hd
