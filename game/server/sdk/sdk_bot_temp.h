//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef SDK_BOT_TEMP_H
#define SDK_BOT_TEMP_H
#ifdef _WIN32
#pragma once
#endif

//BG2 - Tjoppen - HACKHACK
#define CSDKPlayer CBasePlayer

static void RunPlayerMove( CSDKPlayer *fakeclient, CUserCmd &cmd, float frametime );

// This is our bot class.
class CSDKBot // : public CSDKPlayer
{
public:
	bool			m_bBackwards, m_bInuse, m_bLastTurnToRight;

	float			m_flSideMove,
					m_flForwardMove,
					m_flNextStrafeTime,
					m_flNextTurnTime,
					m_flNextVoice;

	QAngle			m_ForwardAngle;
	QAngle			m_LastAngles;

	CSDKPlayer		*m_pPlayer;

	//BG2 - Tjoppen
	int				reload,
					attack,
					attack2,
					respawn; 

	CUserCmd		m_LastCmd;

	CSDKBot()
	{
		m_bInuse = false;
		m_pPlayer = NULL;

		m_flSideMove = m_flForwardMove = 0;
		reload = attack = attack2 = respawn = 0;
	}
};

extern int		g_CurBotNumber;
extern CSDKBot	gBots[MAX_PLAYERS];
extern bool m_bServerReady;

// If iTeam or iClass is -1, then a team or class is randomly chosen.
//CBasePlayer *BotPutInServer( bool bFrozen, int iTeam, int iClass );

void Bot_RunAll();


#endif // SDK_BOT_TEMP_H
