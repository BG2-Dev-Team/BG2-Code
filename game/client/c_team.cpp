//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Client side CTeam class
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_team.h"

//BG2 - Tjoppen - #includes
#include "hl2mp_gamerules.h"
#include "c_hl2mp_player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: RecvProxy that converts the Team's player UtlVector to entindexes
//-----------------------------------------------------------------------------
//BG2 - Tjoppen - don't need this
/*void RecvProxy_PlayerList(  const CRecvProxyData *pData, void *pStruct, void *pOut )
{
C_Team *pTeam = (C_Team*)pOut;
pTeam->m_aPlayers[pData->m_iElement] = pData->m_Value.m_Int;
}


void RecvProxyArrayLength_PlayerArray( void *pStruct, int objectID, int currentArrayLength )
{
C_Team *pTeam = (C_Team*)pStruct;

if ( pTeam->m_aPlayers.Size() != currentArrayLength )
pTeam->m_aPlayers.SetSize( currentArrayLength );
}
*/


IMPLEMENT_CLIENTCLASS_DT_NOBASE(C_Team, DT_Team, CTeam)
	RecvPropInt( RECVINFO(m_iTeamNum)),
	RecvPropInt( RECVINFO(m_iScore)),
	RecvPropInt( RECVINFO(m_iRoundsWon) ),
	RecvPropInt(RECVINFO(m_iTicketsLeft)),
	RecvPropString( RECVINFO(m_szTeamname)),
	
	//BG2 - Tjoppen - don't need this
	/*RecvPropArray2(
	RecvProxyArrayLength_PlayerArray,
	RecvPropInt( "player_array_element", 0, SIZEOF_IGNORE, 0, RecvProxy_PlayerList ),
	MAX_PLAYERS,
	0,
	"player_array"
	)*/
END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_Team )
	DEFINE_PRED_ARRAY( m_szTeamname, FIELD_CHARACTER, MAX_TEAM_NAME_LENGTH, FTYPEDESC_PRIVATE ),
	DEFINE_PRED_FIELD( m_iScore, FIELD_INTEGER, FTYPEDESC_PRIVATE ),
	DEFINE_PRED_FIELD( m_iRoundsWon, FIELD_INTEGER, FTYPEDESC_PRIVATE ),
	DEFINE_PRED_FIELD( m_iDeaths, FIELD_INTEGER, FTYPEDESC_PRIVATE ),
	DEFINE_PRED_FIELD( m_iPing, FIELD_INTEGER, FTYPEDESC_PRIVATE ),
	DEFINE_PRED_FIELD( m_iPacketloss, FIELD_INTEGER, FTYPEDESC_PRIVATE ),
	DEFINE_PRED_FIELD( m_iTeamNum, FIELD_INTEGER, FTYPEDESC_PRIVATE ),
END_PREDICTION_DATA();

// Global list of client side team entities
CUtlVector< C_Team * > g_Teams;

//=================================================================================================
// C_Team functionality

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_Team::C_Team()
{
	m_iScore = 0;
	m_iRoundsWon = 0;
	memset( m_szTeamname, 0, sizeof(m_szTeamname) );

	m_iDeaths = 0;
	m_iPing = 0;
	m_iPacketloss = 0;

	// Add myself to the global list of team entities
	g_Teams.AddToTail( this );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_Team::~C_Team()
{
	g_Teams.FindAndRemove( this );
}


//BG2 - Tjoppen - part of bandwidth saving
/*void C_Team::RemoveAllPlayers()
{
m_aPlayers.RemoveAll();
}*/

void C_Team::PreDataUpdate( DataUpdateType_t updateType )
{
	BaseClass::PreDataUpdate( updateType );
}


//-----------------------------------------------------------------------------
// Gets the ith player on the team (may return NULL) 
//-----------------------------------------------------------------------------
//BG2 - Tjoppen - part of bandwidth saving
/*C_BasePlayer* C_Team::GetPlayer( int idx )
{
return (C_BasePlayer*)cl_entitylist->GetEnt(m_aPlayers[idx]);
}*/


int C_Team::GetTeamNumber() const
{
	return m_iTeamNum;
}


//=================================================================================================
// TEAM HANDLING
//=================================================================================================
// Purpose: 
//-----------------------------------------------------------------------------
char *C_Team::Get_Name( void )
{
	return m_szTeamname;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int C_Team::Get_Score( void )
{
	return m_iScore;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int C_Team::Get_Deaths( void )
{
	return m_iDeaths;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int C_Team::Get_Ping( void )
{
	return m_iPing;
}

//-----------------------------------------------------------------------------
// Purpose: Return the number of players in this team
//-----------------------------------------------------------------------------
int C_Team::Get_Number_Players( void )
{
	//BG3 - use network data for important teams, this data is more accurate
	if (m_iTeamNum == TEAM_BRITISH) {
		return g_PR->GetNumBritish();
	}
	else if (m_iTeamNum == TEAM_AMERICANS) {
		return g_PR->GetNumAmericans();
	}

	//BG2 - Tjoppen - part of bandwidth saving
	int n = 0;
	for (int x = 1; x <= gpGlobals->maxClients; x++)
	{
		C_BasePlayer *pPlayer = UTIL_PlayerByIndex(x);

		if (pPlayer && pPlayer->GetTeamNumber() == GetTeamNumber())
			n++;
	}

	return n;
	//return m_aPlayers.Size();
}

int C_Team::GetNumPlayersAlive(void)
{
	if (m_iTeamNum == TEAM_BRITISH) {
		return g_PR->GetNumAliveBritish();
	}
	else if (m_iTeamNum == TEAM_AMERICANS) {
		return g_PR->GetNumAliveAmericans();
	}

	//BG2 - Tjoppen - part of bandwidth saving
	int n = 0;
	for (int x = 1; x <= gpGlobals->maxClients; x++)
	{
		C_BasePlayer *pPlayer = UTIL_PlayerByIndex(x);

		if (pPlayer && pPlayer->GetTeamNumber() == GetTeamNumber() && pPlayer->IsAlive())
			n++;
	}

	return n;
	//return m_aPlayers.Size();
}

//-----------------------------------------------------------------------------
// Purpose: Returns true if the specified player is on this team
//-----------------------------------------------------------------------------
//BG2 - Tjoppen - part of bandwidth saving
/*bool C_Team::ContainsPlayer( int iPlayerIndex )
{
for (int i = 0; i < m_aPlayers.Size(); i++ )
{
if ( m_aPlayers[i] == iPlayerIndex )
return true;
}

return false;
}*/


void C_Team::ClientThink()
{
}


//=================================================================================================
// GLOBAL CLIENT TEAM HANDLING
//=================================================================================================
// Purpose: Get the C_Team for the local player
//-----------------------------------------------------------------------------
C_Team *GetLocalTeam( void )
{
	C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();

	if ( !player )
		return NULL;
	
	return GetPlayersTeam( player->index );
}

//-----------------------------------------------------------------------------
// Purpose: Get the C_Team for the specified team number
//-----------------------------------------------------------------------------
C_Team *GetGlobalTeam( int iTeamNumber )
{
	for (int i = 0; i < g_Teams.Count(); i++ )
	{
		if ( g_Teams[i]->GetTeamNumber() == iTeamNumber )
			return g_Teams[i];
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Returns the number of teams you can access via GetGlobalTeam() (hence the +1)
//-----------------------------------------------------------------------------
int GetNumTeams()
{
	return g_Teams.Count() + 1; 
}

//-----------------------------------------------------------------------------
// Purpose: Get the team of the specified player
//-----------------------------------------------------------------------------
C_Team *GetPlayersTeam( int iPlayerIndex )
{
	//BG2 - Tjoppen - part of bandwidth saving
	C_BasePlayer *pPlayer = static_cast<C_BasePlayer*>(cl_entitylist->GetEnt(iPlayerIndex));

	if (!pPlayer)
		return NULL;

	for (int i = 0; i < g_Teams.Count(); i++)
	{
		//if ( g_Teams[i]->ContainsPlayer( iPlayerIndex ) )
		if (g_Teams[i]->GetTeamNumber() == pPlayer->GetTeamNumber())
			return g_Teams[i];
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Get the team of the specified player
//-----------------------------------------------------------------------------
C_Team *GetPlayersTeam( C_BasePlayer *pPlayer )
{
	return GetPlayersTeam( pPlayer->entindex() );
}

//-----------------------------------------------------------------------------
// Purpose: Returns true if the two specified players are on the same team
//-----------------------------------------------------------------------------
bool ArePlayersOnSameTeam( int iPlayerIndex1, int iPlayerIndex2 )
{
	//BG2 - Tjoppen - part of bandwidth saving
	C_BasePlayer	*pPlayer1 = static_cast<C_BasePlayer*>(cl_entitylist->GetEnt(iPlayerIndex1)),
		*pPlayer2 = static_cast<C_BasePlayer*>(cl_entitylist->GetEnt(iPlayerIndex2));

	return pPlayer1 && pPlayer2 && pPlayer1->GetTeamNumber() == pPlayer2->GetTeamNumber();

	/*for (int i = 0; i < g_Teams.Count(); i++ )
	{
	if ( g_Teams[i]->ContainsPlayer( iPlayerIndex1 ) && g_Teams[i]->ContainsPlayer( iPlayerIndex2 ) )
	return true;
	}

	return false;*/
}

//-----------------------------------------------------------------------------
// Purpose: Get the number of team managers
//-----------------------------------------------------------------------------
int GetNumberOfTeams( void )
{
	return g_Teams.Size();
}

int C_Team::GetNumOfNextClass(int iClass)
{
	int iAmount = 0;

	for (int x = 1; x <= gpGlobals->maxClients; x++)
	{
		CHL2MP_Player *pHL2Player = ToHL2MPPlayer(cl_entitylist->GetEnt(x));

		if (pHL2Player && pHL2Player->GetTeamNumber() == GetTeamNumber() && pHL2Player->GetNextClass() == iClass)
			iAmount++;
	}

	return iAmount;
}

/*int C_Team::GetNumOfClassRealPlayers(int iClass)
{
	int iAmount = 0;

	for (int x = 0; x < GetNumPlayers(); x++)
	{
		CHL2MP_Player *pHL2Player = ToHL2MPPlayer(cl_entitylist->GetEnt(x));

		if (pHL2Player && !(pHL2Player->GetFlags() & FL_FAKECLIENT) && pHL2Player->GetClass() == iClass)
			iAmount++;
	}

	return iAmount;
}*/

int C_Team::GetNumOfNextClassRealPlayers(int iNextClass)
{
	int iAmount = 0;

	for (int x = 1; x <= gpGlobals->maxClients; x++)
	{
		CHL2MP_Player *pPlayer = ToHL2MPPlayer(UTIL_PlayerByIndex(x));

		if (pPlayer && pPlayer->GetNextClass() == iNextClass && pPlayer->GetTeamNumber() == GetTeamNumber() && !pPlayer->IsFakeClient()) {
			iAmount++;
		}

	}

	return iAmount;
}

void C_Team::GetNumOfNextClass(int iClass, uint8* pBotCount, uint8* pRealCount) {
	*pBotCount = *pRealCount = 0;
	for (int x = 1; x <= gpGlobals->maxClients; x++)
	{
		CHL2MP_Player *pPlayer = ToHL2MPPlayer(UTIL_PlayerByIndex(x));

		if (pPlayer && pPlayer->GetNextClass() == iClass && pPlayer->GetTeamNumber() == GetTeamNumber()) {
			uint8* pCount = pPlayer->IsFakeClient() ? pBotCount : pRealCount;
			(*pCount)++;
		}
			
	}
}