/*
This is pretty much a complete ripoff of Tjoppen's bullet code. 
While I have changed a few things here and there to work better for the cannon, 
he deserves 95% of the credit for this code. -HairyPotter
*/

#include "weapon_bg2base.h"
#include "ammodef.h"

//BG2 - Draco - no hl2 player stuff on server!
	#include "hl2mp_player.h"
	#include "te_effect_dispatch.h"
	#include "IEffects.h"
	#include "Sprite.h"
	#include "beam_shared.h"

	#include "shot_manipulator.h"

#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "effect_dispatch_data.h"
#include "cannonball.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define CANNONBALL_MODEL			"models/game/musket_ball.mdl"

//-----------------------------------------------------------------------------
// Crossbow Bolt
//-----------------------------------------------------------------------------

LINK_ENTITY_TO_CLASS( cannonball, CCannonBall );

BEGIN_DATADESC( CCannonBall )
	// Function Pointers
	DEFINE_FUNCTION( BubbleThink ),
	DEFINE_FUNCTION( BoltTouch ),

END_DATADESC()

CCannonBall *CCannonBall::Create( const Vector &vecOrigin, const QAngle &angAngles, int iDamage, float flConstantDamageRange, float flRelativeDrag, float flMuzzleVelocity )
{
	// Create a new entity with CCannonBall private data
	CCannonBall *pBall = (CCannonBall *)CreateEntityByName( "cannonball" );

	if ( !pBall ) //Just in case. -HairyPotter
		return NULL;

	UTIL_SetOrigin( pBall, vecOrigin );
	pBall->SetAbsAngles( angAngles );
	Vector vecDir;
	AngleVectors( angAngles, &vecDir );
	pBall->SetAbsVelocity( vecDir * flMuzzleVelocity );

	pBall->Spawn();

	pBall->m_iDamage = iDamage;

#define LIFETIME	10.f //10.f //3 seconds is definately long enough lifetime.
	pBall->m_flDyingTime = gpGlobals->curtime + LIFETIME;
	pBall->m_flConstantDamageRange = flConstantDamageRange;
	pBall->m_flRelativeDrag = flRelativeDrag;
	pBall->m_flMuzzleVelocity = flMuzzleVelocity;
	pBall->m_vTrajStart = vecOrigin;//pBullet->GetAbsOrigin();
	pBall->m_bHasPlayedNearmiss = false;
	return pBall;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CCannonBall::~CCannonBall( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CCannonBall::CreateVPhysics( void )
{
	// Create the object in the physics system
	//BG2 - Tjoppen - SOLID_VPHYSICS
	VPhysicsInitNormal( SOLID_VPHYSICS, FSOLID_NOT_STANDABLE, false );	

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
unsigned int CCannonBall::PhysicsSolidMaskForEntity() const
{
	return ( BaseClass::PhysicsSolidMaskForEntity() | CONTENTS_HITBOX ) & ~CONTENTS_GRATE;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCannonBall::Spawn( void )
{
	Precache( );

	SetModel( CANNONBALL_MODEL );
	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM );
	AddEffects( EF_NODRAW ); //We'll try having the bullets be invisible for now. -HairyPotter
	SetSolid( SOLID_VPHYSICS );
	SetSolidFlags( FSOLID_NOT_STANDABLE );
	SetGravity( 1.0f );

	// Make sure we're updated if we're underwater
	UpdateWaterState();

	SetTouch( &CCannonBall::BoltTouch );

	SetThink( &CCannonBall::BubbleThink );
	SetNextThink( gpGlobals->curtime );//+ 0.01f );
}


void CCannonBall::Precache( void )
{
	PrecacheModel( CANNONBALL_MODEL );

	//BG2 - Tjoppen - Bullet.Hit
	PrecacheScriptSound( "Bullets.DefaultNearmiss" );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pOther - 
//-----------------------------------------------------------------------------
void CCannonBall::BoltTouch( CBaseEntity *pOther )
{
	if ( !pOther->IsSolid() || pOther->IsSolidFlagSet(FSOLID_VOLUME_CONTENTS) )
		return;

	//StopSound( entindex(), "Bullets.DefaultNearmiss" ); //We've hit something, stop the nearmiss sound.

	if ( pOther->m_takedamage != DAMAGE_NO )
	{
		trace_t	tr;
		Vector	vecNormalizedVel = GetAbsVelocity();

		ClearMultiDamage();
		VectorNormalize( vecNormalizedVel );

		float	speed = GetAbsVelocity().Length();
		if( speed < 100 )
		{
			//too slow. die
			UTIL_RemoveImmediate( this );
			return;
		}

		float	dmg;
		if( (GetAbsOrigin() - m_vTrajStart).Length() < m_flConstantDamageRange )
			dmg = (float)m_iDamage;
		else
			dmg = floorf( (float)m_iDamage * speed * speed / (float)(m_flMuzzleVelocity*m_flMuzzleVelocity) );

		Vector vForward;

		AngleVectors( GetAbsAngles(), &vForward );
		VectorNormalize ( vForward );

		//BG2 - Tjoppen - We want musket balls to hit body parts, not just HITGROUP_GENERIC
		UTIL_TraceLine( GetAbsOrigin() - vForward * speed * gpGlobals->frametime, GetAbsOrigin() + vForward * speed * gpGlobals->frametime, MASK_SHOT, GetOwnerEntity(), COLLISION_GROUP_NONE, &tr );
		if( tr.fraction == 1.0 || (tr.hitgroup == HITGROUP_GENERIC && pOther->IsPlayer()) )
		{
            //only hit hull - keep going
			//move forward a bit so we don't get stuck
			SetAbsOrigin( GetAbsOrigin() + vForward * 4.f );
			return;
		}

		UTIL_ImpactTrace( &tr, DMG_BULLET );	//BG2 - Tjoppen - surface blood

		//no force!
		CTakeDamageInfo	dmgInfo( GetOwnerEntity(), GetOwnerEntity(), dmg, DMG_BULLET | /*DMG_PREVENT_PHYSICS_FORCE |*/DMG_CRUSH | DMG_NEVERGIB ); //Changed to avoid asserts. -HairyPotter
		dmgInfo.SetDamagePosition( tr.endpos );
		pOther->DispatchTraceAttack( dmgInfo, vecNormalizedVel, &tr );

		ApplyMultiDamage();

		//Adrian: keep going through the glass.
		if ( pOther->GetCollisionGroup() == COLLISION_GROUP_BREAKABLE_GLASS )
			 return;

		SetAbsVelocity( Vector( 0, 0, 0 ) );

		// play body "thwack" sound
		//BG2 - Tjoppen - no sound..
		//EmitSound( "Bullet.HitBody" );

		UTIL_TraceLine( GetAbsOrigin(),	GetAbsOrigin() + vForward * 128, MASK_OPAQUE, pOther, COLLISION_GROUP_NONE, &tr );

		if ( tr.fraction != 1.0f )
		{
//			NDebugOverlay::Box( tr2.endpos, Vector( -16, -16, -16 ), Vector( 16, 16, 16 ), 0, 255, 0, 0, 10 );
//			NDebugOverlay::Box( GetAbsOrigin(), Vector( -16, -16, -16 ), Vector( 16, 16, 16 ), 0, 0, 255, 0, 10 );

			if ( tr.m_pEnt == NULL || ( tr.m_pEnt && tr.m_pEnt->GetMoveType() == MOVETYPE_NONE ) )
			{
				CEffectData	data;

				data.m_vOrigin = tr.endpos;
				data.m_vNormal = vForward;
				data.m_nEntIndex = tr.fraction != 1.0f;
			
				DispatchEffect( "Impact", data );
			}
		}
		
		SetTouch( NULL );
		SetThink( NULL );

		//BG2 - Tjoppen - remove bullets immediately before they have time to mess with the target
		//this doesn't seem to completely solve the problem of players flying when being hit by bullets though
		UTIL_RemoveImmediate( this );
		//
	}
	else
	{
		trace_t	tr;
		tr = BaseClass::GetTouchTrace();

		// See if we struck the world
		if ( pOther->GetMoveType() == MOVETYPE_NONE && !( tr.surface.flags & SURF_SKY ) )
		{
			//BG2 - Tjoppen - Bullet.HitWorld
			//EmitSound( "Bullet.HitWorld" );

			// if what we hit is static architecture, can stay around for a while.
			Vector vecDir = GetAbsVelocity();
			float speed = VectorNormalize( vecDir );

			// See if we should reflect off this surface
			float hitDot = DotProduct( tr.plane.normal, -vecDir );
			
			// BG2 - BP original was( hitDot < 0.5f ) but a musket ball should not bounce off walls if the angle is too big
			//BG2 - Tjoppen - don't ricochet unless we're hitting a surface at a 60 degree horisontal angle or more
			//					this is a hack so that bullets don't ricochet off the ground
			//if ( ( hitDot < 0.2f ) && ( speed > 100 ) )
			if ( ( hitDot < 0.2f ) && ( speed > 100 ) && tr.plane.normal.z < 0.5f )
			{
				Vector vReflection = 2.0f * tr.plane.normal * hitDot + vecDir;
				
				QAngle reflectAngles;

				VectorAngles( vReflection, reflectAngles );

				SetLocalAngles( reflectAngles );

				SetAbsVelocity( vReflection * speed * 0.75f );

				// Start to sink faster
				SetGravity( 1.0f );
			}
			else
			{
				//FIXME: We actually want to stick (with hierarchy) to what we've hit
				SetMoveType( MOVETYPE_NONE );
			
				Vector vForward;

				AngleVectors( GetAbsAngles(), &vForward );
				VectorNormalize ( vForward );

				CEffectData	data;

				data.m_vOrigin = tr.endpos;
				data.m_vNormal = vForward;
				data.m_nEntIndex = 0;
			
				DispatchEffect( "Impact", data ); 
				
				UTIL_ImpactTrace( &tr, DMG_BULLET );

				SetTouch( NULL );

				//Just remove the bullet instantly. -HairyPotter
				UTIL_Remove( this );
			}
			
			//BG2 - BP  TODO: musket balls only create sparks on metal surfaces Shoot some sparks
			/*if ( UTIL_PointContents( GetAbsOrigin() ) == CONTENTS_WATER)
			{
				g_pEffects->Sparks( GetAbsOrigin() );
			}*/
		}
		else
		{
			// Put a mark unless we've hit the sky
			if ( ( tr.surface.flags & SURF_SKY ) == false )
			{
				UTIL_ImpactTrace( &tr, DMG_BULLET );
			}

			UTIL_Remove( this );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCannonBall::BubbleThink( void )
{
	QAngle angNewAngles;

	VectorAngles( GetAbsVelocity(), angNewAngles );
	SetAbsAngles( angNewAngles );

	SetNextThink( gpGlobals->curtime );	//BG2 - Tjoppen - think every frame

	if( gpGlobals->curtime > m_flDyingTime )
	{
		UTIL_Remove( this );
		return;
	}

	if ( GetWaterLevel()  == 0 )
	{
		//apply drag
		Vector	vecDir = GetAbsVelocity();
		extern	ConVar	sv_simulatedbullets_drag,
						sv_gravity;

		float	speed = vecDir.NormalizeInPlace(),
				//drag = 0.0001f;
				drag = sv_simulatedbullets_drag.GetFloat() * m_flRelativeDrag;

		speed -= drag * speed*speed * gpGlobals->frametime;
		if( speed < 1000 )
			speed = 1000;	//clamp

		SetAbsVelocity( vecDir * speed );

		vecDir.NormalizeInPlace();

		for( int x = 1; x <= gpGlobals->maxClients; x++ )
		{
			CBasePlayer *pPlayer = UTIL_PlayerByIndex( x );

			//only want connected, alive players
			if( !pPlayer || !pPlayer->IsAlive() )
				continue;

			//make sure only this player hears it
			CSingleUserRecipientFilter filter( pPlayer );
			EmitSound( filter, entindex(), "Bullets.DefaultNearmiss" ); //By using the entindex of the bullet itself, you're making IT play the sound, not the player. -HairyPotter
			m_bHasPlayedNearmiss = true;

			//break;
		}

		//Msg( "\n" );

		return;
	}

	UTIL_BubbleTrail( GetAbsOrigin() - GetAbsVelocity() * 0.1f, GetAbsOrigin(), 5 );
}