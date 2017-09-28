//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "hl2mp_gamerules.h"
#include "viewport_panel_names.h"
#include "gameeventdefs.h"
#include <KeyValues.h>
#include "ammodef.h"

#include "../../server/bg3/bg3_class.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
#else

	#include "eventqueue.h"
	#include "player.h"
	#include "gamerules.h"
	#include "game.h"
	#include "items.h"
	#include "entitylist.h"
	#include "mapentities.h"
	#include "in_buttons.h"
	#include <ctype.h>
	#include "voice_gamemgr.h"
	#include "iscorer.h"
	#include "hl2mp_player.h"
	#include "weapon_hl2mpbasehlmpcombatweapon.h"
	#include "team.h"
	#include "voice_gamemgr.h"
	#include "hl2mp_gameinterface.h"
	#include "hl2mp_cvars.h"
//BG2 - Draco
	#include "triggers.h"
	#include "../../server/bg2/bg2_maptriggers.h"
	#include "../../server/bg2/flag.h"
	#include "../../server/bg2/ctfflag.h"
	#include "../../server/bg2/mapfilter.h" 
	#include "../../shared/bg3/bg3_buffs.h"
//BG2 - Tjoppen - #includes
#include "sdk/bg3_bot.h"
#include "bg3_bot_manager.h"
#include "bg3_bot_vcomms.h"
#ifdef DEBUG	
	#include "hl2mp_bot_temp.h"
#endif

extern void respawn(CBaseEntity *pEdict, bool fCopyCorpse);

extern bool FindInList( const char **pStrings, const char *pToFind );

ConVar sv_hl2mp_weapon_respawn_time( "sv_hl2mp_weapon_respawn_time", "20", FCVAR_GAMEDLL | FCVAR_NOTIFY );
ConVar sv_hl2mp_item_respawn_time( "sv_hl2mp_item_respawn_time", "30", FCVAR_GAMEDLL | FCVAR_NOTIFY );
ConVar sv_report_client_settings("sv_report_client_settings", "0", FCVAR_GAMEDLL | FCVAR_NOTIFY );

extern ConVar mp_chattime;

//BG2 - Draco - Start
ConVar sv_restartround("sv_restartround", "0", FCVAR_GAMEDLL | FCVAR_NOTIFY);
ConVar sv_restartmap("sv_restartmap", "0", FCVAR_GAMEDLL | FCVAR_NOTIFY);
ConVar mp_americanscore("mp_americanscore", "0", FCVAR_GAMEDLL /*| FCVAR_NOTIFY*/ | FCVAR_CHEAT);
ConVar mp_britishscore("mp_britishscore", "0", FCVAR_GAMEDLL /*| FCVAR_NOTIFY*/ | FCVAR_CHEAT);
ConVar mp_autobalanceteams("mp_autobalanceteams", "1", FCVAR_GAMEDLL | FCVAR_NOTIFY);
ConVar mp_autobalancetolerance("mp_autobalancetolerance", "3", FCVAR_GAMEDLL | FCVAR_NOTIFY);
ConVar mp_timeleft("mp_timeleft", "0", FCVAR_GAMEDLL | FCVAR_REPLICATED, "");

//BG2 - Draco - End
//BG2 - Tjoppen - mp_winbonus
ConVar mp_winbonus("mp_winbonus", "200", FCVAR_GAMEDLL | FCVAR_NOTIFY, "Amount of points awarded to team winning the round");
ConVar mp_swapteams("mp_swapteams", "0", FCVAR_GAMEDLL | FCVAR_NOTIFY, "Whether to swap teams at end of round (-1 force no, 0 default, 1 force yes)");
//

//BG2 - Tjoppen - away with these
/*extern CBaseEntity	 *g_pLastCombineSpawn;
extern CBaseEntity	 *g_pLastRebelSpawn;*/
//

#define WEAPON_MAX_DISTANCE_FROM_SPAWN 64

#endif

//BG2 - Tjoppen - beautiful defines. you will see another one further down
#ifdef CLIENT_DLL
#define CVAR_FLAGS	(FCVAR_REPLICATED | FCVAR_NOTIFY)
#define CVAR_FLAGS_HIDDEN (FCVAR_REPLICATED)
#else
#define CVAR_FLAGS	(FCVAR_GAMEDLL | FCVAR_REPLICATED | FCVAR_NOTIFY)
#define CVAR_FLAGS_HIDDEN (FCVAR_GAMEDLL | FCVAR_REPLICATED)
#endif

#define LIMIT_DEFINES( size, sizename )\
	ConVar mp_limit_inf_a_##size( "mp_limit_inf_a_"#size, "-1", CVAR_FLAGS,\
									"Max number of Continental Soldiers on " sizename " maps" );\
	ConVar mp_limit_off_a_##size( "mp_limit_off_a_"#size, "-1", CVAR_FLAGS,\
									"Max number of Continental Officers on " sizename " maps" );\
	ConVar mp_limit_rif_a_##size( "mp_limit_rif_a_"#size, "-1", CVAR_FLAGS,\
									"Max number of Frontiersmen on " sizename " maps" );\
	ConVar mp_limit_ski_a_##size( "mp_limit_ski_a_"#size, "-1", CVAR_FLAGS,\
									"Max number of Minuteman on " sizename " maps" );\
	ConVar mp_limit_linf_a_##size("mp_limit_linf_a_"#size, "-1", CVAR_FLAGS,\
									"Max number of State Militia on " sizename " maps" );\
	ConVar mp_limit_gre_a_##size("mp_limit_gre_a_"#size, "-1", CVAR_FLAGS,\
									"Max number of French Grenadiers on " sizename " maps" );\
	ConVar mp_limit_inf_b_##size( "mp_limit_inf_b_"#size, "-1", CVAR_FLAGS,\
									"Max number of Royal Infantry on " sizename " maps" );\
	ConVar mp_limit_off_b_##size( "mp_limit_off_b_"#size, "-1", CVAR_FLAGS,\
									"Max number of Royal Commanders on " sizename " maps" );\
	ConVar mp_limit_rif_b_##size( "mp_limit_rif_b_"#size, "-1", CVAR_FLAGS,\
									"Max number of Jägers on " sizename " maps" );\
	ConVar mp_limit_ski_b_##size( "mp_limit_ski_b_"#size, "-1", CVAR_FLAGS,\
									"Max number of Natives on " sizename " maps" );\
	ConVar mp_limit_linf_b_##size("mp_limit_linf_b_"#size, "-1", CVAR_FLAGS,\
									"Max number of Light infantry on " sizename " maps" );\
	ConVar mp_limit_gre_b_##size("mp_limit_gre_b_"#size, "-1", CVAR_FLAGS,\
									"Max number of Royal Grenadiers on " sizename " maps" );
//as you can see, the macro is a shorthand and should also help avoid misspellings and such that are
//usually common with repetitive stuff like this
//BG3 - moved these definitions to the bg3_class files
//LIMIT_DEFINES(sml, "small")
//LIMIT_DEFINES(med, "medium")
//LIMIT_DEFINES(lrg, "large")

extern ConVar mp_limit_mapsize_low;
extern ConVar mp_limit_mapsize_high;

ConVar mp_respawnstyle("mp_respawnstyle", "1", CVAR_FLAGS, "0 = regular dm, 1 = waves, 2 = rounds (LMS), 3 = rounds with tickets, 4 = LMS with linebattle extensions");
ConVar mp_respawntime("mp_respawntime", "14", CVAR_FLAGS, "Time between waves, or the maximum length of the round with mp_respawnstyle 2");

//For punishing officers who waste the buff during skirmish
ConVar mp_punish_bad_officer("mp_punish_bad_officer", "1", CVAR_FLAGS, "Whether or not to auto-switch officers who use the buff ability with none of their teammates around off of the officer class. They can switch back after spending one life as a non-officer.");
ConVar mp_punish_bad_officer_nextclass("mp_bad_officer_nextclass", "1", CVAR_FLAGS, "What class to auto-switch bad officers to. 1-6 is inf, off, sniper, skirm, linf, grenadier.");

//ticket system
ConVar mp_tickets_rounds("mp_tickets_rounds", "5", CVAR_FLAGS, "Maximum number of rounds - rounds are restarted until this or mp_timelimit");
ConVar mp_tickets_roundtime("mp_tickets_roundtime", "300", CVAR_FLAGS, "Maximum length of round");
ConVar mp_tickets_a("mp_tickets_a", "100", CVAR_FLAGS, "Tickets given to americans on round start");
ConVar mp_tickets_b("mp_tickets_b", "100", CVAR_FLAGS, "Tickets given to british on round start");
ConVar mp_tickets_drain_a("mp_tickets_drain_a", "12.5", CVAR_FLAGS, "Number of tickets drained every ten seconds when the americans have more than half of their cappable flags. Can have decimals");
ConVar mp_tickets_drain_b("mp_tickets_drain_b", "12.5", CVAR_FLAGS, "Number of tickets drained every ten seconds when the british have more than half of their cappable flags. Can have decimals");

//BG3 - Awesome - linebattle cvars
ConVar lb_enforce_weapon_amer("lb_enforce_weapon_amer", "0", CVAR_FLAGS_HIDDEN, "How to enforce continental soldier's weapon in linebattle mode. 0 is disabled, 1 is enforce first weapon, 2 is enforce second weapon");
ConVar lb_enforce_weapon_brit("lb_enforce_weapon_brit", "0", CVAR_FLAGS_HIDDEN, "How to enforce royal infantry's weapon in linebattle mode. 0 is disabled, 1 is enforce first weapon, 2 is enforce second weapon");
ConVar lb_enforce_class_amer("lb_enforce_class_amer", "0", CVAR_FLAGS_HIDDEN, "How to force class change on a new linebattle round. Excludes officers. 0 is disabled, 1-6 is inf, off, sniper, skirm, linf, grenadier");
ConVar lb_enforce_class_brit("lb_enforce_class_brit", "0", CVAR_FLAGS_HIDDEN, "How to force class change on a new linebattle round. Excludes officers. 0 is disabled, 1-6 is inf, off, sniper, skirm, linf, grenadier");
ConVar lb_enforce_no_buckshot("lb_enforce_no_buckshot", "0", CVAR_FLAGS, "Whether or not to allow buckshot during linebattle mode");

ConVar lb_officer_protect("lb_officer_protect", "2", CVAR_FLAGS, "Whether or not to protect officers during early-round long-range linebattle shooting. 0 is off, 1 is first officer only, 2 is both officers.");
ConVar lb_officer_autodetect("lb_officer_autodetect", "1", CVAR_FLAGS, "Whether or not to auto-detect officers who are the same class as their teammates, based on their position in the line. 0 is off, 1 is on.");
ConVar lb_officer_classoverride_a("lb_officer_classoverride_a", "0", CVAR_FLAGS_HIDDEN, "Override for which American class is used as officer during linebattle mode. Includes officer's rallying abilities. 0 is disabled, 1-6 is inf, off, sniper, skirm, linf, grenadier");
ConVar lb_officer_classoverride_b("lb_officer_classoverride_b", "0", CVAR_FLAGS_HIDDEN, "Override for which British class is used as officer during linebattle mode. Includes officer's rallying abilities. 0 is disabled, 1-6 is inf, off, sniper, skirm, linf, grenadier");

ConVar lb_enforce_volley_fire("lb_enforce_volley_fire", "1", CVAR_FLAGS, "Whether or not to enforce volley fire on non-officers. 0 is off, 1 is on. When on, players can only fire if their officer has ironsighted or for a short time after the officer fires");
ConVar lb_enforce_volley_fire_tolerance("lb_enforce_volley_fire_tolerance", "2", CVAR_FLAGS, "If volley fire is enforced, players are given this amount of time to fire after their officer shoots.");
ConVar lb_enforce_no_troll("lb_enforce_no_troll", "1", CVAR_FLAGS, "If on, prevents rambos from shooting or stabbing, and prevents trolls from stabbing teammates in non-melee situations. 0 is off, 1 is on.");

// BG2 - VisualMelon - Can't find a better place to put this
int hitVerificationHairs = 0;
int hitVerificationLatency = 1.5;
#define TEAM_NONE -1
//



REGISTER_GAMERULES_CLASS( CHL2MPRules );

BEGIN_NETWORK_TABLE_NOBASE( CHL2MPRules, DT_HL2MPRules )

	#ifdef CLIENT_DLL
		RecvPropBool( RECVINFO( m_bTeamPlayEnabled ) ),
		RecvPropFloat( RECVINFO( m_fLastRespawnWave ) ), //BG2 This needs to be here for the timer to work. -HairyPotter
		RecvPropFloat(RECVINFO(m_fLastRoundRestart)),
		RecvPropInt(RECVINFO(m_iCurrentRound)),
	#else
		SendPropBool( SENDINFO( m_bTeamPlayEnabled ) ),
		SendPropFloat(SENDINFO(m_fLastRespawnWave)), //BG2 This needs to be here for the timer to work. -HairyPotter
		SendPropFloat(SENDINFO(m_fLastRoundRestart)),
		SendPropInt(SENDINFO(m_iCurrentRound), 8, SPROP_UNSIGNED),
	#endif

END_NETWORK_TABLE()


LINK_ENTITY_TO_CLASS( hl2mp_gamerules, CHL2MPGameRulesProxy );
IMPLEMENT_NETWORKCLASS_ALIASED( HL2MPGameRulesProxy, DT_HL2MPGameRulesProxy )

static HL2MPViewVectors g_HL2MPViewVectors(
Vector(0, 0, 60),       //VEC_VIEW (m_vView) //BG2 - Awesome[Was 64, but lowered it to match 2007 height. The 2007 height was 64 too but for some reason the 2013 one was still too high so I lowered it here]
							  
	Vector(-16, -16, 0 ),	  //VEC_HULL_MIN (m_vHullMin)
	Vector( 16,  16,  72 ),	  //VEC_HULL_MAX (m_vHullMax)
							  					
	Vector(-16, -16, 0 ),	  //VEC_DUCK_HULL_MIN (m_vDuckHullMin)
	Vector( 16,  16,  36 ),	  //VEC_DUCK_HULL_MAX	(m_vDuckHullMax)
	Vector(0, 0, 36),		  //VEC_DUCK_VIEW		(m_vDuckView) //BG2 - Awesome[Was 24, but increased it to match perceived 2007 height. See comment above]
							  					
	Vector(-10, -10, -10 ),	  //VEC_OBS_HULL_MIN	(m_vObsHullMin)
	Vector( 10,  10,  10 ),	  //VEC_OBS_HULL_MAX	(m_vObsHullMax)
							  					
	Vector( 0, 0, 14 ),		  //VEC_DEAD_VIEWHEIGHT (m_vDeadViewHeight)

	Vector(-16, -16, 0 ),	  //VEC_CROUCH_TRACE_MIN (m_vCrouchTraceMin)
	Vector( 16,  16,  60 )	  //VEC_CROUCH_TRACE_MAX (m_vCrouchTraceMax)
);

static const char *s_PreserveEnts[] =
{
	"ai_network",
	"ai_hint",
	"hl2mp_gamerules",
	"team_manager",
	"player_manager",
	"env_soundscape",
	"env_soundscape_proxy",
	"env_soundscape_triggerable",
	"env_sun",
	"env_wind",
	"env_fog_controller",
	"func_brush",
	"func_wall",
	"func_buyzone",
	"func_illusionary",
	"infodecal",
	"info_projecteddecal",
	"info_node",
	"info_target",
	"info_node_hint",
	"info_player_deathmatch",
	"info_player_combine",
	"info_player_rebel",
	"info_map_parameters",
	"keyframe_rope",
	"move_rope",
	"info_ladder",
	"player",
	"point_viewcontrol",
	"scene_manager",
	"shadow_control",
	"sky_camera",
	"soundent",
	"trigger_soundscape",
	"viewmodel",
	"predicted_viewmodel",
	"worldspawn",
	"point_devshot_camera",
	"", // END Marker
};



#ifdef CLIENT_DLL
	void RecvProxy_HL2MPRules( const RecvProp *pProp, void **pOut, void *pData, int objectID )
	{
		CHL2MPRules *pRules = HL2MPRules();
		Assert( pRules );
		*pOut = pRules;
	}

	BEGIN_RECV_TABLE( CHL2MPGameRulesProxy, DT_HL2MPGameRulesProxy )
		RecvPropDataTable( "hl2mp_gamerules_data", 0, 0, &REFERENCE_RECV_TABLE( DT_HL2MPRules ), RecvProxy_HL2MPRules )
	END_RECV_TABLE()
#else
	void* SendProxy_HL2MPRules( const SendProp *pProp, const void *pStructBase, const void *pData, CSendProxyRecipients *pRecipients, int objectID )
	{
		CHL2MPRules *pRules = HL2MPRules();
		Assert( pRules );
		return pRules;
	}

	BEGIN_SEND_TABLE( CHL2MPGameRulesProxy, DT_HL2MPGameRulesProxy )
		SendPropDataTable( "hl2mp_gamerules_data", 0, &REFERENCE_SEND_TABLE( DT_HL2MPRules ), SendProxy_HL2MPRules )
	END_SEND_TABLE()
#endif

#ifndef CLIENT_DLL

	class CVoiceGameMgrHelper : public IVoiceGameMgrHelper
	{
	public:
		virtual bool		CanPlayerHearPlayer( CBasePlayer *pListener, CBasePlayer *pTalker, bool &bProximity )
		{
			return ( pListener->GetTeamNumber() == pTalker->GetTeamNumber() );
		}
	};
	CVoiceGameMgrHelper g_VoiceGameMgrHelper;
	IVoiceGameMgrHelper *g_pVoiceGameMgrHelper = &g_VoiceGameMgrHelper;

#endif

// NOTE: the indices here must match TEAM_TERRORIST, TEAM_CT, TEAM_SPECTATOR, etc.
char *sTeamNames[] =
{
	"Unassigned",
	"Spectator",
	//BG2 - Tjoppen - team names...
	"Americans",	//combine team color is blue..
	"British",		//.. and rebel is red. no need to mess around with that
};

CHL2MPRules::CHL2MPRules()
{
#ifndef CLIENT_DLL
	// Create the team managers
	for ( int i = 0; i < ARRAYSIZE( sTeamNames ); i++ )
	{
		CTeam *pTeam = static_cast<CTeam*>(CreateEntityByName( "team_manager" ));
		pTeam->Init( sTeamNames[i], i );

		g_Teams.AddToTail( pTeam );
	}

	//reset the bot manager's think time
	CBotManager::SetNextThink(gpGlobals->curtime + 10);

	m_bTeamPlayEnabled = teamplay.GetBool();
	m_flIntermissionEndTime = 0.0f;
	m_flGameStartTime = 0;

	m_hRespawnableItemsAndWeapons.RemoveAll();
	m_tmNextPeriodicThink = 0;
	m_flRestartGameTime = 0;
	m_bCompleteReset = false;
	m_bHeardAllPlayersReady = false;
	m_bAwaitingReadyRestart = false;
	m_bChangelevelDone = false;

	//BG2 - Tjoppen - ClientPrintAll()-
	extern float flNextClientPrintAll;
	extern bool bNextClientPrintAllForce;

	flNextClientPrintAll = 0;
	bNextClientPrintAllForce = false;

	extern CBaseEntity *g_pLastIntermission;
	g_pLastIntermission = NULL;
	//
	mp_britishscore.SetValue(0);
	mp_americanscore.SetValue(0);
	//m_fEndRoundTime = -1;

	//perform drain check at the appropriate time
	//HACKHACK: We should check UsingTickets() and set the update to happen 
	//          one second from now if we're not in tickets mode.
	//          The function always returns false when called from here though.
	//          So we also delay checking fullcap by ten seconds.
	//          This is probably OK.
	m_fNextFlagUpdate = gpGlobals->curtime + 10;

	//m_iWaveTime = 0;
	//m_fEndRoundTime = gpGlobals->curtime + mp_respawntime.GetInt();
	m_fLastRoundRestart = m_fLastRespawnWave = gpGlobals->curtime;
	m_iTDMTeamThatWon = TEAM_NONE;
	m_bHasDoneWinSong = false;
	m_bHasLoggedScores = false;
	m_iAmericanDmg = 0;
	m_iBritishDmg = 0;
	m_fNextWinSong = gpGlobals->curtime;
	g_bServerReady = false; //Do this too, this will make it so map changes with bots work.
	m_iCurrentRound = 1;
	//BG2 - Skillet
#else
	m_hRagdollList.RemoveAll();

#endif
}

const CViewVectors* CHL2MPRules::GetViewVectors()const
{
	return &g_HL2MPViewVectors;
}

const HL2MPViewVectors* CHL2MPRules::GetHL2MPViewVectors()const
{
	return &g_HL2MPViewVectors;
}
	
CHL2MPRules::~CHL2MPRules( void )
{
#ifndef CLIENT_DLL
	// Note, don't delete each team since they are in the gEntList and will 
	// automatically be deleted from there, instead.
	g_Teams.Purge();
#endif
}

void CHL2MPRules::CreateStandardEntities( void )
{

#ifndef CLIENT_DLL
	// Create the entity that will send our data to the client.

	BaseClass::CreateStandardEntities();

	// BG2 - Away with these.
	/*g_pLastCombineSpawn = NULL;
	g_pLastRebelSpawn = NULL;*/

#ifdef DBGFLAG_ASSERT
	CBaseEntity *pEnt = 
#endif
	CBaseEntity::Create( "hl2mp_gamerules", vec3_origin, vec3_angle );
	Assert( pEnt );
#endif
}

//=========================================================
// FlWeaponRespawnTime - what is the time in the future
// at which this weapon may spawn?
//=========================================================
float CHL2MPRules::FlWeaponRespawnTime( CBaseCombatWeapon *pWeapon )
{
#ifndef CLIENT_DLL
	if ( weaponstay.GetInt() > 0 )
	{
		// make sure it's only certain weapons
		if ( !(pWeapon->GetWeaponFlags() & ITEM_FLAG_LIMITINWORLD) )
		{
			return 0;		// weapon respawns almost instantly
		}
	}

	return sv_hl2mp_weapon_respawn_time.GetFloat();
#endif

	return 0;		// weapon respawns almost instantly
}

void CHL2MPRules::ResetMap()
{
#ifndef CLIENT_DLL
	CMapEntityFilter filter;

	CBaseEntity *pEnt;
	// find the first entity in the entity list
	pEnt = gEntList.FirstEnt();
	// as long as we've got a valid pointer, keep looping through the list
	while (pEnt)
	{
		if (filter.ShouldCreateEntity(pEnt->GetClassname()))
		{
			//It's not in the "keep" list, so remove the ent.
			UTIL_Remove(pEnt);
			//Msg("Removed %s \n", pEnt->GetClassname());
		}
		//else
		//	Msg("Did not remove : %s \n", pEnt->GetClassname());

		pEnt = gEntList.NextEnt(pEnt);
	}
	// force the entities we've set to be removed to actually be removed
	gEntList.CleanupDeleteList();
	// with any unrequired entities removed, we use MapEntity_ParseAllEntities to reparse the map entities
	// this in effect causes them to spawn back to their normal position.
	MapEntity_ParseAllEntities(engine->GetMapEntitiesString(), &filter, true);
#endif
}

bool CHL2MPRules::IsIntermission( void )
{
#ifndef CLIENT_DLL
	return m_flIntermissionEndTime > gpGlobals->curtime;
#endif

	return false;
}

int CHL2MPRules::NumConnectedClients() {
	int num = 0;
	for (int x = 1; x <= gpGlobals->maxClients; x++)
		if (UTIL_PlayerByIndex(x))
			num++;
	return num;
}

void CHL2MPRules::PlayerKilled( CBasePlayer *pVictim, const CTakeDamageInfo &info )
{
#ifndef CLIENT_DLL
	if ( IsIntermission() )
		return;
	BaseClass::PlayerKilled( pVictim, info );
#endif
}

#ifndef CLIENT_DLL
//BG2 - This should handle all the score settings after each round, and also fire any triggers and play win music. -HairyPotter
void CHL2MPRules::HandleScores(int iTeam, int iScore, int msg_type, bool bRestart)
{
	CMapTrigger *BG2Trigger = NULL;

	while ((BG2Trigger = static_cast<CMapTrigger*>(gEntList.FindEntityByClassname(BG2Trigger, "bg2_maptrigger"))) != NULL)
	{
		switch (iTeam)
		{
		case TEAM_AMERICANS:
			if (bRestart)
				BG2Trigger->AmericanRoundWin();
			else
				BG2Trigger->AmericanMapWin();
			break;
		case TEAM_BRITISH:
			if (bRestart)
				BG2Trigger->BritishRoundWin();
			else
				BG2Trigger->BritishMapWin();
			break;
		default: //Assume a draw
			BG2Trigger->Draw();
			break;
		}
	}

	if (iScore > 0 && iTeam != TEAM_NONE)
		g_Teams[iTeam]->AddScore(iScore);

	if (bRestart)
	{
		if (UsingTickets())
		{
			//if last round, then go to intermission (next map)
			if (m_iCurrentRound >= mp_tickets_rounds.GetInt())
			{
				GoToIntermission();
				return;
			}

			m_iCurrentRound++;
		}

		//swap teams if using tickets
		// BG2 - VisualMelon - decide whether to swap teams or not
		int iSwapTeam = mp_swapteams.GetInt();
		bool bSwapTeam = false;
		if (iSwapTeam == 1 || (iSwapTeam == 0 && UsingTickets()))
			bSwapTeam = true;
		//
		RestartRound(bSwapTeam);

		//do not cause two simultaneous round restarts..
		m_bIsRestartingRound = false;
		m_flNextRoundRestart = gpGlobals->curtime + 1;
	}

	WinSong(iTeam, true);

	if (msg_type)
		ClientPrintAll(msg_type);
}

int CHL2MPRules::CountAlivePlayersAndTickets(int iTeamNumber)
{
	CTeam *pTeam = g_Teams[iTeamNumber];
	int alive = UsingTickets() ? pTeam->GetTicketsLeft() : 0;
	int x = 0;

	for (; x < pTeam->GetNumPlayers(); x++)
	{
		CBasePlayer *pPlayer = pTeam->GetPlayer(x);
		if (pPlayer && pPlayer->IsAlive())
			alive++;
	}

	return alive;
}
void CHL2MPRules::SwapPlayerTeam(CHL2MP_Player *pPlayer, bool skipAlive)
{
	if (!pPlayer || (pPlayer->IsAlive() && skipAlive))
		return;

	int edict = engine->IndexOfEdict(pPlayer->edict());
	int iTeam = pPlayer->GetTeamNumber();
	int iOtherTeam;
	int playerClass;
	const char *kitvar = NULL;
	const char *prevclassvar = NULL;

	//default to using normal kit
	bool useDefaultKit = true;

	if (iTeam == TEAM_BRITISH)
	{
		iOtherTeam = TEAM_AMERICANS;

		prevclassvar = "cl_a_prevclass";
		const char *value = engine->GetClientConVarValue(edict, prevclassvar);

		if (value && sscanf(value, "%i", &playerClass) == 1)
		{
			pPlayer->SetNextClass(playerClass);
		}
		else
		{
			playerClass = pPlayer->GetClass();
		}

		switch (playerClass)
		{
		case CLASS_INFANTRY: kitvar = "cl_kit_a_inf"; break;
		case CLASS_SKIRMISHER: kitvar = "cl_kit_a_ski"; break;
		case CLASS_OFFICER: kitvar = "cl_kit_a_off"; break;
		}
	}
	else if (iTeam == TEAM_AMERICANS)
	{
		iOtherTeam = TEAM_BRITISH;

		prevclassvar = "cl_b_prevclass";
		const char *value = engine->GetClientConVarValue(edict, prevclassvar);

		if (value && sscanf(value, "%i", &playerClass) == 1)
		{
			pPlayer->SetNextClass(playerClass);
		}
		else
		{
			playerClass = pPlayer->GetClass();
		}

		switch (playerClass)
		{
		case CLASS_INFANTRY: kitvar = "cl_kit_b_inf"; break;
		case CLASS_SKIRMISHER: kitvar = "cl_kit_b_ski"; break;
		case CLASS_LIGHT_INFANTRY: kitvar = "cl_kit_b_light"; break;
		case CLASS_OFFICER: kitvar = "cl_kit_b_off"; break;
		}
	}
	else
	{
		//ignore spectators and such
		return;
	}

	if (kitvar)
	{
		const char *value = engine->GetClientConVarValue(edict, kitvar);

		if (value && sscanf(value, "%i %i %i", &pPlayer->m_iGunKit, &pPlayer->m_iAmmoKit, &pPlayer->m_iClassSkin) == 3)
		{
			//kit successfully parsed
			useDefaultKit = false;
		}
	}

	if (useDefaultKit)
	{
		pPlayer->m_iGunKit = 1;
		pPlayer->m_iAmmoKit = AMMO_KIT_BALL;
	}

	//don't kill the player
	pPlayer->m_bNoJoinMessage = true;
	pPlayer->ChangeTeam(iOtherTeam, false);
}

void CHL2MPRules::SwapTeams(void)
{
	//Merge the british and american player lists.
	//We can't iterate over the teams separately since
	//all players would then end up on the same team.

	//Fun fact: CUtlVector tries to be noncopyable by hiding
	//its copy constructor for some reason.
	//However, operator=() is perfectly acceptable.
	CUtlVector<CBasePlayer*> players;
	players = g_Teams[TEAM_AMERICANS]->m_aPlayers;

	players.AddVectorToTail(g_Teams[TEAM_BRITISH]->m_aPlayers);

	for (int x = 0; x < players.Count(); x++)
		SwapPlayerTeam(ToHL2MPPlayer(players[x]), false);

	//swap scores
	int a = g_Teams[TEAM_AMERICANS]->GetScore();
	int b = g_Teams[TEAM_BRITISH]->GetScore();
	g_Teams[TEAM_AMERICANS]->SetScore(b);
	g_Teams[TEAM_BRITISH]->SetScore(a);
}
#endif



void CHL2MPRules::Think( void )
{

#ifndef CLIENT_DLL
	
	CGameRules::Think();

	CTeam *pAmericans = g_Teams[TEAM_AMERICANS];
	CTeam *pBritish = g_Teams[TEAM_BRITISH];

	if ( g_fGameOver )   // someone else quit the game already
	{
		if (!m_bHasDoneWinSong)
		{
			m_bHasDoneWinSong = true;
			if (pAmericans->GetScore() < pBritish->GetScore())
			{
				//British Win
				HandleScores(TEAM_BRITISH, 0, BRITISH_MAP_WIN, false);
			}

			if (pAmericans->GetScore() > pBritish->GetScore())
			{
				//Americans Win
				HandleScores(TEAM_AMERICANS, 0, AMERICAN_MAP_WIN, false);
			}

			if (pAmericans->GetScore() == pBritish->GetScore())
			{
				//Draw!
				HandleScores(TEAM_NONE, 0, MAP_DRAW, false);
			}
		}

		if (!m_bHasLoggedScores)
		{
			//BG2 - Log Damages and Scores. -HairyPotter
			for (int x = 0; x < pAmericans->GetNumPlayers(); x++)
			{
				CBasePlayer *pPlayer = pAmericans->GetPlayer(x);
				if (!pPlayer)
					continue;
				m_iAmericanDmg += pPlayer->DeathCount();
			}
			for (int x = 0; x < pBritish->GetNumPlayers(); x++)
			{
				CBasePlayer *pPlayer = pBritish->GetPlayer(x);
				if (!pPlayer)
					continue;
				m_iBritishDmg += pPlayer->DeathCount();
			}

			UTIL_LogPrintf("American Scores: DAMAGE: %i   SCORE: %i   \n", m_iAmericanDmg, mp_americanscore.GetInt());
			UTIL_LogPrintf("British Scores: DAMAGE: %i   SCORE: %i   \n", m_iBritishDmg, mp_britishscore.GetInt());

			m_bHasLoggedScores = true; //Don't do it again.
		}
		// check to see if we should change levels now
		if ( m_flIntermissionEndTime < gpGlobals->curtime )
		{
			m_bHasDoneWinSong = false;
			ChangeLevel(); // intermission is over
		}

		return;
	}

	float flTimeLimit = GetMapRemainingTime();
	float flFragLimit = fraglimit.GetFloat();

	if (flTimeLimit != 0 && gpGlobals->curtime >= flTimeLimit)
	{
		GoToIntermission();
		return;
	}

	//BG2 - Draco - Start
	if (m_fNextFlagUpdate <= gpGlobals->curtime)
	{
		if (UsingTickets())
		{
			CheckTicketDrain();
			m_fNextFlagUpdate = gpGlobals->curtime + 10;
		}
		else
		{
			CheckFullcap();
			m_fNextFlagUpdate = gpGlobals->curtime + 1;
		}
	}
	else if (m_fNextFlagUpdate == 0)
	{
		m_fNextFlagUpdate = gpGlobals->curtime + 1;
	}
	//=========================
	//Time Left
	//=========================
	mp_timeleft.SetValue((flTimeLimit - gpGlobals->curtime)); //timeleft works just as well..

	//=========================
	//Score Cvars
	//=========================
	if (mp_britishscore.GetInt() != pBritish->GetScore())
	{
		pBritish->SetScore(mp_britishscore.GetInt());
	}
	if (mp_americanscore.GetInt() != pAmericans->GetScore())
	{
		pAmericans->SetScore(mp_americanscore.GetInt());
	}
	//=========================
	//Auto Team Balance
	//=========================
	if (mp_autobalanceteams.GetInt() == 1)
	{
		//use the right sum to find diff, I don't like negative numbers...
		int iAutoTeamBalanceTeamDiff = 0;
		int iAutoTeamBalanceBiggerTeam = TEAM_BRITISH;
		if (pAmericans->GetNumPlayers() > pBritish->GetNumPlayers())
		{
			iAutoTeamBalanceTeamDiff = (pAmericans->GetNumPlayers() - pBritish->GetNumPlayers());
			iAutoTeamBalanceBiggerTeam = TEAM_AMERICANS;
		}
		else
		{
			iAutoTeamBalanceTeamDiff = (pBritish->GetNumPlayers() - pAmericans->GetNumPlayers());
			iAutoTeamBalanceBiggerTeam = TEAM_BRITISH;
		}

		if (iAutoTeamBalanceTeamDiff >= mp_autobalancetolerance.GetInt())
		{
			//here comes the tricky part, who to swap, how to swap?
			//meh, go random for now, maybe lowest scorer later...
			CTeam *pBiggerTeam = g_Teams[iAutoTeamBalanceBiggerTeam];
			int index = random->RandomInt(0, pBiggerTeam->GetNumPlayers() - 1);
			CHL2MP_Player *pPlayer = ToHL2MPPlayer(pBiggerTeam->GetPlayer(index));

			//swap player unless they're alive
			//if they are alive, we'll pick another player next think
			SwapPlayerTeam(pPlayer, true);
		}
		//well, if we aren't even now, there's always next think...
	}
	//=========================
	//Restart Round/Map
	//=========================
	if (sv_restartmap.GetInt() > 0)
	{
		m_fNextGameReset = gpGlobals->curtime + sv_restartmap.GetInt();
		sv_restartmap.SetValue(0);
	}
	//now if the time was set we can check for it, above zero means we are restarting
	if ((m_fNextGameReset > 0) && (m_fNextGameReset <= gpGlobals->curtime))
	{
		m_fNextGameReset = 0;//dont reset again

		//reset scores...
		pAmericans->SetScore(0);//...for teams...
		pBritish->SetScore(0);
		int x;
		for (x = 0; x < g_Teams[TEAM_AMERICANS]->GetNumPlayers(); x++)
		{
			CBasePlayer *pPlayer = g_Teams[TEAM_AMERICANS]->GetPlayer(x);
			if (!pPlayer)
				continue;

			pPlayer->ResetFragCount();//...for cap points...
			pPlayer->ResetDeathCount();//...and damage
		}
		for (x = 0; x < g_Teams[TEAM_BRITISH]->GetNumPlayers(); x++)
		{
			CBasePlayer *pPlayer = g_Teams[TEAM_BRITISH]->GetPlayer(x);
			if (!pPlayer)
				continue;

			pPlayer->ResetFragCount();//...for cap points...
			pPlayer->ResetDeathCount();//...and damage
		}
		m_iCurrentRound = 1;
		RestartRound(false);	//BG2 - Tjoppen - restart round

		//Reset the map time
		m_flGameStartTime = gpGlobals->curtime;
	}

	if (sv_restartround.GetInt() > 0)
	{
		m_fNextRoundReset = gpGlobals->curtime + sv_restartround.GetInt();
		sv_restartround.SetValue(0);
	}
	if ((m_fNextRoundReset > 0) && (m_fNextRoundReset <= gpGlobals->curtime))
	{
		m_fNextRoundReset = 0;//dont reset again
		RestartRound(false);	//BG2 - restart round
	}

	//=========================
	//Round systems
	//=========================
	if (mp_respawnstyle.GetInt() == 1 || UsingTickets())//wave spawning
	{
		if ((m_fLastRespawnWave + mp_respawntime.GetFloat()) <= gpGlobals->curtime)
		{
			RespawnWave();
			m_fLastRespawnWave = gpGlobals->curtime;
		}
	}
	if (IsLMS() || UsingTickets())//if line battle all at once spawn style - Draco
	{
		//Tjoppen - start
		//count alive players in each team
		int aliveamericans = CountAlivePlayersAndTickets(TEAM_AMERICANS);
		int alivebritish = CountAlivePlayersAndTickets(TEAM_BRITISH);

		if (pAmericans->GetNumPlayers() == 0 && pBritish->GetNumPlayers() == 0)
			return;

		//allow ticket round to restart even if there aren't any players on the other team
		if (!UsingTickets() && (pAmericans->GetNumPlayers() == 0 || pBritish->GetNumPlayers() == 0))
			return;

		//Tjoppen - End
		//BG2 - Tjoppen - restart rounds a few seconds after the last person is killed
		//wins
		float roundLength = UsingTickets() ? mp_tickets_roundtime.GetFloat() : mp_respawntime.GetFloat();

		if (aliveamericans == 0 || alivebritish == 0 || m_fLastRoundRestart + roundLength <= gpGlobals->curtime || m_bIsRestartingRound)
		{
			if (!m_bIsRestartingRound)
			{
				m_flNextRoundRestart = gpGlobals->curtime + 5;
				m_bIsRestartingRound = true;

				//print different messages depending on whether the losing has any players left alive
				if (aliveamericans > alivebritish)
				{
					//ClientPrintAll( alivebritish ? AMERICAN_DEFAULT_WIN : AMERICAN_ROUND_WIN );
					m_iTDMTeamThatWon = TEAM_AMERICANS;
				}
				else if (aliveamericans < alivebritish)
				{
					//ClientPrintAll( aliveamericans ? BRITISH_DEFAULT_WIN : BRITISH_ROUND_WIN );
					m_iTDMTeamThatWon = TEAM_BRITISH;
				}
				else
				{
					//ClientPrintAll( aliveamericans ? DEFAULT_DRAW : ROUND_DRAW );
					m_iTDMTeamThatWon = TEAM_NONE;
				}
			}
			else if (m_flNextRoundRestart < gpGlobals->curtime)
			{
				int score = 0;

				if (m_iTDMTeamThatWon != TEAM_NONE)
					score = 1;

				HandleScores(m_iTDMTeamThatWon, score, 0, true);
			}
		}
	}
	//BG2 - Draco - End

	if (flFragLimit)
	{
		if (pAmericans->GetScore() >= flFragLimit || pBritish->GetScore() >= flFragLimit)
		{
			GoToIntermission();
			return;
		}
	}

	/*if ( gpGlobals->curtime > m_tmNextPeriodicThink )
	{		
		CheckAllPlayersReady();
		CheckRestartGame();
		m_tmNextPeriodicThink = gpGlobals->curtime + 1.0;
	}

	if ( m_flRestartGameTime > 0.0f && m_flRestartGameTime <= gpGlobals->curtime )
	{
		RestartGame();
	}

	if( m_bAwaitingReadyRestart && m_bHeardAllPlayersReady )
	{
		UTIL_ClientPrintAll( HUD_PRINTCENTER, "All players ready. Game will restart in 5 seconds" );
		UTIL_ClientPrintAll( HUD_PRINTCONSOLE, "All players ready. Game will restart in 5 seconds" );

		m_flRestartGameTime = gpGlobals->curtime + 5;
		m_bAwaitingReadyRestart = false;
	}*/

	ManageObjectRelocation();

#endif
}

void CHL2MPRules::GoToIntermission( void )
{
#ifndef CLIENT_DLL
	if ( g_fGameOver )
		return;

	g_fGameOver = true;

	m_flIntermissionEndTime = gpGlobals->curtime + mp_chattime.GetInt();

	for ( int i = 0; i < MAX_PLAYERS; i++ )
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );

		if ( !pPlayer )
			continue;

		pPlayer->ShowViewPortPanel( PANEL_SCOREBOARD );
		pPlayer->AddFlag( FL_FROZEN );
	}
#endif
	
}

bool CHL2MPRules::CheckGameOver()
{
#ifndef CLIENT_DLL
	if ( g_fGameOver )   // someone else quit the game already
	{
		// check to see if we should change levels now
		if ( m_flIntermissionEndTime < gpGlobals->curtime )
		{
			ChangeLevel(); // intermission is over			
		}

		return true;
	}
#endif

	return false;
}

// when we are within this close to running out of entities,  items 
// marked with the ITEM_FLAG_LIMITINWORLD will delay their respawn
#define ENTITY_INTOLERANCE	100

//=========================================================
// FlWeaponRespawnTime - Returns 0 if the weapon can respawn 
// now,  otherwise it returns the time at which it can try
// to spawn again.
//=========================================================
float CHL2MPRules::FlWeaponTryRespawn( CBaseCombatWeapon *pWeapon )
{
#ifndef CLIENT_DLL
	if ( pWeapon && (pWeapon->GetWeaponFlags() & ITEM_FLAG_LIMITINWORLD) )
	{
		if ( gEntList.NumberOfEntities() < (gpGlobals->maxEntities - ENTITY_INTOLERANCE) )
			return 0;

		// we're past the entity tolerance level,  so delay the respawn
		return FlWeaponRespawnTime( pWeapon );
	}
#endif
	return 0;
}

//=========================================================
// VecWeaponRespawnSpot - where should this weapon spawn?
// Some game variations may choose to randomize spawn locations
//=========================================================
Vector CHL2MPRules::VecWeaponRespawnSpot( CBaseCombatWeapon *pWeapon )
{
#ifndef CLIENT_DLL
	CWeaponHL2MPBase *pHL2Weapon = dynamic_cast< CWeaponHL2MPBase*>( pWeapon );

	if ( pHL2Weapon )
	{
		return pHL2Weapon->GetOriginalSpawnOrigin();
	}
#endif
	
	return pWeapon->GetAbsOrigin();
}

#ifndef CLIENT_DLL

CItem* IsManagedObjectAnItem( CBaseEntity *pObject )
{
	return dynamic_cast< CItem*>( pObject );
}

CWeaponHL2MPBase* IsManagedObjectAWeapon( CBaseEntity *pObject )
{
	return dynamic_cast< CWeaponHL2MPBase*>( pObject );
}

bool GetObjectsOriginalParameters( CBaseEntity *pObject, Vector &vOriginalOrigin, QAngle &vOriginalAngles )
{
	if ( CItem *pItem = IsManagedObjectAnItem( pObject ) )
	{
		if ( pItem->m_flNextResetCheckTime > gpGlobals->curtime )
			 return false;
		
		vOriginalOrigin = pItem->GetOriginalSpawnOrigin();
		vOriginalAngles = pItem->GetOriginalSpawnAngles();

		pItem->m_flNextResetCheckTime = gpGlobals->curtime + sv_hl2mp_item_respawn_time.GetFloat();
		return true;
	}
	else if ( CWeaponHL2MPBase *pWeapon = IsManagedObjectAWeapon( pObject )) 
	{
		if ( pWeapon->m_flNextResetCheckTime > gpGlobals->curtime )
			 return false;

		vOriginalOrigin = pWeapon->GetOriginalSpawnOrigin();
		vOriginalAngles = pWeapon->GetOriginalSpawnAngles();

		pWeapon->m_flNextResetCheckTime = gpGlobals->curtime + sv_hl2mp_weapon_respawn_time.GetFloat();
		return true;
	}

	return false;
}

void CHL2MPRules::ManageObjectRelocation( void )
{
	int iTotal = m_hRespawnableItemsAndWeapons.Count();

	if ( iTotal > 0 )
	{
		for ( int i = 0; i < iTotal; i++ )
		{
			CBaseEntity *pObject = m_hRespawnableItemsAndWeapons[i].Get();
			
			if ( pObject )
			{
				Vector vSpawOrigin;
				QAngle vSpawnAngles;

				if ( GetObjectsOriginalParameters( pObject, vSpawOrigin, vSpawnAngles ) == true )
				{
					float flDistanceFromSpawn = (pObject->GetAbsOrigin() - vSpawOrigin ).Length();

					if ( flDistanceFromSpawn > WEAPON_MAX_DISTANCE_FROM_SPAWN )
					{
						bool shouldReset = false;
						IPhysicsObject *pPhysics = pObject->VPhysicsGetObject();

						if ( pPhysics )
						{
							shouldReset = pPhysics->IsAsleep();
						}
						else
						{
							shouldReset = (pObject->GetFlags() & FL_ONGROUND) ? true : false;
						}

						if ( shouldReset )
						{
							pObject->Teleport( &vSpawOrigin, &vSpawnAngles, NULL );
							pObject->EmitSound( "AlyxEmp.Charge" );

							IPhysicsObject *pPhys = pObject->VPhysicsGetObject();

							if ( pPhys )
							{
								pPhys->Wake();
							}
						}
					}
				}
			}
		}
	}
}

//=========================================================
//AddLevelDesignerPlacedWeapon
//=========================================================
void CHL2MPRules::AddLevelDesignerPlacedObject( CBaseEntity *pEntity )
{
	if ( m_hRespawnableItemsAndWeapons.Find( pEntity ) == -1 )
	{
		m_hRespawnableItemsAndWeapons.AddToTail( pEntity );
	}
}

//=========================================================
//RemoveLevelDesignerPlacedWeapon
//=========================================================
void CHL2MPRules::RemoveLevelDesignerPlacedObject( CBaseEntity *pEntity )
{
	if ( m_hRespawnableItemsAndWeapons.Find( pEntity ) != -1 )
	{
		m_hRespawnableItemsAndWeapons.FindAndRemove( pEntity );
	}
}

//=========================================================
// Where should this item respawn?
// Some game variations may choose to randomize spawn locations
//=========================================================
Vector CHL2MPRules::VecItemRespawnSpot( CItem *pItem )
{
	return pItem->GetOriginalSpawnOrigin();
}

//=========================================================
// What angles should this item use to respawn?
//=========================================================
QAngle CHL2MPRules::VecItemRespawnAngles( CItem *pItem )
{
	return pItem->GetOriginalSpawnAngles();
}

//=========================================================
// At what time in the future may this Item respawn?
//=========================================================
float CHL2MPRules::FlItemRespawnTime( CItem *pItem )
{
	return sv_hl2mp_item_respawn_time.GetFloat();
}


//=========================================================
// CanHaveWeapon - returns false if the player is not allowed
// to pick up this weapon
//=========================================================
bool CHL2MPRules::CanHavePlayerItem( CBasePlayer *pPlayer, CBaseCombatWeapon *pItem )
{
	if ( weaponstay.GetInt() > 0 )
	{
		if ( pPlayer->Weapon_OwnsThisType( pItem->GetClassname(), pItem->GetSubType() ) )
			 return false;
	}

	return BaseClass::CanHavePlayerItem( pPlayer, pItem );
}

#endif

//=========================================================
// WeaponShouldRespawn - any conditions inhibiting the
// respawning of this weapon?
//=========================================================
int CHL2MPRules::WeaponShouldRespawn( CBaseCombatWeapon *pWeapon )
{
#ifndef CLIENT_DLL
	if ( pWeapon->HasSpawnFlags( SF_NORESPAWN ) )
	{
		return GR_WEAPON_RESPAWN_NO;
	}
#endif

	return GR_WEAPON_RESPAWN_YES;
}

//-----------------------------------------------------------------------------
// Purpose: Player has just left the game
//-----------------------------------------------------------------------------
void CHL2MPRules::ClientDisconnected( edict_t *pClient )
{
#ifndef CLIENT_DLL
	// Msg( "CLIENT DISCONNECTED, REMOVING FROM TEAM.\n" );

	CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance( pClient );
	if ( pPlayer )
	{
		// Remove the player from his team
		if ( pPlayer->GetTeam() )
		{
			pPlayer->GetTeam()->RemovePlayer( pPlayer );
		}
	}

	//BG2 - Tjoppen - disconnecting. remove from flags
	CHL2MP_Player *pPlayer2 = dynamic_cast<CHL2MP_Player*>(pPlayer);
	if (pPlayer2) {
		pPlayer2->RemoveSelfFromFlags();

		//BG3 - remove self from vcomm manager
		CBotComManager* pComms = CBotComManager::GetBotCommsOfPlayer(pPlayer2);
		if (pComms->m_pContextPlayer == pPlayer2) {
			pComms->m_pContextPlayer = nullptr;
			pComms->ResetThinkTime(bot_randfloat(4.0f, 16.0f));
		}
	}
		
	//

	BaseClass::ClientDisconnected( pClient );

#endif
}


//=========================================================
// Deathnotice. 
//=========================================================
void CHL2MPRules::DeathNotice( CBasePlayer *pVictim, const CTakeDamageInfo &info )
{
#ifndef CLIENT_DLL
	// Work out what killed the player, and send a message to all clients about it
	const char *killer_weapon_name = "world";		// by default, the player is killed by the world
	int killer_ID = 0;

	// Find the killer & the scorer
	CBaseEntity *pInflictor = info.GetInflictor();
	CBaseEntity *pKiller = info.GetAttacker();
	CBasePlayer *pScorer = GetDeathScorer( pKiller, pInflictor );

	// Custom kill type?
	if ( info.GetDamageCustom() )
	{
		killer_weapon_name = GetDamageCustomString( info );
		if ( pScorer )
		{
			killer_ID = pScorer->GetUserID();
		}
	}
	else
	{
		// Is the killer a client?
		if ( pScorer )
		{
			killer_ID = pScorer->GetUserID();
			
			if ( pInflictor )
			{
				if ( pInflictor == pScorer )
				{
					// If the inflictor is the killer,  then it must be their current weapon doing the damage
					if ( pScorer->GetActiveWeapon() )
					{
#ifdef HL1MP_DLL
						killer_weapon_name = pScorer->GetActiveWeapon()->GetClassname();
#else
						killer_weapon_name = pScorer->GetActiveWeapon()->GetDeathNoticeName();
#endif
					}
				}
				else
				{
					killer_weapon_name = pInflictor->GetClassname();  // it's just that easy
				}
			}
		}
		else
		{
			killer_weapon_name = pInflictor->GetClassname();
		}

		// strip the NPC_* or weapon_* from the inflictor's classname
		if ( strncmp( killer_weapon_name, "weapon_", 7 ) == 0 )
		{
			killer_weapon_name += 7;
		}
		else if ( strncmp( killer_weapon_name, "npc_", 4 ) == 0 )
		{
			killer_weapon_name += 4;
		}
		else if ( strncmp( killer_weapon_name, "func_", 5 ) == 0 )
		{
			killer_weapon_name += 5;
		}
		else if ( strstr( killer_weapon_name, "physics" ) )
		{
			killer_weapon_name = "physics";
		}

		/*if ( strcmp( killer_weapon_name, "prop_combine_ball" ) == 0 )
		{
			killer_weapon_name = "combine_ball";
		}
		else if ( strcmp( killer_weapon_name, "grenade_ar2" ) == 0 )
		{
			killer_weapon_name = "smg1_grenade";
		}
		else if ( strcmp( killer_weapon_name, "satchel" ) == 0 || strcmp( killer_weapon_name, "tripmine" ) == 0)
		{
			killer_weapon_name = "slam";
		}*/


	}

	IGameEvent *event = gameeventmanager->CreateEvent( "player_death" );
	if( event )
	{
		event->SetInt("userid", pVictim->GetUserID() );
		event->SetInt("attacker", killer_ID );
		event->SetString("weapon", killer_weapon_name );
		event->SetInt( "priority", 7 );
		gameeventmanager->FireEvent( event );
	}
#endif

}

void CHL2MPRules::ClientSettingsChanged( CBasePlayer *pPlayer )
{
#ifndef CLIENT_DLL
	
	CHL2MP_Player *pHL2Player = ToHL2MPPlayer( pPlayer );

	if ( pHL2Player == NULL )
		return;

	//BG2 - The only thing cl_playermodel does at this point is change your team when you change the value. What's the point? 
	//Just use the class selection menu like everyone else so we can skim more shit off. -HairyPotter
	/*const char *pCurrentModel = modelinfo->GetModelName( pPlayer->GetModel() );
	const char *szModelName = engine->GetClientConVarValue( engine->IndexOfEdict( pPlayer->edict() ), "cl_playermodel" );

	//If we're different.
	if ( stricmp( szModelName, pCurrentModel ) )
	{
		//Too soon, set the cvar back to what it was.
		//Note: this will make this function be called again
		//but since our models will match it'll just skip this whole dealio.
		if ( pHL2Player->GetNextModelChangeTime() >= gpGlobals->curtime )
		{
			char szReturnString[512];

			Q_snprintf( szReturnString, sizeof (szReturnString ), "cl_playermodel %s\n", pCurrentModel );
			engine->ClientCommand ( pHL2Player->edict(), szReturnString );

			Q_snprintf( szReturnString, sizeof( szReturnString ), "Please wait %d more seconds before trying to switch.\n", (int)(pHL2Player->GetNextModelChangeTime() - gpGlobals->curtime) );
			ClientPrint( pHL2Player, HUD_PRINTTALK, szReturnString );
			return;
		}

		if ( HL2MPRules()->IsTeamplay() == false )
		{
			pHL2Player->SetPlayerModel();

			const char *pszCurrentModelName = modelinfo->GetModelName( pHL2Player->GetModel() );

			char szReturnString[128];
			Q_snprintf( szReturnString, sizeof( szReturnString ), "Your player model is: %s\n", pszCurrentModelName );

			ClientPrint( pHL2Player, HUD_PRINTTALK, szReturnString );
		}
		else
		{
			if ( Q_stristr( szModelName, "models/human") )
			{
				pHL2Player->ChangeTeam( TEAM_REBELS );
			}
			else
			{
				pHL2Player->ChangeTeam( TEAM_COMBINE );
			}
		}
	}*/
	if ( sv_report_client_settings.GetInt() == 1 )
	{
		UTIL_LogPrintf( "\"%s\" cl_cmdrate = \"%s\"\n", pHL2Player->GetPlayerName(), engine->GetClientConVarValue( pHL2Player->entindex(), "cl_cmdrate" ));
	}

	BaseClass::ClientSettingsChanged( pPlayer );
#endif
	
}

int CHL2MPRules::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
{
/*#ifndef CLIENT_DLL
	// half life multiplay has a simple concept of Player Relationships.
	// you are either on another player's team, or you are not.
	if ( !pPlayer || !pTarget || !pTarget->IsPlayer() || IsTeamplay() == false )
		return GR_NOTTEAMMATE;

	if ( (*GetTeamID(pPlayer) != '\0') && (*GetTeamID(pTarget) != '\0') && !stricmp( GetTeamID(pPlayer), GetTeamID(pTarget) ) )
	{
		return GR_TEAMMATE;
	}
#endif*/

	return GR_NOTTEAMMATE;
}

const char *CHL2MPRules::GetGameDescription( void )
{ 
	//BG2 - Tjoppen - our game descriptions - putting the current version number in these might be a good idea
	return "Battle Grounds 3";
	// 
} 

bool CHL2MPRules::IsConnectedUserInfoChangeAllowed( CBasePlayer *pPlayer )
{
	return true;
}
 
float CHL2MPRules::GetMapRemainingTime()
{
	// if timelimit is disabled, return 0
	if ( mp_timelimit.GetInt() <= 0 )
		return 0;

	// timelimit is in minutes

	float timeleft = (m_flGameStartTime + mp_timelimit.GetInt() * 60.0f ) - gpGlobals->curtime;

	return timeleft;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MPRules::Precache( void )
{
	//CBaseEntity::PrecacheScriptSound( "AlyxEmp.Charge" );
}

bool CHL2MPRules::ShouldCollide( int collisionGroup0, int collisionGroup1 )
{
	if ( collisionGroup0 > collisionGroup1 )
	{
		// swap so that lowest is always first
		V_swap(collisionGroup0,collisionGroup1);
	}

	if ( (collisionGroup0 == COLLISION_GROUP_PLAYER || collisionGroup0 == COLLISION_GROUP_PLAYER_MOVEMENT) &&
		collisionGroup1 == COLLISION_GROUP_WEAPON )
	{
		return false;
	}

	return BaseClass::ShouldCollide( collisionGroup0, collisionGroup1 ); 

}

bool CHL2MPRules::ClientCommand( CBaseEntity *pEdict, const CCommand &args )
{
#ifndef CLIENT_DLL
	if( BaseClass::ClientCommand( pEdict, args ) )
		return true;


	CHL2MP_Player *pPlayer = (CHL2MP_Player *) pEdict;

	if ( pPlayer->ClientCommand( args ) )
		return true;
#endif

	return false;
}

// shared ammo definition
// JAY: Trying to make a more physical bullet response
#define BULLET_MASS_GRAINS_TO_LB(grains)	(0.002285*(grains)/16.0f)
#define BULLET_MASS_GRAINS_TO_KG(grains)	lbs2kg(BULLET_MASS_GRAINS_TO_LB(grains))

// exaggerate all of the forces, but use real numbers to keep them consistent
//BG2 - Tjoppen - we want more.. realistic forces
//#define BULLET_IMPULSE_EXAGGERATION			3.5
#define BULLET_IMPULSE_EXAGGERATION			0//.8
// convert a velocity in ft/sec and a mass in grains to an impulse in kg in/s
#define BULLET_IMPULSE(grains, ftpersec)	((ftpersec)*12*BULLET_MASS_GRAINS_TO_KG(grains)*BULLET_IMPULSE_EXAGGERATION)


CAmmoDef *GetAmmoDef()
{
	static CAmmoDef def;
	static bool bInitted = false;
	
	if ( !bInitted )
	{
		bInitted = true;

		//BG2 - Tjoppen - more ammo..
		def.AddAmmoType("357", DMG_BULLET, TRACER_LINE_AND_WHIZ, 0, 0, 36, BULLET_IMPULSE(800, 5000), 0);
		def.AddAmmoType("Grenade", DMG_BURN, TRACER_NONE, "sk_plr_dmg_grenade", "sk_npc_dmg_grenade", "sk_max_grenade", 0, 0); //BG3 - Awesome - grenades!
	}

	return &def;
}

#ifdef CLIENT_DLL

	/*ConVar cl_autowepswitch(
		"cl_autowepswitch",
		"1",
		FCVAR_ARCHIVE | FCVAR_USERINFO,
		"Automatically switch to picked up weapons (if more powerful)" );*/

#else

	bool CHL2MPRules::FShouldSwitchWeapon( CBasePlayer *pPlayer, CBaseCombatWeapon *pWeapon )
	{		
		//BG2 - Why so serious? -HairyPotter
		/*if ( pPlayer->GetActiveWeapon() && pPlayer->IsNetClient() )
		{
			// Player has an active item, so let's check cl_autowepswitch.
			const char *cl_autowepswitch = engine->GetClientConVarValue( engine->IndexOfEdict( pPlayer->edict() ), "cl_autowepswitch" );
			if ( cl_autowepswitch && atoi( cl_autowepswitch ) <= 0 )
			{
				return false;
			}
		}*/

		return BaseClass::FShouldSwitchWeapon( pPlayer, pWeapon );
	}

#endif

#ifndef CLIENT_DLL

void CHL2MPRules::RestartGame()
{
	// bounds check
	if ( mp_timelimit.GetInt() < 0 )
	{
		mp_timelimit.SetValue( 0 );
	}
	m_flGameStartTime = gpGlobals->curtime;
	if ( !IsFinite( m_flGameStartTime.Get() ) )
	{
		Warning( "Trying to set a NaN game start time\n" );
		m_flGameStartTime.GetForModify() = 0.0f;
	}

	//CleanUpMap();
	
	// now respawn all players
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CHL2MP_Player *pPlayer = (CHL2MP_Player*) UTIL_PlayerByIndex( i );

		if ( !pPlayer )
			continue;

		if ( pPlayer->GetActiveWeapon() )
		{
			pPlayer->GetActiveWeapon()->Holster();
		}
		pPlayer->RemoveAllItems( true );
		respawn( pPlayer, false );
		pPlayer->Reset();
	}

	// Respawn entities (glass, doors, etc..)

	CTeam *pBritish = GetGlobalTeam(TEAM_BRITISH);
	CTeam *pAmericans = GetGlobalTeam(TEAM_AMERICANS);

	if (pAmericans)
		pAmericans->SetScore(0);

	if (pBritish)
		pBritish->SetScore(0);

	m_flIntermissionEndTime = 0;
	m_flRestartGameTime = 0.0;		
	m_bCompleteReset = false;

	IGameEvent * event = gameeventmanager->CreateEvent( "round_start" );
	if ( event )
	{
		event->SetInt("fraglimit", 0 );
		event->SetInt( "priority", 6 ); // HLTV event priority, not transmitted

		event->SetString("objective","DEATHMATCH");

		gameeventmanager->FireEvent( event );
	}
}

void CHL2MPRules::CleanUpMap()
{
	/* //BG2 - May have to come back to this - HairyPotter
	// Recreate all the map entities from the map data (preserving their indices),
	// then remove everything else except the players.

	// Get rid of all entities except players.
	CBaseEntity *pCur = gEntList.FirstEnt();
	while ( pCur )
	{
		CBaseHL2MPCombatWeapon *pWeapon = dynamic_cast< CBaseHL2MPCombatWeapon* >( pCur );
		// Weapons with owners don't want to be removed..
		if ( pWeapon )
		{
			if ( !pWeapon->GetPlayerOwner() )
			{
				UTIL_Remove( pCur );
			}
		}
		// remove entities that has to be restored on roundrestart (breakables etc)
		else if ( !FindInList( s_PreserveEnts, pCur->GetClassname() ) )
		{
			UTIL_Remove( pCur );
		}

		pCur = gEntList.NextEnt( pCur );
	}

	// Really remove the entities so we can have access to their slots below.
	gEntList.CleanupDeleteList();

	// Cancel all queued events, in case a func_bomb_target fired some delayed outputs that
	// could kill respawning CTs
	g_EventQueue.Clear();

	// Now reload the map entities.
	class CHL2MPMapEntityFilter : public IMapEntityFilter
	{
	public:
		virtual bool ShouldCreateEntity( const char *pClassname )
		{
			// Don't recreate the preserved entities.
			if ( !FindInList( s_PreserveEnts, pClassname ) )
			{
				return true;
			}
			else
			{
				// Increment our iterator since it's not going to call CreateNextEntity for this ent.
				if ( m_iIterator != g_MapEntityRefs.InvalidIndex() )
					m_iIterator = g_MapEntityRefs.Next( m_iIterator );

				return false;
			}
		}


		virtual CBaseEntity* CreateNextEntity( const char *pClassname )
		{
			if ( m_iIterator == g_MapEntityRefs.InvalidIndex() )
			{
				// This shouldn't be possible. When we loaded the map, it should have used 
				// CCSMapLoadEntityFilter, which should have built the g_MapEntityRefs list
				// with the same list of entities we're referring to here.
				Assert( false );
				return NULL;
			}
			else
			{
				CMapEntityRef &ref = g_MapEntityRefs[m_iIterator];
				m_iIterator = g_MapEntityRefs.Next( m_iIterator );	// Seek to the next entity.

				if ( ref.m_iEdict == -1 || engine->PEntityOfEntIndex( ref.m_iEdict ) )
				{
					// Doh! The entity was delete and its slot was reused.
					// Just use any old edict slot. This case sucks because we lose the baseline.
					return CreateEntityByName( pClassname );
				}
				else
				{
					// Cool, the slot where this entity was is free again (most likely, the entity was 
					// freed above). Now create an entity with this specific index.
					return CreateEntityByName( pClassname, ref.m_iEdict );
				}
			}
		}

	public:
		int m_iIterator; // Iterator into g_MapEntityRefs.
	};
	CHL2MPMapEntityFilter filter;
	filter.m_iIterator = g_MapEntityRefs.Head();

	// DO NOT CALL SPAWN ON info_node ENTITIES!
	
	MapEntity_ParseAllEntities( engine->GetMapEntitiesString(), &filter, true );*/
}

void CHL2MPRules::CheckChatForReadySignal( CHL2MP_Player *pPlayer, const char *chatmsg )
{
	if( m_bAwaitingReadyRestart && FStrEq( chatmsg, mp_ready_signal.GetString() ) )
	{
		if( !pPlayer->IsReady() )
		{
			pPlayer->SetReady( true );
		}		
	}
}

void CHL2MPRules::CheckRestartGame( void )
{
	// Restart the game if specified by the server
	int iRestartDelay = mp_restartgame.GetInt();

	if ( iRestartDelay > 0 )
	{
		if ( iRestartDelay > 60 )
			iRestartDelay = 60;


		// let the players know
		char strRestartDelay[64];
		Q_snprintf( strRestartDelay, sizeof( strRestartDelay ), "%d", iRestartDelay );
		UTIL_ClientPrintAll( HUD_PRINTCENTER, "Game will restart in %s1 %s2", strRestartDelay, iRestartDelay == 1 ? "SECOND" : "SECONDS" );
		UTIL_ClientPrintAll( HUD_PRINTCONSOLE, "Game will restart in %s1 %s2", strRestartDelay, iRestartDelay == 1 ? "SECOND" : "SECONDS" );

		m_flRestartGameTime = gpGlobals->curtime + iRestartDelay;
		m_bCompleteReset = true;
		mp_restartgame.SetValue( 0 );
	}

	if( mp_readyrestart.GetBool() )
	{
		m_bAwaitingReadyRestart = true;
		m_bHeardAllPlayersReady = false;
		

		const char *pszReadyString = mp_ready_signal.GetString();


		// Don't let them put anything malicious in there
		if( pszReadyString == NULL || Q_strlen(pszReadyString) > 16 )
		{
			pszReadyString = "ready";
		}

		IGameEvent *event = gameeventmanager->CreateEvent( "hl2mp_ready_restart" );
		if ( event )
			gameeventmanager->FireEvent( event );

		mp_readyrestart.SetValue( 0 );

		// cancel any restart round in progress
		m_flRestartGameTime = -1;
	}
}

void CHL2MPRules::CheckAllPlayersReady( void )
{
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CHL2MP_Player *pPlayer = (CHL2MP_Player*) UTIL_PlayerByIndex( i );

		if ( !pPlayer )
			continue;
		if ( !pPlayer->IsReady() )
			return;
	}
	m_bHeardAllPlayersReady = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CHL2MPRules::GetChatFormat( bool bTeamOnly, CBasePlayer *pPlayer )
{
	//BG2 - Let's remove this huge "Fuck You" that Valve put into their own code. -HairyPotter
	return NULL;
	//

	if ( !pPlayer )  // dedicated server output
	{
		return NULL;
	}

	const char *pszFormat = NULL;

	// team only
	if ( bTeamOnly == TRUE )
	{
		if ( pPlayer->GetTeamNumber() == TEAM_SPECTATOR )
		{
			pszFormat = "HL2MP_Chat_Spec";
		}
		else
		{
			const char *chatLocation = GetChatLocation( bTeamOnly, pPlayer );
			if ( chatLocation && *chatLocation )
			{
				pszFormat = "HL2MP_Chat_Team_Loc";
			}
			else
			{
				pszFormat = "HL2MP_Chat_Team";
			}
		}
	}
	// everyone
	else
	{
		if ( pPlayer->GetTeamNumber() != TEAM_SPECTATOR )
		{
			pszFormat = "HL2MP_Chat_All";	
		}
		else
		{
			pszFormat = "HL2MP_Chat_AllSpec";
		}
	}

	return pszFormat;
}

#endif

#ifndef CLIENT_DLL

void CHL2MPRules::RestartRound(bool swapTeams)
{
	Msg("Beginning Round Restart...");
	//restart current round. immediately.
	ResetMap();
	ResetFlags();
	CSDKBot::ResetAllBots();

	if (swapTeams)
		SwapTeams();

	RespawnAll();

	//reset next officer rallying times
	BG3Buffs::SetNextRallyTime(TEAM_BRITISH, gpGlobals->curtime);
	BG3Buffs::SetNextRallyTime(TEAM_AMERICANS, gpGlobals->curtime);

	//BG2 - Tjoppen - tickets
	if (UsingTickets())
	{
		g_Teams[TEAM_AMERICANS]->ResetTickets();
		g_Teams[TEAM_BRITISH]->ResetTickets();

		//perform drain check at the appropriate time
		m_fNextFlagUpdate = gpGlobals->curtime + 10;
	}

	if (mp_respawntime.GetInt() > 0)
		m_fLastRoundRestart = m_fLastRespawnWave = gpGlobals->curtime;
	Msg("Finished!\n");
}

void CHL2MPRules::RespawnAll()
{
	if (g_Teams.Size() < NUM_TEAMS)	//in case teams haven't been inited or something
		return;

	int x;
	for (x = 0; x < g_Teams[TEAM_AMERICANS]->GetNumPlayers(); x++)
	{
		CHL2MP_Player *pPlayer = ToHL2MPPlayer(g_Teams[TEAM_AMERICANS]->GetPlayer(x));

		if (!pPlayer)
			continue;

		pPlayer->Spawn();

		//BG2 - Tjoppen - remove ragdoll - remember to change this to remove multiple ones if we decide to enable more corpses
		if (pPlayer->m_hRagdoll)
		{
			UTIL_RemoveImmediate(pPlayer->m_hRagdoll);
			pPlayer->m_hRagdoll = NULL;
		}
	}

	for (x = 0; x < g_Teams[TEAM_BRITISH]->GetNumPlayers(); x++)
	{
		CHL2MP_Player *pPlayer = ToHL2MPPlayer(g_Teams[TEAM_BRITISH]->GetPlayer(x));

		if (!pPlayer)
			continue;

		pPlayer->Spawn();

		//BG2 - Tjoppen - remove ragdoll - remember to change this to remove multiple ones if we decide to enable more corpses
		if (pPlayer->m_hRagdoll)
		{
			UTIL_RemoveImmediate(pPlayer->m_hRagdoll);
			pPlayer->m_hRagdoll = NULL;
		}
	}
}

void CHL2MPRules::WinSong(int team, bool m_bWonMap)
{
	if (g_Teams.Size() < NUM_TEAMS)	//in case teams haven't been inited or something
		return;

	const char *w_sTeam = team == TEAM_AMERICANS ? "Americans" : "British";

	//This is a good place to do the round_win log stuff, since this function is pretty much called every time a team wins a map/round. -HairyPotter
	if (!m_bWonMap) //So this is just an ordinary round.
	{
		UTIL_LogPrintf("Team \"%s\" triggered \"round_win\"\n", w_sTeam);
	}
	else //Or it's the end of a map.
	{
		UTIL_LogPrintf("Team \"%s\" triggered \"map_win\"\n", w_sTeam);
	}
	//

	if (m_fNextWinSong > gpGlobals->curtime)
		return;

	m_fNextWinSong = gpGlobals->curtime + 20;

	CRecipientFilter recpfilter;
	recpfilter.AddAllPlayers();
	recpfilter.MakeReliable();

	UserMessageBegin(recpfilter, "WinMusic");
	WRITE_BYTE(team);
	MessageEnd();

	/*int x;

	for( x = 0; x < g_Teams[TEAM_AMERICANS]->GetNumPlayers(); x++ )
	{
	CHL2MP_Player *pPlayer = ToHL2MPPlayer( g_Teams[TEAM_AMERICANS]->GetPlayer( x ) );

	if( !pPlayer )
	continue;

	if( pSound )
	{
	CPASAttenuationFilter filter( pPlayer, 10.0f );	//high attenuation so only this player hears it
	filter.UsePredictionRules();
	pPlayer->EmitSound( filter, pPlayer->entindex(), pSound );
	}
	}

	for( x = 0; x < g_Teams[TEAM_BRITISH]->GetNumPlayers(); x++ )
	{
	CHL2MP_Player *pPlayer = ToHL2MPPlayer( g_Teams[TEAM_BRITISH]->GetPlayer( x ) );

	if( !pPlayer )
	continue;

	if( pSound )
	{
	CPASAttenuationFilter filter( pPlayer, 10.0f );	//high attenuation so only this player hears it
	filter.UsePredictionRules();
	pPlayer->EmitSound( filter, pPlayer->entindex(), pSound );
	}
	}*/
}

void CHL2MPRules::RespawnWave()
{
	if (g_Teams.Size() < NUM_TEAMS)	//in case teams haven't been inited or something
		return;

	for (int x = 0; x < g_Teams[TEAM_AMERICANS]->GetNumPlayers(); x++)
	{
		CHL2MP_Player *pPlayer = ToHL2MPPlayer(g_Teams[TEAM_AMERICANS]->GetPlayer(x));

		//BG2 - Tjoppen - tickets
		if (UsingTickets() && g_Teams[TEAM_AMERICANS]->GetTicketsLeft() <= 0)
			break;

		if (!pPlayer)
			continue;

		if (pPlayer->IsAlive())
			continue;

		pPlayer->Spawn();
	}

	for (int x = 0; x < g_Teams[TEAM_BRITISH]->GetNumPlayers(); x++)
	{
		CHL2MP_Player *pPlayer = ToHL2MPPlayer(g_Teams[TEAM_BRITISH]->GetPlayer(x));

		//BG2 - Tjoppen - tickets
		if (UsingTickets() && g_Teams[TEAM_BRITISH]->GetTicketsLeft() <= 0)
			break;

		if (!pPlayer)
			continue;

		if (pPlayer->IsAlive())
			continue;

		pPlayer->Spawn();
	}
}

/*void CFlagHandler::PlayCaptureSound( void )
{
/*CBasePlayer *pPlayer = NULL;
while( (pPlayer = (CBasePlayer*)gEntList.FindEntityByClassname( pPlayer, "player" )) != NULL )
pPlayer->EmitSound( "Flag.capture" );*//*
}*/

void CHL2MPRules::ResetFlags(void)
{
	CBaseEntity *pEntity = NULL;

	while ((pEntity = gEntList.FindEntityByClassname(pEntity, "flag")) != NULL)
	{
		CFlag *pFlag = static_cast<CFlag*>(pEntity);
		if (!pFlag)
			continue;

		pFlag->ResetFlag(); //It's just that easy.
	}

	//BG2 - Reset CTF Flags as well. -HairyPotter
	while ((pEntity = gEntList.FindEntityByClassname(pEntity, "ctf_flag")) != NULL)
	{
		CtfFlag *pFlag = static_cast<CtfFlag*>(pEntity);
		if (!pFlag)
			continue;

		pFlag->ResetFlag(); //It's just that easy.
	}
	//
}

void CHL2MPRules::CountHeldFlags(int &american_flags, int &british_flags, int &neutral_flags, int &foramericans, int &forbritish)
{
	CBaseEntity *pEntity = NULL;

	american_flags = 0;
	british_flags = 0;
	neutral_flags = 0;
	foramericans = 0;
	forbritish = 0;

	while ((pEntity = gEntList.FindEntityByClassname(pEntity, "flag")) != NULL)
	{
		CFlag *pFlag = dynamic_cast<CFlag*>(pEntity);

		if (!pFlag || !pFlag->IsActive())
			continue;

		int FullCap = pFlag->m_iFullCap;

		//if ( FullCap < 0 || FullCap == NULL ) //This is needed for backwards compat reasons. -HairyPotter
		//	FullCap = 0;
		//Msg("Full cap for %s = %i \n", pFlag->m_sFlagName, FullCap );

		if (FullCap == 3 && !UsingTickets())
		{
			//we're not doing full caps on this flag.
			//0 = normal, 1 = Americans Fullcap, 2 = Brits Fullcap, 3 = No Fullcap.
			continue;
		}

		switch (pFlag->GetTeamNumber())
		{
		case TEAM_AMERICANS:
			american_flags++;
			break;
		case TEAM_BRITISH:
			british_flags++;
			break;
		default:
			neutral_flags++;
			break;
		}

		if (UsingTickets())
		{
			//ignore FullCap in tickets mode
			switch (pFlag->m_iForTeam)
			{
			case 0:
			default:
				foramericans++;
				forbritish++;
				break;
			case 1:
				foramericans++;
				break;
			case 2:
				forbritish++;
				break;
			}
		}
		else
			//So the flag is set to be capped by this team.
			switch (pFlag->m_iForTeam)
		{
			case 0:
			default://assume both
				switch (FullCap)
				{
				case 0:
					foramericans++;
					forbritish++;
					break;
				case 1:
					foramericans++;
					break;
				case 2:
					forbritish++;
					break;

				}
				break;
			case 1:
				if (FullCap == 1 || FullCap == 0)
					foramericans++;
				break;
			case 2:
				if (FullCap == 2 || FullCap == 0)
					forbritish++;
				break;
		}
	}

	/*Msg( "american_flags = %i & foramericans flags = %i\n", american_flags, foramericans );
	Msg( "british_flags = %i & forbritish flags = %i\n", british_flags, forbritish );
	Msg( "neutral_flags = %i\n", neutral_flags );*/
}

void CHL2MPRules::CheckFullcap(void)
{
	int american_flags, british_flags, neutral_flags, foramericans, forbritish;
	CountHeldFlags(american_flags, british_flags, neutral_flags, foramericans, forbritish);

	if (!american_flags && !british_flags && !neutral_flags)
		return;

	if (neutral_flags > 0)
	{
		if ((foramericans - american_flags) == 0 && foramericans != 0)
		{
			HandleScores(TEAM_AMERICANS, mp_winbonus.GetInt(), AMERICAN_ROUND_WIN, true);
			return;
		}
		if ((forbritish - british_flags) == 0 && forbritish != 0)
		{
			HandleScores(TEAM_BRITISH, mp_winbonus.GetInt(), BRITISH_ROUND_WIN, true);
			return;
		}
	}
	else
	{
		if (american_flags <= 0 && british_flags <= 0)
		{
			//draw
			HandleScores(TEAM_NONE, 0, ROUND_DRAW, true);
			//Msg( "draw\n" );
			return;
		}

		if (american_flags <= 0 && (forbritish - british_flags) == 0)
		{
			//british win
			//Msg( "british win\n" );
			HandleScores(TEAM_BRITISH, mp_winbonus.GetInt(), BRITISH_ROUND_WIN, true);
			return;
		}

		if (british_flags <= 0 && (foramericans - american_flags) == 0)
		{
			//americans win
			//Msg( "americans win\n" );
			HandleScores(TEAM_AMERICANS, mp_winbonus.GetInt(), AMERICAN_ROUND_WIN, true);
			return;
		}
	}
}

void CHL2MPRules::CheckTicketDrain(void)
{
	int american_flags, british_flags, neutral_flags, foramericans, forbritish;
	CountHeldFlags(american_flags, british_flags, neutral_flags, foramericans, forbritish);

	CTeam *pAmericans = g_Teams[TEAM_AMERICANS];
	CTeam *pBritish = g_Teams[TEAM_BRITISH];

	if (american_flags > foramericans / 2)
		pBritish->RemoveTickets(mp_tickets_drain_b.GetFloat());

	if (british_flags > forbritish / 2)
		pAmericans->RemoveTickets(mp_tickets_drain_a.GetFloat());
}
#endif

int CHL2MPRules::GetLimitTeamClass(int iTeam, int iClass)
{
	//just get it from the class itself!
	const CPlayerClass* pClass = CPlayerClass::fromNums(iTeam, iClass);
	return CPlayerClass::getClassLimit(pClass);

	//mp_limit_<inf/off/rif>_<a/b>_<sml/med/lrg> - mp_limit_inf_a_sml

	//count players - is there a better way? this looks stupid
	/*int num = 0;
	for (int x = 1; x <= gpGlobals->maxClients; x++)
		if (UTIL_PlayerByIndex(x))
			num++;

	//BG2 - Tjoppen - more macro goodness
#define LIMIT_SWITCH( size )\
	switch( iTeam ){\
	case TEAM_AMERICANS:\
		switch( iClass ){\
		case CLASS_INFANTRY: return mp_limit_inf_a_##size.GetInt();\
		case CLASS_OFFICER: return mp_limit_off_a_##size.GetInt();\
		case CLASS_SNIPER: return mp_limit_rif_a_##size.GetInt();\
		case CLASS_SKIRMISHER: return mp_limit_ski_a_##size.GetInt();\
		case CLASS_LIGHT_INFANTRY: return mp_limit_linf_a_##size.GetInt();\
		case CLASS_GRENADIER: return mp_limit_gre_a_##size.GetInt();\
		default: return -1;}\
	case TEAM_BRITISH:\
		switch( iClass ){\
		case CLASS_INFANTRY: return mp_limit_inf_b_##size.GetInt();\
		case CLASS_OFFICER: return mp_limit_off_b_##size.GetInt();\
		case CLASS_SNIPER: return mp_limit_rif_b_##size.GetInt();\
		case CLASS_SKIRMISHER: return mp_limit_ski_b_##size.GetInt();\
		case CLASS_LIGHT_INFANTRY: return mp_limit_linf_b_##size.GetInt();\
		case CLASS_GRENADIER: return mp_limit_gre_b_##size.GetInt();\
		default: return -1;}\
	default: return -1;}

	if (num <= mp_limit_mapsize_low.GetInt())
	{
		//small
		LIMIT_SWITCH(sml)
	}
	else if (num <= mp_limit_mapsize_high.GetInt())
	{
		//medium
		LIMIT_SWITCH(med)
	}
	else
	{
		//large
		LIMIT_SWITCH(lrg)
	}*/
}

bool CHL2MPRules::UsingTickets()
{
	return mp_respawnstyle.GetInt() == 3 /*|| mp_respawnstyle.GetInt() == 4*/;
}