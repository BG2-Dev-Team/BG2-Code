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

#include "cbase.h"
#include "triggers.h"
#include "flag.h"
#include "hl2mp_player.h"
#include "hl2mp_gamerules.h"
#include "gamerules.h"
#include "team.h"
#include "engine/IEngineSound.h"
#include "bg2/spawnpoint.h"
#include "../shared/bg3/bg3_buffs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//#ifndef CLIENT_DLL
//BG2 - Tjoppen - just make these global so we can reset them
float	flNextClientPrintAll = 0;
bool	bNextClientPrintAllForce = false;
extern bool		g_fGameOver;

void ClientPrintAll( char *str, bool printfordeadplayers, bool forcenextclientprintall )
{
	if( !str )
		return;

	if( flNextClientPrintAll > gpGlobals->curtime && (!printfordeadplayers || bNextClientPrintAllForce) && !forcenextclientprintall )
		return;

	flNextClientPrintAll = gpGlobals->curtime + 1.0f;
	bNextClientPrintAllForce = forcenextclientprintall;

	CBasePlayer *pPlayer = NULL;

	while( (pPlayer = dynamic_cast<CBasePlayer*>(gEntList.FindEntityByClassname( pPlayer, "player" ))) != NULL )
	{
		if( !pPlayer->IsAlive() && !printfordeadplayers )	//this doesn't concern dead players
			continue;

		ClientPrint( pPlayer, HUD_PRINTCENTER, str );
	}
}

void ClientPrintAll( int msg_type, int msg_dest, const char * param1, const char * param2 )
{
	CReliableBroadcastRecipientFilter filter;

	UserMessageBegin( filter, "BG2Events" );
		WRITE_BYTE( msg_type );
		WRITE_BYTE( msg_dest );

		if ( param1 )
			WRITE_STRING( param1 );
		else
			WRITE_STRING( "" );

		if ( param2 )
			WRITE_STRING( param2 );
		else
			WRITE_STRING( "" );
	MessageEnd();
}

void ClientPrint( CBasePlayer *pPlayer, int msg_type, int msg_dest, const char * param1, const char * param2 )
{
	if ( !pPlayer )
		return;

	CSingleUserRecipientFilter user( pPlayer );
	user.MakeReliable();

	UserMessageBegin( user, "BG2Events" );
		WRITE_BYTE( msg_type );
		WRITE_BYTE( msg_dest );

		if ( param1 )
			WRITE_STRING( param1 );
		else
			WRITE_STRING( "" );

		if ( param2 )
			WRITE_STRING( param2 );
		else
			WRITE_STRING( "" );
	MessageEnd();
}
//#endif

/*===========================================================================================


			T	H	E			P	O	I	N	T			F	L	A	G

A model that is used to display the status of the point and show players where it is.
===========================================================================================*/

//BG2 - SaintGreg - dynamic flag settings
//input functions
bool CFlag::IsActive( void )
{
	return m_bActive;
}

void CFlag::InputEnable( inputdata_t &inputData )
{
	if( m_bActive )
		return;

	//char msg[512];
	//Q_snprintf( msg, 512, "flag(%s) has been enabled", STRING( GetEntityName() ) );
	//ClientPrintAll( msg, true, true ); // saying whether it is enabled or not is important, so force

	m_bActive = true;
	ChangeTeam( TEAM_UNASSIGNED );
	m_iLastTeam = TEAM_UNASSIGNED;
	m_nSkin = GetNeutralSkin();
	m_OnEnable.FireOutput( inputData.pActivator, this );
	Think(); // think immediately and restart the thinking cycle
}
void CFlag::InputDisable( inputdata_t &inputData )
{
	if( !m_bActive )
		return;

	//char msg[512];
	//Q_snprintf( msg, 512, "flag(%s) has been disabled", STRING( GetEntityName() ) );
	//ClientPrintAll( msg, true, true ); // saying whether it is enabled or not is important, so force

	m_bActive = false;

	HandleLoseOutputs();

	m_OnDisable.FireOutput( inputData.pActivator, this );
}
void CFlag::InputToggle( inputdata_t &inputData )
{
	if (m_bActive)
		InputDisable( inputData );
	else
		InputEnable( inputData );
}

void CFlag::InputForceCap( inputdata_t &inputData )
{
	int m_iForceCap = inputData.value.Int();

	if ( !m_bNotUncappable ) //If this is true, the outputs will be called in the Capture function. No need to call them twice.
		HandleLoseOutputs(); //Takes care of the OnLosePoint outputs.

	switch( m_iForceCap )
	{
		case 1:
			Capture( TEAM_AMERICANS );
			break;
		case 2:
			Capture( TEAM_BRITISH );
			break;
	}
}

void CFlag::HandleLoseOutputs( void )
{
	switch ( GetTeamNumber() )
	{
		case TEAM_AMERICANS:
			m_OnAmericanLosePoint.FireOutput( this, this );
			break;
		case TEAM_BRITISH:
			m_OnBritishLosePoint.FireOutput( this, this );
			break;
		case TEAM_UNASSIGNED: //Neutral flags aren't "Lost"
			return;
	}

	m_OnLosePoint.FireOutput( this, this );
}

void CFlag::Spawn( void )
{
	Precache( );

	if ( !m_bInvisible ) 
	{
		SetModel( "models/other/flag.mdl" ); //Always first.
		// BG2 - This is all for finding the animation for the flag model. -HairyPotter
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
			Msg( "Flag sequence is busted...\n");
			SetSequence( 0 );
		}
	//
	}

	if ( !m_iCaptureSound )
		SoundFile = "Flag.capture";
	else
		SoundFile = (char *)m_iCaptureSound;

	//if ( !m_sCaptureSound )
		//n_cFlagName.Set( MAKE_STRING( cFlagName ) );
		//m_iCaptureSound = MAKE_STRING(  );

	m_iSavedHUDSlot = m_iHUDSlot;

	if (HasSpawnFlags( CFlag_START_DISABLED ))
	{
		m_bActive = false;
		//m_nSkin = GetDisabledSkin(); Disabled flags are invisible anyway.
	}
	else
	{
		m_bActive = true;
		//ChangeTeam( TEAM_UNASSIGNED );	//ChangeTeam handles everything..
		switch( m_iStartingTeam ) //The team that gets the flag at the start of the map.
		{
			case 1:
				ChangeTeam( TEAM_AMERICANS );
				break;
			case 2:
				ChangeTeam( TEAM_BRITISH );
				break;
			default:
				ChangeTeam( TEAM_UNASSIGNED );
				break;
		}
	}

	m_iRequestingCappers = TEAM_UNASSIGNED;
	m_iLastTeam = TEAM_UNASSIGNED;

	BaseClass::Spawn( );

	SetThink( &CFlag::Think );
	SetNextThink( gpGlobals->curtime );
}
void CFlag::Precache( void )
{
	PrecacheModel( "models/other/flag.mdl" );

	//PrecacheScriptSound( "Flag.capture" );
}

void CFlag::Think( void )
{
	/*
	1.0F style flags

	when not capped, they behave just like the old flags
	- wait for m_iCapturePlayers(of only one team?) to stand near it for m_flCaptureTime seconds
	- after said time, flag is captured by that team
	- a held flag will periodically award the team with some amount of points

	the difference now is:
	- when all the people who captured the flag die, it's uncapped(tweak this to something other than zero players?)
	- more than m_iCapturePlayers players can hold the flag by touching it after it has been captured(overloading)
	- any enemy walking into a capture zone without a friendly player inside guarding the flag, will uncap the flag

	*/

	extern ConVar mp_respawnstyle;
	if ( !m_bActive || IsLMS() )
	{
		ChangeTeam( TEAM_UNASSIGNED );
		m_iLastTeam = TEAM_UNASSIGNED;
		AddEffects( EF_NODRAW );
		m_iHUDSlot = -1; //Make it disappear from the hud as well.
		SetNextThink( gpGlobals->curtime + 1.25f ); //No need to think soo much. 
		return; //Die here
	}

	//For the flag animation. Yes, It's a sort of hack, but the frame can only be advanced on a think.. Animation is 8 FPS, so must think 8 times a second. -HairyPotter
	//SetNextThink( gpGlobals->curtime + 0.5f );
	SetNextThink( gpGlobals->curtime + 0.125f );
	if ( !m_bInvisible )
		StudioFrameAdvance();
	//

	if ( IsEffectActive( EF_NODRAW ) ) //If you've come this far into the code you're definately not disabled, just make sure you're visible.
	{
		m_iHUDSlot = m_iSavedHUDSlot;
		RemoveEffects( EF_NODRAW );
	}


	//award any time bonii
	//don't give score in ticket mode
	if ( !g_fGameOver && !HL2MPRules()->UsingTickets() )
	{
		//The game hasn't ended yet, keep adding points to the team.
		switch( GetTeamNumber() )
		{
			case TEAM_AMERICANS:
				if (m_iTeamBonusInterval != 0)
				{
					if ((m_flNextTeamBonus <= gpGlobals->curtime) && (m_iForTeam != 1))
					{
						g_Teams[TEAM_AMERICANS]->AddScore( m_iTeamBonus );
						m_flNextTeamBonus += m_iTeamBonusInterval;
					}
				}
				break;
			case TEAM_BRITISH:
				if (m_iTeamBonusInterval != 0)
				{
					if ((m_flNextTeamBonus <= gpGlobals->curtime) && (m_iForTeam != 2))
					{
						g_Teams[TEAM_BRITISH]->AddScore( m_iTeamBonus );
						m_flNextTeamBonus += m_iTeamBonusInterval;
					}
				}
				break;
			default:
				switch (m_iForTeam)
				{
					case 1://amer
						if (m_iTeamBonusInterval != 0)
						{
							if (m_flNextTeamBonus <= gpGlobals->curtime)
							{
								g_Teams[TEAM_BRITISH]->AddScore( m_iTeamBonus );
								m_flNextTeamBonus += m_iTeamBonusInterval;
							}
						}
						break;
					case 2://brit
						if (m_iTeamBonusInterval != 0)
						{
							if (m_flNextTeamBonus <= gpGlobals->curtime)
							{
								g_Teams[TEAM_AMERICANS]->AddScore( m_iTeamBonus );
								m_flNextTeamBonus += m_iTeamBonusInterval;
							}
						}
						break;
				}
		}
	}

	//safeguard against the possibility that m_flNextTeamBonus is somehow out of bounds after above check
	//could happen if the flag stays uncapped for a long time. if so, the team that caps it would get a lot of "stored up"
	//points.
	if (m_flNextTeamBonus <= gpGlobals->curtime)
		m_flNextTeamBonus = gpGlobals->curtime + m_iTeamBonusInterval;

	if( GetTeamNumber() != TEAM_UNASSIGNED )
		ThinkCapped();	//don't need to check for overloading or uncapping on uncapped flags

	ThinkUncapped();	//always run this, so non-uncappable flags can be taken
}

//this is actually the old think funtion, slightly modified
void CFlag::ThinkUncapped( void )
{
	CBasePlayer *pPlayer = NULL;
	
	if ( !m_bIsParent )
	{
		americans = 0;
		british = 0;

		while( (pPlayer = static_cast<CBasePlayer*>(gEntList.FindEntityByClassnameWithin( pPlayer, "player", GetLocalOrigin(), m_flCaptureRadius ))) != NULL )
		{
			if( !pPlayer->IsAlive() )	//dead players don't cap
				continue;
	
			switch( pPlayer->GetTeamNumber() )
			{
			case TEAM_AMERICANS:
				if ((m_iForTeam == 1) || (m_iForTeam == 0))
				{
					americans++;
				}
				break;
			case TEAM_BRITISH:
				if ((m_iForTeam == 2) || (m_iForTeam == 0))
				{
					british++;
				}
				break;
			default:
				break;
			}
		}
	}
	else //The trigger is talking to this flag.
	{
		americans = 0;
		british = 0;

		americans = m_vTriggerAmericanPlayers.Count();
		british = m_vTriggerBritishPlayers.Count();
	} //

	//char *msg = NULL;

	//default number in flag indicator
	m_iNearbyPlayers = m_vOverloadingPlayers.Count();

	if( americans + british > 0 && (americans <= 0 || british <= 0) )
	{
		//Msg( "\namericans = %i  british = %i\n", americans, british );

		//only americans or british at the flag
		//if we don't already own it, and we've been here for at least three seconds - capture

		//Yeah that's right, apparently Linux is anal about Const references. -HairyPotter
		const int BritishTeam = TEAM_BRITISH; 
		const int AmericanTeam = TEAM_AMERICANS; 
		//

		if( americans > 0 && GetTeamNumber() != TEAM_AMERICANS )
		{
			m_iRequestingCappers = AmericanTeam; //FIXME: Linux bitches about this, don't ask me why. -HairyPotter
			m_iNearbyPlayers = americans;

			if (americans >= min( m_iCapturePlayers, g_Teams[TEAM_AMERICANS]->GetNumPlayers() ) )
			{
				//Msg( "americans\n" );
				if( m_iLastTeam != TEAM_AMERICANS )
				{
					//char msg2[512];
					//Msg( "americans are capturing a flag(\"%s\")\n", STRING( m_sFlagName.Get() ) );
					//Q_snprintf( msg2, 512, "The americans are capturing a flag(%s)", STRING( m_sFlagName.Get() ) );
					//msg = msg2;
					m_iLastTeam = AmericanTeam; //FIXME: Linux bitches about this, don't ask me why. -HairyPotter
					m_flNextCapture = gpGlobals->curtime + m_flCaptureTime;

					m_OnAmericanStartCapture.FireOutput( this, this );
					m_OnStartCapture.FireOutput( this, this );
				}
				else if( gpGlobals->curtime >= m_flNextCapture )
				{
					//char msg2[512];
					//CFlagHandler::PlayCaptureSound();
					//Q_snprintf( msg2, 512, "The americans captured a flag(%s)", STRING( m_sFlagName.Get() ) );
					//msg = msg2;

					Capture( TEAM_AMERICANS );
				}
			}
			else //This simply prevents he one man cap exploit. Sure there may still be an american nearby, but if it's under required amount; why continue the cap?
			{
				m_iLastTeam = TEAM_UNASSIGNED;
				m_flNextCapture = 0;
			} //
		}
		else if( british > 0 && GetTeamNumber() != TEAM_BRITISH )
		{

			m_iRequestingCappers = BritishTeam; //FIXME: Linux bitches about this, don't ask me why. -HairyPotter
			m_iNearbyPlayers = british;

			if (british >= min( m_iCapturePlayers, g_Teams[TEAM_BRITISH]->GetNumPlayers() ) )
			{
				//Msg( "british\n" );
				if( m_iLastTeam != TEAM_BRITISH )
				{
					//char msg2[512];
					//Msg( "british are capturing a flag(\"%s\")\n", STRING( m_sFlagName.Get() ) );
					//Q_snprintf( msg2, 512, "The british are capturing a flag(%s)", STRING( m_sFlagName.Get() ) );
					//msg = msg2;
					m_iLastTeam = BritishTeam; //FIXME: Linux bitches about this, don't ask me why. -HairyPotter
					m_flNextCapture = gpGlobals->curtime + m_flCaptureTime;

					m_OnBritishStartCapture.FireOutput( this, this );
					m_OnStartCapture.FireOutput( this, this );
				}
				else if( gpGlobals->curtime >= m_flNextCapture )
				{
					//char msg2[512];
					//CFlagHandler::PlayCaptureSound();
					//Q_snprintf( msg2, 512, "The british captured a flag(%s)", STRING( m_sFlagName.Get() ) );
					//msg = msg2;

					Capture( TEAM_BRITISH );
				}
			}
			else //This simply prevents he one man cap exploit. Sure there may still be a brit nearby, but if it's under required amount; why continue the cap?
			{
				m_iLastTeam = TEAM_UNASSIGNED;
				m_flNextCapture = 0;
			} //
		}
	}
	else
	{
		if( m_iLastTeam != TEAM_UNASSIGNED && GetTeamNumber() != m_iLastTeam )
		{
			//Msg( "stopped capturing a flag\n" );
			if( m_iLastTeam == TEAM_AMERICANS )
			{
				//char msg2[512];
				//Q_snprintf( msg2, 512, "The americans stopped capturing a flag(%s)", STRING( m_sFlagName.Get() ) );
				//msg = msg2;

				m_OnAmericanStopCapture.FireOutput( this, this );
				m_OnStopCapture.FireOutput( this, this );
			}
			else if( m_iLastTeam == TEAM_BRITISH )
			{
				//char msg2[512];
				//Q_snprintf( msg2, 512, "The british stopped capturing a flag(%s)", STRING( m_sFlagName.Get() ) );
				//msg = msg2;

				m_OnBritishStopCapture.FireOutput( this, this );
				m_OnStopCapture.FireOutput( this, this );
			}
		}
		//noone here
		m_iLastTeam = TEAM_UNASSIGNED;
		m_iRequestingCappers = TEAM_UNASSIGNED;
		//m_iNearbyPlayers = 0;
		
		m_flNextCapture = 0;
	}

	//ClientPrintAll( msg );
}

void CFlag::Capture( int iTeam )
{
	//iTeam is either americans or british
	//this function handles a lot of the capture related stuff
	//CBasePlayer *pPlayer = NULL;
	CHL2MP_Player *pHL2Player = NULL;

	CRecipientFilter recpfilter;
	//recpfilter.AddAllPlayers();
	recpfilter.AddRecipientsByPAS( GetAbsOrigin() ); //Instead, let's send this to players that are at least close enough to hear it.. -HairyPotter
	recpfilter.MakeReliable();
	
	UserMessageBegin( recpfilter, "CaptureSounds" );
		WRITE_VEC3COORD( GetAbsOrigin() );
		WRITE_STRING( SoundFile );
	MessageEnd();

	//don't give score in ticket mode
	if ( !HL2MPRules()->UsingTickets() )
		g_Teams[iTeam]->AddScore( m_iTeamBonus );

	m_flNextTeamBonus = (gpGlobals->curtime + m_iTeamBonusInterval);

	//award capping players some points and put them on the overload list
	m_vOverloadingPlayers.RemoveAll();

	if ( !m_bIsParent )
	{
		while( (pHL2Player = static_cast<CHL2MP_Player*>(gEntList.FindEntityByClassnameWithin( pHL2Player, "player", GetLocalOrigin(), m_flCaptureRadius ))) != NULL )
		{
			if( !pHL2Player->IsAlive() )	//dead players don't cap
				continue;

			if( pHL2Player->GetTeamNumber() == iTeam )
			{
				pHL2Player->IncrementFragCount(m_iPlayerBonus);
				//BG2 - Tjoppen - rewards put on hold
				//pPlayer->IncreaseReward(1);
				m_vOverloadingPlayers.AddToHead( pHL2Player );
			}
		}
	}
	else //Flag has a trigger.
	{
		switch( iTeam )
		{
			case TEAM_AMERICANS:
				for ( int i = 0; i < m_vTriggerAmericanPlayers.Count(); i++ )
				{
					pHL2Player = ToHL2MPPlayer( UTIL_PlayerByIndex( i ) );

					if ( !pHL2Player || !pHL2Player->IsAlive() ) //Just to make sure.
						continue;

					pHL2Player->IncrementFragCount(m_iPlayerBonus);
					m_vOverloadingPlayers.AddToHead( pHL2Player );
				}

				break;
			case TEAM_BRITISH:
				for ( int i = 0; i < m_vTriggerBritishPlayers.Count(); i++ )
				{
					pHL2Player = ToHL2MPPlayer( UTIL_PlayerByIndex( i ) );

					if ( !pHL2Player || !pHL2Player->IsAlive() ) //Just to make sure.
						continue;

					pHL2Player->IncrementFragCount(m_iPlayerBonus);
					m_vOverloadingPlayers.AddToHead( pHL2Player );
				}
				break;
		}
	}

	m_iLastTeam = TEAM_UNASSIGNED;
	m_iRequestingCappers = TEAM_UNASSIGNED;
	m_iNearbyPlayers = m_vOverloadingPlayers.Count();

	if ( m_bNotUncappable ) //This is here because when NotUncappable is true, a flag is never neutral. 
		HandleLoseOutputs();	//And therefore can only be "Lost" when it's fully captured.
		

//#ifndef _DEBUG //For whatever reason, debug compiles crash on this code. We'll just omit this if we're debugging.
#if 0 //BG3 - TODO sort out the declaration of pPlayer - Awesome
	//BG2 - Added for HlstatsX Support. -HairyPotter
	const char *team = iTeam == TEAM_AMERICANS ? "Americans" : "British";
	char playerinfo[3072] = "";
	char playerdata[256] = "";

	while( (pPlayer = static_cast<CBasePlayer*>(gEntList.FindEntityByClassname( pPlayer, "player" ))) != NULL )
	{
		if( !pPlayer->IsAlive() || pPlayer->GetTeamNumber() != iTeam )
			continue;

		if( m_vOverloadingPlayers.Find( pPlayer ) != -1 ) //So we're actually overloading this flag.
		{
			CTeam *Cteam = pPlayer->GetTeam();

			//Get all the info from this player.
			Q_snprintf( playerdata, sizeof(playerdata), "(player \"%s<%i><%s><%s>\") ",
				pPlayer->GetPlayerName(), 
				pPlayer->GetUserID(), 
				pPlayer->GetNetworkIDString(), 
				Cteam ? Cteam->GetName() : "" );
			//

			strcat( playerinfo, playerdata );// Bunch it together with other play info so we can print out the final string.
		}
	}
	//Only a team can be logged for regular flags, mostly because multiple people are often required to cap.
	UTIL_LogPrintf( "Team \"%s\" triggered \"flag_capture\" (flagname \"%s\") (numplayers \"%i\") %s\n", team, STRING( m_sFlagName.Get() ), m_iNearbyPlayers, playerinfo ); 
	//
#endif

	ChangeTeam( iTeam );

	switch( iTeam )
	{
		case TEAM_AMERICANS:
			m_OnAmericanCapture.FireOutput( this, this );
			break;
		case TEAM_BRITISH:
			m_OnBritishCapture.FireOutput( this, this );
			break;
	}

	m_OnCapture.FireOutput( this, this );
}

void CFlag::ThinkCapped( void )
{
	//check if anyone's overloading or uncapping this flag
	//start by counting people near the flag

	CBasePlayer *pPlayer = NULL;

	if ( !m_bIsParent )
	{
		friendlies = 0,
		enemies = 0;

		while( (pPlayer = static_cast<CBasePlayer*>(gEntList.FindEntityByClassnameWithin( pPlayer, "player", GetLocalOrigin(), m_flCaptureRadius ))) != NULL )
		{
			if( !pPlayer->IsAlive() )	//dead players don't cap
				continue;

			//BG2 - Tjoppen - TODO: m_iForTeam troubles?

			if( pPlayer->GetTeamNumber() == GetTeamNumber() )
			{
				friendlies++;
				if( m_vOverloadingPlayers.Find( pPlayer ) == -1 )
				{
					//friendly player that's not on the list. add
					//BG2 - Tjoppen - TODO: add some sort of bonus for overloading?
					m_vOverloadingPlayers.AddToHead( pPlayer );
				}
			}
			else
			{
				switch( pPlayer->GetTeamNumber() )
				{
					case TEAM_AMERICANS:
						if ((m_iForTeam == 1) || (m_iForTeam == 0))
							enemies++;
						break;
					case TEAM_BRITISH:
						if ((m_iForTeam == 2) || (m_iForTeam == 0))
							enemies++;
						break;
					default:
						break;
				}
			}
		}
	} 
	else //The trigger is talking to this flag.
	{
		friendlies = 0,
		enemies = 0;

		switch( GetTeamNumber() )
		{
			case TEAM_AMERICANS:
				for ( int i = 0; i < m_vTriggerAmericanPlayers.Count(); i++ )
				{
					pPlayer = ToBasePlayer( UTIL_PlayerByIndex( i ) );

					if ( !pPlayer )
						continue;
					
					if( m_vOverloadingPlayers.Find( pPlayer ) == -1 )
					{
						//friendly player that's not on the list. add
						//BG2 - Tjoppen - TODO: add some sort of bonus for overloading?
						m_vOverloadingPlayers.AddToHead( pPlayer );
					}
				}
				friendlies = m_vTriggerAmericanPlayers.Count();
				enemies = m_vTriggerBritishPlayers.Count();

				break;
			case TEAM_BRITISH:
				for ( int i = 0; i < m_vTriggerBritishPlayers.Count(); i++ )
				{
					pPlayer = ToBasePlayer( UTIL_PlayerByIndex( i ) );

					if ( !pPlayer )
						continue;

					if( m_vOverloadingPlayers.Find( pPlayer ) == -1 )
					{
						//friendly player that's not on the list. add
						//BG2 - Tjoppen - TODO: add some sort of bonus for overloading?
						m_vOverloadingPlayers.AddToHead( pPlayer );
					}
				}
				friendlies = m_vTriggerBritishPlayers.Count();
				enemies = m_vTriggerAmericanPlayers.Count();

				break;
		}
	}//


	//if someone steals the flag, or we run out of holders - uncap
	//don't uncap flags that aren't non-uncappable
	if( !m_bNotUncappable && (enemies > 0 && friendlies <= 0 || (m_bUncapOnDeath && m_vOverloadingPlayers.Count() <= 0)) )
	{
		//uncap
		HandleLoseOutputs(); //Remember to call this BEFORE changing the team.
		ChangeTeam( TEAM_UNASSIGNED );
		m_iNearbyPlayers = 0;
		m_vOverloadingPlayers.RemoveAll();
	}
	else
	{
		//we're safe
		m_iNearbyPlayers = m_vOverloadingPlayers.Count();
	}
}
void CFlag::ResetFlag( void )
{
	switch( m_iStartingTeam ) //The team that gets the flag at the start of the map.
	{
		case 1:
			ChangeTeam( TEAM_AMERICANS );
			break;
		case 2:
			ChangeTeam( TEAM_BRITISH );
			break;
		default:
			ChangeTeam( TEAM_UNASSIGNED );
			break;
	}

	m_iLastTeam = TEAM_UNASSIGNED;
	m_iRequestingCappers = TEAM_UNASSIGNED;
	m_vOverloadingPlayers.RemoveAll();
	m_vTriggerBritishPlayers.RemoveAll();
	m_vTriggerAmericanPlayers.RemoveAll();
	m_iNearbyPlayers = 0;

	if ( HasSpawnFlags( CFlag_START_DISABLED ) )
		m_bActive = false;
	else
		m_bActive = true;
}

void CFlag::ChangeTeam( int iTeamNum )
{
	if ( !m_bInvisible )
	{
		switch( iTeamNum )
		{
		case TEAM_AMERICANS:
			m_nSkin = GetAmericanSkin();
			break;
		case TEAM_BRITISH:
			m_nSkin = GetBritishSkin();
			break;
		default:
			switch( m_iForTeam )
			{
				case 1://amer
					m_nSkin = GetBritishSkin();
					break;
				case 2://brit
					m_nSkin = GetAmericanSkin();
					break;
				default:
					m_nSkin = GetNeutralSkin();
					break;
			}
			break;
		}
	}

	BaseClass::ChangeTeam( iTeamNum );

	//set out rally buff
	int buffIndex = GetTeamNumber() == TEAM_AMERICANS ? m_iAmericanBuff : m_iBritishBuff;
	if (buffIndex > 0 && buffIndex < 5) {
		const int buffs[] = {
			0,
			VCOMM2_ADVANCE,
			VCOMM2_RALLY_ROUND,
			VCOMM2_FIRE,
			VCOMM2_RETREAT
		};
		int buff = buffs[buffIndex];
		BG3Buffs::RallyRequest(buff, this, m_flCaptureRadius);
	}
}

int CFlag::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}

IMPLEMENT_NETWORKCLASS_ALIASED( Flag, DT_Flag )

int SendProxyArrayLength_IsOverloading( const void *pStruct, int objectID )
{
	//BG2 - Tjoppen - TODO: only send as many bits as there are players connected?
	return gpGlobals->maxClients;
}

void SendProxy_IsOverloading_Bit( const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID )
{
	//each bit corresponds to player #id overloading current flag or not
	CFlag *pFlag = (CFlag*)pStruct;
	
	if( pFlag )
	{
		//see if iElement has corresponding client id overloading pFlag
		for( int x = 0; x < pFlag->m_vOverloadingPlayers.Count(); x++ )
			if( pFlag->m_vOverloadingPlayers[x]->GetClientIndex() == iElement )
			{
				pOut->m_Int = 1;
				return;
			}

		pOut->m_Int = 0;
	}
	else
	{
		pOut->m_Int = 0;
	}
}

BEGIN_NETWORK_TABLE( CFlag, DT_Flag )
	SendPropInt( SENDINFO( m_iLastTeam ), Q_log2(NUM_TEAMS), SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_iRequestingCappers ), Q_log2(NUM_TEAMS), SPROP_UNSIGNED ),
	SendPropFloat( SENDINFO( m_flNextCapture ) ),
	SendPropInt( SENDINFO( m_iCapturePlayers ), Q_log2(MAX_PLAYERS), SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_iNearbyPlayers ), Q_log2(MAX_PLAYERS), SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_iForTeam ), 2, SPROP_UNSIGNED ),
	SendPropFloat( SENDINFO( m_flCaptureTime ) ),
	SendPropStringT( SENDINFO( m_sFlagName ) ),
	SendPropInt( SENDINFO( m_iHUDSlot ), 5 ),	//15 slots.. 0 = sequential tile, -1 = hidden(don't draw)
	//SendPropBool( SENDINFO( m_bActive ) ), //Tweaked the hudslots to not show anyway. Why send excess shit over the network when it does nothing?
	SendPropBool( SENDINFO( m_bNotUncappable ) ),
	SendPropBool( SENDINFO( m_bUncapOnDeath ) ),
	
	//each bit corresponds to player #id overloading current flag or not
	//BG2 - Tjoppen - TODO: is there a way to set a specific bit depending on who the recipient is?
	//						At the moment bandwidth usage for this is O(N²) instead of O(N) for N clients.
	//						I'd like to send to each client only one bit indicating if that client is
	//						overloading the current flag or not, instead of sending a bitmask of all client's
	//						overload state to all clients.. The current method reveals too much information
	//						for any would-be cheater on the opposing team. Or perhaps it's just good because
	//						we can figure out the names of the people overloading the flag and print them.
	//						This depends on gameplay stuff - do we want/need everyone knowing which flags
	//						everyone else is overloading? Perhaps.
	//BG2 - Tjoppen - TODO: implement this on client, with drawing in hud etc.
	SendPropArray2( 
		SendProxyArrayLength_IsOverloading,
		SendPropInt("IsOverloading_Bit", 0, SIZEOF_IGNORE, 1, SPROP_UNSIGNED, SendProxy_IsOverloading_Bit),
		MAX_PLAYERS, 
		0,
		"IsOverloading"
		),

END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CFlag )
END_PREDICTION_DATA()

BEGIN_DATADESC( CFlag )

	DEFINE_KEYFIELD( m_iCapturePlayers, FIELD_INTEGER, "CapturePlayers" ),
	DEFINE_KEYFIELD( m_flCaptureRadius, FIELD_FLOAT, "CaptureRadius" ),
	DEFINE_KEYFIELD( m_iStartingTeam, FIELD_INTEGER, "StartingTeam" ),
	DEFINE_KEYFIELD( m_flCaptureTime, FIELD_FLOAT, "CaptureTime" ),
	DEFINE_KEYFIELD( m_iTeamBonus, FIELD_INTEGER, "TeamBonus" ),
	DEFINE_KEYFIELD( m_iTeamBonusInterval, FIELD_INTEGER, "TeamBonusInterval" ),
	DEFINE_KEYFIELD( m_iPlayerBonus, FIELD_INTEGER, "PlayerBonus" ),
	DEFINE_KEYFIELD( m_iForTeam, FIELD_INTEGER, "ForTeam" ),
	DEFINE_KEYFIELD( m_sFlagName, FIELD_STRING, "FlagName" ),
	DEFINE_KEYFIELD( m_iHUDSlot, FIELD_INTEGER, "HUDSlot" ),
	DEFINE_KEYFIELD( m_bNotUncappable, FIELD_BOOLEAN, "NotUncappable" ),
	DEFINE_KEYFIELD( m_bUncapOnDeath, FIELD_BOOLEAN, "UncapOnDeath" ),
	DEFINE_KEYFIELD( m_sNeutralFlagModelName, FIELD_STRING, "NeutralFlagModelName" ), //These strings are only here for compat reasons. -HairyPotter
	DEFINE_KEYFIELD( m_sDisabledFlagModelName, FIELD_STRING, "DisabledFlagModelName" ),
	DEFINE_KEYFIELD( m_sBritishFlagModelName, FIELD_STRING, "BritishFlagModelName" ),
	DEFINE_KEYFIELD( m_sAmericanFlagModelName, FIELD_STRING, "AmericanFlagModelName" ), //
	DEFINE_KEYFIELD( m_iCaptureSound, FIELD_SOUNDNAME, "CaptureSound" ),
	DEFINE_KEYFIELD( m_iNeutralFlagSkin, FIELD_INTEGER, "NeutralFlagSkin" ), //New skin values.
	DEFINE_KEYFIELD( m_iDisabledFlagSkin, FIELD_INTEGER, "DisabledFlagSkin" ),
	DEFINE_KEYFIELD( m_iBritishFlagSkin, FIELD_INTEGER, "BritishFlagSkin" ),
	DEFINE_KEYFIELD( m_iAmericanFlagSkin, FIELD_INTEGER, "AmericanFlagSkin" ), //
	DEFINE_KEYFIELD( m_iFullCap, FIELD_INTEGER, "FullCap" ), //This is used in the CheckFullcap function in hl2mp_gamerules.cpp -HairyPotter
	DEFINE_KEYFIELD( m_bInvisible, FIELD_BOOLEAN, "Invisible" ), //This is used in the CheckFullcap function in hl2mp_gamerules.cpp -HairyPotter
	DEFINE_KEYFIELD( m_flBotNoticeRange, FIELD_FLOAT, "BotNoticeRange" ),

	DEFINE_KEYFIELD( m_iAmericanBuff, FIELD_INTEGER, "CaptureBuffAmer"),
	DEFINE_KEYFIELD( m_iBritishBuff, FIELD_INTEGER, "CaptureBuffBrit"),
#ifndef CLIENT_DLL
	DEFINE_THINKFUNC( Think ),
#endif

#ifndef CLIENT_DLL
	//BG2 - SaintGreg - dynamic flags
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle", InputToggle ),
	DEFINE_INPUTFUNC( FIELD_INTEGER, "ForceCap", InputForceCap ),

	//BG2 - SaintGreg - Output functions similar to BG's
	DEFINE_OUTPUT( m_OnAmericanStartCapture, "OnAmericanStartCapture" ),
	DEFINE_OUTPUT( m_OnBritishStartCapture, "OnBritishStartCapture" ),
	DEFINE_OUTPUT( m_OnStartCapture, "OnStartCapture" ),
	DEFINE_OUTPUT( m_OnAmericanCapture, "OnAmericanCapture" ),
	DEFINE_OUTPUT( m_OnBritishCapture, "OnBritishCapture" ),
	DEFINE_OUTPUT( m_OnCapture, "OnCapture" ),
	DEFINE_OUTPUT( m_OnAmericanStopCapture, "OnAmericanStopCapture" ),
	DEFINE_OUTPUT( m_OnBritishStopCapture, "OnBritishStopCapture" ),
	DEFINE_OUTPUT( m_OnStopCapture, "OnStopCapture" ),
	DEFINE_OUTPUT( m_OnAmericanLosePoint, "OnAmericanLosePoint" ),
	DEFINE_OUTPUT( m_OnBritishLosePoint, "OnBritishLosePoint" ),
	DEFINE_OUTPUT( m_OnLosePoint, "OnLosePoint" ),
	DEFINE_OUTPUT( m_OnEnable, "OnEnable" ),
	DEFINE_OUTPUT( m_OnDisable, "OnDisable" ),
#endif // CLIENT_DLL

END_DATADESC()

LINK_ENTITY_TO_CLASS(flag, CFlag);
PRECACHE_REGISTER(flag);
