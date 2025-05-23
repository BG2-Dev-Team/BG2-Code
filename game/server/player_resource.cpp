//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Entity that propagates general data needed by clients for every player.
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "player.h"
#include "player_resource.h"
#include <coordsize.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Datatable
IMPLEMENT_SERVERCLASS_ST_NOBASE(CPlayerResource, DT_PlayerResource)
//	SendPropArray( SendPropString( SENDINFO(m_szName[0]) ), SENDARRAYINFO(m_szName) ),
	SendPropArray3( SENDINFO_ARRAY3(m_iPing), SendPropInt( SENDINFO_ARRAY(m_iPing), 10, SPROP_UNSIGNED ) ),
//	SendPropArray( SendPropInt( SENDINFO_ARRAY(m_iPacketloss), 7, SPROP_UNSIGNED ), m_iPacketloss ),
	SendPropArray3( SENDINFO_ARRAY3(m_iScore), SendPropInt( SENDINFO_ARRAY(m_iScore), 12, SPROP_UNSIGNED ) ),
	SendPropArray3( SENDINFO_ARRAY3(m_iDamageScore), SendPropInt( SENDINFO_ARRAY(m_iDamageScore), 20 ) ),
	SendPropArray3( SENDINFO_ARRAY3(m_bConnected), SendPropInt( SENDINFO_ARRAY(m_bConnected), 1, SPROP_UNSIGNED ) ),
	SendPropArray3( SENDINFO_ARRAY3(m_iTeam), SendPropInt( SENDINFO_ARRAY(m_iTeam), 4 ) ),
	SendPropArray3( SENDINFO_ARRAY3(m_bAlive), SendPropInt( SENDINFO_ARRAY(m_bAlive), 1, SPROP_UNSIGNED ) ),
	SendPropArray3( SENDINFO_ARRAY3(m_iHealth), SendPropInt( SENDINFO_ARRAY(m_iHealth), -1, SPROP_VARINT | SPROP_UNSIGNED ) ),
	SendPropInt(SENDINFO(m_iPlayerCounts)),
END_SEND_TABLE()

BEGIN_DATADESC( CPlayerResource )

	// DEFINE_ARRAY( m_iPing, FIELD_INTEGER, MAX_PLAYERS+1 ),
	// DEFINE_ARRAY( m_iPacketloss, FIELD_INTEGER, MAX_PLAYERS+1 ),
	// DEFINE_ARRAY( m_iScore, FIELD_INTEGER, MAX_PLAYERS+1 ),
	// DEFINE_ARRAY( m_iDeaths, FIELD_INTEGER, MAX_PLAYERS+1 ),
	// DEFINE_ARRAY( m_bConnected, FIELD_INTEGER, MAX_PLAYERS+1 ),
	// DEFINE_FIELD( m_flNextPingUpdate, FIELD_FLOAT ),
	// DEFINE_ARRAY( m_iTeam, FIELD_INTEGER, MAX_PLAYERS+1 ),
	// DEFINE_ARRAY( m_bAlive, FIELD_INTEGER, MAX_PLAYERS+1 ),
	// DEFINE_ARRAY( m_iHealth, FIELD_INTEGER, MAX_PLAYERS+1 ),
	// DEFINE_FIELD( m_nUpdateCounter, FIELD_INTEGER ),

	// Function Pointers
	DEFINE_FUNCTION( ResourceThink ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( player_manager, CPlayerResource );

CPlayerResource *g_pPlayerResource;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPlayerResource::Spawn( void )
{
	for ( int i=0; i < MAX_PLAYERS+1; i++ )
	{
		m_iPing.Set( i, 0 );
		m_iScore.Set( i, 0 );
		m_iDamageScore.Set( i, 0 );
		m_bConnected.Set( i, 0 );
		m_iTeam.Set( i, 0 );
		m_bAlive.Set( i, 0 );
	}

	SetThink( &CPlayerResource::ResourceThink );
	SetNextThink( gpGlobals->curtime );
	m_nUpdateCounter = 0;
}

//BG2 - Tjoppen - health fix
//-----------------------------------------------------------------------------
// Purpose: Sets health value for a particular client (used to set on spawn so the data will be sent synchronously)
//-----------------------------------------------------------------------------
void CPlayerResource::SetPlayerHealth(int entindex, int health)
{
	m_iHealth.Set(entindex, health);
}
//

//-----------------------------------------------------------------------------
// Purpose: Sets score value for a particular client
//-----------------------------------------------------------------------------
void CPlayerResource::SetPlayerScore(int entindex, int score)
{
	m_iScore.Set(entindex, score);
}
//

//-----------------------------------------------------------------------------
// Purpose: Sets damage score value for a particular client
//-----------------------------------------------------------------------------
void CPlayerResource::SetPlayerDamageScore(int entindex, int damage)
{
	m_iDamageScore.Set(entindex, damage);
}
//

//-----------------------------------------------------------------------------
// Purpose: The Player resource is always transmitted to clients
//-----------------------------------------------------------------------------
int CPlayerResource::UpdateTransmitState()
{
	// ALWAYS transmit to all clients.
	return SetTransmitState( FL_EDICT_ALWAYS );
}

//-----------------------------------------------------------------------------
// Purpose: Wrapper for the virtual GrabPlayerData Think function
//-----------------------------------------------------------------------------
void CPlayerResource::ResourceThink( void )
{
	m_nUpdateCounter++;

	UpdatePlayerData();

	SetNextThink( gpGlobals->curtime + 0.1f );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPlayerResource::UpdatePlayerData( void )
{
	bool periodicUpdate = !(m_nUpdateCounter % 20);

	uint8 numAmericans, numBritish, numAliveAmericans, numAliveBritish;
	numAmericans = numBritish = numAliveAmericans = numAliveBritish = 0;
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *pPlayer = (CBasePlayer*)UTIL_PlayerByIndex( i );
		
		if ( pPlayer && pPlayer->IsConnected() )
		{
			m_iScore.Set( i, pPlayer->FragCount() );
			m_iDamageScore.Set( i, pPlayer->DamageScoreCount() );
			m_bConnected.Set( i, 1 );
			m_iTeam.Set( i, pPlayer->GetTeamNumber() );
			m_bAlive.Set( i, pPlayer->IsAlive()?1:0 );
			m_iHealth.Set(i, MAX( 0, pPlayer->GetHealth() ) );

			
			// Don't update ping / packetloss / player counts everytime
			if (periodicUpdate)
			{
				// update ping all 20 think ticks = (20*0.1=2seconds)
				int ping, packetloss;
				UTIL_GetPlayerConnectionInfo( i, ping, packetloss );
				
				// calc avg for scoreboard so it's not so jittery
				ping = 0.8f * m_iPing.Get(i) + 0.2f * ping;

				
				m_iPing.Set( i, ping );
				// m_iPacketloss.Set( i, packetloss );

				if (pPlayer->GetTeamNumber() == TEAM_BRITISH) {
					if (pPlayer->IsAlive()) numAliveBritish++;
					numBritish++;
				}
				else if (pPlayer->GetTeamNumber() == TEAM_AMERICANS) {
					if (pPlayer->IsAlive()) numAliveAmericans++;
					numAmericans++;
				}
			}
		}
		else
		{
			m_bConnected.Set( i, 0 );
		}
	}

	//put player counts into variable
	if (periodicUpdate) {
		int playerCounts = 0;
		playerCounts |= numBritish << 24;
		playerCounts |= numAmericans << 16;
		playerCounts |= numAliveBritish << 8;
		playerCounts |= numAliveAmericans;
		m_iPlayerCounts = playerCounts;
	}
}
