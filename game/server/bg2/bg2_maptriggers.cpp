#include "cbase.h"
#include "bg2_maptriggers.h"
#include "hl2mp_gamerules.h"
#include "flag.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar mp_winbonus;

void CMapTrigger::Spawn( void )
{
	BaseClass::Spawn( );
}
void CMapTrigger::AmericanRoundWin( void )
{
	m_OnAmericanWinRound.FireOutput( this, this );
}
void CMapTrigger::AmericanMapWin( void )
{
	m_OnAmericanWinMap.FireOutput( this, this );
}
void CMapTrigger::BritishRoundWin( void )
{
	m_OnBritishWinRound.FireOutput( this, this );
}
void CMapTrigger::BritishMapWin( void )
{
	m_OnBritishWinMap.FireOutput( this, this );
}
void CMapTrigger::Draw( void )
{
	m_OnDraw.FireOutput( this, this );
}
void CMapTrigger::InputForceAmericanWinRound( inputdata_t &inputData )
{
	//No need to fire an output from here. The rest of the round code will handle that.
	HL2MPRules()->HandleScores( TEAM_AMERICANS, mp_winbonus.GetInt(), AMERICAN_ROUND_WIN, true );
}
void CMapTrigger::InputForceBritishWinRound( inputdata_t &inputData )
{
	//No need to fire an output from here. The rest of the round code will handle that.
	HL2MPRules()->HandleScores( TEAM_BRITISH, mp_winbonus.GetInt(), BRITISH_ROUND_WIN, true );
}
void CMapTrigger::InputForceDrawRound( inputdata_t &inputData )
{
	//No need to fire an output from here. The rest of the round code will handle that.
	HL2MPRules()->HandleScores( -1, 0, ROUND_DRAW, true );
}


BEGIN_DATADESC( CMapTrigger )

	DEFINE_INPUTFUNC( FIELD_VOID, "ForceAmericanWinRound", InputForceAmericanWinRound ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ForceBritishWinRound", InputForceBritishWinRound ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ForceDrawRound", InputForceDrawRound ),

	DEFINE_OUTPUT( m_OnAmericanWinRound, "OnAmericanWinRound" ),
	DEFINE_OUTPUT( m_OnBritishWinRound, "OnBritishWinRound" ),
	DEFINE_OUTPUT( m_OnAmericanWinMap, "OnAmericanWinMap" ),
	DEFINE_OUTPUT( m_OnBritishWinMap, "OnBritishWinMap" ),
	DEFINE_OUTPUT( m_OnDraw, "OnDraw" ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( bg2_maptrigger, CMapTrigger);