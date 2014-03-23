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
/*
	Contrib by HairyPotter: These bots could do soo much more, and also be soo much more efficient as far as CPU goes.
	Maybe I'll remove some code and do a few teaks here and there. Then again it's not like these will be used much anyway.
*/

//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Basic BOT handling.
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "player.h"
//#include "sdk_player.h"
#include "in_buttons.h"
#include "movehelper_server.h"
#include "gameinterface.h"
#include "team.h"
#include "hl2mp_gamerules.h"
#include "sdk_bot_temp.h"
#include "hl2mp_player.h"
#include "../bg2/flag.h"
#include "../bg2/weapon_bg2base.h"

class CSDKBot;
void Bot_Think( CSDKBot *pBot );


ConVar bot_forcefireweapon( "bot_forcefireweapon", "", 0, "Force bots with the specified weapon to fire." );
ConVar bot_forceattack2( "bot_forceattack2", "0", 0, "When firing, use attack2." );
ConVar bot_forceattackon( "bot_forceattackon", "0", 0, "When firing, don't tap fire, hold it down." );
ConVar bot_flipout( "bot_flipout", "1", 0, "When on, all bots fire their guns." );
ConVar bot_limitclass( "bot_limitclass", "1", 0, "Force bots to conform to class regulations." );
ConVar bot_forceclass( "bot_forceclass", "-1", 0, "Force bots to spawn as given class. 0 = Infantry, 1 = Officer, 2 = Sniper, 3 = Skirmisher" );
static ConVar bot_mimic( "bot_mimic", "0", 0, "Bot uses usercmd of player by index." );
static ConVar bot_mimic_yaw_offset( "bot_mimic_yaw_offset", "0", 0, "Offsets the bot yaw." );
ConVar bot_pause("bot_pause", "0", 0, "Stops the bot thinking cycle entirely." );
ConVar bot_voicecomms("bot_voicecomms", "0", 0, "Allows bots to use voicecomm commands.");

//ConVar bot_sendcmd( "bot_sendcmd", "", 0, "Forces bots to send the specified command." );

//So what if these are global? Want to fight about it?
int g_CurBotNumber = 1; //This int here is pretty much used for the bot names only.
CSDKBot	gBots[MAX_PLAYERS];
bool m_bServerReady = false; //Are we ready to use the temp int below?
int m_iWaitingAmount = 0; //This is just a temp int really.


//-----------------------------------------------------------------------------
// Purpose: Create a new Bot and put it in the game.
// Output : Pointer to the new Bot, or NULL if there's no free clients.
//-----------------------------------------------------------------------------
CBasePlayer *BotPutInServer( int iAmount, bool bFrozen )
{
	int i = 1;
	while( i <= iAmount )
	{
		char botname[ 64 ];
		Q_snprintf( botname, sizeof( botname ), "Bot%02i", g_CurBotNumber );

		// This trick lets us create a CSDKBot for this client instead of the CSDKPlayer
		// that we would normally get when ClientPutInServer is called.
		//ClientPutInServerOverride( &CBotManager::ClientPutInServerOverride_Bot );
		edict_t *pEdict = engine->CreateFakeClient( botname );
		//ClientPutInServerOverride( NULL );

		if (!pEdict)
		{
			Msg( "Failed to create Bot(%s).\n", botname );
			return NULL;
		}

		// Allocate a player entity for the bot, and call spawn
		CSDKPlayer *pPlayer = ((CSDKPlayer *)CBaseEntity::Instance( pEdict ));

		if( !pPlayer )
		{
			Msg( "Couldn't cast bot to player\n" );
			return NULL;
		}

		//gBots[pPlayer->GetClientIndex()].m_bInuse = true;
		gBots[pPlayer->GetClientIndex()].m_pPlayer = pPlayer;
		gBots[pPlayer->GetClientIndex()].reload = 0;
		gBots[pPlayer->GetClientIndex()].attack = 0;
		gBots[pPlayer->GetClientIndex()].attack2 = 0;
		gBots[pPlayer->GetClientIndex()].respawn = 0;
		gBots[pPlayer->GetClientIndex()].m_flNextStrafeTime = 0;

		pPlayer->ClearFlags();

		//new team/class picking code. it works.
		static int lastTeam = 0;

		//lastClass = (lastClass + 1) % 4; //4 classes now.
		int lastClass = RandomInt( 0, 3 );

		int iTeam = TEAM_AMERICANS + lastTeam;
		pPlayer->ChangeTeam( iTeam );

		//BG2 - Obey Class Limits now. -HairyPotter
		if ( bot_forceclass.GetInt() > -1 ) //Force the bot to spawn as the given class.
			((CHL2MP_Player*)pPlayer)->SetNextClass( bot_forceclass.GetInt() );

		else if ( bot_limitclass.GetBool() && bot_forceclass.GetInt() < 0 ) //Just make sure we're not trying to force a class that has a limit.
		{
			int limit = HL2MPRules()->GetLimitTeamClass( iTeam, lastClass );
			if ( limit >= 0 && g_Teams[iTeam]->GetNumOfNextClass(lastClass) >= limit )
			{
				Msg("Tried to spawn too much of class %i. Spawning as infantry. \n", lastClass );
				((CHL2MP_Player*)pPlayer)->SetNextClass( 0 ); //Infantry by default
			}
			else
				((CHL2MP_Player*)pPlayer)->SetNextClass( lastClass );
		}
		else //Otherwise just spawn as whatever RandomInt() turns up.
			((CHL2MP_Player*)pPlayer)->SetNextClass( lastClass );

		lastTeam = !lastTeam; //Alternate after we've aready set the other bot's team.

		pPlayer->AddFlag( FL_CLIENT | FL_FAKECLIENT );
		pPlayer->Spawn();

		g_CurBotNumber++;
		
		i++;
		//return pPlayer;
	}
	return NULL;
}

// Handler for the "bot" command.
/*void BotAdd_f()
{
	if ( args.ArgC()> 1 )
		BotPutInServer( args[1], false );//bFrozen ); //Spawn given number of bots.
	else
		BotPutInServer( 1, false );//bFrozen ); //Just spawn 1 bot.
}*/

//ConCommand  cc_Bot( "bot_add", BotAdd_f, "Add a bot", FCVAR_CHEAT );
CON_COMMAND_F( bot_add, "Creates bot(s)in the server. <Bot Count>", FCVAR_CHEAT )
{
	int m_iCount = 0;
	if ( args.ArgC() > 1 )
		m_iCount = atoi( args[1] ); //Spawn given number of bots.
	else
		m_iCount = 1; //Just spawn 1 bot.

	if ( m_bServerReady ) //Server is already loaded, just do it.
		BotPutInServer( m_iCount, false );
	else				  //Server just exec'd the server.cfg, but isn't ready to handle players. Just wait a while.
		m_iWaitingAmount = m_iCount;
}	

//-----------------------------------------------------------------------------
// Purpose: Run through all the Bots in the game and let them think.
//-----------------------------------------------------------------------------
void Bot_RunAll( void )
{
	if ( bot_pause.GetBool() ) //If we're true, just don't run the thinking cycle. Effectively "pausing" the bots.
		return;

	if ( m_iWaitingAmount && m_bServerReady ) //Kind of a shitty hack. But this will allow people to spawn a certain amount of bots in 
	{										  //the server.cfg. Anyway, the server is ready, so do it.
		BotPutInServer( m_iWaitingAmount, false );
		m_iWaitingAmount = 0; //Make sure we've reset the waiting count.
	}

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CSDKPlayer *pPlayer = UTIL_PlayerByIndex( i );// );

		if ( pPlayer && (pPlayer->GetFlags() & FL_FAKECLIENT) )
		{
			CSDKBot *pBot = &gBots[pPlayer->GetClientIndex()];
			if( pBot && pBot->m_pPlayer->IsAlive() ) //Do most of the "filtering" here.
				Bot_Think( pBot );
		}
	}
}

//returns whether entity B is in sight of entity A
bool IsInSight( CBaseEntity *pA, CBaseEntity *pB )
{
	trace_t tr;	
	UTIL_TraceLine( pA->GetLocalOrigin() + Vector(0,0,36), 
					pB->GetLocalOrigin() + Vector(0,0,36),
					MASK_SOLID, pA, COLLISION_GROUP_DEBRIS_TRIGGER, &tr );

	return !tr.DidHitWorld();
}

CBasePlayer *FindClosestEnemy( CSDKBot *pBot, bool insight )
{
	//Msg( "FindClosestEnemy(insight=%s)\n", insight ? "true" : "false" );
	//if( !pBot->m_pPlayer->IsAlive() ) //This is now checked by Bot_Runall to save CPU.
	//	return NULL;

	int team = pBot->m_pPlayer->GetTeam()->GetTeamNumber();

	if( team < TEAM_AMERICANS && team < TEAM_BRITISH )
		return NULL;	//spectator or unassigned

	int otherteam = team == TEAM_AMERICANS ? TEAM_BRITISH : TEAM_AMERICANS;

	CBasePlayer *pClosest = NULL;
	float		maxdist = 1500.0f; //3000.0f

	CTeam *pTeam = g_Teams[otherteam];
	for( int x = 0; x < pTeam->GetNumPlayers(); x++ )
	{
		CBasePlayer *pEnemy = pTeam->GetPlayer(x);

		if ( !pEnemy )
			continue; 

		if( !pEnemy->IsAlive() )
			continue;

		float dist = (pEnemy->GetLocalOrigin() - pBot->m_pPlayer->GetLocalOrigin()).Length();
		if( dist < maxdist )
		{
			//check to make sure enemy is in sight
			if( !IsInSight( pBot->m_pPlayer, pEnemy ) )
				continue;

			//mindist = dist;
			pClosest = pEnemy;
		}
	}

	//if( pdist )
	//	*pdist = mindist;

	return pClosest;
}

CBasePlayer *FindClosestFriend( CSDKBot *pBot, bool insight, float *pdist )
{
	//Msg( "FindClosestEnemy(insight=%s)\n", insight ? "true" : "false" );
	//if( !pBot->m_pPlayer->IsAlive() ) //This is now run in Bot_Runall to save CPU.
	//	return NULL;

	int team = pBot->m_pPlayer->GetTeam()->GetTeamNumber();

	if( team < TEAM_AMERICANS && team < TEAM_BRITISH )
		return NULL;	//spectator or unassigned

	CBasePlayer *pClosest = NULL;
	float		mindist = 3000.0f;

	CTeam *pTeam = g_Teams[team];
	for( int x = 0; x < pTeam->GetNumPlayers(); x++ )
	{
		CBasePlayer *pEnemy = pTeam->GetPlayer(x);

		if ( !pEnemy )
			continue; 

		if( !pEnemy->IsAlive() )
			continue;

		float dist = (pEnemy->GetLocalOrigin() - pBot->m_pPlayer->GetLocalOrigin()).Length();
		if( dist < mindist )
		{
			if( insight )
			{
				//check to make sure enemy is in sight
				if( !IsInSight( pBot->m_pPlayer, pEnemy ) )
					continue;
			}

			mindist = dist;
			pClosest = pEnemy;
		}
	}

	//if( pdist )
	//	*pdist = mindist;

	return pClosest;
}

CFlag *FindClosestFlagToSpot( CSDKBot *pBot, bool insight, bool checkTeam, Vector vSpot )
{
	//if checkTeam, ignore flags that belong to the bot's team
	int team = pBot->m_pPlayer->GetTeam()->GetTeamNumber();
	CFlag *pClosest = NULL;
	vec_t dist = 1000000000;
	CBaseEntity *pEntity = NULL;

	while( (pEntity = gEntList.FindEntityByClassname( pEntity, "flag" )) != NULL )
	{
		CFlag *pFlag = dynamic_cast<CFlag*>(pEntity);
		
		if( !pFlag || (checkTeam && pFlag->GetTeamNumber() == team) )
			continue;

		if( insight )
		{
			//check to make sure flag is in sight
			if( !IsInSight( pBot->m_pPlayer, pFlag ) )
				continue;
		}

		vec_t dist2 = pFlag->GetLocalOrigin().DistToSqr(vSpot);

		if( !pClosest || dist2 < dist )
		{
			pClosest = pFlag;
			dist = dist2;
		}
	}

	return pClosest;
}

CFlag *FindClosestFlag( CSDKBot *pBot, bool insight )
{
	CFlag *pClosestFlagInSight = FindClosestFlagToSpot( pBot, true, true, pBot->m_pPlayer->GetLocalOrigin() );

	if( pClosestFlagInSight )
	{
		//we have a flag in sight - done
		return pClosestFlagInSight;
	}

	//backup plan - find flag in sight which is closest to the closest enemy flag not in sight
	//additionally, the flag in sight must be a little bit closer to the desired flag than the bot is
	CFlag *pClosestFlag = FindClosestFlagToSpot( pBot, false, true, pBot->m_pPlayer->GetLocalOrigin() );

	if( !pClosestFlag )
	{
		//no enemy flags left - nothing to do
		return NULL;
	}

	//OK, we have some flag (pClosestFlag) out of sight we want to get to
	//navigate via the flag in sight that is closest to pClosestFlag
	CFlag *pClosestAnyFlagInSight = FindClosestFlagToSpot( pBot, true, false, pClosestFlag->GetLocalOrigin() );

	if( !pClosestAnyFlagInSight )
	{
		//no luck
		return NULL;
	}

	//finally, compare distances
	if( pClosestAnyFlagInSight->GetLocalOrigin().DistToSqr(pClosestFlag->GetLocalOrigin()) <
	           pBot->m_pPlayer->GetLocalOrigin().DistToSqr(pClosestFlag->GetLocalOrigin()) - 1024 )
	{
		//the flag is closer than we are. good enough
		return pClosestAnyFlagInSight;
	}
	else
	{
		//no good
		return NULL;
	}
}

bool Bot_RunMimicCommand( CUserCmd& cmd )
{
	if ( bot_mimic.GetInt() <= 0 )
		return false;

	if ( bot_mimic.GetInt() > gpGlobals->maxClients )
		return false;

	
	CBasePlayer *pPlayer = UTIL_PlayerByIndex( bot_mimic.GetInt()  );
	if ( !pPlayer )
		return false;

	if ( !pPlayer->GetLastUserCommand() )
		return false;

	cmd = *pPlayer->GetLastUserCommand();
	cmd.viewangles[YAW] += bot_mimic_yaw_offset.GetFloat();

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Simulates a single frame of movement for a player
// Input  : *fakeclient - 
//			*viewangles - 
//			forwardmove - 
//			m_flSideMove - 
//			upmove - 
//			buttons - 
//			impulse - 
//			msec - 
// Output : 	virtual void
//-----------------------------------------------------------------------------
static void RunPlayerMove( CSDKPlayer *fakeclient, CUserCmd &cmd, float frametime )
{
	//if ( !fakeclient )
	//	return;

	// Store off the globals.. they're gonna get whacked
	float flOldFrametime = gpGlobals->frametime;
	float flOldCurtime = gpGlobals->curtime;

	float flTimeBase = gpGlobals->curtime + gpGlobals->frametime - frametime;
	fakeclient->SetTimeBase( flTimeBase );

	MoveHelperServer()->SetHost( fakeclient );
	fakeclient->PlayerRunCommand( &cmd, MoveHelperServer() );

	// save off the last good usercmd
	fakeclient->SetLastUserCommand( cmd );

	// Clear out any fixangle that has been set
	fakeclient->pl.fixangle = FIXANGLE_NONE;

	// Restore the globals..
	gpGlobals->frametime = flOldFrametime;
	gpGlobals->curtime = flOldCurtime;
	MoveHelperServer()->SetHost( NULL );
}

void Bot_FlipOut( CSDKBot *pBot, CUserCmd &cmd, bool mayAttack, bool mayReload )
{
	if ( bot_flipout.GetInt() > 0 /*&& pBot->m_pPlayer->IsAlive()*/ )
	{
		if ( pBot->attack++ >= 20 )//(RandomFloat(0.0,1.0) > 0.5) )
		{
			//cmd.buttons |= bot_forceattack2.GetBool() ? IN_ATTACK2 : IN_ATTACK;
			if( mayAttack )
				cmd.buttons |= IN_ATTACK;

			pBot->attack = 0;
		}

		if( pBot->attack2++ >= 13 )
		{
			if( mayAttack )
				cmd.buttons |= IN_ATTACK2;

			pBot->attack2 = 0;
		}

		/*if( RandomFloat( 0, 1 ) > 0.9 )
			cmd.buttons |= IN_RELOAD;*/
		if( pBot->reload++ >= 40 )
		{
			if( mayReload )
				cmd.buttons |= IN_RELOAD;

			pBot->reload = 0;
		}

		if ( bot_flipout.GetInt() >= 2 )
		{
			QAngle angOffset = RandomAngle( -1, 1 );

			pBot->m_LastAngles += angOffset;

			for ( int i = 0 ; i < 2; i++ )
			{
				if ( fabs( pBot->m_LastAngles[ i ] - pBot->m_ForwardAngle[ i ] ) > 15.0f )
				{
					if ( pBot->m_LastAngles[ i ] > pBot->m_ForwardAngle[ i ] )
					{
						pBot->m_LastAngles[ i ] = pBot->m_ForwardAngle[ i ] + 15;
					}
					else
					{
						pBot->m_LastAngles[ i ] = pBot->m_ForwardAngle[ i ] - 15;
					}
				}
			}

			pBot->m_LastAngles[ 2 ] = 0;

			pBot->m_pPlayer->SetLocalAngles( pBot->m_LastAngles );
		}
	}
}

/**
 * A simple lagged Fibonacchi generator.
 * We need this because something's wrong with Valve's PRNG.
 * Both RandomInt() and random->RandomInt() seem to eventually "run of out randomness".
 * Also, Valve decided to replace rand() with a call to random->RandomInt() for some reason.
 */
int bot_rand()
{
	static int a = 9158152, b = 14257153;

	a += b + (a >> 5);
	b += a + (b >> 11);

	return b;
}

void Bot_UpdateStrafing( CSDKBot *pBot, CUserCmd &cmd )
{
	if ( gpGlobals->curtime >= pBot->m_flNextStrafeTime )
	{
		pBot->m_flNextStrafeTime = gpGlobals->curtime + RandomFloat( 0.7f, 1.5f );

		if ( bot_rand() % 5 == 0 )
		{
			pBot->m_flSideMove = -600.0f + (bot_rand() % 1200);
		}
		else
		{
			pBot->m_flSideMove = 0;
		}

		if ( bot_rand() % 20 == 0 )
		{
			pBot->m_bBackwards = true;
		}
		else
		{
			pBot->m_bBackwards = false;
		}
	}
	else
		pBot->m_flSideMove *= 0.9f;

	cmd.sidemove = pBot->m_flSideMove;
}

/**
 * Traces in front of the bot to see if something's in the way.
 * Used for basic obstacle avoidance.
 */
bool Bot_TraceAhead( CSDKBot *pBot, CUserCmd &cmd )
{
	trace_t tr;	
	Vector forward;

	AngleVectors( pBot->m_LastAngles, &forward );

	for( int ofs = -40; ofs < 56; ofs += 8 )
	{
		UTIL_TraceLine( pBot->m_pPlayer->Weapon_ShootPosition() + Vector(0,0,ofs),
						pBot->m_pPlayer->Weapon_ShootPosition() + Vector(0,0,ofs) + forward * 56,
						MASK_SOLID, pBot->m_pPlayer, COLLISION_GROUP_NONE, &tr );

		if( tr.fraction < 1.0 )
			return true;
	}

	return false;
}

void Bot_UpdateDirectionAndSpeed( CSDKBot *pBot, CUserCmd &cmd )
{
	bool shouldMove = true;
	bool mayAttack = false;
	bool mayReload = true;
	CBasePlayer *pEnemy = FindClosestEnemy( pBot, true );
	CFlag *pFlag = FindClosestFlag( pBot, true );
	CBaseBG2Weapon *pActiveWeapon = dynamic_cast<CBaseBG2Weapon*>(pBot->m_pPlayer->GetActiveWeapon());
	float flagDistance = 0;

	if( pFlag ) //Found a flag
		flagDistance = pBot->m_pPlayer->GetLocalOrigin().DistToSqr( pFlag->GetLocalOrigin() );

	if( pEnemy ) //Found an enemy.
	{
		QAngle angles;
		//Vector forward = pEnemy->GetLocalOrigin() - pBot->m_pPlayer->GetLocalOrigin();
		//aim for the head if you can find it...
		Vector forward = pEnemy->Weapon_ShootPosition() - pBot->m_pPlayer->Weapon_ShootPosition();
		forward.z -= 8;	//slightly below
		VectorAngles( forward, angles );
		pBot->m_pPlayer->SetLocalAngles( angles );
		cmd.viewangles = angles;
		pBot->m_LastAngles = angles;

		//don't move if near the flag
		if( flagDistance < 4096 )
			shouldMove = false;

		//only attack if close enough
		mayAttack = pBot->m_pPlayer->GetLocalOrigin().DistToSqr( pEnemy->GetLocalOrigin() ) < 1000000;
		
		//only allow reload if the current weapon lacks a melee attack
		if( pActiveWeapon && (pActiveWeapon->GetAttackType( CBaseBG2Weapon::ATTACK_PRIMARY ) == CBaseBG2Weapon::ATTACKTYPE_STAB ||
				pActiveWeapon->GetAttackType( CBaseBG2Weapon::ATTACK_PRIMARY ) == CBaseBG2Weapon::ATTACKTYPE_SLASH ||
				pActiveWeapon->GetAttackType( CBaseBG2Weapon::ATTACK_SECONDARY ) == CBaseBG2Weapon::ATTACKTYPE_STAB ||
				pActiveWeapon->GetAttackType( CBaseBG2Weapon::ATTACK_SECONDARY ) == CBaseBG2Weapon::ATTACKTYPE_SLASH) )
			mayReload = false;
		else
			mayReload = true;
	}
	else if( pFlag ) //Flags do not take priority.
	{
		if( flagDistance > 1024 )
		{
			QAngle angles;
			//Vector forward = pEnemy->GetLocalOrigin() - pBot->m_pPlayer->GetLocalOrigin();
			//aim for the head if you can find it...
			Vector forward = pFlag->GetLocalOrigin() - pBot->m_pPlayer->GetLocalOrigin();
			forward.z -= 8;	//slightly below
			VectorAngles( forward, angles );
			pBot->m_pPlayer->SetLocalAngles( angles );
			cmd.viewangles = angles;
			pBot->m_LastAngles = angles;
		}
		else
			shouldMove = false;
	}
	else if( Bot_TraceAhead( pBot, cmd ) )
	{
		//something is in the way - turn right a bit
		QAngle angles = pBot->m_pPlayer->GetLocalAngles();

		//head in random direction
		angles.y = bot_rand() % 360;

		pBot->m_pPlayer->SetLocalAngles( angles );
		pBot->m_LastAngles = angles;
		cmd.viewangles = angles;
	}
	else
	{
		pBot->m_pPlayer->SetLocalAngles( pBot->m_LastAngles );
		cmd.viewangles = pBot->m_LastAngles;
	}

	if( shouldMove )
	{
		cmd.forwardmove = 600 * ( pBot->m_bBackwards ? -1 : 1 );

		if ( pBot->m_flSideMove != 0.0f )
			cmd.forwardmove *= RandomFloat( 0.1, 1.0f );

		Bot_UpdateStrafing( pBot, cmd );
	}
	else
	{
		cmd.forwardmove = 0;
		cmd.sidemove = pBot->m_flSideMove = 0;
	}

	Bot_FlipOut( pBot, cmd, mayAttack, mayReload );
	

	//This code just makes the bot go insane anyway.
	/*float angledelta = 15.0;
	QAngle angle;

	int maxtries = (int)360.0/angledelta;

	if ( pBot->m_bLastTurnToRight )
	{
		angledelta = -angledelta;
	}

	angle = pBot->m_pPlayer->GetLocalAngles();

	trace_t trace;
	Vector vecSrc, vecEnd, forward;
	while ( --maxtries >= 0 )
	{
		AngleVectors( angle, &forward );

		vecSrc = pBot->m_pPlayer->GetLocalOrigin() + Vector( 0, 0, 36 );

		vecEnd = vecSrc + forward * 10;

		UTIL_TraceHull( vecSrc, vecEnd, VEC_HULL_MIN, VEC_HULL_MAX, 
			MASK_PLAYERSOLID, pBot->m_pPlayer, COLLISION_GROUP_NONE, &trace );

		if ( trace.fraction == 1.0 )
		{
			if ( gpGlobals->curtime < pBot->m_flNextTurnTime )
			{
				break;
			}
		}

		angle.y += angledelta;

		if ( angle.y > 180 )
			angle.y -= 360;
		else if ( angle.y < -180 )
			angle.y += 360;

		pBot->m_flNextTurnTime = gpGlobals->curtime + 2.0;
		pBot->m_bLastTurnToRight = RandomInt( 0, 1 ) == 0 ? true : false;

		pBot->m_ForwardAngle = angle;
		pBot->m_LastAngles = angle;
	}
	
	pBot->m_pPlayer->SetLocalAngles( angle );*/
}

/*
void Bot_HandleSendCmd( CSDKBot *pBot )
{
	if ( strlen( bot_sendcmd.GetString() ) > 0 ) //Fix This. -HairyPotter
	{
		const char *ccmd = bot_sendcmd.GetString();
		//send the cmd from this bot
		pBot->ClientCommand( ccmd );

		bot_sendcmd.SetValue("");
	}
}*/


// If bots are being forced to fire a weapon, see if I have it
void Bot_ForceFireWeapon( CSDKBot *pBot, CUserCmd &cmd )
{
	if ( bot_forcefireweapon.GetString() )
	{
		CBaseCombatWeapon *pWeapon = pBot->m_pPlayer->Weapon_OwnsThisType( bot_forcefireweapon.GetString() );
		if ( pWeapon )
		{
			// Switch to it if we don't have it out
			CBaseCombatWeapon *pActiveWeapon = pBot->m_pPlayer->GetActiveWeapon();

			// Switch?
			if ( pActiveWeapon != pWeapon )
			{
				pBot->m_pPlayer->Weapon_Switch( pWeapon );
			}
			else
			{
				// Start firing
				// Some weapons require releases, so randomise firing
				if ( bot_forceattackon.GetBool() || (RandomFloat(0.0,1.0) > 0.5) )
				{
					cmd.buttons |= bot_forceattack2.GetBool() ? IN_ATTACK2 : IN_ATTACK;
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Run this Bot's AI for one frame.
//-----------------------------------------------------------------------------
char vcmd[512];
void Bot_Think( CSDKBot *pBot )
{
	/*if( !pBot ) //These are figured out by bot_runall anyway.
		return;
	if( !pBot->m_bInuse || !pBot->m_pPlayer )
		return;*/

	// Make sure we stay being a bot
	//pBot->m_pPlayer->AddFlag( FL_FAKECLIENT );

	CUserCmd cmd;
	Q_memset( &cmd, 0, sizeof( cmd ) );


	//For testing Voicecomms -HairyPotter
	if ( bot_voicecomms.GetInt() && pBot->m_flNextVoice < gpGlobals->curtime )
	{
		Q_snprintf( vcmd, 512, "voicecomm %i\n", RandomInt(1,7)  );
		CBaseEntity *ent = dynamic_cast< CBaseEntity * >( pBot->m_pPlayer );
		engine->ClientCommand( ent->edict(), vcmd );
		//engine->ClientCommand( pBot->m_pPlayer->edict(), UTIL_VarArgs( "voicecomm %i\n", RandomInt(1,7) ) );
		Msg( vcmd );
		pBot->m_flNextVoice = gpGlobals->curtime + 2.0f;
	}

	// Finally, override all this stuff if the bot is being forced to mimic a player.
	if ( !Bot_RunMimicCommand( cmd ) )
	{
		Bot_UpdateDirectionAndSpeed( pBot, cmd );

		// Handle console settings.
		Bot_ForceFireWeapon( pBot, cmd );
		//Bot_HandleSendCmd( pBot );

		// Fix up the m_fEffects flags
		//pBot->m_pPlayer->PostClientMessagesSent();
	}
	//cmd.upmove = 0;
	//cmd.impulse = 0;

	pBot->m_LastCmd = cmd;

	RunPlayerMove( pBot->m_pPlayer, cmd, gpGlobals->frametime );
}


