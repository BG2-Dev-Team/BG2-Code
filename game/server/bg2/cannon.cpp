//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ====
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "cannon.h"
#include "cannonball.h"
#include "Sprite.h"
#include "basecombatweapon.h"
#include "shot_manipulator.h"
#include "eventqueue.h"
#include "gamerules.h"
#include "ammodef.h"
#include "in_buttons.h"
#include "soundent.h"
#include "ndebugoverlay.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"
#include "physics_cannister.h"
#include "shake.h"
#include "player.h"
#include "entitylist.h"
#include "IEffects.h"
#include "ai_basenpc.h"
#include "effects.h"
#include "soundenvelope.h"
#include "effect_dispatch_data.h"
#include "te_effect_dispatch.h"
#include "props.h"

#ifdef HL2_DLL
#include "hl2_player.h"
#endif //HL2_DLL

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern Vector PointOnLineNearestPoint(const Vector& vStartPos, const Vector& vEndPos, const Vector& vPoint);
ConVar testx( "testx", "10", 0 );
ConVar testy( "testy", "0", 0 );
ConVar testz( "testz", "0", 0 );

#define CANNON_MODEL "models/bg2_props/bg_cannon/cannon_new.mdl"

BEGIN_DATADESC( CCannon )
	DEFINE_KEYFIELD( m_yawRate, FIELD_FLOAT, "yawrate" ),
	DEFINE_KEYFIELD( m_yawRange, FIELD_FLOAT, "yawrange" ),
	DEFINE_KEYFIELD( m_yawTolerance, FIELD_FLOAT, "yawtolerance" ),
	DEFINE_KEYFIELD( m_pitchRate, FIELD_FLOAT, "pitchrate" ),
	DEFINE_KEYFIELD( m_pitchRange, FIELD_FLOAT, "pitchrange" ),
	DEFINE_KEYFIELD( m_pitchTolerance, FIELD_FLOAT, "pitchtolerance" ),
	DEFINE_KEYFIELD( m_fireRate, FIELD_FLOAT, "firerate" ),
	DEFINE_FIELD( m_fireTime, FIELD_TIME ),
	DEFINE_KEYFIELD( m_minRange, FIELD_FLOAT, "minRange" ),
	DEFINE_KEYFIELD( m_maxRange, FIELD_FLOAT, "maxRange" ),
	DEFINE_FIELD( m_flMinRange2, FIELD_FLOAT ),
	DEFINE_FIELD( m_flMaxRange2, FIELD_FLOAT ),
	DEFINE_KEYFIELD( m_iAmmoCount, FIELD_INTEGER, "ammo_count" ),
	DEFINE_KEYFIELD( m_spriteScale, FIELD_FLOAT, "spritescale" ),
	DEFINE_KEYFIELD( m_iszSpriteSmoke, FIELD_STRING, "spritesmoke" ),
	DEFINE_KEYFIELD( m_iszSpriteFlash, FIELD_STRING, "spriteflash" ),
	DEFINE_KEYFIELD( m_bulletType, FIELD_INTEGER, "bullet" ),
	DEFINE_FIELD( m_nBulletCount, FIELD_INTEGER ),
	DEFINE_KEYFIELD( m_spread, FIELD_INTEGER, "firespread" ),
	DEFINE_KEYFIELD( m_iBulletDamage, FIELD_INTEGER, "bullet_damage" ),
	DEFINE_KEYFIELD( m_iszMaster, FIELD_STRING, "master" ),

	DEFINE_KEYFIELD( m_soundStartRotate, FIELD_SOUNDNAME, "rotatestartsound" ),
	DEFINE_KEYFIELD( m_soundStopRotate, FIELD_SOUNDNAME, "rotatestopsound" ),
	DEFINE_KEYFIELD( m_soundLoopRotate, FIELD_SOUNDNAME, "rotatesound" ),
	DEFINE_KEYFIELD( m_flPlayerGracePeriod, FIELD_FLOAT, "playergraceperiod" ),
	DEFINE_KEYFIELD( m_flIgnoreGraceUpto, FIELD_FLOAT, "ignoregraceupto" ),
	DEFINE_KEYFIELD( m_flPlayerLockTimeBeforeFire, FIELD_FLOAT, "playerlocktimebeforefire" ),
	DEFINE_FIELD( m_flLastSawNonPlayer, FIELD_TIME ),

	DEFINE_FIELD( m_yawCenter, FIELD_FLOAT ),
	DEFINE_FIELD( m_yawCenterWorld, FIELD_FLOAT ),
	DEFINE_FIELD( m_pitchCenter, FIELD_FLOAT ),
	DEFINE_FIELD( m_pitchCenterWorld, FIELD_FLOAT ),
	DEFINE_FIELD( m_fireLast, FIELD_TIME ),
	DEFINE_FIELD( m_lastSightTime, FIELD_TIME ),
	DEFINE_FIELD( m_barrelPos, FIELD_VECTOR ),
	DEFINE_FIELD( m_sightOrigin, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_hFuncTankTarget, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hController, FIELD_EHANDLE ),
	DEFINE_FIELD( m_vecControllerUsePos, FIELD_VECTOR ),
	DEFINE_FIELD( m_flNextAttack, FIELD_TIME ),
	DEFINE_FIELD( m_targetEntityName, FIELD_STRING ),
	DEFINE_FIELD( m_hTarget, FIELD_EHANDLE ),
	DEFINE_FIELD( m_vTargetPosition, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_persist2burst, FIELD_FLOAT),
	//DEFINE_FIELD( m_parentMatrix, FIELD_MATRIX ), // DON'T SAVE
	DEFINE_KEYFIELD( m_iszControlVolume, FIELD_STRING, "control_volume" ),
	DEFINE_FIELD( m_flNextControllerSearch, FIELD_TIME ),
	DEFINE_FIELD( m_bReadyToFire, FIELD_BOOLEAN ),

	// Used for when the gun is attached to another entity
//	DEFINE_FIELD( m_nBarrelAttachment, FIELD_INTEGER ),

	DEFINE_KEYFIELD( m_iEffectHandling, FIELD_INTEGER, "effecthandling" ),

	// Inputs
	DEFINE_INPUTFUNC( FIELD_VOID, "Activate", InputActivate ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Deactivate", InputDeactivate ),
	DEFINE_INPUTFUNC( FIELD_INTEGER, "SetDamage", InputSetDamage ),
	DEFINE_INPUTFUNC( FIELD_VECTOR, "SetTargetPosition", InputSetTargetPosition ),
	DEFINE_INPUTFUNC( FIELD_VECTOR, "SetTargetDir", InputSetTargetDir ),
	DEFINE_INPUTFUNC( FIELD_STRING, "SetTargetEntityName", InputSetTargetEntityName ),
	DEFINE_INPUTFUNC( FIELD_EHANDLE, "SetTargetEntity", InputSetTargetEntity ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ClearTargetEntity", InputClearTargetEntity ),

	// Outputs
	DEFINE_OUTPUT(m_OnFire,					"OnFire"),
	DEFINE_OUTPUT(m_OnLoseTarget,			"OnLoseTarget"),
	DEFINE_OUTPUT(m_OnAquireTarget,			"OnAquireTarget"),
	DEFINE_OUTPUT(m_OnAmmoDepleted,			"OnAmmoDepleted"),
	DEFINE_OUTPUT(m_OnGotPlayerController,	"OnGotPlayerController"),
	DEFINE_OUTPUT(m_OnLostPlayerController,	"OnLostPlayerController"),
	DEFINE_OUTPUT(m_OnReadyToFire,			"OnReadyToFire"),
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CCannon::CCannon()
{
	m_nBulletCount = 0;

	m_flNextControllerSearch = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CCannon::~CCannon( void )
{
	if ( m_soundLoopRotate != NULL_STRING && ( m_spawnflags & SF_TANK_SOUNDON ) )
	{
		StopSound( entindex(), CHAN_STATIC, STRING(m_soundLoopRotate) );
	}
}


//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
inline bool CCannon::CanFire( void )
{ 
	return true;
}

//------------------------------------------------------------------------------
// Purpose: Input handler for activating the tank.
//------------------------------------------------------------------------------
void CCannon::InputActivate( inputdata_t &inputdata )
{	
	TankActivate();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCannon::TankActivate( void )
{
	m_spawnflags |= SF_TANK_ACTIVE; 
	SetNextThink( gpGlobals->curtime + 0.1f ); 
	m_fireLast = gpGlobals->curtime;
}

//-----------------------------------------------------------------------------
// Purpose: Input handler for deactivating the tank.
//-----------------------------------------------------------------------------
void CCannon::InputDeactivate( inputdata_t &inputdata )
{
	TankDeactivate();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCannon::TankDeactivate( void )
{
	m_spawnflags &= ~SF_TANK_ACTIVE; 
	m_fireLast = 0; 
	StopRotSound();
}

//-----------------------------------------------------------------------------
// Purpose: Input handler for changing the name of the tank's target entity.
//-----------------------------------------------------------------------------
void CCannon::InputSetTargetEntityName( inputdata_t &inputdata )
{
	m_targetEntityName = inputdata.value.StringID();
	m_hTarget = FindTarget( m_targetEntityName, inputdata.pActivator );

	// No longer aim at target position if have one
	m_spawnflags &= ~SF_TANK_AIM_AT_POS; 
}

//-----------------------------------------------------------------------------
// Purpose: Input handler for setting a new target entity by ehandle.
//-----------------------------------------------------------------------------
void CCannon::InputSetTargetEntity( inputdata_t &inputdata )
{
	if ( inputdata.value.Entity() != NULL )
	{
		m_targetEntityName = inputdata.value.Entity()->GetEntityName();
	}
	else
	{
		m_targetEntityName = NULL_STRING;
	}
	m_hTarget = inputdata.value.Entity();

	// No longer aim at target position if have one
	m_spawnflags &= ~SF_TANK_AIM_AT_POS; 
}

//-----------------------------------------------------------------------------
// Purpose: Input handler for clearing the tank's target entity
//-----------------------------------------------------------------------------
void CCannon::InputClearTargetEntity( inputdata_t &inputdata )
{
	m_targetEntityName = NULL_STRING;
	m_hTarget = NULL;

	// No longer aim at target position if have one
	m_spawnflags &= ~SF_TANK_AIM_AT_POS; 
}

//-----------------------------------------------------------------------------
// Purpose: Input handler for setting the damage
//-----------------------------------------------------------------------------
void CCannon::InputSetDamage( inputdata_t &inputdata )
{
	m_iBulletDamage = inputdata.value.Int();
}

//-----------------------------------------------------------------------------
// Purpose: Input handler for setting the target as a position.
//-----------------------------------------------------------------------------
void CCannon::InputSetTargetPosition( inputdata_t &inputdata )
{
	m_spawnflags |= SF_TANK_AIM_AT_POS; 
	m_hTarget = NULL;

	inputdata.value.Vector3D( m_vTargetPosition );
}

//-----------------------------------------------------------------------------
// Purpose: Input handler for setting the target as a position.
//-----------------------------------------------------------------------------
void CCannon::InputSetTargetDir( inputdata_t &inputdata )
{
	m_spawnflags |= SF_TANK_AIM_AT_POS; 
	m_hTarget = NULL;

	Vector vecTargetDir;
	inputdata.value.Vector3D( vecTargetDir );
	m_vTargetPosition = GetAbsOrigin() + m_barrelPos.LengthSqr() * vecTargetDir;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : pAttacker - 
//			flDamage - 
//			vecDir - 
//			ptr - 
//			bitsDamageType - 
//-----------------------------------------------------------------------------
void CCannon::TraceAttack( CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, trace_t *ptr, int bitsDamageType)
{
	if (m_spawnflags & SF_TANK_DAMAGE_KICK)
	{
		// Deflect the func_tank
		// Only adjust yaw for now
		if (pAttacker)
		{
			Vector vFromAttacker = (pAttacker->EyePosition()-GetAbsOrigin());
			vFromAttacker.z = 0;
			VectorNormalize(vFromAttacker);

			Vector vFromAttacker2 = (ptr->endpos-GetAbsOrigin());
			vFromAttacker2.z = 0;
			VectorNormalize(vFromAttacker2);


			Vector vCrossProduct;
			CrossProduct(vFromAttacker,vFromAttacker2, vCrossProduct);

			QAngle angles;
			angles = GetLocalAngles();
			if (vCrossProduct.z > 0)
			{
				angles.y		+= 10;
			}
			else
			{
				angles.y		-= 10;
			}

			// Limit against range in y
			if ( angles.y > m_yawCenter + m_yawRange )
			{
				angles.y = m_yawCenter + m_yawRange;
			}
			else if ( angles.y < (m_yawCenter - m_yawRange) )
			{
				angles.y = (m_yawCenter - m_yawRange);
			}

			SetLocalAngles( angles );
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : targetName - 
//			pActivator - 
//-----------------------------------------------------------------------------
CBaseEntity *CCannon::FindTarget( string_t targetName, CBaseEntity *pActivator ) 
{
	return gEntList.FindEntityGenericNearest( STRING( targetName ), GetAbsOrigin(), 0, this, pActivator );
}


//-----------------------------------------------------------------------------
// Purpose: Caches entity key values until spawn is called.
// Input  : szKeyName - 
//			szValue - 
// Output : 
//-----------------------------------------------------------------------------
bool CCannon::KeyValue( const char *szKeyName, const char *szValue )
{
	if (FStrEq(szKeyName, "barrel"))
	{
		m_barrelPos.x = atof(szValue);
		return true;
	}
	
	if (FStrEq(szKeyName, "barrely"))
	{
		m_barrelPos.y = atof(szValue);
		return true;
	}
	
	if (FStrEq(szKeyName, "barrelz"))
	{
		m_barrelPos.z = atof(szValue);
		return true;
	}

	return BaseClass::KeyValue( szKeyName, szValue );
}


static Vector gTankSpread[] =
{
	Vector( 0, 0, 0 ),		// perfect
	Vector( 0.025, 0.025, 0.025 ),	// small cone
	Vector( 0.05, 0.05, 0.05 ),  // medium cone
	Vector( 0.1, 0.1, 0.1 ),	// large cone
	Vector( 0.25, 0.25, 0.25 ),	// extra-large cone
};
#define MAX_FIRING_SPREADS ARRAYSIZE(gTankSpread)


//-----------------------------------------------------------------------------
// Purpose: Spawns the cannon.
//-----------------------------------------------------------------------------
void CCannon::Spawn( void )
{
	Precache();

	SetModel( CANNON_MODEL );

	SetMoveType( MOVETYPE_PUSH );  // so it doesn't get pushed by anything
	SetSolid( SOLID_VPHYSICS );
	SetModel( STRING( GetModelName() ) );
	AddEFlags( EFL_USE_PARTITION_WHEN_NOT_SOLID );

	m_yawCenter			= GetLocalAngles().y;
	m_yawCenterWorld	= GetAbsAngles().y;
	m_pitchCenter		= GetLocalAngles().x;
	m_pitchCenterWorld	= GetAbsAngles().y;
	m_vTargetPosition	= vec3_origin;

	if ( IsActive() || (IsControllable() && !HasController()) )
	{
		// Think to find controllers.
		SetNextThink( gpGlobals->curtime + 1.0f );
		m_flNextControllerSearch = gpGlobals->curtime + 1.0f;
	}

	UpdateMatrix();

	m_sightOrigin = WorldBarrelPosition(); // Point at the end of the barrel

	if ( m_spread > MAX_FIRING_SPREADS )
	{
		m_spread = 0;
	}

	// No longer aim at target position if have one
	m_spawnflags		&= ~SF_TANK_AIM_AT_POS; 

	if (m_spawnflags & SF_TANK_DAMAGE_KICK)
	{
		m_takedamage = DAMAGE_YES;
	}

	// UNDONE: Do this?
	//m_targetEntityName = m_target;
	CreateVPhysics();

	// Setup squared min/max range.
	m_flMinRange2 = m_minRange * m_minRange;
	m_flMaxRange2 = m_maxRange * m_maxRange;
	m_flIgnoreGraceUpto *= m_flIgnoreGraceUpto;

	m_flLastSawNonPlayer = 0;

	if( IsActive() )
	{
		m_OnReadyToFire.FireOutput( this, this );
	}
}

bool CCannon::CreateVPhysics()
{
	VPhysicsInitShadow( false, false );
	return true;
}


void CCannon::Precache( void )
{
	if ( m_iszSpriteSmoke != NULL_STRING )
		PrecacheModel( STRING(m_iszSpriteSmoke) );
	if ( m_iszSpriteFlash != NULL_STRING )
		PrecacheModel( STRING(m_iszSpriteFlash) );

	if ( m_soundStartRotate != NULL_STRING )
		PrecacheScriptSound( STRING(m_soundStartRotate) );
	if ( m_soundStopRotate != NULL_STRING )
		PrecacheScriptSound( STRING(m_soundStopRotate) );
	if ( m_soundLoopRotate != NULL_STRING )
		PrecacheScriptSound( STRING(m_soundLoopRotate) );

	PrecacheScriptSound( "Func_Tank.BeginUse" );
	PrecacheScriptSound( "Weapon_Cannon.Fire" );

	PrecacheModel( CANNON_MODEL );
}

void CCannon::UpdateOnRemove( void )
{
	if ( HasController() )
	{
		StopControl();
	}
	BaseClass::UpdateOnRemove();
}


//-----------------------------------------------------------------------------
// Barrel position
//-----------------------------------------------------------------------------
void CCannon::UpdateMatrix( void )
{
	m_parentMatrix.InitFromEntity( GetParent(), GetParentAttachment() );
}

	
//-----------------------------------------------------------------------------
// Barrel position
//-----------------------------------------------------------------------------
Vector CCannon::WorldBarrelPosition( void )
{
	if ( (m_nBarrelAttachment == 0) || !GetParent() )
	{
		EntityMatrix tmp;
		tmp.InitFromEntity( this );
		return tmp.LocalToWorld( m_barrelPos );
	}

	Vector vecOrigin;
	QAngle vecAngles;
	CBaseAnimating *pAnim = GetParent()->GetBaseAnimating();
	pAnim->GetAttachment( m_nBarrelAttachment, vecOrigin, vecAngles );
	return vecOrigin;
}


//-----------------------------------------------------------------------------
// Make the parent's pose parameters match the func_tank 
//-----------------------------------------------------------------------------
void CCannon::PhysicsSimulate( void )
{
	BaseClass::PhysicsSimulate();
}

//=============================================================================
//
// TANK CONTROLLING
//

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CCannon::OnControls( CBaseEntity *pTest )
{
	// Is the tank controllable.
	if ( !IsControllable() )
		return false;
	
	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CCannon::StartControl( CBaseCombatCharacter *pController )
{
	// Check to see if we have a controller.
	if ( HasController() && GetController() != pController )
		return false;

	// Team only or disabled?
	if ( m_iszMaster != NULL_STRING )
	{
		if ( !UTIL_IsMasterTriggered( m_iszMaster, pController ) )
			return false;
	}

	// Set func_tank as manned by player/npc.
	m_hController = pController;
	if ( pController->IsPlayer() )
	{
		m_spawnflags |= SF_TANK_PLAYER; 

		CBasePlayer *pPlayer = static_cast<CBasePlayer*>( m_hController.Get() );
		pPlayer->m_Local.m_iHideHUD |= HIDEHUD_WEAPONSELECTION;
	}

	// Holster player/npc weapon
	if ( m_hController->GetActiveWeapon() )
	{
		m_hController->GetActiveWeapon()->Holster();
	}

	// Set the controller's position to be the use position.
	m_vecControllerUsePos = m_hController->GetLocalOrigin();

	EmitSound( "Func_Tank.BeginUse" );
	
	SetNextThink( gpGlobals->curtime + 0.1f );
	
	// Let the map maker know a controller has been found
	m_OnGotPlayerController.FireOutput( this, this );

	OnStartControlled();
	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
// TODO: bring back the controllers current weapon
//-----------------------------------------------------------------------------
void CCannon::StopControl()
{
	// Do we have a controller?
	if ( !m_hController )
		return;

	OnStopControlled();

	// Arm player/npc weapon.
	if ( m_hController->GetActiveWeapon() )
	{
		m_hController->GetActiveWeapon()->Deploy();
	}


	CBasePlayer *pPlayer = static_cast<CBasePlayer*>( m_hController.Get() );
	pPlayer->m_Local.m_iHideHUD &= ~HIDEHUD_WEAPONSELECTION;

	// Stop thinking.
	SetNextThink( TICK_NEVER_THINK );
	
	m_OnLostPlayerController.FireOutput( this, this );


	m_spawnflags &= ~SF_TANK_PLAYER;

	m_hController = NULL;

	// Set think, if the func_tank can think on its own.
	if ( IsActive() || (IsControllable() && !HasController()) )
	{
		// Delay the think to find controllers a bit
		m_flNextControllerSearch = gpGlobals->curtime + 5.0f;

		SetNextThink( m_flNextControllerSearch );
	}

	SetLocalAngularVelocity( vec3_angle );
}

//-----------------------------------------------------------------------------
// Purpose:
// Called each frame by the player's ItemPostFrame
//-----------------------------------------------------------------------------
void CCannon::ControllerPostFrame( void )
{
	// Make sure we have a contoller.
	Assert( m_hController != NULL );

	// Control the firing rate.
	if ( gpGlobals->curtime < m_flNextAttack )
		return;

	if ( !IsPlayerManned() )
		return;

	CBasePlayer *pPlayer = static_cast<CBasePlayer*>( m_hController.Get() );
	if ( ( pPlayer->m_nButtons & IN_ATTACK ) == 0 )
		return;

	Vector forward;
	AngleVectors( GetAbsAngles(), &forward );
	m_fireLast = gpGlobals->curtime - (1/m_fireRate) - 0.01;  // to make sure the gun doesn't fire too many bullets
	
	int bulletCount = (gpGlobals->curtime - m_fireLast) * m_fireRate;
	
	Fire( bulletCount, WorldBarrelPosition(), forward, pPlayer, false );
	
	if( m_iAmmoCount > -1 )
	{
		if( !(m_iAmmoCount % 10) )
		{
			Msg("Ammo Remaining: %d\n", m_iAmmoCount );
		}
		
		if( --m_iAmmoCount == 0 )
		{
			// Kick the player off the gun, and make myself not usable.
			m_spawnflags &= ~SF_TANK_CANCONTROL;
			StopControl();
			return;				
		}
	}
	
	SetNextAttack( gpGlobals->curtime + (1/m_fireRate) );
}


//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CCannon::HasController( void )
{ 
	return (m_hController != NULL); 
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : CBaseCombatCharacter
//-----------------------------------------------------------------------------
CBaseCombatCharacter *CCannon::GetController( void ) 
{ 
	return m_hController; 
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pActivator - 
//			*pCaller - 
//			useType - 
//			value - 
//-----------------------------------------------------------------------------
void CCannon::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !IsControllable() )
		return;

	// player controlled turret
	CBasePlayer *pPlayer = ToBasePlayer( pActivator );
	if ( !pPlayer )
		return;

	if ( value == 2 && useType == USE_SET )
	{
		ControllerPostFrame();
	}
	else if ( m_hController != pPlayer && useType != USE_OFF )
	{

		pPlayer->SetUseEntity( this );
		StartControl( pPlayer );
	}
	else 
	{
		StopControl();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CCannon::InRange2( float flRange2 )
{
	if ( flRange2 < m_flMinRange2 )
		return false;

	if ( ( m_flMaxRange2 > 0.0f ) && ( flRange2 > m_flMaxRange2 ) )
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CCannon::Think( void )
{
	StudioFrameAdvance();

	m_hFuncTankTarget = NULL;

	// Look for a new controller?
	if ( IsControllable() && !HasController() && (m_flNextControllerSearch <= gpGlobals->curtime) )
	{
		// Keep thinking, in case they turn NPC finding back on
		if ( !HasController() )
		{
			SetNextThink( gpGlobals->curtime + 2.0f );
		}

		m_flNextControllerSearch = gpGlobals->curtime + 2.0f;
	}

	// refresh the matrix
	UpdateMatrix();

	SetLocalAngularVelocity( vec3_angle );
	TrackTarget();

	if ( fabs(GetLocalAngularVelocity().x) > 1 || fabs(GetLocalAngularVelocity().y) > 1 )
	{
		StartRotSound();
	}
	else
	{
		StopRotSound();
	}
}


//-----------------------------------------------------------------------------
// Purpose: Aim the offset barrel at a position in parent space
// Input  : parentTarget - the position of the target in parent space
// Output : Vector - angles in local space
//-----------------------------------------------------------------------------
QAngle CCannon::AimBarrelAt( const Vector &parentTarget )
{
	Vector target = parentTarget - GetLocalOrigin();
	float quadTarget = target.LengthSqr();
	float quadTargetXY = target.x*target.x + target.y*target.y;

	// Target is too close!  Can't aim at it
	if ( quadTarget <= m_barrelPos.LengthSqr() )
	{
		return GetLocalAngles();
	}
	else
	{
		// We're trying to aim the offset barrel at an arbitrary point.
		// To calculate this, I think of the target as being on a sphere with 
		// it's center at the origin of the gun.
		// The rotation we need is the opposite of the rotation that moves the target 
		// along the surface of that sphere to intersect with the gun's shooting direction
		// To calculate that rotation, we simply calculate the intersection of the ray 
		// coming out of the barrel with the target sphere (that's the new target position)
		// and use atan2() to get angles

		// angles from target pos to center
		float targetToCenterYaw = atan2( target.y, target.x );
		float centerToGunYaw = atan2( m_barrelPos.y, sqrt( quadTarget - (m_barrelPos.y*m_barrelPos.y) ) );

		float targetToCenterPitch = atan2( target.z, sqrt( quadTargetXY ) );
		float centerToGunPitch = atan2( -m_barrelPos.z, sqrt( quadTarget - (m_barrelPos.z*m_barrelPos.z) ) );
		return QAngle( -RAD2DEG(targetToCenterPitch+centerToGunPitch), RAD2DEG( targetToCenterYaw + centerToGunYaw ), 0 );
	}
}


//-----------------------------------------------------------------------------
// Aim the tank at the player crosshair 
//-----------------------------------------------------------------------------
void CCannon::CalcPlayerCrosshairTarget( Vector *pVecTarget )
{
	// Get the player.
	CBasePlayer *pPlayer = static_cast<CBasePlayer*>( m_hController.Get() );

	// Tank aims at player's crosshair.
	Vector vecStart, vecDir;
	trace_t	tr;
	
	vecStart = pPlayer->EyePosition();

	vecDir = pPlayer->EyeDirection3D();
	
	// Make sure to start the trace outside of the player's bbox!
	UTIL_TraceLine( vecStart + vecDir * 24, vecStart + vecDir * 8192, MASK_BLOCKLOS_AND_NPCS, this, COLLISION_GROUP_NONE, &tr );

	*pVecTarget = tr.endpos;
}

//-----------------------------------------------------------------------------
// Aim the tank at the player crosshair 
//-----------------------------------------------------------------------------
void CCannon::AimBarrelAtPlayerCrosshair( QAngle *pAngles )
{
	Vector vecTarget;
	CalcPlayerCrosshairTarget( &vecTarget );
	*pAngles = AimBarrelAt( m_parentMatrix.WorldToLocal( vecTarget ) );
}

//-----------------------------------------------------------------------------
// Returns true if the desired angles are out of range 
//-----------------------------------------------------------------------------
bool CCannon::RotateTankToAngles( const QAngle &angles, float *pDistX, float *pDistY )
{
	bool bClamped = false;

	// Force the angles to be relative to the center position
	float offsetY = UTIL_AngleDistance( angles.y, m_yawCenter );
	float offsetX = UTIL_AngleDistance( angles.x, m_pitchCenter );

	float flActualYaw = m_yawCenter + offsetY;
	float flActualPitch = m_pitchCenter + offsetX;

	if ( ( fabs( offsetY ) > m_yawRange + m_yawTolerance ) ||
		 ( fabs( offsetX ) > m_pitchRange + m_pitchTolerance ) )
	{
		// Limit against range in x
		flActualYaw = clamp( flActualYaw, m_yawCenter - m_yawRange, m_yawCenter + m_yawRange );
		flActualPitch = clamp( flActualPitch, m_pitchCenter - m_pitchRange, m_pitchCenter + m_pitchRange );

		bClamped = true;
	}

	// Get at the angular vel
	QAngle vecAngVel = GetLocalAngularVelocity();

	// Move toward target at rate or less
	float distY = UTIL_AngleDistance( flActualYaw, GetLocalAngles().y );
	vecAngVel.y = distY * 10;
	vecAngVel.y = clamp( vecAngVel.y, -m_yawRate, m_yawRate );
	
	//tempAngle.y = vecAngVel.y;

	// Move toward target at rate or less
	float distX = UTIL_AngleDistance( flActualPitch, GetLocalAngles().x );
	vecAngVel.x = distX  * 10;
	vecAngVel.x = clamp( vecAngVel.x, -m_pitchRate, m_pitchRate );

	if ( distY < -1 )
	{
		PlayAnimation( ACT_BG2_CANNON_RIGHT, 0.7 );
	}
	else if ( distY > 1 )
	{
		PlayAnimation( ACT_BG2_CANNON_LEFT, 0.7 );
	}
			

	// How exciting! We're done
	SetLocalAngularVelocity( vecAngVel );

	if ( pDistX && pDistY )
	{
		*pDistX = distX;
		*pDistY = distY;
	}

	return bClamped;
}


//-----------------------------------------------------------------------------
// We lost our target! 
//-----------------------------------------------------------------------------
void CCannon::LostTarget( void )
{
	if (m_fireLast != 0)
	{
		m_OnLoseTarget.FireOutput(this, this);
		m_fireLast = 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCannon::AimFuncTankAtTarget( void )
{
	// Get world target position
	CBaseEntity *pTarget = NULL;
	trace_t tr;
	QAngle angles;
	bool bUpdateTime = false;

	CBaseEntity *pTargetVehicle = NULL;
	Vector barrelEnd = WorldBarrelPosition();
	Vector worldTargetPosition;
	if (m_spawnflags & SF_TANK_AIM_AT_POS)
	{
		worldTargetPosition = m_vTargetPosition;
	}
	else
	{
		CBaseEntity *pEntity = (CBaseEntity *)m_hTarget;
		if ( !pEntity || ( pEntity->GetFlags() & FL_NOTARGET ) )
		{
			if( m_targetEntityName != NULL_STRING )
			{
				m_hTarget = FindTarget( m_targetEntityName, NULL );
			}
			
			LostTarget();
			return;
		}

		pTarget = pEntity;

		// Calculate angle needed to aim at target
		worldTargetPosition = pEntity->EyePosition();
		if ( pEntity->IsPlayer() )
		{
			CBasePlayer *pPlayer = assert_cast<CBasePlayer*>(pEntity);
			pTargetVehicle = pPlayer->GetVehicleEntity();
			if ( pTargetVehicle )
			{
				worldTargetPosition = pTargetVehicle->BodyTarget( GetAbsOrigin(), false );
			}
		}
	}

	float range2 = worldTargetPosition.DistToSqr( barrelEnd );
	if ( !InRange2( range2 ) )
	{
		if ( m_hTarget )
		{
			m_hTarget = NULL;
			LostTarget();
		}
		return;
	}

	Vector vecAimOrigin = m_sightOrigin;
	if (m_spawnflags & SF_TANK_AIM_AT_POS)
	{
		bUpdateTime		= true;
		m_sightOrigin	= m_vTargetPosition;
		vecAimOrigin = m_sightOrigin;
	}
	else
	{
		if ( m_spawnflags & SF_TANK_LINEOFSIGHT )
		{
			AI_TraceLOS( barrelEnd, worldTargetPosition, this, &tr );
		}
		else
		{
			tr.fraction = 1.0f;
			tr.m_pEnt = pTarget;
		}

		// No line of sight, don't track
		if ( tr.fraction == 1.0 || tr.m_pEnt == pTarget || (pTargetVehicle && (tr.m_pEnt == pTargetVehicle)) )
		{
			if ( InRange2( range2 ) && pTarget && pTarget->IsAlive() )
			{
				bUpdateTime = true;

				// Sight position is BodyTarget with no noise (so gun doesn't bob up and down)
				CBaseEntity *pInstance = pTargetVehicle ? pTargetVehicle : pTarget;
				m_hFuncTankTarget = pInstance;

				m_sightOrigin = pInstance->BodyTarget( GetAbsOrigin(), false );
				vecAimOrigin = m_sightOrigin;
			}
		}
	}

	// Convert targetPosition to parent
	Vector vecLocalOrigin = m_parentMatrix.WorldToLocal( vecAimOrigin );
	angles = AimBarrelAt( vecLocalOrigin );

	// FIXME: These need to be the clamped angles
	float distX, distY;
	bool bClamped = RotateTankToAngles( angles, &distX, &distY );
	if ( bClamped )
	{
		bUpdateTime = false;
	}

	if ( bUpdateTime )
	{
		if( (gpGlobals->curtime - m_lastSightTime >= 1.0) && (gpGlobals->curtime > m_flNextAttack) )
		{
			// Enemy was hidden for a while, and I COULD fire right now. Instead, tack a delay on.
			m_flNextAttack = gpGlobals->curtime + 0.5;
		}

		m_lastSightTime = gpGlobals->curtime;
		m_persist2burst = 0;
	}

	SetMoveDoneTime( 0.1 );

	if ( CanFire() && ( (fabs(distX) <= m_pitchTolerance) && (fabs(distY) <= m_yawTolerance) || (m_spawnflags & SF_TANK_LINEOFSIGHT) ) )
	{
		bool fire = false;
		Vector forward;
		AngleVectors( GetLocalAngles(), &forward );
		forward = m_parentMatrix.ApplyRotation( forward );

		if ( m_spawnflags & SF_TANK_LINEOFSIGHT )
		{
			AI_TraceLine( barrelEnd, pTarget->WorldSpaceCenter(), MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );

			if ( tr.fraction == 1.0f || (tr.m_pEnt && tr.m_pEnt == pTarget) )
			{
				fire = true;
			}
		}
		else
		{
			fire = true;
		}

		if ( fire )
		{
			if (m_fireLast == 0)
			{
				m_OnAquireTarget.FireOutput(this, this);
			}
			FiringSequence( barrelEnd, forward, this );
		}
		else 
		{
			LostTarget();
		}
	}
	else 
	{
		LostTarget();
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCannon::TrackTarget( void )
{
	QAngle angles;

	if( !m_bReadyToFire && m_flNextAttack <= gpGlobals->curtime )
	{
		m_OnReadyToFire.FireOutput( this, this );
		m_bReadyToFire = true;
	}

	if ( IsPlayerManned() )
	{
		AimBarrelAtPlayerCrosshair( &angles );
		RotateTankToAngles( angles );
		SetNextThink( gpGlobals->curtime + 0.05f );
		SetMoveDoneTime( 0.1 );
		return;
	}

	if ( !IsActive() )
	{
		// If we're not active, but we're controllable, we need to keep thinking
		if ( IsControllable() && !HasController() )
		{
			// Think to find controllers.
			SetNextThink( m_flNextControllerSearch );
		}
		return;
	}

	// Clean room for unnecessarily complicated old code
	SetNextThink( gpGlobals->curtime + 0.1f );
	AimFuncTankAtTarget();
}


//-----------------------------------------------------------------------------
// Purpose: Start of firing sequence.  By default, just fire now.
// Input  : &barrelEnd - 
//			&forward - 
//			*pAttacker - 
//-----------------------------------------------------------------------------
void CCannon::FiringSequence( const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker )
{
	return;
	if ( m_fireLast != 0 )
	{
		int bulletCount = (gpGlobals->curtime - m_fireLast) * m_fireRate;
		
		if ( bulletCount > 0 )
		{
			// NOTE: Set m_fireLast first so that Fire can adjust it
			m_fireLast = gpGlobals->curtime;

			Fire( bulletCount, barrelEnd, forward, pAttacker, false );
		}
	}
	else
	{
		m_fireLast = gpGlobals->curtime;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCannon::DoMuzzleFlash( void )
{
}

void CCannon::PlayAnimation( Activity idealActivity, float rate )
{
	int animDesired = SelectWeightedSequence( idealActivity  );
	//int animDesired = LookupSequence( szAnim );

	if ( !IsSequenceFinished() && GetSequence() == animDesired ) //Same sequence is already playing.
		return;

	if ( animDesired > ACTIVITY_NOT_AVAILABLE )
	{
		SetSequence( animDesired );
		SetCycle( 0 );
		ResetSequence( animDesired );
		ResetClientsideFrame();
		m_flPlaybackRate = rate;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : const char
//-----------------------------------------------------------------------------
const char *CCannon::GetTracerType( void )
{
		return "AR2Tracer";
}

//-----------------------------------------------------------------------------
// Purpose: Fire targets and spawn sprites.
// Input  : bulletCount - 
//			barrelEnd - 
//			forward - 
//			pAttacker - 
//-----------------------------------------------------------------------------
void CCannon::Fire( int bulletCount, const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker, bool bIgnoreSpread )
{
	DoMuzzleFlash();

	EmitSound( "Weapon_Cannon.Fire" );

	m_OnFire.FireOutput(this, this);
	m_bReadyToFire = false;
}

	
void CCannon::StartRotSound( void )
{
	m_spawnflags |= SF_TANK_SOUNDON;
	
	if ( m_soundLoopRotate != NULL_STRING )
	{
		CPASAttenuationFilter filter( this );
		filter.MakeReliable();

		EmitSound_t ep;
		ep.m_nChannel = CHAN_STATIC;
		ep.m_pSoundName = (char*)STRING(m_soundLoopRotate);
		ep.m_flVolume = 0.85;
		ep.m_SoundLevel = SNDLVL_NORM;

		EmitSound( filter, entindex(), ep );
	}
	
	if ( m_soundStartRotate != NULL_STRING )
	{
		CPASAttenuationFilter filter( this );

		EmitSound_t ep;
		ep.m_nChannel = CHAN_BODY;
		ep.m_pSoundName = (char*)STRING(m_soundStartRotate);
		ep.m_flVolume = 1.0f;
		ep.m_SoundLevel = SNDLVL_NORM;

		EmitSound( filter, entindex(), ep );
	}
}


void CCannon::StopRotSound( void )
{
	if ( m_spawnflags & SF_TANK_SOUNDON )
	{
		if ( m_soundLoopRotate != NULL_STRING )
		{
			StopSound( entindex(), CHAN_STATIC, (char*)STRING(m_soundLoopRotate) );
		}
		if ( m_soundStopRotate != NULL_STRING )
		{
			CPASAttenuationFilter filter( this );

			EmitSound_t ep;
			ep.m_nChannel = CHAN_BODY;
			ep.m_pSoundName = (char*)STRING(m_soundStopRotate);
			ep.m_flVolume = 1.0f;
			ep.m_SoundLevel = SNDLVL_NORM;

			EmitSound( filter, entindex(), ep );
		}
	}
	m_spawnflags &= ~SF_TANK_SOUNDON;
}

// #############################################################################
//   CFuncTankGun
// #############################################################################
class CCannonFire : public CCannon
{
public:
	DECLARE_CLASS( CCannonFire, CCannon );

	void Fire( int bulletCount, const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker, bool bIgnoreSpread );
};
LINK_ENTITY_TO_CLASS( cannon, CCannonFire );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCannonFire::Fire( int bulletCount, const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker, bool bIgnoreSpread )
{

	Vector vecSrc;
	QAngle vecAngle;

	m_nBarrelAttachment = GetAttachment( LookupAttachment( "muzzle" ), vecSrc, vecAngle );
	if ( !m_nBarrelAttachment )
		Warning("Could not find muzzle attachment! \n");

	CBaseAnimating *pAnim = GetBaseAnimating();
	if ( !pAnim )
		return;

	//QAngle angDir;
	//VectorAngles( vecDir, angDir );

	CCannonBall::Create( vecSrc, vecAngle /*+ QAngle( testx.GetInt(), testy.GetInt(), testz.GetInt() )*/, 100, 1000, 2.0, 10000 );
	Msg(" %f %f %f and %f %f %f \n", vecSrc.x, vecSrc.y, vecSrc.z, vecAngle.x, vecAngle.y, vecAngle.z);

	/*CEffectData data;
	data.m_nEntIndex = pAnim->entindex();
	data.m_nAttachmentIndex = m_nBarrelAttachment;
	data.m_flScale = 1.0f;
	data.m_vOrigin = vecSrc;
	data.m_vAngles = vecAngle;
	data.m_fFlags = MUZZLEFLASH_SHOTGUN;

	DispatchEffect( "MuzzleFlash", data );*/

	CCannon::Fire( 1, barrelEnd, forward, pAttacker, bIgnoreSpread );
	PlayAnimation( ACT_BG2_CANNON_SHOOT, RandomFloat( 1.2, 1.5 ) );
}