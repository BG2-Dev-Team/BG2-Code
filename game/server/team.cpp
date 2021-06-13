//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Team management class. Contains all the details for a specific team
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "team.h"
#include "player.h"
#include "team_spawnpoint.h"
//BG2 - Tjoppen - #includes
#include "hl2mp_player.h"
#include "hl2mp_gamerules.h"
//

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CUtlVector< CTeam * > g_Teams;

//-----------------------------------------------------------------------------
// Purpose: SendProxy that converts the Team's player UtlVector to entindexes
//-----------------------------------------------------------------------------
//BG2 - Tjoppen - don't need this
/*void SendProxy_PlayerList( const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID )
{
	CTeam *pTeam = (CTeam*)pData;

	// If this assertion fails, then SendProxyArrayLength_PlayerArray must have failed.
	Assert( iElement < pTeam->m_aPlayers.Size() );

	CBasePlayer *pPlayer = pTeam->m_aPlayers[iElement];
	pOut->m_Int = pPlayer->entindex();
}


int SendProxyArrayLength_PlayerArray( const void *pStruct, int objectID )
{
	CTeam *pTeam = (CTeam*)pStruct;
	return pTeam->m_aPlayers.Count();
}*/


// Datatable
IMPLEMENT_SERVERCLASS_ST_NOBASE(CTeam, DT_Team)
	SendPropInt( SENDINFO(m_iTeamNum), 5 ),
	//BG2 - Tjoppen - allocate more bits for team score. 65535 points ought to be enough for anyone
	//SendPropInt( SENDINFO(m_iScore), 8 ),
	SendPropInt(SENDINFO(m_iScore), 16, SPROP_UNSIGNED),
	//
	SendPropInt( SENDINFO(m_iRoundsWon), 8 ),
	SendPropInt(SENDINFO(m_iTicketsLeft), 12, SPROP_UNSIGNED),
	SendPropString( SENDINFO( m_szTeamname ) ),

	//BG2 - Tjoppen - don't need this
	/*SendPropArray2(
	SendProxyArrayLength_PlayerArray,
	SendPropInt("player_array_element", 0, 4, 10, SPROP_UNSIGNED, SendProxy_PlayerList),
	MAX_PLAYERS,
	0,
	"player_array"
	)*/
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( team_manager, CTeam );

//-----------------------------------------------------------------------------
// Purpose: Get a pointer to the specified team manager
//-----------------------------------------------------------------------------
CTeam *GetGlobalTeam( int iIndex )
{
	if ( iIndex < 0 || iIndex >= GetNumberOfTeams() )
		return NULL;

	return g_Teams[ iIndex ];
}

//-----------------------------------------------------------------------------
// Purpose: Get the number of team managers
//-----------------------------------------------------------------------------
int GetNumberOfTeams( void )
{
	return g_Teams.Size();
}


//-----------------------------------------------------------------------------
// Purpose: Needed because this is an entity, but should never be used
//-----------------------------------------------------------------------------
CTeam::CTeam( void )
{
	memset( m_szTeamname.GetForModify(), 0, sizeof(m_szTeamname) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTeam::~CTeam( void )
{
	m_aSpawnPoints.Purge();
	m_aPlayers.Purge();
}

//-----------------------------------------------------------------------------
// Purpose: Called every frame
//-----------------------------------------------------------------------------
void CTeam::Think( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: Teams are always transmitted to clients
//-----------------------------------------------------------------------------
int CTeam::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}

//-----------------------------------------------------------------------------
// Visibility/scanners
//-----------------------------------------------------------------------------
bool CTeam::ShouldTransmitToPlayer( CBasePlayer* pRecipient, CBaseEntity* pEntity )
{
	// Always transmit the observer target to players
	if ( pRecipient && pRecipient->IsObserver() && pRecipient->GetObserverTarget() == pEntity )
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------
void CTeam::Init( const char *pName, int iNumber )
{
	InitializeSpawnpoints();
	InitializePlayers();

	m_iScore = 0;

	Q_strncpy( m_szTeamname.GetForModify(), pName, MAX_TEAM_NAME_LENGTH );
	m_iTeamNum = iNumber;
}

//-----------------------------------------------------------------------------
// DATA HANDLING
//-----------------------------------------------------------------------------
int CTeam::GetTeamNumber( void ) const
{
	return m_iTeamNum;
}

//-----------------------------------------------------------------------------
// Purpose: Get the team's name
//-----------------------------------------------------------------------------
const char *CTeam::GetName( void )
{
	return m_szTeamname;
}


//-----------------------------------------------------------------------------
// Purpose: Update the player's client data
//-----------------------------------------------------------------------------
void CTeam::UpdateClientData( CBasePlayer *pPlayer )
{
}

//------------------------------------------------------------------------------------------------------------------
// SPAWNPOINTS
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeam::InitializeSpawnpoints( void )
{
	m_iLastSpawn = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeam::AddSpawnpoint( CTeamSpawnPoint *pSpawnpoint )
{
	m_aSpawnPoints.AddToTail( pSpawnpoint );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeam::RemoveSpawnpoint( CTeamSpawnPoint *pSpawnpoint )
{
	for (int i = 0; i < m_aSpawnPoints.Size(); i++ )
	{
		if ( m_aSpawnPoints[i] == pSpawnpoint )
		{
			m_aSpawnPoints.Remove( i );
			return;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Spawn the player at one of this team's spawnpoints. Return true if successful.
//-----------------------------------------------------------------------------
CBaseEntity *CTeam::SpawnPlayer( CBasePlayer *pPlayer )
{
	if ( m_aSpawnPoints.Size() == 0 )
		return NULL;

	// Randomize the start spot
	int iSpawn = m_iLastSpawn + random->RandomInt( 1,3 );
	if ( iSpawn >= m_aSpawnPoints.Size() )
		iSpawn -= m_aSpawnPoints.Size();
	int iStartingSpawn = iSpawn;

	// Now loop through the spawnpoints and pick one
	int loopCount = 0;
	do 
	{
		if ( iSpawn >= m_aSpawnPoints.Size() )
		{
			++loopCount;
			iSpawn = 0;
		}

		// check if pSpot is valid, and that the player is on the right team
		if ( (loopCount > 3) || m_aSpawnPoints[iSpawn]->IsValid( pPlayer ) )
		{
			// DevMsg( 1, "player: spawning at (%s)\n", STRING(m_aSpawnPoints[iSpawn]->m_iName) );
			m_aSpawnPoints[iSpawn]->m_OnPlayerSpawn.FireOutput( pPlayer, m_aSpawnPoints[iSpawn] );

			m_iLastSpawn = iSpawn;
			return m_aSpawnPoints[iSpawn];
		}

		iSpawn++;
	} while ( iSpawn != iStartingSpawn ); // loop if we're not back to the start

	return NULL;
}

//------------------------------------------------------------------------------------------------------------------
// PLAYERS
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeam::InitializePlayers( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: Add the specified player to this team. Remove them from their current team, if any.
//-----------------------------------------------------------------------------
void CTeam::AddPlayer( CBasePlayer *pPlayer )
{
	m_aPlayers.AddToTail( pPlayer );
	NetworkStateChanged();
}

//-----------------------------------------------------------------------------
// Purpose: Remove this player from the team
//-----------------------------------------------------------------------------
void CTeam::RemovePlayer( CBasePlayer *pPlayer )
{
	m_aPlayers.FindAndRemove( pPlayer );
	NetworkStateChanged();
}

//-----------------------------------------------------------------------------
// Purpose: Return the number of players in this team.
//-----------------------------------------------------------------------------
int CTeam::GetNumPlayers( void )
{
	return m_aPlayers.Size();
}

//-----------------------------------------------------------------------------
// Purpose: Get a specific player
//-----------------------------------------------------------------------------
CBasePlayer *CTeam::GetPlayer( int iIndex )
{
	Assert( iIndex >= 0 && iIndex < m_aPlayers.Size() );
	return m_aPlayers[ iIndex ];
}

//------------------------------------------------------------------------------------------------------------------
// SCORING
//-----------------------------------------------------------------------------
// Purpose: Add / Remove score for this team
//-----------------------------------------------------------------------------
void CTeam::AddScore( int iScore )
{
	//BG2 - Tjoppen - scoreboard fix
	//m_iScore += iScore;
	SetScore(m_iScore + iScore);
	//
}

void CTeam::SetScore( int iScore )
{
	m_iScore = iScore;
	//BG2 - Tjoppen - scoreboard fix
	extern ConVar	mp_americanscore,
		mp_britishscore;

	switch (GetTeamNumber())
	{
	case TEAM_AMERICANS:
		mp_americanscore.SetValue(GetScore());
		break;
	case TEAM_BRITISH:
		mp_britishscore.SetValue(GetScore());
		break;
	default:
		break;
	}
	//
}

//-----------------------------------------------------------------------------
// Purpose: Get this team's score
//-----------------------------------------------------------------------------
int CTeam::GetScore( void )
{
	return m_iScore;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeam::ResetScores( void )
{
	SetScore(0);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeam::AwardAchievement( int iAchievement )
{
	Assert( iAchievement >= 0 && iAchievement < 255 );	// must fit in short 

	CRecipientFilter filter;

	int iNumPlayers = GetNumPlayers();

	for ( int i=0;i<iNumPlayers;i++ )
	{
		if ( GetPlayer(i) )
		{
			filter.AddRecipient( GetPlayer(i) );
		}
	}

	UserMessageBegin( filter, "AchievementEvent" );
		WRITE_SHORT( iAchievement );
	MessageEnd();
}

int CTeam::GetAliveMembers( void )
{
	int iAlive = 0;

	int iNumPlayers = GetNumPlayers();

	for ( int i=0;i<iNumPlayers;i++ )
	{
		if ( GetPlayer(i) && GetPlayer(i)->IsAlive() )
		{
			iAlive++;
		}
	}

	return iAlive;
}

//BG2 - Draco - Start
int CTeam::GetNumInfantry()
{
	return GetNumOfClass(CLASS_INFANTRY);
}

int CTeam::GetNumOfficers()
{
	return GetNumOfClass(CLASS_OFFICER);
}
int CTeam::GetNumSnipers()
{
	return GetNumOfClass(CLASS_SNIPER);
}

int CTeam::GetNumOfClass(int iClass)
{
	int iAmount = 0;

	for (int x = 0; x < GetNumPlayers(); x++)
	{
		CHL2MP_Player *pHL2Player = ToHL2MPPlayer(GetPlayer(x));

		if (pHL2Player && pHL2Player->GetClass() == iClass)
			iAmount++;
	}

	return iAmount;
}

int CTeam::GetNumOfNextClass(int iNextClass)
{
	int iAmount = 0;

	for (int x = 0; x < GetNumPlayers(); x++)
	{
		CHL2MP_Player *pHL2Player = ToHL2MPPlayer(GetPlayer(x));

		if (pHL2Player && pHL2Player->GetNextClass() == iNextClass)
			iAmount++;
	}

	return iAmount;
}

int CTeam::GetNumOfClassRealPlayers(int iClass)
{
	int iAmount = 0;

	for (int x = 0; x < GetNumPlayers(); x++)
	{
		CHL2MP_Player *pHL2Player = ToHL2MPPlayer(GetPlayer(x));

		if (pHL2Player && !(pHL2Player->GetFlags() & FL_FAKECLIENT) && pHL2Player->GetClass() == iClass)
			iAmount++;
	}

	return iAmount;
}

int CTeam::GetNumOfNextClassRealPlayers(int iNextClass)
{
	int iAmount = 0;

	for (int x = 0; x < GetNumPlayers(); x++)
	{
		CHL2MP_Player *pHL2Player = ToHL2MPPlayer(GetPlayer(x));

		if (pHL2Player && !(pHL2Player->GetFlags() & FL_FAKECLIENT) && pHL2Player->GetNextClass() == iNextClass)
			iAmount++;
	}

	return iAmount;
}

int CTeam::GetNumOfAmmoKit(int iAmmoKit)
{
	int iAmount = 0;

	for (int x = 0; x < GetNumPlayers(); x++)
	{
		CHL2MP_Player *pHL2Player = ToHL2MPPlayer(GetPlayer(x));

		if (pHL2Player && pHL2Player->m_iAmmoKit == iAmmoKit)
			iAmount++;
	}

	return iAmount;
}
/*
void CTeam::AddMorale(float New, float Time)
{
m_flMorale += New;
if (gpGlobals->curtime > m_flMoraleTime)
{
m_flMoraleTime = Time + gpGlobals->curtime;
}
else
{
m_flMoraleTime += Time;
}
}

void CTeam::AddMoraleBonus(float New)
{
m_flMoraleBonus += New;
}

float CTeam::GetMorale()
{
return (m_flMorale + m_flMoraleBonus);
}

void CTeam::AddMoralePoint(CBaseEntity * pEnt)
{
m_aMoralePoints.AddToTail( pEnt );
}

void CTeam::RemoveMoralePoint(CBaseEntity * pEnt)
{
m_aMoralePoints.FindAndRemove( pEnt );
}
*/
//BG2 - Draco - End

void CTeam::ResetTickets(void)
{
	m_flTicketRemovalFraction = 0;

	if (m_iTeamNum == TEAM_AMERICANS)
		m_iTicketsLeft = mp_tickets_a.GetInt();
	else
		m_iTicketsLeft = mp_tickets_b.GetInt();
}

int CTeam::GetTicketsLeft(void)
{
	return m_iTicketsLeft;
}

void CTeam::RemoveTicket(void)
{
	if (m_iTicketsLeft > 0) {
		m_iTicketsLeft--;

		extern ConVar mp_tickets_timelimit;
		if (m_iTicketsLeft == 0) {
			const char* pszMessage = m_iTeamNum == TEAM_AMERICANS ? "#BG3_Amer_Out_Of_Tickets" : "#BG3_Brit_Out_Of_Tickets";
			MSay(pszMessage);

			if (mp_tickets_timelimit.GetBool() && HL2MPRules()->GetRemainingRoundTime() > 60) {
				HL2MPRules()->SetRemainingRoundTime(60.f);
				CSay("Remaining round time set to one minute");
			}
		}
	}
}

void CTeam::RemoveTickets(float number)
{
	number += m_flTicketRemovalFraction;
	int intPart = (int)number;

	if (intPart >= m_iTicketsLeft)
	{
		m_iTicketsLeft = 0;
		m_flTicketRemovalFraction = 0;
	}
	else
	{
		m_iTicketsLeft -= intPart;
		m_flTicketRemovalFraction = number - intPart;
	}
}

void CTeam::ChangeTickets(int number) {
	m_iTicketsLeft += number;
	if (m_iTicketsLeft < 0) m_iTicketsLeft = 0;
}