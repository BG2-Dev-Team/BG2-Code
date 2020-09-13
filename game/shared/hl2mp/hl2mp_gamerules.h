//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#ifndef HL2MP_GAMERULES_H
#define HL2MP_GAMERULES_H
#pragma once

#include "gamerules.h"
#include "teamplay_gamerules.h"
#include "gamevars_shared.h"
#include "../shared/bg3/bg3_player_shared.h"

//BG2 - Skillet - For ragdoll removal stuff
#ifdef CLIENT_DLL
#include "c_hl2mp_player.h"
#endif

#ifndef CLIENT_DLL
#include "hl2mp_player.h"
#endif

//BG2 - Tjoppen - beautiful defines
#ifdef CLIENT_DLL
#define CVAR_FLAGS	(FCVAR_REPLICATED | FCVAR_NOTIFY)
#define CVAR_FLAGS_HIDDEN (FCVAR_REPLICATED)
#else
#define CVAR_FLAGS	(FCVAR_GAMEDLL | FCVAR_REPLICATED | FCVAR_NOTIFY)
#define CVAR_FLAGS_HIDDEN (FCVAR_GAMEDLL | FCVAR_REPLICATED)
#endif


#define VEC_CROUCH_TRACE_MIN	HL2MPRules()->GetHL2MPViewVectors()->m_vCrouchTraceMin
#define VEC_CROUCH_TRACE_MAX	HL2MPRules()->GetHL2MPViewVectors()->m_vCrouchTraceMax

extern ConVar mp_respawnstyle, mp_respawntime, mp_rounds, mp_roundtime, mp_tickets_a, mp_tickets_b;
extern ConVar mp_punish_bad_officer, mp_punish_bad_officer_nextclass;

extern ConVar lb_enforce_weapon_amer, lb_enforce_weapon_brit; // , lb_enforce_class_amer, lb_enforce_class_brit;
extern ConVar lb_enforce_no_buckshot;
extern ConVar lb_officer_protect, lb_officer_autodetect, lb_officer_classoverride_a, lb_officer_classoverride_b;
extern ConVar lb_officer_a; //for delegating the american officer for next round
extern ConVar lb_officer_b;	//for delegating the british officer for next round
extern ConVar lb_enforce_volley_fire, lb_enforce_volley_fire_tolerance, lb_enforce_no_troll;

inline bool IsLinebattle()	{ return mp_respawnstyle.GetInt() == 4; }
inline bool IsTicketMode()	{ return mp_respawnstyle.GetInt() == 3; }
inline bool IsLMS()			{ return mp_respawnstyle.GetInt() == 2 || IsLinebattle(); }
inline bool IsLMSstrict()	{ return mp_respawnstyle.GetInt() == 2; }
inline bool IsSkirmish()	{ return mp_respawnstyle.GetInt() == 1; }
inline bool UseLineSpawn()	{ return IsLinebattle(); }

void CSay(const char* pszFormat, ...);
void MSay(const char* pszFormat, ...);

#ifndef CLIENT_DLL
#define CON_COMMAND_SERVER(name, tooltip) \
	static void PerformCommand_##name(ConVar* pVar, const char* pszOldValue, float flOldValue); \
	ConVar cvar_command_##name(#name, "0", FCVAR_NOTIFY, tooltip, (FnChangeCallback_t)[](IConVar* pVar, const char* pszOldValue, float flOldValue){ \
		if (pszOldValue && pszOldValue[0] == '0') { \
			PerformCommand_##name((ConVar*)pVar, pszOldValue, flOldValue); \
				} \
		pVar->SetValue("0"); \
	}); \
	void PerformCommand_##name(ConVar* pVar, const char* pszOldValue, float flOldValue)
#endif

/*enum
{
	TEAM_AMERICANS = 2,
	TEAM_BRITISH,
	//BG2 - Tjoppen - NUM_TEAMS is useful
	NUM_TEAMS,	//!! must be last !!
};*/

inline char charForTeam(int iTeam) { return iTeam == TEAM_AMERICANS ? 'a' : 'b'; }

enum
{
	ROUND_DRAW = 1,
	MAP_DRAW,
	DEFAULT_DRAW,
	BRITISH_ROUND_WIN,
	AMERICAN_ROUND_WIN,
	BRITISH_MAP_WIN,
	AMERICAN_MAP_WIN,
	BRITISH_DEFAULT_WIN,
	AMERICAN_DEFAULT_WIN,
	CTF_DENY_CAPTURE,
	CTF_DENY_PICKUP,
	CTF_CAPTURE,
	CTF_PICKUP,
	CTF_DROP,
	CTF_RETURNED,
};


#ifdef CLIENT_DLL
	#define CHL2MPRules C_HL2MPRules
	#define CHL2MPGameRulesProxy C_HL2MPGameRulesProxy
#endif

class CHL2MPGameRulesProxy : public CGameRulesProxy
{
public:
	DECLARE_CLASS( CHL2MPGameRulesProxy, CGameRulesProxy );
	DECLARE_NETWORKCLASS();
};

class HL2MPViewVectors : public CViewVectors
{
public:
	HL2MPViewVectors( 
		Vector vView,
		Vector vHullMin,
		Vector vHullMax,
		Vector vDuckHullMin,
		Vector vDuckHullMax,
		Vector vDuckView,
		Vector vObsHullMin,
		Vector vObsHullMax,
		Vector vDeadViewHeight,
		Vector vCrouchTraceMin,
		Vector vCrouchTraceMax ) :
			CViewVectors( 
				vView,
				vHullMin,
				vHullMax,
				vDuckHullMin,
				vDuckHullMax,
				vDuckView,
				vObsHullMin,
				vObsHullMax,
				vDeadViewHeight )
	{
		m_vCrouchTraceMin = vCrouchTraceMin;
		m_vCrouchTraceMax = vCrouchTraceMax;
	}

	Vector m_vCrouchTraceMin;
	Vector m_vCrouchTraceMax;	
};

class CHL2MPRules : public CTeamplayRules
{
public:
	DECLARE_CLASS( CHL2MPRules, CTeamplayRules );

#ifdef CLIENT_DLL

	DECLARE_CLIENTCLASS_NOBASE(); // This makes datatables able to access our private vars.

#else

	DECLARE_SERVERCLASS_NOBASE(); // This makes datatables able to access our private vars.
#endif
	
	CHL2MPRules();

	void ResetMap(); //-BG2

	virtual ~CHL2MPRules();

	virtual void Precache( void );
	virtual bool ShouldCollide( int collisionGroup0, int collisionGroup1 );
	virtual bool ClientCommand( CBaseEntity *pEdict, const CCommand &args );

	virtual float FlWeaponRespawnTime( CBaseCombatWeapon *pWeapon );
	virtual float FlWeaponTryRespawn( CBaseCombatWeapon *pWeapon );
	virtual Vector VecWeaponRespawnSpot( CBaseCombatWeapon *pWeapon );
	virtual int WeaponShouldRespawn( CBaseCombatWeapon *pWeapon );
	virtual void Think( void );
	//virtual void PeriodicThink( void );
	virtual void CreateStandardEntities( void );
	virtual void ClientSettingsChanged( CBasePlayer *pPlayer );
	virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget );
	virtual void GoToIntermission( void );
	virtual void DeathNotice( CBasePlayer *pVictim, const CTakeDamageInfo &info );
	virtual const char *GetGameDescription( void );
	// derive this function if you mod uses encrypted weapon info files
	virtual const unsigned char *GetEncryptionKey( void ) { return (unsigned char *)"x9Ke0BY7"; }
	virtual const CViewVectors* GetViewVectors() const;
	const HL2MPViewVectors* GetHL2MPViewVectors() const;

	float	GetMapRemainingTime();
	void	CleanUpMap();
#ifndef CLIENT_DLL
	void	RestartGame();
	void	AutobalanceTeams();
	float	m_flNextAutobalanceCheck;
	virtual Vector VecItemRespawnSpot( CItem *pItem );
	virtual QAngle VecItemRespawnAngles( CItem *pItem );
	virtual float	FlItemRespawnTime( CItem *pItem );
	virtual bool	CanHavePlayerItem( CBasePlayer *pPlayer, CBaseCombatWeapon *pItem );
	virtual bool FShouldSwitchWeapon( CBasePlayer *pPlayer, CBaseCombatWeapon *pWeapon );

	void	AddLevelDesignerPlacedObject( CBaseEntity *pEntity );
	void	RemoveLevelDesignerPlacedObject( CBaseEntity *pEntity );
	void	ManageObjectRelocation( void );
	void    CheckChatForReadySignal( CHL2MP_Player *pPlayer, const char *chatmsg );
	const char *GetChatFormat( bool bTeamOnly, CBasePlayer *pPlayer );
	//BG2 - This should handle all the score settings after each round, and also fire any triggers and play win music. -HairyPotter
	void HandleScores(int iTeam, int iScore, int msg_type, bool bRestart, bool bCycleRound = true);
	virtual bool ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen);
#else
	//BG2 - Skillet - List of player ragdolls
	CUtlVector<C_HL2MPRagdoll*> m_hRagdollList;

#endif
	virtual void ClientDisconnected( edict_t *pClient );
	
	//void RestartGame( void );
	bool CheckGameOver( void );
	bool IsIntermission( void );
	int NumConnectedClients(); //includes bots
	void PlayerKilled( CBasePlayer *pVictim, const CTakeDamageInfo &info );
	
	
	//bool	IsTeamplay( void ) { return m_bTeamPlayEnabled;	}
	void	CheckAllPlayersReady( void );

	/**
	* Current round, when UsingTickets().
	* 1-based, so runs from 1 up to mp_rounds.
	*/
	CNetworkVar(int, m_iCurrentRound);

	//BG2 - Draco - Start
	CNetworkVar(float, m_fLastRespawnWave);
	CNetworkVar(float, m_fLastRoundRestart);
	float m_fNextGameReset;
	float m_fNextRoundReset;
	//float m_fEndRoundTime;	//use m_fLastRespawnWave + mp_respawntime.GetFloat() instead
	float m_fNextFlagUpdate;
	//BG2 - Draco - End
	//BG2 - Tjoppen - restart rounds a few seconds after the last person is killed. and other stuff
	float	m_flNextRoundRestart = FLT_MAX;
	bool	m_bIsRestartingRound = false;
	int		m_iTDMTeamThatWon, m_iAmericanDmg, m_iBritishDmg; //BG2 - HairyPotter
	bool	m_bHasDoneWinSong, m_bHasLoggedScores;
	float	m_fNextWinSong;
	//
	//CNetworkVar( int, m_iWaveTime );

	virtual bool IsConnectedUserInfoChangeAllowed( CBasePlayer *pPlayer );
	
	CNetworkVar( float, m_flGameStartTime );
private:
	
	CNetworkVar( bool, m_bTeamPlayEnabled );
	
	CUtlVector<EHANDLE> m_hRespawnableItemsAndWeapons;
	float m_tmNextPeriodicThink;
	float m_flRestartGameTime;
	bool m_bCompleteReset;
	bool m_bAwaitingReadyRestart;
	bool m_bHeardAllPlayersReady;

	/** BG2 Stuff -----
	* Counts the number of alive players in the specified team.
	* If UsingTickets(), then the number of remaining tickets is
	* added to the number.
	*/
public:
	int CountAlivePlayersAndTickets(int iTeamNumber);
private:
	/**
	* Swaps the specified player's team, making sure they
	* get the correct kit and so on.
	* If skipAlive is set, then the player won't be swapped if they're alive.
	*/
	void SwapPlayerTeam(CHL2MP_Player *pPlayer, bool skipAlive);

	//BG2 - Tjoppen - stuff in CHL2MPRules
public:
	/**
	* Swap team for all players on both teams
	* Note that no players are killed.
	*/
	void SwapTeams(void);

	void RestartRound(bool swapTeams, bool bSetLastRoundTime = true); //bSetLastRoundTime set to false is for fullcaps, so that fullcaps don't reset round clock
	void RespawnAll();
	void WinSong(int team, bool m_bWonMap = false);
	void RespawnWave();
	void ResetFlags();
	void CountHeldFlags(int &american_flags, int &british_flags, int &neutral_flags, int &foramericans, int &forbritish);
	void CheckFullcap();
	void CheckTicketDrain();

	//Call this for fullcap to be checked next frame
	void MarkFullcapCheckNextFrame() { m_fNextFlagUpdate = -FLT_MAX; }

	//

	/*
	LINEBATTLE STUFF
	*/
	/*bool			LbTeamCanSkirmish(int iTeam);
	void			LbGetOfficersOfTeam(int iTeam, CHL2MP_Player ** loadedFirst, CHL2MP_Player ** loadedSecond); //Can return nullptr. Dynamicly detects who the officers of the team are
	bool			LbTeamsWithinChargeDistance();
	#define			LB_MAX_ITERATED_TEAMMATES	5 //Used to save performance when comparing teams. Lower numbers improve performance but decrease reliability of result 
	bool			LbPlayerIsInLine(CHL2MP_Player * pPlayer);*/


	//BG2 - Tjoppen - tickets
	bool UsingTickets();	//returns whether we're currently using ticket based respawn

	static int getDefaultInfantryAmmo() { return AMMO_INFANTRY; }
	static int getDefaultLightInfantryAmmo() { return AMMO_LIGHT_INFANTRY; }
	static int getDefaultSkirmisherAmmo() { return AMMO_SKIRMISHER; }
	static int getDefaultSniperAmmo() { return AMMO_SNIPER; }
	static int getDefaultOfficerAmmo() { return AMMO_OFFICER; }
	static int getDefaultGrenadierAmmo() { return AMMO_GRENADIER; }

#ifndef CLIENT_DLL
	bool m_bChangelevelDone;
#endif
};

inline CHL2MPRules* HL2MPRules()
{
	return static_cast<CHL2MPRules*>(g_pGameRules);
}

#endif //HL2MP_GAMERULES_H
