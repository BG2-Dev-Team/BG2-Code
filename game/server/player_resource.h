//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Entity that propagates general data needed by clients for every player.
//
// $NoKeywords: $
//=============================================================================//

#ifndef PLAYER_RESOURCE_H
#define PLAYER_RESOURCE_H
#ifdef _WIN32
#pragma once
#endif

#include "shareddefs.h"

class CPlayerResource : public CBaseEntity
{
	DECLARE_CLASS( CPlayerResource, CBaseEntity );
public:
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	virtual void Spawn( void );
	virtual	int	 ObjectCaps( void ) { return BaseClass::ObjectCaps() | FCAP_DONT_SAVE; }
	virtual void ResourceThink( void );
	virtual void UpdatePlayerData( void );
	virtual int  UpdateTransmitState(void);
	//BG2 - Tjoppen - health fix
	void SetPlayerHealth(int entindex, int health);
	void SetPlayerScore(int entindex, int score);
	void SetPlayerDamageScore(int entindex, int damage);
	//

	int GetPing(int index) const { return m_iPing[index]; }
	int GetDamageScore(int index) const { return m_iDamageScore[index]; }

	int GetNumBritish() const { return m_iPlayerCounts >> 24; };
	int GetNumAmericans() const { return (m_iPlayerCounts >> 16) & 0xFF; };
	int GetNumAliveBritish() const { return (m_iPlayerCounts >> 8) & 0xFF; };
	int GetNumAliveAmericans() const { return m_iPlayerCounts & 0xFF; }

protected:
	// Data for each player that's propagated to all clients
	// Stored in individual arrays so they can be sent down via datatables
	CNetworkArray( int, m_iPing, MAX_PLAYERS+1 );
	CNetworkArray( int, m_iScore, MAX_PLAYERS+1 );
	CNetworkArray( int, m_iDamageScore, MAX_PLAYERS+1 );
	CNetworkArray( int, m_bConnected, MAX_PLAYERS+1 );
	CNetworkArray( int, m_iTeam, MAX_PLAYERS+1 );
	CNetworkArray( int, m_bAlive, MAX_PLAYERS+1 );
	CNetworkArray( int, m_iHealth, MAX_PLAYERS+1 );
		
	CNetworkVar(int, m_iPlayerCounts);

	int	m_nUpdateCounter; //BRITISH (8 bits) - AMERICAN (8 bits) - ALIVE BRITISH (8 bits) - ALIVE AMERICANS (8 bits)
};

extern CPlayerResource *g_pPlayerResource;

#endif // PLAYER_RESOURCE_H
