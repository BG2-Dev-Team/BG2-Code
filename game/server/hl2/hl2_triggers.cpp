//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_physcannon.h"
#include "hl2_player.h"
#include "saverestore_utlvector.h"
#include "triggers.h"

//BG2 - For the triggers. -HairyPotter
#include "hl2mp_gamerules.h"
#include "ammodef.h"
#include "team.h"
#include "../bg2/flag.h"
#include "../bg2/ctfflag.h"
//

extern ConVar sv_ctf_capturestyle;

//-----------------------------------------------------------------------------
// Weapon-dissolve trigger; all weapons in this field (sans the physcannon) are destroyed!
//-----------------------------------------------------------------------------
class CTriggerWeaponDissolve : public CTriggerMultiple
{
	DECLARE_CLASS( CTriggerWeaponDissolve, CTriggerMultiple );
	DECLARE_DATADESC();

public:
				~CTriggerWeaponDissolve( void );

	virtual void Spawn( void );
	virtual void Precache( void );
	virtual void Activate( void );
	virtual void StartTouch( CBaseEntity *pOther );

	inline bool HasWeapon( CBaseCombatWeapon *pWeapon );

	Vector	GetConduitPoint( CBaseEntity *pTarget );

	void	InputStopSound( inputdata_t &inputdata );

	void	AddWeapon( CBaseCombatWeapon *pWeapon );
	void	CreateBeam( const Vector &vecSource, CBaseEntity *pDest, float flLifetime );
	void	DissolveThink( void );

private:

	COutputEvent	m_OnDissolveWeapon;
	COutputEvent	m_OnChargingPhyscannon;

	CUtlVector< CHandle<CBaseCombatWeapon> >	m_pWeapons;
	CUtlVector< CHandle<CBaseEntity> >			m_pConduitPoints;
	string_t									m_strEmitterName;
	int											m_spriteTexture;
};

LINK_ENTITY_TO_CLASS( trigger_weapon_dissolve, CTriggerWeaponDissolve );

BEGIN_DATADESC( CTriggerWeaponDissolve )

	DEFINE_KEYFIELD( m_strEmitterName,	FIELD_STRING, "emittername" ),
	DEFINE_UTLVECTOR( m_pWeapons,		FIELD_EHANDLE ),
	DEFINE_UTLVECTOR( m_pConduitPoints, FIELD_EHANDLE ),
	DEFINE_FIELD( m_spriteTexture,		FIELD_MODELINDEX ),

	DEFINE_OUTPUT( m_OnDissolveWeapon, "OnDissolveWeapon" ),
	DEFINE_OUTPUT( m_OnChargingPhyscannon, "OnChargingPhyscannon" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "StopSound", InputStopSound ),

	DEFINE_THINKFUNC( DissolveThink ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CTriggerWeaponDissolve::~CTriggerWeaponDissolve( void )
{
	m_pWeapons.Purge();
	m_pConduitPoints.Purge();
}

//-----------------------------------------------------------------------------
// Purpose: Call precache for our sprite texture
//-----------------------------------------------------------------------------
void CTriggerWeaponDissolve::Spawn( void )
{
	BaseClass::Spawn();
	Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Precache our sprite texture
//-----------------------------------------------------------------------------
void CTriggerWeaponDissolve::Precache( void )
{
	BaseClass::Precache();

	m_spriteTexture = PrecacheModel( "sprites/lgtning.vmt" );

	PrecacheScriptSound( "WeaponDissolve.Dissolve" );
	PrecacheScriptSound( "WeaponDissolve.Charge" );
	PrecacheScriptSound( "WeaponDissolve.Beam" );
}

static const char *s_pDissolveThinkContext = "DissolveThinkContext";

//-----------------------------------------------------------------------------
// Purpose: Collect all our known conduit points
//-----------------------------------------------------------------------------
void CTriggerWeaponDissolve::Activate( void )
{
	BaseClass::Activate();

	CBaseEntity *pEntity = NULL;

	while ( ( pEntity = gEntList.FindEntityByName( pEntity, m_strEmitterName ) ) != NULL )
	{
		m_pConduitPoints.AddToTail( pEntity );
	}

	SetContextThink( &CTriggerWeaponDissolve::DissolveThink, gpGlobals->curtime + 0.1f, s_pDissolveThinkContext );
}

//-----------------------------------------------------------------------------
// Purpose: Checks to see if a weapon is already known
// Input  : *pWeapon - weapon to check for
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CTriggerWeaponDissolve::HasWeapon( CBaseCombatWeapon *pWeapon )
{
	if ( m_pWeapons.Find( pWeapon ) == m_pWeapons.InvalidIndex() )
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Adds a weapon to the known weapon list
// Input  : *pWeapon - weapon to add
//-----------------------------------------------------------------------------
void CTriggerWeaponDissolve::AddWeapon( CBaseCombatWeapon *pWeapon )
{
	if ( HasWeapon( pWeapon ) )
		return;

	m_pWeapons.AddToTail( pWeapon );
}

//-----------------------------------------------------------------------------
// Purpose: Collect any weapons inside our volume
// Input  : *pOther - 
//-----------------------------------------------------------------------------
void CTriggerWeaponDissolve::StartTouch( CBaseEntity *pOther )
{
	BaseClass::StartTouch( pOther );

	if ( PassesTriggerFilters( pOther ) == false )
		return;

	CBaseCombatWeapon *pWeapon = dynamic_cast<CBaseCombatWeapon *>(pOther);

	if ( pWeapon == NULL )
		return;

	AddWeapon( pWeapon );
}

//-----------------------------------------------------------------------------
// Purpose: Creates a beam between a conduit point and a weapon
// Input  : &vecSource - conduit point
//			*pDest - weapon
//			flLifetime - amount of time
//-----------------------------------------------------------------------------
void CTriggerWeaponDissolve::CreateBeam( const Vector &vecSource, CBaseEntity *pDest, float flLifetime )
{
	CBroadcastRecipientFilter filter;

	te->BeamEntPoint( filter, 0.0,
		0,
		&vecSource,
		pDest->entindex(), 
		&(pDest->WorldSpaceCenter()),
		m_spriteTexture,
		0,				// No halo
		1,				// Frame
		30,
		flLifetime,
		16.0f,			// Start width
		4.0f,			// End width
		0,				// No fade
		8,				// Amplitude
		255,
		255,
		255,
		255,
		16 );			// Speed
}

//-----------------------------------------------------------------------------
// Purpose: Returns the closest conduit point to a weapon
// Input  : *pTarget - weapon to check for
// Output : Vector - position of the conduit
//-----------------------------------------------------------------------------
Vector CTriggerWeaponDissolve::GetConduitPoint( CBaseEntity *pTarget )
{
	float	nearDist = 9999999.0f;
	Vector	bestPoint = vec3_origin;
	float	testDist;

	// Find the nearest conduit to the target
	for ( int i = 0; i < m_pConduitPoints.Count(); i++ )
	{
		testDist = ( m_pConduitPoints[i]->GetAbsOrigin() - pTarget->GetAbsOrigin() ).LengthSqr();

		if ( testDist < nearDist )
		{
			bestPoint = m_pConduitPoints[i]->GetAbsOrigin();
			nearDist = testDist;
		}
	}

	return bestPoint;
}

//-----------------------------------------------------------------------------
// Purpose: Dissolve all weapons within our volume
//-----------------------------------------------------------------------------
void CTriggerWeaponDissolve::DissolveThink( void )
{
	int	numWeapons = m_pWeapons.Count();

	// Dissolve all the items within the volume
	for ( int i = 0; i < numWeapons; i++ )
	{
		CBaseCombatWeapon *pWeapon = m_pWeapons[i];
		Vector vecConduit = GetConduitPoint( pWeapon );
		
		// The physcannon upgrades when this happens
		if ( FClassnameIs( pWeapon, "weapon_physcannon" ) )
		{
			// This must be the last weapon for us to care
			if ( numWeapons > 1 )
				continue;

			//FIXME: Make them do this on a stagger!

			// All conduits send power to the weapon
			for ( int i = 0; i < m_pConduitPoints.Count(); i++ )
			{
				CreateBeam( m_pConduitPoints[i]->GetAbsOrigin(), pWeapon, 4.0f );
			}

			//PhysCannonBeginUpgrade( pWeapon ); //BG2 - no phys gun
			m_OnChargingPhyscannon.FireOutput( this, this );

			EmitSound( "WeaponDissolve.Beam" );

			// We're done
			m_pWeapons.Purge();
			m_pConduitPoints.Purge();
			SetContextThink( NULL, 0, s_pDissolveThinkContext );
			return;
		}

		// Randomly dissolve them all
		float flLifetime = random->RandomFloat( 2.5f, 4.0f );
		CreateBeam( vecConduit, pWeapon, flLifetime );
		pWeapon->Dissolve( NULL, gpGlobals->curtime + ( 3.0f - flLifetime ), false );

		m_OnDissolveWeapon.FireOutput( this, this );

		CPASAttenuationFilter filter( pWeapon );
		EmitSound( filter, pWeapon->entindex(), "WeaponDissolve.Dissolve" );
		
		// Beam looping sound
		EmitSound( "WeaponDissolve.Beam" );

		m_pWeapons.Remove( i );
		SetContextThink( &CTriggerWeaponDissolve::DissolveThink, gpGlobals->curtime + random->RandomFloat( 0.5f, 1.5f ), s_pDissolveThinkContext );
		return;
	}

	SetContextThink( &CTriggerWeaponDissolve::DissolveThink, gpGlobals->curtime + 0.1f, s_pDissolveThinkContext );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CTriggerWeaponDissolve::InputStopSound( inputdata_t &inputdata )
{
	StopSound( "WeaponDissolve.Beam" );
	StopSound( "WeaponDissolve.Charge" );
}

//-----------------------------------------------------------------------------
// Weapon-strip trigger; can't pick up weapons while in the field
//-----------------------------------------------------------------------------
class CTriggerWeaponStrip : public CTriggerMultiple
{
	DECLARE_CLASS( CTriggerWeaponStrip, CTriggerMultiple );
	DECLARE_DATADESC();

public:
	void StartTouch(CBaseEntity *pOther);
	void EndTouch(CBaseEntity *pOther);

private:
	bool m_bKillWeapons;
};


//-----------------------------------------------------------------------------
// Save/load
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_weapon_strip, CTriggerWeaponStrip );

BEGIN_DATADESC( CTriggerWeaponStrip )
	DEFINE_KEYFIELD( m_bKillWeapons,	FIELD_BOOLEAN, "KillWeapons" ),
END_DATADESC()


//-----------------------------------------------------------------------------
// Drops all weapons, marks the character as not being able to pick up weapons
//-----------------------------------------------------------------------------
void CTriggerWeaponStrip::StartTouch(CBaseEntity *pOther)
{
	BaseClass::StartTouch( pOther );

	if ( PassesTriggerFilters(pOther) == false )
		return;

	CBaseCombatCharacter *pCharacter = pOther->MyCombatCharacterPointer();
	
	if ( m_bKillWeapons )
	{
		for ( int i = 0 ; i < pCharacter->WeaponCount(); ++i )
		{
			CBaseCombatWeapon *pWeapon = pCharacter->GetWeapon( i );
			if ( !pWeapon )
				continue;

			pCharacter->Weapon_Drop( pWeapon );
			UTIL_Remove( pWeapon );
		}
		return;
	}

	// Strip the player of his weapons
	if ( pCharacter && pCharacter->IsAllowedToPickupWeapons() )
	{
		CBaseCombatWeapon *pBugbait = pCharacter->Weapon_OwnsThisType( "weapon_bugbait" );
		if ( pBugbait )
		{
			pCharacter->Weapon_Drop( pBugbait );
			UTIL_Remove( pBugbait );
		}

		pCharacter->Weapon_DropAll( true );
		pCharacter->SetPreventWeaponPickup( true );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called when an entity stops touching us.
// Input  : pOther - The entity that was touching us.
//-----------------------------------------------------------------------------
void CTriggerWeaponStrip::EndTouch(CBaseEntity *pOther)
{
	if ( IsTouching( pOther ) )
	{
		CBaseCombatCharacter *pCharacter = pOther->MyCombatCharacterPointer();
		if ( pCharacter )
		{
			pCharacter->SetPreventWeaponPickup( false );
		}
	}

	BaseClass::EndTouch( pOther );
}



//-----------------------------------------------------------------------------
// Teleport trigger
//-----------------------------------------------------------------------------
class CTriggerPhysicsTrap : public CTriggerMultiple
{
	DECLARE_CLASS( CTriggerPhysicsTrap, CTriggerMultiple );
	DECLARE_DATADESC();

public:
	void Touch( CBaseEntity *pOther );

private:
	void InputEnable( inputdata_t &inputdata );
	void InputDisable( inputdata_t &inputdata );
	void InputToggle( inputdata_t &inputdata );

	int m_nDissolveType;
};


//-----------------------------------------------------------------------------
// Save/load
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_physics_trap, CTriggerPhysicsTrap );

BEGIN_DATADESC( CTriggerPhysicsTrap )

	DEFINE_KEYFIELD( m_nDissolveType,	FIELD_INTEGER,	"dissolvetype" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle", InputToggle ),

END_DATADESC()

//------------------------------------------------------------------------------
// Inputs
//------------------------------------------------------------------------------
void CTriggerPhysicsTrap::InputToggle( inputdata_t &inputdata )
{
	if ( m_bDisabled )
	{
		InputEnable( inputdata );
	}
	else
	{
		InputDisable( inputdata );
	}
}

void CTriggerPhysicsTrap::InputEnable( inputdata_t &inputdata )
{
	if ( m_bDisabled )
	{
		Enable();
	}
}

void CTriggerPhysicsTrap::InputDisable( inputdata_t &inputdata )
{
	if ( !m_bDisabled )
	{
		Disable();
	}
}

//-----------------------------------------------------------------------------
// Traps the entities
//-----------------------------------------------------------------------------
#define JOINTS_TO_CONSTRAIN 1

void CTriggerPhysicsTrap::Touch( CBaseEntity *pOther )
{
	if ( !PassesTriggerFilters(pOther) )
		return;

	CBaseAnimating *pAnim = pOther->GetBaseAnimating();
	if ( !pAnim )
		return;

	pAnim->Dissolve( NULL, gpGlobals->curtime, false, m_nDissolveType );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------

class CWateryDeathLeech : public CBaseAnimating
{
	DECLARE_CLASS( CWateryDeathLeech, CBaseAnimating );
public:
	DECLARE_DATADESC();

	void Spawn( void );
	void Precache( void );
	void LeechThink( void );

	int m_iFadeState;
};

LINK_ENTITY_TO_CLASS( ent_watery_leech, CWateryDeathLeech );

BEGIN_DATADESC( CWateryDeathLeech )
	DEFINE_THINKFUNC( LeechThink ),
	DEFINE_FIELD( m_iFadeState, FIELD_INTEGER ),
END_DATADESC()

void CWateryDeathLeech::Precache( void )
{
	//Ugh this is temporary until Jakob finishes the animations and doesn't need the command anymore.
	bool allowPrecache = CBaseEntity::IsPrecacheAllowed();
	CBaseEntity::SetAllowPrecache( true );

	BaseClass::Precache();

	PrecacheModel( "models/leech.mdl" );
	CBaseEntity::SetAllowPrecache( allowPrecache );
}

void CWateryDeathLeech::Spawn( void )
{
	Precache();
	BaseClass::Spawn();

	SetSolid ( SOLID_NONE );

	SetMoveType( MOVETYPE_NONE );
	AddEffects( EF_NOSHADOW );
	
	SetModel( "models/leech.mdl" );

	SetThink( &CWateryDeathLeech::LeechThink );
	SetNextThink( gpGlobals->curtime + 0.1 );

	m_flPlaybackRate = random->RandomFloat( 0.5, 1.5 );
	SetCycle( random->RandomFloat( 0.0f, 0.9f ) );

	QAngle vAngle;
	vAngle[YAW] = random->RandomFloat( 0, 360 );
	SetAbsAngles( vAngle );

	m_iFadeState = 1;
	SetRenderColorA( 1 );
}

void CWateryDeathLeech::LeechThink( void )
{
	if ( IsMarkedForDeletion() )
		 return;

	StudioFrameAdvance();
	SetNextThink( gpGlobals->curtime + 0.1 );

	if ( m_iFadeState != 0 )
	{
		float dt = gpGlobals->frametime;
		if ( dt > 0.1f )
		{
			dt = 0.1f;
		}
		m_nRenderMode = kRenderTransTexture;
		int speed = MAX(1,256*dt); // fade out over 1 second

		if ( m_iFadeState == -1 )
			 SetRenderColorA( UTIL_Approach( 0, m_clrRender->a, speed ) );
		else
			 SetRenderColorA( UTIL_Approach( 255, m_clrRender->a, speed ) );

		if ( m_clrRender->a == 0 )
		{
			UTIL_Remove(this);
		}
		else if ( m_clrRender->a == 255 )
		{
			m_iFadeState = 0;
		}
		else
		{
			SetNextThink( gpGlobals->curtime );
		}
	}


	if ( GetOwnerEntity() )
	{
		if ( GetOwnerEntity()->GetWaterLevel() < 3 )
		{
			AddEffects( EF_NODRAW );
		}
		else
		{
			RemoveEffects( EF_NODRAW );
		}

		SetAbsOrigin( GetOwnerEntity()->GetAbsOrigin() + GetOwnerEntity()->GetViewOffset() );
	}
}

class CTriggerWateryDeath : public CBaseTrigger
{
	DECLARE_CLASS( CTriggerWateryDeath, CBaseTrigger );
public:
	DECLARE_DATADESC();

	void Spawn( void );
	void Precache( void );
	void Touch( CBaseEntity *pOther );
	void SpawnLeeches( CBaseEntity *pOther );
	
	// Ignore non-living entities
	virtual bool PassesTriggerFilters(CBaseEntity *pOther)
	{
		if ( !BaseClass::PassesTriggerFilters(pOther) )
			return false;

		return (pOther->m_takedamage == DAMAGE_YES);
	}

	virtual void StartTouch(CBaseEntity *pOther);
	virtual void EndTouch(CBaseEntity *pOther);

private:

	CUtlVector< EHANDLE > m_hLeeches;

	// Kill times for entities I'm touching
	CUtlVector< float >	m_flEntityKillTimes;
	float				m_flNextPullSound;
	float				m_flPainValue;
};

BEGIN_DATADESC( CTriggerWateryDeath )
	DEFINE_UTLVECTOR( m_flEntityKillTimes, FIELD_TIME ),
	DEFINE_UTLVECTOR( m_hLeeches, FIELD_EHANDLE ),
	DEFINE_FIELD( m_flNextPullSound, FIELD_TIME ),
	DEFINE_FIELD( m_flPainValue, FIELD_FLOAT ),
END_DATADESC()


LINK_ENTITY_TO_CLASS( trigger_waterydeath, CTriggerWateryDeath );

// Stages of the waterydeath trigger, in time offsets from the initial touch
#define WD_KILLTIME_NEXT_BITE	0.3
#define WD_PAINVALUE_STEP 2.0
#define WD_MAX_DAMAGE 15.0f

//-----------------------------------------------------------------------------
// Purpose: Called when spawning, after keyvalues have been handled.
//-----------------------------------------------------------------------------
void CTriggerWateryDeath::Spawn( void )
{
	BaseClass::Spawn();
	Precache();

	m_flNextPullSound = 0;
	m_flPainValue = 0;
	InitTrigger();
}

void CTriggerWateryDeath::Precache( void )
{
	//Ugh this is temporary until Jakob finishes the animations and doesn't need the command anymore.
	BaseClass::Precache();
	PrecacheModel( "models/leech.mdl" );
	
	PrecacheScriptSound( "coast.leech_bites_loop" );
	PrecacheScriptSound( "coast.leech_water_churn_loop" );
}

void CTriggerWateryDeath::SpawnLeeches( CBaseEntity *pOther )
{
	if ( pOther	== NULL )
		 return;

	if ( m_hLeeches.Count() > 0 )
		 return;

	int iMaxLeeches = 12;
	
	for ( int i = 0; i < iMaxLeeches; i++ )
	{
		CWateryDeathLeech *pLeech = (CWateryDeathLeech*)CreateEntityByName( "ent_watery_leech" );

		if ( pLeech )
		{
			m_hLeeches.AddToTail( pLeech );

			pLeech->Spawn();
			pLeech->SetAbsOrigin( pOther->GetAbsOrigin() );
			pLeech->SetOwnerEntity( pOther );

			if ( i <= 8 )
				 pLeech->SetSequence( i % 4 );
			else 
				 pLeech->SetSequence( ( i % 4 ) + 4 ) ;
			pLeech->ResetSequenceInfo();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerWateryDeath::Touch( CBaseEntity *pOther )
{	
	if (!PassesTriggerFilters(pOther))
		return;

	// Find our index
	EHANDLE hOther;
	hOther = pOther;
	int iIndex = m_hTouchingEntities.Find( hOther );
	if ( iIndex == m_hTouchingEntities.InvalidIndex() )
		return;

	float flKillTime = m_flEntityKillTimes[iIndex];
	
	// Time to kill it?
	if ( gpGlobals->curtime > flKillTime )
	{
		//EmitSound( filter, entindex(), "WateryDeath.Bite", &pOther->GetAbsOrigin() );
		// Kill it
		if ( pOther->IsPlayer() )
		{
			m_flPainValue = MIN( m_flPainValue + WD_PAINVALUE_STEP, WD_MAX_DAMAGE );
		}
		else
		{
			m_flPainValue = WD_MAX_DAMAGE;
		}

		// Use DMG_GENERIC & make the target inflict the damage on himself.
		// This ensures that if the target is the player, the damage isn't modified by skill
		CTakeDamageInfo info = CTakeDamageInfo( pOther, pOther, m_flPainValue, DMG_GENERIC );

		GuessDamageForce( &info, (pOther->GetAbsOrigin() - GetAbsOrigin()), pOther->GetAbsOrigin() );
		pOther->TakeDamage( info );

		m_flEntityKillTimes[iIndex] = gpGlobals->curtime + WD_KILLTIME_NEXT_BITE;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called when an entity starts touching us.
// Input  : pOther - The entity that is touching us.
//-----------------------------------------------------------------------------
void CTriggerWateryDeath::StartTouch(CBaseEntity *pOther)
{
	BaseClass::StartTouch( pOther );

	m_flPainValue = 0.0f;

	// If we added him to our list, store the start time
	EHANDLE hOther;
	hOther = pOther;
	if ( m_hTouchingEntities.Find( hOther ) != m_hTouchingEntities.InvalidIndex() )
	{
		// Always added to the end
		// Players get warned, everything else gets et quick.
		if ( pOther->IsPlayer() )
		{
			m_flEntityKillTimes.AddToTail( gpGlobals->curtime + WD_KILLTIME_NEXT_BITE );
		}
		else
		{
			m_flEntityKillTimes.AddToTail( gpGlobals->curtime + WD_KILLTIME_NEXT_BITE );
		}
	}

#ifdef HL2_DLL
	if ( pOther->IsPlayer() )
	{
		SpawnLeeches( pOther );

		CHL2_Player *pHL2Player = dynamic_cast<CHL2_Player*>( pOther );

		if ( pHL2Player )
		{
			pHL2Player->StartWaterDeathSounds();
		}
	}
#endif
	
}


//-----------------------------------------------------------------------------
// Purpose: Called when an entity stops touching us.
// Input  : pOther - The entity that was touching us.
//-----------------------------------------------------------------------------
void CTriggerWateryDeath::EndTouch( CBaseEntity *pOther )
{
	if ( IsTouching( pOther ) )
	{
		EHANDLE hOther;
		hOther = pOther;

		// Remove the time from our list
		int iIndex = m_hTouchingEntities.Find( hOther );
		if ( iIndex != m_hTouchingEntities.InvalidIndex() )
		{
			m_flEntityKillTimes.Remove( iIndex );
		}
	}

#ifdef HL2_DLL
	if ( pOther->IsPlayer() )
	{
		for (int i = 0; i < m_hLeeches.Count(); i++ )
		{
			CWateryDeathLeech *pLeech = dynamic_cast<CWateryDeathLeech*>( m_hLeeches[i].Get() );

			if ( pLeech )
			{
				pLeech->m_iFadeState = -1;
			}
		}

		if ( m_hLeeches.Count() > 0 )
			 m_hLeeches.Purge();

		CHL2_Player *pHL2Player = dynamic_cast<CHL2_Player*>( pOther );

		if ( pHL2Player )
		{
			//Adrian: Hi, you might be wondering why I'm doing this, yes?
			//        Well, EndTouch is called not only when the player leaves
			//		  the trigger, but also on level shutdown. We can't let the
			//		  soundpatch fade the sound out since we'll hit a nasty assert
			//        cause it'll try to fade out a sound using an entity that might
			//        be gone since we're shutting down the server.
			if ( !(pHL2Player->GetFlags() & FL_DONTTOUCH ) )
				  pHL2Player->StopWaterDeathSounds();
		}
	}
#endif

	BaseClass::EndTouch( pOther );
}


//-----------------------------------------------------------------------------
// Purpose: Triggers whenever an RPG is fired within it
//-----------------------------------------------------------------------------
class CTriggerRPGFire : public CTriggerMultiple
{
	DECLARE_CLASS( CTriggerRPGFire, CTriggerMultiple );
public:
	~CTriggerRPGFire();

	void Spawn( void );
	void OnRestore( void );
};

LINK_ENTITY_TO_CLASS( trigger_rpgfire, CTriggerRPGFire );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTriggerRPGFire::~CTriggerRPGFire( void )
{
	g_hWeaponFireTriggers.FindAndRemove( this );
}

//-----------------------------------------------------------------------------
// Purpose: Called when spawning, after keyvalues have been handled.
//-----------------------------------------------------------------------------
void CTriggerRPGFire::Spawn( void )
{
	BaseClass::Spawn();

	InitTrigger();

	g_hWeaponFireTriggers.AddToTail( this );

	// Stomp the touch function, because we don't want to respond to touch
	SetTouch( NULL );
}

//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CTriggerRPGFire::OnRestore()
{
	BaseClass::OnRestore();

	g_hWeaponFireTriggers.AddToTail( this );
}

//-----------------------------------------------------------------------------
// BG2 - Remote Flag Capture Trigger. -HairyPotter
//-----------------------------------------------------------------------------
class CFlagTriggerBG2 : public CTriggerMultiple
{
	DECLARE_CLASS(CFlagTriggerBG2, CTriggerMultiple);
	DECLARE_DATADESC();

public:
	void Spawn(void);
	void StartTouch(CBaseEntity *pOther);
	void EndTouch(CBaseEntity *pOther);

private:
	string_t m_strParentName;
	CFlag *FlagEnt;
};


//-----------------------------------------------------------------------------
// Save/load
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(flag_trigger, CFlagTriggerBG2);

BEGIN_DATADESC(CFlagTriggerBG2)
DEFINE_KEYFIELD(m_strParentName, FIELD_STRING, "FlagParentName"),
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Set up the trigger.
//-----------------------------------------------------------------------------
void CFlagTriggerBG2::Spawn(void)
{
	BaseClass::Spawn();
	//We want to get the parent setting out of the way, let's connect it and be done with it. Also have the flag notice this.
	if (!GetParent())
	{
		FlagEnt = static_cast< CFlag* >(gEntList.FindEntityByName(NULL, m_strParentName));
		if (!FlagEnt)	//Check to see if this is even a flag. Return and remove if false, don't take the whole server down from one error.
		{
			Warning("This trigger has determined that the parent named '%s' is not a BG3 flag or does not exist. Trigger disabled. \n", m_strParentName);
			UTIL_Remove(this);
			return;
		}
		SetParent(m_strParentName, this); //Assuming we passed the test, go ahead and set the parent.
		FlagEnt->m_bIsParent = true; //Go ahead and let the flag know it's a parent to this trigger. 
	}
	//
}
//-----------------------------------------------------------------------------
// Purpose: Called when an entity starts touching us.
// Input  : pOther - The entity that was touching us.
//-----------------------------------------------------------------------------
void CFlagTriggerBG2::StartTouch(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer()) //Ask yourself, would anything else be able to capture a flag?
		return;

	//Defines.
	CBasePlayer *pPlayer = static_cast< CBasePlayer* >(pOther->MyCombatCharacterPointer());

	if (!pPlayer || !FlagEnt || !FlagEnt->m_bActive) //Flag is inactive?
		return;				   //Die here.

	//BaseClass::StartTouch( pOther );

	if (!pPlayer->IsAlive())	//dead players don't cap
		return;

	switch (pPlayer->GetTeamNumber()) //Let's do most of the sorting work here rather than bunching the teams together.
	{
	case TEAM_AMERICANS:
		if ((FlagEnt->m_iForTeam == 1) || (FlagEnt->m_iForTeam == 0))
		{
			FlagEnt->m_vTriggerAmericanPlayers.AddToTail(pPlayer); //Add this player to the american player list.
		}
		break;
	case TEAM_BRITISH:
		if ((FlagEnt->m_iForTeam == 2) || (FlagEnt->m_iForTeam == 0))
		{
			FlagEnt->m_vTriggerBritishPlayers.AddToTail(pPlayer); //Add this player to the british player list.
		}
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called when an entity stops touching us.
// Input  : pOther - The entity that was touching us.
//----------------------------------------1-------------------------------------
void CFlagTriggerBG2::EndTouch(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer()) //Ask yourself, would anything else be able to capture a flag? Should bullets be able to capture?
		return;

	//Defines
	CBasePlayer *pPlayer = static_cast< CBasePlayer* >(pOther->MyCombatCharacterPointer());

	if (!pPlayer || !FlagEnt || !FlagEnt->m_bActive) //Flag is inactive?
		return;				   //Die here.

	//BaseClass::EndTouch( pOther );

	switch (pPlayer->GetTeamNumber()) //Let's do most of the sorting work here rather than bunching the teams together.
	{
	case TEAM_AMERICANS:
		if ((FlagEnt->m_iForTeam == 1) || (FlagEnt->m_iForTeam == 0))
		{
			FlagEnt->m_vTriggerAmericanPlayers.FindAndRemove(pPlayer); //Remove this player from the american player list.
		}
		break;
	case TEAM_BRITISH:
		if ((FlagEnt->m_iForTeam == 2) || (FlagEnt->m_iForTeam == 0))
		{
			FlagEnt->m_vTriggerBritishPlayers.FindAndRemove(pPlayer); //Remove this player from the british player list.
		}
		break;
	}
}
//-----------------------------------------------------------------------------
// CTF Capture Trigger - HairyPotter - My crowning achievement thus far.
//-----------------------------------------------------------------------------
class CTriggerCTFCapture : public CTriggerMultiple
{
	DECLARE_CLASS(CTriggerCTFCapture, CTriggerMultiple);
	DECLARE_DATADESC();

public:
	void StartTouch(CBaseEntity *pOther);
	void EndTouch(CBaseEntity *pOther);

private:
	int m_iAffectedTeam, iTeam, m_iTeamBonus, m_iPlayerBonus, m_iSound;

	COutputEvent m_OnFlagCaptured;
};


//-----------------------------------------------------------------------------
// Keyfields
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(trigger_ctf_capturepoint, CTriggerCTFCapture);

BEGIN_DATADESC(CTriggerCTFCapture)
DEFINE_KEYFIELD(m_iAffectedTeam, FIELD_INTEGER, "TeamCapture"),
DEFINE_KEYFIELD(m_iTeamBonus, FIELD_INTEGER, "TeamBonus"),
DEFINE_KEYFIELD(m_iPlayerBonus, FIELD_INTEGER, "PlayerBonus"),
DEFINE_KEYFIELD(m_iSound, FIELD_SOUNDNAME, "CaptureSound"),

DEFINE_OUTPUT(m_OnFlagCaptured, "OnFlagCaptured"),
END_DATADESC()


//-----------------------------------------------------------------------------
// Can the player be set as the parent?
//-----------------------------------------------------------------------------
void CTriggerCTFCapture::StartTouch(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer()) //Nothing else should trigger this.
		return;

	//Defines
	CHL2MP_Player *pPlayer = ToHL2MPPlayer(pOther);
	CtfFlag *pFlag = NULL;
	bool m_bHomeFlagTaken = false;

	if (!pPlayer || !pPlayer->IsAlive()) //Still alive?
		return;

	if (sv_ctf_capturestyle.GetInt() > 1)
	{
		while ((pFlag = static_cast<CtfFlag*>(gEntList.FindEntityByClassname(pFlag, "ctf_flag"))) != NULL)
		{
			if (pFlag->iTeam == pPlayer->GetTeamNumber()) //This flag is an enemy flag, we want the player's team's flag.
				continue;

			if (pFlag->GetAbsOrigin() != pFlag->FlagOrigin) //This flag belongs to the player's team. Is it at home?
				m_bHomeFlagTaken = true;
		}
	}


	while ((pFlag = static_cast<CtfFlag*>(gEntList.FindEntityByClassname(pFlag, "ctf_flag"))) != NULL)
	{
		if (pFlag->GetParent() && pFlag->GetParent() == pPlayer) //So the flag has a parent.. and it's the player who touched the trigger.
		{
			int TeamNumber = pPlayer->GetTeamNumber();

			//For team affected by the trigger.
			switch (m_iAffectedTeam)
			{
			case 0:
				iTeam = TeamNumber;
				break;
			case 1:
				iTeam = TEAM_BRITISH;
				break;
			case 2:
				iTeam = TEAM_AMERICANS;
				break;
			}

			if (TeamNumber != iTeam) //Trigger isn't for this player's team. Die.
				return;
			//

			if (m_bHomeFlagTaken) //The player's team's flag is taken. So we cannot cap an enemy flag.
			{
				ClientPrint(pPlayer, CTF_DENY_CAPTURE); //Let the player know.
				return; //Die here.
			}

			//Assuming we've made it thus far, you're probably carrying a flag that belongs to the enemy. Go ahead and cap it.
			pFlag->PlaySound(GetAbsOrigin(), m_iSound); //Play the capture sound.

			pFlag->ResetFlag();

			g_Teams[TeamNumber]->AddScore(m_iTeamBonus); //Adds the team score bonus.
			pPlayer->IncrementFragCount(m_iPlayerBonus); //Give the player the points.

			//reset the capturer's speed
			//pPlayer->SetSpeedModifier(0);
			pPlayer->RemoveSpeedModifier(ESpeedModID::Flag); //BG3 - Awesome - removed to more robust speed modifier system

			pFlag->PrintAlert(CTF_CAPTURE, pPlayer->GetPlayerName(), pFlag->cFlagName);

			//Do the log stuff.
			CTeam *team = pPlayer->GetTeam();

			UTIL_LogPrintf("\"%s<%i><%s><%s>\" triggered \"ctf_flag_capture\"\n",
				pPlayer->GetPlayerName(),
				pPlayer->GetUserID(),
				pPlayer->GetNetworkIDString(),
				team ? team->GetName() : "");
			//

			m_OnFlagCaptured.FireOutput(this, this); //Fire the OnFlagCaptured output, set it last just in case.
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called when an entity stops touching us.
// Input  : pOther - The entity that was touching us.
//-----------------------------------------------------------------------------
void CTriggerCTFCapture::EndTouch(CBaseEntity *pOther)
{
	//BaseClass::EndTouch( pOther );	Just do nothing. Doesn't matter.
}