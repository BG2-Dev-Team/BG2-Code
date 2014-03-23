/*
	Added by HairyPotter for CTF mode. -- Work in progress. 
	And yes, this will only be for flag models. Not various entities and such.
*/

#include "cbase.h"
#include "triggers.h"
#include "ctfflag.h"
#include "hl2mp_gamerules.h"
#include "team.h"
#include "engine/IEngineSound.h"
#include "flag.h"

//ConVar sv_ctf_flagweight ("sv_ctf_flagweight", "0", FCVAR_NOTIFY | FCVAR_GAMEDLL, "How much speed does carrying this flag drain?");
ConVar sv_ctf_returnstyle ("sv_ctf_returnstyle", "1", FCVAR_NOTIFY | FCVAR_GAMEDLL, "Which way is a flag returned? Setting this to '2' will allow teams to return their own flags when they touch them.");
ConVar sv_ctf_capturestyle ("sv_ctf_capturestyle", "1", FCVAR_NOTIFY | FCVAR_GAMEDLL, "Which way is a flag captured Setting this to '2' will not allow you to pick up a flag if your own is not at home.");
ConVar sv_ctf_flagalerts ("sv_ctf_flagalerts", "0", FCVAR_NOTIFY | FCVAR_GAMEDLL, "Print out flag notifications to hud?");

void CtfFlag::Spawn( void )
{
	Precache( );
	SetModel( "models/other/flag.mdl" ); //Always first.

	m_bActive = true;

	if (HasSpawnFlags( CtfFlag_START_DISABLED ))
	{
		m_bActive = false;
		AddEffects( EF_NODRAW );
	}

	BaseClass::Spawn( );
	//cFlagName = m_strName;
	//iTeam = m_iForTeam;

	switch( m_iForTeam )
	{
		case 0:
			m_nSkin = 2;
			//cFlagName = "Neutral";
			iTeam = NULL;
			break;
		case 1: //British Flag
			m_nSkin = 0;
			//cFlagName = "American"; //This is just for the capture text. (Capped American Flag!)
			iTeam = TEAM_BRITISH; 
			break;
		case 2: //American Flag
			m_nSkin = 1; 
			//cFlagName = "British"; //This is just for the capture text. (Capped British Flag!)
			iTeam = TEAM_AMERICANS; 
			break;
	}

	if ( !m_strName )
	{
		switch( iTeam )
		{
		case NULL:
			cFlagName = "Neutral";
			break;
		case TEAM_BRITISH: //This flag is picked up by the brits... so it's actually the american flag.
			cFlagName = "American"; //This is just for the capture text. (Capped American Flag!)
			break;
		case TEAM_AMERICANS: //This flag is picked up by the americans... so it's actually the british flag.
			cFlagName = "British"; //This is just for the capture text. (Capped British Flag!)
			break;
		}
	}
	else
		cFlagName = m_strName; //This is for custom flag names if one has been set. I dunno.. call it a nifty feature? More options for mappers.

	FlagOrigin = GetAbsOrigin();
	FlagAngle = GetAbsAngles();

	int nSequence = LookupSequence( "flag_idle1" );
	if ( nSequence > ACTIVITY_NOT_AVAILABLE )
	{
		SetSequence(nSequence);
		SetCycle( 0 );
		ResetSequence( nSequence );
		ResetClientsideFrame();
	}
	else
	{
		Msg( "Sequence is busted...\n");
		SetSequence( 0 );
	}

	SetThink( &CtfFlag::Think );
	SetNextThink( gpGlobals->curtime );

	//HACKHACK - Set the networked flag name here since it's not going to change after this. -HairyPotter
	n_cFlagName.Set( MAKE_STRING( cFlagName ) ); //Holy shit.
}
void CtfFlag::Precache( void )
{
	PrecacheModel( "models/other/flag.mdl" );
	PrecacheModel( "models/other/flag_nopole.mdl" );
}
void CtfFlag::Think( void )
{
	if ( !m_bActive ) //If it isn't active...
	{
		SetNextThink( gpGlobals->curtime + 1.0f ); //Think Less
		return; //Die here.
	}

	SetNextThink( gpGlobals->curtime + 0.125f );

	StudioFrameAdvance(); //For the flag animation. -HairyPotter

	if ( GetParent() ) //Is a player holding the flag??
		return; //Do nothing, we're all set here.

	else //If there isn't someone holding the flag, we need to search for a potential capturer or perhaps return it.
	{
		if ( m_bFlagIsDropped && gpGlobals->curtime > fReturnTime /*&& sv_ctf_returnstyle.GetInt() == 1*/ ) //Should the flag return when time is up?
			ReturnFlag();

		CHL2MP_Player *pPlayer = NULL;
		while( (pPlayer = ToHL2MPPlayer(gEntList.FindEntityByClassnameWithin( pPlayer, "player", GetLocalOrigin(), m_flPickupRadius ))) != NULL )
		{
			if ( !pPlayer->IsAlive() ) //Dead players cannot pick up the flag.
				continue;
		
			int TeamNumber = pPlayer->GetTeamNumber();
			if ( iTeam != NULL && TeamNumber != iTeam ) //Check to see if the player's team will work for the flag.
			{
				if( sv_ctf_returnstyle.GetInt() > 1 && m_bFlagIsDropped ) //Should the flag return if a friendly touches it?
					ReturnFlag();
	
				continue;
			}
			
			//This will see if our flag is at home. If it's not, we can't take the enemy flag! 
			if ( sv_ctf_capturestyle.GetInt() > 1 ) 
			{
				CtfFlag *pFlag = NULL;
				while( (pFlag = static_cast<CtfFlag*>(gEntList.FindEntityByClassname( pFlag, "ctf_flag" ))) != NULL ) //Check all flags.
				{
					if ( pFlag->iTeam == TeamNumber ) //This flag can be capped by the player's team, we want the other team's flag.
						continue;

					if ( pFlag->GetAbsOrigin() != pFlag->FlagOrigin ) //This flag belongs to the player's team. Is it at home?
					{
						ClientPrint( pPlayer, CTF_DENY_PICKUP ); //Let the player know.
						return; //Die here.
					}
				}
			}
			//

			//Assuming you've passed all the checks, you deserve to pick up the flag. So run the code below.
			SetModel( "models/other/flag_nopole.mdl" ); //Something I came up with. Since it doesn't look lik the player is actually holding the flag, just have it work like an indicator.
			SetAbsAngles( pPlayer->GetAbsAngles() + QAngle( 0,180,0 ) ); // Make sure the flag is flying with the player model, not against it.
			SetAbsOrigin( pPlayer->GetAbsOrigin() + Vector( 0,0,25 ) );	//Keeps the flag out of the player's FOV, also raises it so it doesn't look like it's stuck in the player's grill.
			SetParent( pPlayer );	//Attach the entity to the player.
			m_bIsCarried = true;

			//For the player speed difference.
			float scale = 1;

			switch( pPlayer->GetClass() )
			{
				case CLASS_INFANTRY:
					scale = 1;
					break;
				case CLASS_OFFICER:
					scale = 1.6f;
					break;
				case CLASS_SNIPER:
					scale = 1.2f;
					break;
				case CLASS_SKIRMISHER:
					scale = 1.1;
					break;
				case CLASS_LIGHT_INFANTRY:
					scale = 1.05;
					break;
			}

			//decrease the carrier's speed
			pPlayer->SetSpeedModifier( -m_iFlagWeight * scale );

			m_bFlagIsDropped = false; //So it doesn't return while you're carrying it!
			PrintAlert( CTF_PICKUP, pPlayer->GetPlayerName(), cFlagName );
			PlaySound( GetAbsOrigin(), m_iPickupSound );
			m_OnPickedUp.FireOutput( this, this ); //Fire the OnPickedUp output.
		}
	}
}
void CtfFlag::PrintAlert( char *Msg, const char * PlayerName, char * FlagName )
{
	if ( !PlayerName )
		Q_snprintf( CTFMsg, 512, Msg, FlagName );
	else 
		Q_snprintf( CTFMsg, 512, Msg, PlayerName, FlagName );

	switch( sv_ctf_flagalerts.GetInt() )
	{
		case 1:
			UTIL_ClientPrintAll( HUD_PRINTCENTER, CTFMsg );
			break;
		case 2:
			UTIL_ClientPrintAll( HUD_PRINTTALK, CTFMsg );
			break;
	}
}

void CtfFlag::PrintAlert( int msg_type, const char * param1, const char * param2 )
{
	switch( sv_ctf_flagalerts.GetInt() )
	{
		case 1:
			ClientPrintAll( msg_type, HUD_PRINTCENTER, param1, param2 );
			break;
		case 2:
			ClientPrintAll( msg_type, HUD_PRINTTALK, param1, param2 );
			break;
	}
}
void CtfFlag::DropFlag( void ) //This function now fires precisely when a player dies / changes teams / disconnects, 
{							   //this will fix a bug where a player can spawn within 1/8th of a second and still hold the flag after dying.
	CBasePlayer *pPlayer = static_cast< CBasePlayer* >( GetParent() );
	if ( !pPlayer )
		return;

	SetModel( "models/other/flag.mdl" );
	SetParent( NULL );
	SetAbsOrigin( GetAbsOrigin() - Vector( 0,0,25 ) ); //HACKHACK: Bring the flag down to it's original height.
	m_bFlagIsDropped = true;
	fReturnTime = gpGlobals->curtime + m_fReturnTime;
	PrintAlert( CTF_DROP, pPlayer->GetPlayerName(), cFlagName );
	PlaySound( GetAbsOrigin(), m_iDropSound );
	m_OnDropped.FireOutput( this, this ); //Fire the OnDropped output.
}
void CtfFlag::ReturnFlag( void )
{
	PlaySound( GetAbsOrigin(), m_iReturnSound );
	PrintAlert( CTF_RETURNED, cFlagName );
	ResetFlag();
	m_OnReturned.FireOutput( this, this ); //Fire the OnReturned output.
}
void CtfFlag::ResetFlag( void )
{
	SetParent( NULL );
	m_bFlagIsDropped = false;
	m_bIsCarried = false;
	SetModel( "models/other/flag.mdl" );
	SetAbsOrigin( FlagOrigin );
	SetAbsAngles( FlagAngle );
}
void CtfFlag::PlaySound( Vector origin, int sound )
{
	if ( sound == NULL ) //No sound? Just stop right there.
		return;

	char *SoundFile = (char *)sound;
	CRecipientFilter recpfilter;
	//recpfilter.AddAllPlayers();
	recpfilter.AddRecipientsByPAS( origin ); //Instead, let's send this to players that are at least close enough to hear it.. -HairyPotter
	recpfilter.MakeReliable();
	UserMessageBegin( recpfilter, "CaptureSounds" );
		WRITE_VEC3COORD( origin );
		WRITE_STRING( SoundFile );
	MessageEnd();
}

//Inputs Below ------------------------------------------------------------
void CtfFlag::InputReset( inputdata_t &inputData )
{
	if ( GetAbsOrigin() == FlagOrigin )
		return;

	ReturnFlag();
}
void CtfFlag::InputEnable( inputdata_t &inputData )
{
	RemoveEffects( EF_NODRAW );
	m_bActive = true;
	m_OnEnable.FireOutput( inputData.pActivator, this );

	ReturnFlag();
}
void CtfFlag::InputDisable( inputdata_t &inputData )
{
	AddEffects( EF_NODRAW );
	m_bActive = false;
	m_OnDisable.FireOutput( inputData.pActivator, this );

	ReturnFlag();
}
void CtfFlag::InputToggle( inputdata_t &inputData )
{
	if (m_bActive)
		InputDisable( inputData );
	else
		InputEnable( inputData );
}
IMPLEMENT_NETWORKCLASS_ALIASED( tfFlag, DT_CtfFlag ) //Doesn't need the "C" in front of it? Mmmmkay...
//-------------------------------------------------------------------------

BEGIN_NETWORK_TABLE( CtfFlag, DT_CtfFlag )
	SendPropBool( SENDINFO( m_bIsCarried ) ),
	SendPropInt( SENDINFO( m_iForTeam ), 2, SPROP_UNSIGNED ),
	SendPropStringT( SENDINFO( n_cFlagName ) ),
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CtfFlag )
END_PREDICTION_DATA()

BEGIN_DATADESC( CtfFlag )

	DEFINE_KEYFIELD( m_flPickupRadius, FIELD_FLOAT, "PickupRadius" ),
	DEFINE_KEYFIELD( m_iForTeam, FIELD_INTEGER, "ForTeam" ),
	DEFINE_KEYFIELD( m_iFlagWeight, FIELD_INTEGER, "FlagWeight" ),
	DEFINE_KEYFIELD( m_fReturnTime, FIELD_FLOAT, "ReturnTime" ),
	DEFINE_KEYFIELD( m_iPickupSound, FIELD_SOUNDNAME, "PickupSound" ),
	DEFINE_KEYFIELD( m_iDropSound, FIELD_SOUNDNAME, "DropSound" ),
	DEFINE_KEYFIELD( m_iReturnSound, FIELD_SOUNDNAME, "ReturnSound" ),
	DEFINE_KEYFIELD( m_strName,	FIELD_STRING,	"Name" ), //I don't know... I guess if you wanted to call it something other than British/American flag?

	DEFINE_THINKFUNC( Think ),

	DEFINE_OUTPUT( m_OnDropped, "OnDropped" ),
	DEFINE_OUTPUT( m_OnPickedUp, "OnPickedUp" ),
	DEFINE_OUTPUT( m_OnReturned, "OnReturned" ),
	DEFINE_OUTPUT( m_OnEnable, "OnEnabled" ),
	DEFINE_OUTPUT( m_OnDisable, "OnDisabled" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Reset", InputReset ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle", InputToggle ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( ctf_flag, CtfFlag);
PRECACHE_REGISTER(ctf_flag);
