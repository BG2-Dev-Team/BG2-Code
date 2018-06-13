//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Team management class. Contains all the details for a specific team
//
// $NoKeywords: $
//=============================================================================//

#ifndef TEAM_H
#define TEAM_H
#ifdef _WIN32
#pragma once
#endif

#include "shareddefs.h"
#include "utlvector.h"

class CBasePlayer;
class CTeamSpawnPoint;

class CTeam : public CBaseEntity
{
	DECLARE_CLASS( CTeam, CBaseEntity );
public:
	CTeam( void );
	virtual ~CTeam( void );

	DECLARE_SERVERCLASS();

	virtual void Precache( void ) { return; };

	virtual void Think( void );
	virtual int  UpdateTransmitState( void );

	//-----------------------------------------------------------------------------
	// Initialization
	//-----------------------------------------------------------------------------
	virtual void		Init( const char *pName, int iNumber );

	//-----------------------------------------------------------------------------
	// Data Handling
	//-----------------------------------------------------------------------------
	virtual int			GetTeamNumber( void ) const;
	virtual const char *GetName( void );
	virtual void		UpdateClientData( CBasePlayer *pPlayer );
	virtual bool		ShouldTransmitToPlayer( CBasePlayer* pRecipient, CBaseEntity* pEntity );

	//-----------------------------------------------------------------------------
	// Spawnpoints
	//-----------------------------------------------------------------------------
	virtual void InitializeSpawnpoints( void );
	virtual void AddSpawnpoint( CTeamSpawnPoint *pSpawnpoint );
	virtual void RemoveSpawnpoint( CTeamSpawnPoint *pSpawnpoint );
	virtual CBaseEntity *SpawnPlayer( CBasePlayer *pPlayer );

	//-----------------------------------------------------------------------------
	// Players
	//-----------------------------------------------------------------------------
	virtual void InitializePlayers( void );
	virtual void AddPlayer( CBasePlayer *pPlayer );
	virtual void RemovePlayer( CBasePlayer *pPlayer );
	virtual int  GetNumPlayers( void );
	virtual CBasePlayer *GetPlayer( int iIndex );
	//BG2 - Draco - Get amount of players with this class
	int GetNumInfantry();
	int GetNumOfficers();
	int GetNumSnipers();
	int GetNumOfClass(int iClass);
	int GetNumOfNextClass(int iNextClass);
	int GetNumOfClassRealPlayers(int iClass);
	int GetNumOfNextClassRealPlayers(int iClass);
	int GetNumOfAmmoKit(int iAmmoKit); // BG2 - VisualMelon - May need revising
	/*float GetMorale();
	void AddMorale(float New, float Time);
	void AddMoralePoint(CBaseEntity * pEnt);
	void AddMoraleBonus(float New);
	void RemoveMoralePoint(CBaseEntity * pEnt);*/

	//-----------------------------------------------------------------------------
	// Scoring
	//-----------------------------------------------------------------------------
	virtual void AddScore( int iScore );
	virtual void SetScore( int iScore );
	virtual int  GetScore( void );
	virtual void ResetScores( void );

	// Round scoring
	virtual int GetRoundsWon( void ) { return m_iRoundsWon; }
	virtual void SetRoundsWon( int iRounds ) { m_iRoundsWon = iRounds; }
	virtual void IncrementRoundsWon( void ) { m_iRoundsWon++; }

	void AwardAchievement( int iAchievement );

	virtual int GetAliveMembers( void );
	//BG2 - Commented - HairyPotter
	void ResetTickets(void);
	int GetTicketsLeft(void);
	void RemoveTicket(void);

	//we explicitly allow the number of tickets to be removed to be decimal
	void RemoveTickets(float number);
private:
	//fractional number of tickets left over from last call to RemoveTickets()
	//zeroed on round restart
	float m_flTicketRemovalFraction;
	//

public:
	CUtlVector< CTeamSpawnPoint * > m_aSpawnPoints;
	CUtlVector< CBasePlayer * >		m_aPlayers;

	// Data
	CNetworkString( m_szTeamname, MAX_TEAM_NAME_LENGTH );
	CNetworkVar( int, m_iScore );
	CNetworkVar( int, m_iRoundsWon );
	CNetworkVar(int, m_iTicketsLeft); //BG2 - Commented - HairyPotter
	int		m_iDeaths;

	// Spawnpoints
	int		m_iLastSpawn;		// Index of the last spawnpoint used

	CNetworkVar( int, m_iTeamNum );			// Which team is this?
};

extern CUtlVector< CTeam * > g_Teams;
extern CTeam *GetGlobalTeam( int iIndex );
extern int GetNumberOfTeams( void );

#endif // TEAM_H
