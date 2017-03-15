//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef ILAGCOMPENSATIONMANAGER_H
#define ILAGCOMPENSATIONMANAGER_H
#ifdef _WIN32
#pragma once
#endif

class CBasePlayer;
class CUserCmd;

//-----------------------------------------------------------------------------
// Purpose: This is also an IServerSystem
//-----------------------------------------------------------------------------
abstract_class ILagCompensationManager
{
public:
	// Called during player movement to set up/restore after lag compensation
	virtual void	StartLagCompensation( CBasePlayer *player, CUserCmd *cmd ) = 0;
	virtual void	FinishLagCompensation( CBasePlayer *player ) = 0;
	virtual bool	IsCurrentlyDoingLagCompensation() const = 0;
	//BG2 - Tjoppen - bullet lag compensation
	/**
	* Like StartLagCompensation() above except it takes an explicit target time
	* This is used to fast-forward newly created bullets so they are in sync with the server's clock
	* The CUserCmd is only needed since we need to call CBasePlayer::WantsLagCompensationOnEntity()
	*/
	virtual void	StartLagCompensation(CBasePlayer *player, CUserCmd *cmd, float flTargetTime) = 0;

	/**
	* Returns the target time of the specified player and command
	* This is basically gpGlobals->curtime minus latency and the player's cl_interp value
	*/
	virtual float	GetTargetTime(CBasePlayer *player, CUserCmd *cmd) const = 0;
};

extern ILagCompensationManager *lagcompensation;

#endif // ILAGCOMPENSATIONMANAGER_H
