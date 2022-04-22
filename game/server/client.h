//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef CLIENT_H
#define CLIENT_H

#ifdef _WIN32
#pragma once
#endif
#include <vector>

class CCommand;
class CUserCmd;
class CBasePlayer;


void ClientActive( edict_t *pEdict, bool bLoadGame );
void ClientPutInServer( edict_t *pEdict, const char *playername );
void ClientCommand( CBasePlayer *pSender, const CCommand &args );
void ClientPrecache( void );
// Game specific precaches
void ClientGamePrecache( void );
const char *GetGameDescription( void );
void Host_Say( edict_t *pEdict, bool teamonly );
void Host_Say(edict_t *pEdict, const CCommand &args, bool teamonly, std::vector<CBasePlayer*>* recipients = NULL, bool bAdminOnly = false);



#endif		// CLIENT_H
