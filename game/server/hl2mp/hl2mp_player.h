//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef HL2MP_PLAYER_H
#define HL2MP_PLAYER_H
#pragma once

class CHL2MP_Player;

#include "basemultiplayerplayer.h"
#include "hl2_playerlocaldata.h"
#include "hl2_player.h"
#include "../shared/bg3/bg3_player_shared.h"
#include "../shared/bg3/bg3_class.h"
#include "bg3/bg3_player_commands.h"
#include "simtimer.h"
#include "soundenvelope.h"
#include "hl2mp_player_shared.h"
#include "hl2mp_gamerules.h"
#include "utldict.h"
#include "bg3/Math/bg3_speedmod_list.h"
#include "bg3/Permissions/Permissions.h"

//=============================================================================
// >> HL2MP_Player
//=============================================================================
class CHL2MPPlayerStateInfo
{
public:
	HL2MPPlayerState m_iPlayerState;
	const char *m_pStateName;

	void (CHL2MP_Player::*pfnEnterState)();	// Init and deinit the state.
	void (CHL2MP_Player::*pfnLeaveState)();

	void (CHL2MP_Player::*pfnPreThink)();	// Do a PreThink() in this state.
};

class CHL2MP_Player : public CHL2_Player
{
	friend class CSpawnRoom;

	//BG2 - returns an abitrary free spawn point from the given list
	CBaseEntity* HandleSpawnList(const CUtlVector<CBaseEntity *>& spawns);

public:
	DECLARE_CLASS( CHL2MP_Player, CHL2_Player );

	CHL2MP_Player();
	~CHL2MP_Player( void );
	
	static CHL2MP_Player *CreatePlayer( const char *className, edict_t *ed )
	{
		CHL2MP_Player::s_PlayerEdict = ed;
		return (CHL2MP_Player*)CreateEntityByName( className );
	}

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	virtual void Precache( void );
	virtual void Spawn( void );
	virtual void PostThink( void );
	virtual void PreThink( void );
	virtual void PlayerDeathThink( void );
	virtual void SetAnimation( PLAYER_ANIM playerAnim );
	virtual bool HandleCommand_JoinTeam( int team );
	static CUtlDict<PlayerCommandFunc> s_mPlayerCommands;
	virtual bool ClientCommand( const CCommand &args );
	virtual void CreateViewModel( int viewmodelindex = 0 );
	virtual bool BecomeRagdollOnClient( const Vector &force );
	virtual void Event_Killed( const CTakeDamageInfo &info );
	virtual int	 OnTakeDamage( const CTakeDamageInfo &inputInfo );
	virtual bool WantsLagCompensationOnEntity( const CBasePlayer *pPlayer, const CUserCmd *pCmd, const CBitVec<MAX_EDICTS> *pEntityTransmitBits ) const;
	virtual void FireBullets ( const FireBulletsInfo_t &info );
	virtual void TraceAttack(const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator);
	virtual bool Weapon_Switch( CBaseCombatWeapon *pWeapon, int viewmodelindex = 0);
	virtual bool BumpWeapon( CBaseCombatWeapon *pWeapon );
private:
	virtual void ChangeTeam(int iTeam);
	//BG2 - Like ChangeTeam() except with the ability to manually set whether the player should be killed or not.
	void		 ChangeTeam(int iTeam, bool bKill);
public:
	void		 UpdatePlayerClass(void); //BG3- updates the player class reference to match m_iClass and team number
	//
	virtual void PickupObject ( CBaseEntity *pObject, bool bLimitMassAndSize );
	virtual void PlayStepSound( Vector &vecOrigin, surfacedata_t *psurface, float fvol, bool force );
	virtual void Weapon_Drop( CBaseCombatWeapon *pWeapon, const Vector *pvecTarget = NULL, const Vector *pVelocity = NULL );
	virtual void UpdateOnRemove( void );
	virtual void DeathSound( const CTakeDamageInfo &info );
	virtual CBaseEntity* EntSelectSpawnPoint( void );
	//BG2 - Tjoppen - virtuals in CHL2MP_Player
			bool MayRespawnOnTeamChange(int previousTeam); //BG3 - specialized this function to team changes - Awesome
			void HandleSpeedChanges(void);
	//
		
	int FlashlightIsOn( void );
	//void FlashlightTurnOn( void );
	//void FlashlightTurnOff( void );
	void	PrecacheFootStepSounds( void );
	bool	ValidatePlayerModel( const char *pModel );

	QAngle GetAnimEyeAngles( void ) { return m_angEyeAngles.Get(); }

	Vector GetAttackSpread( CBaseCombatWeapon *pWeapon, CBaseEntity *pTarget = NULL );

	void CheatImpulseCommands( int iImpulse );
	void CreateRagdollEntity( void );
	void GiveAllItems( void );
	void GiveDefaultItems( void );
	void SetDefaultAmmoFull(bool bPlaySound = true);
	bool HasDefaultAmmoFull(void);
	bool IsAiming() const { return GetActiveWeapon() && GetActiveWeapon()->m_bIsIronsighted; }
	bool HasLoadedWeapon() const;
	void NoteWeaponFired( void );
	CBaseCombatWeapon* Weapon_FindMeleeWeapon() const;
	void UpdateToMatchClassWeaponAmmoUniform(); //master class/kit updater

	void ResetAnimation( void );
	void SetPlayerModel( void );
	void SetPlayerTeamModel( void );
	Activity TranslateTeamActivity( Activity ActToTranslate );
	
	void  PickDefaultSpawnTeam( void );
	void  SetupPlayerSoundsByModel( const char *pModelName );
	const char *GetPlayerModelSoundPrefix( void );
	int	  GetPlayerModelType( void ) { return m_iPlayerSoundType;	}
	
	//void  DetonateTripmines( void ); //BG2

	void Reset();

	bool IsReady();
	void SetReady( bool bReady );

	void CheckChatText( char *p, int bufsize );

	bool IsBetaTester() const;

	void State_Transition( HL2MPPlayerState newState );
	void State_Enter( HL2MPPlayerState newState );
	void State_Leave();
	void State_PreThink();
	CHL2MPPlayerStateInfo *State_LookupInfo( HL2MPPlayerState state );

	void State_Enter_ACTIVE();
	void State_PreThink_ACTIVE();
	void State_Enter_OBSERVER_MODE();
	void State_PreThink_OBSERVER_MODE();

	virtual bool StartObserverMode( int mode );
	virtual void StopObserverMode( void );

	Vector m_vecTotalBulletForce;	//Accumulator for bullet force in a single frame

private:
	// Tracks our ragdoll entity.
	CNetworkHandle( CBaseEntity, m_hRagdoll );	// networked entity handle 
public:
	void RemoveRagdolls();


	virtual bool	CanHearAndReadChatFrom( CBasePlayer *pPlayer );

	//BG2 - Tjoppen - GetClass()
	int	GetClass(void) const { return m_iClass; }
	int	GetNextClass(void) const { return m_iNextClass; }
	void SetNextClass(int iNextClass) { m_iNextClass = iNextClass; }

	//inline bool IsBritish(void) { return GetTeamNumber() == TEAM_BRITISH; }

		
private:

	CNetworkQAngle( m_angEyeAngles );
	CPlayerAnimState   m_PlayerAnimState;

	//BG2 - Tjoppen - vars in hl2mp_player
	int m_iLastWeaponFireUsercmd;
	int m_iModelType;
	CNetworkVar( int, m_iSpawnInterpCounter );
	CNetworkVar( int, m_iPlayerSoundType );


public:
	void		PlayermodelTeamClass();
	int			GetAppropriateSkin(); //Looks at ourself to see what player model skin we should have
	void		RemoveSelfFromFlags(void);	//BG2 - Tjoppen - do this whenever we die, change team or disconnect or anything similar
	//int			GetLimitTeamClass(int iTeam, int iClass);
	bool		AttemptJoin(int iTeam, int iClass);
	void		ForceJoin(const CPlayerClass* pClass, int iTeam, int iClass);
	void		CheckQuickRespawn();
	//bool		PlayerMayJoinTeam(int iTeam) const;
	const char* GetHitgroupPainSound(int hitgroup, int team);
	void		HandleVoicecomm(int comm);

	int GetCurrentAmmoKit(void) const { return m_iCurrentAmmoKit; }

	void VerifyKitAmmoUniform();
	int m_iGunKit,
		m_iAmmoKit,
		m_iClassSkin;

	void DrainStamina(int iAmount);
	//BG2 - Tjoppen - made m_iStamina a network cvar
	CNetworkVar(int, m_iStamina);

	const CPlayerClass* m_pCurClass;
	inline const CPlayerClass*	GetPlayerClass() const { return m_pCurClass; }
	inline const CPlayerClass*	GetNextPlayerClass() const { return CPlayerClass::fromNums(GetTeamNumber(), m_iNextClass); }
private:
	CNetworkVar(int, m_iClass);
	CNetworkVar(int, m_iCurrentAmmoKit);	//BG2 - Tjoppen - we need to copy m_iAmmoKit when spawned so players can't change current load by typing "kit ..."
	CNetworkVar(int, m_iSpeedModifier);
	CNetworkVar(int, m_iNextClass); //BG3 - Awesome - clients need to know about other player's next classes so that two players don't claim the same class.
									//				BG2 got away with this because it was only a rare event, BG3 won't

public:
	CNetworkVar(int, m_iCurrentRallies); //BG3 - Awesome - bitfield of rallies which are currently affecting this player - controlled by the server
	inline int RallyGetCurrentRallies() { return m_iCurrentRallies; }
	float	m_flEndRallyTime;

	float	m_flNextAmmoRefill;
private:
	//int		m_iClass;					//BG2 - Tjoppen - class system
	//int		m_iNextClass;					//BG2 - Tjoppen - which class will we become on our next respawn?
	float	m_flNextVoicecomm,				//BG2 - Tjoppen - voice comms
			m_flNextGlobalVoicecomm;		//BG2 - Tjoppen - only battlecries for now
	float	m_fNextStamRegen;				//BG2 - Draco - stamina regen timer
	float	m_flNextRagdollRemoval;
	

	//BG2 - Tjoppen - tickets. sometimes we don't want to remove tickets on spawn, such as when first joining a team
	bool	m_bDontRemoveTicket;	

	//BG3 - spawn room allows for instantaneous spawn
	bool	m_bInSpawnRoom;

	//return the player's speed based on whether which class we are, which weapon kit we're using etc.
	int		GetCurrentSpeed(void) const;

public:
	void	OnRallyEffectEnable(); //visual effects, not functionality. Exact behavior depends on m_iCurrentRallies
	void	OnRallyEffectDisable();

	//used for temporary speed modifiers (carrying flags and such)
	//void	SetSpeedModifier(int iSpeedModifier);
	//BG3 - Awesome - added more robust speed mod manager
private:
	CSpeedModList m_aSpeedMods;
public:
	void AddSpeedModifier(int8 mod, ESpeedModID id);
	void RemoveSpeedModifier(ESpeedModID id);
	void ClearSpeedModifier();

	//used for a lot of things, mostly linebattle rule checking and the like
	CHL2MP_Player * GetNearestPlayerOfTeam(int iTeam, float& loadedDistance);

	CBaseEntity	*m_pIntermission;	//follow that info_intermission!


	HL2MPPlayerState m_iPlayerState;
	CHL2MPPlayerStateInfo *m_pCurStateInfo;

	bool ShouldRunRateLimitedCommand( const CCommand &args );

	// This lets us rate limit the commands the players can execute so they don't overflow things like reliable buffers.
	CUtlDict<float,int>	m_RateLimitLastCommandTimes;

    bool m_bEnterObserver;
	bool m_bReady;

	// BG2 - VisualMelon - When true, cancel the next "is going to fight as" message
	bool m_bNoJoinMessage;

	//BG3 - permissions
	Permissions* m_pPermissions;
};

//BG2 - Tjoppen - ammo kit definitions
#define AMMO_KIT_BALL		0
#define AMMO_KIT_BUCKSHOT	1
//BG2 - Tjoppen - Note: We can save one bit on m_iCurrentAmmoKit if we restrict ourselves to only two ammy types for now
#define AMMO_KIT_RESERVED1	2
#define AMMO_KIT_RESERVED2	3
//
inline CHL2MP_Player *ToHL2MPPlayer( CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return NULL;

	return dynamic_cast<CHL2MP_Player*>( pEntity );
}

inline bool verifyBotPermissions(const char* pszFunctionName) {
	if (UTIL_IsCommandIssuedByServerAdmin())
		return true;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	if (pPlayer) {
		return verifyPermissions(pPlayer, pPlayer->m_pPermissions->m_bBotManage, pszFunctionName);
	}
	return false;
}

inline bool verifyMapModePermissions(const char* pszFunctionName) {
	if (UTIL_IsCommandIssuedByServerAdmin())
		return true;

	CHL2MP_Player* pPlayer = (CHL2MP_Player*)(UTIL_GetCommandClient());
	if (pPlayer) {
		return verifyPermissions(pPlayer, pPlayer->m_pPermissions->m_bBotManage, pszFunctionName);
	}
	return false;
}

#endif //HL2MP_PLAYER_H
