/*
	The Battle Grounds 2 - A Source modification
	Copyright (C) 2005, The Battle Grounds 2 Team and Contributors

	The Battle Grounds 2 free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The Battle Grounds 2 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

	Contact information:
		Tomas "Tjoppen" Härdin		mail, in reverse: se . gamedev @ tjoppen

	You may also contact the (future) team via the Battle Grounds website and/or forum at:
		www.bgmod.com

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
#include "npcevent.h"
#include "in_buttons.h"
#include "../shared/bg3/bg3_player_shared.h"
#include "../shared/bg3/bg3_weapon_def.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
#else
	#include "hl2mp_player.h"
	#include "te_effect_dispatch.h"
#endif

#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "weapon_hl2mpbasebasebludgeon.h"

#define BLUDGEON_HULL_DIM		16

static const Vector g_bludgeonMins(-BLUDGEON_HULL_DIM,-BLUDGEON_HULL_DIM,-BLUDGEON_HULL_DIM);
static const Vector g_bludgeonMaxs(BLUDGEON_HULL_DIM,BLUDGEON_HULL_DIM,BLUDGEON_HULL_DIM);

#ifdef CLIENT_DLL
#define CBaseBG2Weapon C_BaseBG2Weapon
#endif

class CBaseBG2Weapon : public CBaseHL2MPCombatWeapon
{
	DECLARE_CLASS( CBaseBG2Weapon, CBaseHL2MPCombatWeapon );
public:
	

	enum
	{
		ATTACK_NONE = -1,		//this enum used for separating different attacks and stuff
		ATTACK_PRIMARY = 0,
		ATTACK_SECONDARY = 1,
	};

	CBaseBG2Weapon( void );

	void	PrimaryAttack( void );
	void	SecondaryAttack( void );
	void	DoAttack( int iAttack );	//does stuff common to both attacks

	//BG2 - Tjoppen - this one from base_weapon.cpp
	//works out the accuracy cone, so we can use cones of varying sizes
	Vector Cone(float flConeSize)
	{
		//sin(1/2) - see VECTOR_CONE_1DEGREES
		const double flBaseSize = 0.008725;
		float flCone = (float)flBaseSize * flConeSize;
		
		return Vector(flCone, flCone, flCone);
	}

	int		GetAttackType( int iAttack )
	{
		return iAttack != ATTACK_NONE ? Def()->m_Attackinfos[iAttack].m_iAttacktype : ATTACK_NONE;
	}
#ifndef CLIENT_DLL
	int		GetLastAttackType()
	{
		return m_iLastAttackType;
	}
#endif

	float	GetRange( int iAttack )
	{
		return iAttack != ATTACK_NONE ? Def()->m_Attackinfos[iAttack].m_flRange : 0;
	}

	float	GetAttackRate( int iAttack )
	{
		if( iAttack == ATTACK_NONE )
			return 0;

		if( Def()->m_Attackinfos[iAttack].m_flAttackrate < 0 )
			return SequenceDuration();

		return Def()->m_Attackinfos[iAttack].m_flAttackrate;
	}

	float	GetRecoil( int iAttack )
	{
		if( iAttack == ATTACK_NONE )
			return 0;

		return Def()->m_Attackinfos[iAttack].m_flRecoil;
	}

	int		GetDamage( int iAttack )
	{
		if( iAttack == ATTACK_NONE )
			return 0;

		int damage = Def()->m_Attackinfos[iAttack].m_iDamage;

		return damage;
	}

	//static float GetDamageScale(CHL2MP_Player * pAttacker, CHL2MP_Player * pHitPlayer);

	float m_flPreviousAccuracy = 4.f;
	float	GetAccuracy(int iAttack);

	inline bool ShouldTightenVerticalAccuracy() {
		extern ConVar sv_perfectaim;
		extern ConVar sv_perfectaim_training;
		extern ConVar mp_respawnstyle;
		extern ConVar lb_tighten_vertical_accuracy;
		return (m_bIsIronsighted && mp_respawnstyle.GetInt() == 4 && !sv_perfectaim.GetBool() && lb_tighten_vertical_accuracy.GetBool() && !sv_perfectaim_training.GetBool());
	}

	float GetVerticalAccuracyScale() {
		if (!ShouldTightenVerticalAccuracy()) {
			return 1.0f;
		}

		return Def()->m_flVerticalAccuracyScale;
	}

	float GetHitDropProbability();

	Activity	GetActivity( int iAttack )
	{
		if( iAttack == ATTACK_NONE )
			return (Activity)0;

		if (m_iClip1 == 0 && Def()->m_Attackinfos[iAttack].m_iAttacktype == ATTACKTYPE_STAB)
			return Def()->m_Attackinfos[iAttack].m_iAttackActivityEmpty;

		return Def()->m_Attackinfos[iAttack].m_iAttackActivity;
	}

	float GetRetraceDuration( int iAttack )
	{
		extern ConVar sv_bayonet_retrace_duration;

		if (iAttack == ATTACK_NONE)
			return 0;
		else if (Def()->m_Attackinfos[iAttack].m_iAttacktype == ATTACKTYPE_STAB)
			return sv_bayonet_retrace_duration.GetFloat();
		else
			return Def()->m_Attackinfos[iAttack].m_flRetraceDuration;
	}

	int GetStaminaDrain( int iAttack )
	{
		return iAttack == ATTACK_NONE ? 0 : Def()->m_Attackinfos[iAttack].m_iStaminaDrain;
	}

	float GetCosAngleTolerance( void )
	{
		extern ConVar sv_bayonet_angle_tolerance;

		//return tolerance of last attack
		if (m_iLastAttack == ATTACK_NONE)
			return 1.0f;
		else if (Def()->m_Attackinfos[m_iLastAttack].m_iAttacktype == ATTACKTYPE_STAB)
			return cosf(sv_bayonet_angle_tolerance.GetFloat() * M_PI / 180.f);
		else
			return Def()->m_Attackinfos[m_iLastAttack].m_flCosAngleTolerance;
	}

	//minor hack to get bayonet deathnotice
	bool m_bLastAttackStab;
	char *GetDeathNoticeName( void )
	{
		if( m_bLastAttackStab && Def()->m_pBayonetDeathNotice )
			return (char*)Def()->m_pBayonetDeathNotice;
		else
			return (char*)GetClassname();
	}

	float	m_flLastRecoil,					//for multiple recoil supression on client
			m_flStopAttemptingSwing;		//this is for melee attacks that go on for several frames
											// in other words, the game will attempt to do the tracelines for say 100 ms
	float	m_flStartDemotingHeadhits;		//the point in time at which retraced head hits will be demoted to chest hits
	int		m_iLastAttack;					//for reattempting swings
	Vector	m_vLastForward;					//last forward eye vector

	//for delayed fire
	bool	m_bShouldSampleForward;
	float	m_flNextSampleForward;
	bool	m_bShouldFireDelayed;
	float	m_flNextDelayedFire;

	//BG3 - Awesome - moved this to CBaseCombatWeapon
	//void		UpdateBodyGroups();
	void		Equip( CBaseCombatCharacter *pOwner );
	void		Think( void );
	void		ItemPostFrame( void );
	void		Fire( int iAttack );
	void		FireBullets( int iAttack );
	void		Swing( int iAttack, bool bIsFirstAttempt );

	void		Hit( trace_t &traceHit, int iAttack );
	bool		ImpactWater( const Vector &start, const Vector &end );
	void		ImpactEffect( trace_t &traceHit );

	void		Drop( const Vector &vecVelocity );

	virtual bool		Deploy( void );
	virtual void		WeaponIdle( void );
	virtual Activity	GetDrawActivity( void );

	bool		Holster( CBaseCombatWeapon *pSwitchingTo );
	bool		Reload( void );

	//BG2 - For the holster delay. -HairyPotter
	float m_fNextHolster;
	//

	float GetCurrentAmmoSpread( void )
	{
		CHL2MP_Player *pPlayer = ToHL2MPPlayer( GetOwner() );

		if ( !pPlayer || Def()->m_iNumShot <= 0 || (pPlayer->GetCurrentAmmoKit() != AMMO_KIT_BUCKSHOT && !Def()->m_bShotOnly))
			return 0;

		return Def()->m_flShotSpread;
	}

	static float GetGrenadeDamage();

#ifndef CLIENT_DLL
	int		m_iLastAttackType;	//used for HitVerif stuff
#endif

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

/*#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif*/

private:
	CBaseBG2Weapon( const CBaseBG2Weapon & );
};


//these things must go before the macro. appropriately named of course(CWeapon##name C_Weapon##name
//the reason for this is that #define's can't be inside other #define's
//#ifdef CLIENT_DLL
//#define CWeaponRevolutionnaire C_WeaponRevolutionnaire
//#endif
#define DECLARE_BG2_WEAPON( name )								\
class CWeapon##name : public CBaseBG2Weapon {					\
	DECLARE_CLASS( CWeapon##name, CBaseBG2Weapon );				\
public:															\
	CWeapon##name( void );										\
	DECLARE_NETWORKCLASS();										\
	DECLARE_PREDICTABLE();										\
	DECLARE_ACTTABLE();											\
private:														\
	CWeapon##name( const CWeapon##name & );						\
};																\
IMPLEMENT_NETWORKCLASS_ALIASED( Weapon##name, DT_Weapon##name )	\
BEGIN_NETWORK_TABLE( CWeapon##name, DT_Weapon##name )			\
END_NETWORK_TABLE()												\
BEGIN_PREDICTION_DATA( CWeapon##name )							\
END_PREDICTION_DATA()											\
LINK_ENTITY_TO_CLASS( weapon_##name, CWeapon##name );			\
PRECACHE_WEAPON_REGISTER( weapon_##name );						\
DEC_BG3_WEAPON_DEF(weapon_##name);								\
CWeapon##name::CWeapon##name( void ) : CBaseBG2Weapon()			\
{ m_pWeaponDef = &g_Def_weapon_##name; }						\
DEF_BG3_WEAPON_DEF(weapon_##name)
	

//constructor follows

//acttable def for bayonet firearms, only muskets so far. could a rifle use a bayonet? it's just a name anyway
#define MUSKET_ACTTABLE( name )\
acttable_t CWeapon##name::m_acttable[] = \
{\
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_SHOTGUN,					false },\
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_SHOTGUN,					false },\
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_SHOTGUN,			false },\
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_SHOTGUN,			false },\
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SHOTGUN,	false },\
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_HL2MP_GESTURE_RELOAD_MUSKET,		false },\
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_SHOTGUN,					false },\
	{ ACT_RANGE_ATTACK1,				ACT_RANGE_ATTACK_SHOTGUN,				false },\
	{ ACT_RANGE_ATTACK2,				ACT_HL2MP_GESTURE_ATTACK_MELEE_BAYONET,					false },\
};\
IMPLEMENT_ACTTABLE( CWeapon##name );

//acttable def for rifles, without bayo
#define RIFLE_ACTTABLE( name )\
acttable_t CWeapon##name::m_acttable[] = \
{\
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_SHOTGUN,					false },\
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_SHOTGUN,					false },\
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_SHOTGUN,			false },\
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_SHOTGUN,			false },\
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SHOTGUN,	false },\
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_HL2MP_GESTURE_RELOAD_MUSKET, /*SHOTGUN*/		false },\
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_SHOTGUN,					false },\
	{ ACT_RANGE_ATTACK1,				ACT_RANGE_ATTACK_SHOTGUN,				false },\
	{ ACT_RANGE_ATTACK2,				ACT_HL2MP_GESTURE_RANGE_ATTACK_AR2,		false },\
};\
IMPLEMENT_ACTTABLE( CWeapon##name );

//acttable def for pistols
#define PISTOL_ACTTABLE( name )\
acttable_t CWeapon##name::m_acttable[] = \
{\
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_PISTOL,					false },\
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_PISTOL,					false },\
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_PISTOL,			false },\
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_PISTOL,			false },\
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,	false },\
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_BG2_GESTURE_RELOAD_PISTOL,			false },\
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_PISTOL,					false },\
	{ ACT_RANGE_ATTACK1,				ACT_RANGE_ATTACK_PISTOL,				false },\
	{ ACT_RANGE_ATTACK2,				ACT_VM_SECONDARYATTACK,					false },\
};\
IMPLEMENT_ACTTABLE( CWeapon##name );

//acttable def for slash weapons
#define MELEE_ACTTABLE( name )\
acttable_t CWeapon##name::m_acttable[] =\
{\
	{ ACT_RANGE_ATTACK1,				ACT_RANGE_ATTACK_SLAM,					true },\
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_MELEE,					false },\
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_MELEE,					false },\
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_MELEE,			false },\
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_MELEE,			false },\
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_MELEE,	false },\
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_HL2MP_GESTURE_RELOAD_MELEE,			false },\
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_MELEE,					false },\
	{ ACT_RANGE_ATTACK2,				ACT_HL2MP_GESTURE_RANGE_ATTACK_MELEE,	false },\
};\
IMPLEMENT_ACTTABLE( CWeapon##name );

//actable def for carbine
#define CARBINE_ACTTABLE( name )\
acttable_t CWeapon##name::m_acttable[] = \
{\
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_SHOTGUN,					false },\
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_SHOTGUN,					false },\
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_SHOTGUN,			false },\
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_SHOTGUN,			false },\
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SHOTGUN,	false },\
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_BG2_GESTURE_RELOAD_CARBINE,			false },\
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_SHOTGUN,					false },\
	{ ACT_RANGE_ATTACK1,				ACT_RANGE_ATTACK_SHOTGUN,				false },\
	{ ACT_RANGE_ATTACK2,				ACT_HL2MP_GESTURE_ATTACK_MELEE_BAYONET,					false },\
};\
IMPLEMENT_ACTTABLE( CWeapon##name );
/* ACT_HL2MP_GESTURE_RANGE_ATTACK_MELEE */