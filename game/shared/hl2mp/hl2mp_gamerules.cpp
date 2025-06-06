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

#include "../../shared/bg3/bg3_class.h"                                 

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
	// memdbgon must be the last include file in a .cpp file!!!
	#include "tier0/memdbgon.h"
#else

	#include "eventqueue.h"
	#include "player.h"
	#include "gamerules.h"
	#include "game.h"
	#include "items.h"
	#include "entitylist.h"
	#include "mapentities.h"
	#include "player_resource.h"
	#include "in_buttons.h"
	#include <ctype.h>
	#include "voice_gamemgr.h"
	#include "iscorer.h"
	#include "hl2mp_player.h"
	#include "bg3\bg3_line_spawn.h"
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
#include "../bg3/Bots/bg3_bot.h"
#include "../bg3/Bots/bg3_bot_manager.h"
#include "../bg3/Bots/bg3_bot_vcomms.h"
#include "../bg3/bg3_scorepreserve.h"
#include "../bg3/controls/bg3_rtv.h"
#include "../bg3/controls/bg3_voting_system.h"
#include "../shared/bg3/bg3_class_quota.h"
#include "bg3_gungame.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
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
ConVar mp_timeleft("mp_timeleft", "0", FCVAR_GAMEDLL | FCVAR_REPLICATED, "");

//BG2 - Draco - End
//BG2 - Tjoppen - mp_winbonus
ConVar mp_winbonus("mp_winbonus", "200", FCVAR_GAMEDLL | FCVAR_NOTIFY, "Amount of points awarded to team winning the round");
ConVar mp_swapteams("mp_swapteams", "0", FCVAR_GAMEDLL | FCVAR_NOTIFY, "Whether to swap teams at end of round (-1 force no, 0 default, 1 force yes)");
//

ConVar sv_periodic_message("sv_periodic_message", "", FCVAR_GAMEDLL, "Message to show in chat periodically.");
ConVar sv_periodic_message_interval("sv_periodic_message_interval", "180", FCVAR_GAMEDLL, "Period between periodic chat messages");

//BG2 - Tjoppen - away with these
/*extern CBaseEntity	 *g_pLastCombineSpawn;
extern CBaseEntity	 *g_pLastRebelSpawn;*/
//

#define WEAPON_MAX_DISTANCE_FROM_SPAWN 64

#endif

ConVar mp_autobalanceteams("mp_autobalanceteams", "1", FCVAR_NOTIFY | FCVAR_REPLICATED);
ConVar mp_autobalancetolerance("mp_autobalancetolerance", "2", FCVAR_NOTIFY | FCVAR_REPLICATED, "When the teams differ in size by this number or greater, autobalance is active.", true, 2, false, 0);

ConVar sv_chatcommands("sv_chatcommands", "1", FCVAR_NOTIFY | FCVAR_REPLICATED, "Whether or not in-chat commands such as \"ff\" and \"currentmap\" are enabled.");


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
									"Max number of J�gers on " sizename " maps" );\
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

ConVar mp_respawnstyle("mp_respawnstyle", "1", CVAR_FLAGS, "0 = regular dm, 1 = waves (skirmish), 2 = last man standing, 3 = ticket mode, 4 = LMS with linebattle extensions", true, 1, true, 4);
ConVar mp_respawntime("mp_respawntime", "14", CVAR_FLAGS, "Time between waves, or the maximum length of the round with mp_respawnstyle 2 or 3");

//For punishing officers who waste the buff during skirmish
ConVar mp_punish_bad_officer("mp_punish_bad_officer", "1", CVAR_FLAGS, "Whether or not to auto-switch officers who use the buff ability with none of their teammates around off of the officer class. They can switch back after spending one life as a non-officer.");
ConVar mp_punish_bad_officer_nextclass("mp_bad_officer_nextclass", "1", CVAR_FLAGS, "What class to auto-switch bad officers to. 1-6 is inf, off, sniper, skirm, linf, grenadier.");

ConVar mp_competitive("mp_competitive", "0", CVAR_FLAGS, "Controls many options, disabling features for the sake of competitive consistency.");
ConVar mp_competitive_mapblock("mp_competitive_mapblock", "0", CVAR_FLAGS, "Blocks or removes certain features on maps to make them more fair for competitive play, ex. the rooftops on townguard.");

#ifdef CLIENT_DLL
//this is defined in gun game file on server side
namespace NGunGame {
	GLOBAL_BOOL(g_bGunGameActive, mp_gungame, 0, false, FCVAR_GAMEDLL | FCVAR_NOTIFY, 0, 1);
}
#endif

bool g_bFFA = false;
static void onFFA_Change(IConVar* pVar, const char*, float) {
#ifndef CLIENT_DLL
	ConVar* ffa = (ConVar*)pVar;
	bool bNewVal = ffa->GetBool();

	extern CUtlVector<CBaseEntity*> g_FFA_Spawns;
	//verify that there are spawns for us to use
	extern bool g_bServerReady;
	if (bNewVal && g_bServerReady && g_FFA_Spawns.Count() == 0) {
		MSay("Map has no FFA spawns to support FFA mode.");
	}
	else {
		friendlyfire.SetValue(bNewVal);
		g_bFFA = bNewVal;
		if (bNewVal) MSay("Free For All Mode!");
	}
#endif
}
ConVar mp_ffa("mp_ffa", "0", CVAR_FLAGS, "Toggles Free-For-All spawn mechanics and win conditions. Disables non-CTF flags.", &onFFA_Change);


//ticket system
ConVar mp_rounds("mp_rounds", "0", CVAR_FLAGS, "Maximum number of rounds - rounds are restarted until this. A value of 0 deactivates the round system");
ConVar mp_roundtime("mp_roundtime", "300", CVAR_FLAGS, "Maximum length of round");
ConVar mp_tickets_a("mp_tickets_a", "100", CVAR_FLAGS, "Tickets given to americans on round start");
ConVar mp_tickets_b("mp_tickets_b", "100", CVAR_FLAGS, "Tickets given to british on round start");
ConVar mp_tickets_timelimit("mp_tickets_timelimit", "0", CVAR_FLAGS, "Whether or not to set remaining round time to 1 minute after a team loses all its tickets");
ConVar mp_tickets_drain_a("mp_tickets_drain_a", "12.5", CVAR_FLAGS, "Number of tickets drained every interval when the americans have more than half of their cappable flags. Can have decimals");
ConVar mp_tickets_drain_b("mp_tickets_drain_b", "12.5", CVAR_FLAGS, "Number of tickets drained every interval when the british have more than half of their cappable flags. Can have decimals");
ConVar mp_tickets_drain_interval("mp_tickets_drain_interval", "10", CVAR_FLAGS, "How often tickets will be drained in mp_respawnstyle = 3");


void OnBg2SpeedChange(IConVar* pVar, const char* pszOldValue, float flOldValue) {

#ifndef CLIENT_DLL
	CPlayerClass::UpdateClassSpeeds();
	for (int i = 1; i < gpGlobals->maxClients; i++) {
		CBasePlayer* pPlayer = UTIL_PlayerByIndex(i);
		if (pPlayer) {
			engine->ClientCommand(pPlayer->edict(), "update_playerclass_speeds");
		}
	}
#endif
}

ConVar mp_bg2_speed("mp_bg2_speed", "0", FCVAR_REPLICATED | FCVAR_NOTIFY, "If set to 1, removes the 10% player speed increase that was added in BG3", OnBg2SpeedChange);
ConVar sv_vote_duration("sv_vote_duration", "30", CVAR_FLAGS, "This time, in seconds, is how long a vote will last.", true, 5.f, false, 0.f);


//interchangable flag system
#ifdef CLIENT_DLL
ConVar mp_flagmode("mp_flagmode", "0", CVAR_FLAGS, "Controls which set of flags is enabled for the map. -1 = all flags, 0 = default, 1 = CTF, 2 = custom. Not every mode will be available on every map.");
#else
void OnFlagModeChange(IConVar* pVar, const char* pszOldValue, float flOldValue) {
	sv_restartmap.SetValue(3);
}
ConVar mp_flagmode("mp_flagmode", "0", CVAR_FLAGS, "Controls which set of flags is enabled for the map. -1 = all flags, 0 = default, 1 = CTF, 2 = custom. Not every mode will be available on every map.", OnFlagModeChange);
#endif
ConVar mp_flagmode_randomize("mp_flagmode_randomize", "1", CVAR_FLAGS, "Whether or not to randomize flag mode on map start.");

//BG3 - Awesome - linebattle cvars
ConVar lb_enforce_weapon_amer("lb_enforce_weapon_amer", "0", CVAR_FLAGS_HIDDEN, "How to enforce continental soldier's weapon in linebattle mode. 0 is disabled, 1 is enforce first weapon, 2 is enforce second weapon");
ConVar lb_enforce_weapon_brit("lb_enforce_weapon_brit", "0", CVAR_FLAGS_HIDDEN, "How to enforce royal infantry's weapon in linebattle mode. 0 is disabled, 1 is enforce first weapon, 2 is enforce second weapon");
//ConVar lb_enforce_class_amer("lb_enforce_class_amer", "0", CVAR_FLAGS_HIDDEN, "How to force class change on a new linebattle round. Excludes officers. 0 is disabled, 1-6 is inf, off, sniper, skirm, linf, grenadier");
//ConVar lb_enforce_class_brit("lb_enforce_class_brit", "0", CVAR_FLAGS_HIDDEN, "How to force class change on a new linebattle round. Excludes officers. 0 is disabled, 1-6 is inf, off, sniper, skirm, linf, grenadier");
ConVar lb_enforce_no_buckshot("lb_enforce_no_buckshot", "0", CVAR_FLAGS, "Whether or not to allow buckshot during linebattle mode");

ConVar lb_officer_protect("lb_officer_protect", "2", CVAR_FLAGS, "Whether or not to protect officers during early-round long-range linebattle shooting. 0 is off, 1 is first officer only, 2 is both officers.");
ConVar lb_officer_autodetect("lb_officer_autodetect", "1", CVAR_FLAGS, "Whether or not to auto-detect officers who are the same class as their teammates, based on their position in the line. 0 is off, 1 is on.");

void OnOfficerClassOverrideChange(IConVar* cvar, const char* pszOldValue, float oldValue);
#ifdef CLIENT_DLL
void OnOfficerClassOverrideChange(IConVar* cvar, const char* pszOldValue, float oldValue) {}
#endif

ConVar lb_officer_classoverride_a("lb_officer_classoverride_a", "1", CVAR_FLAGS, "Override for which American class is used as officer during linebattle mode. Includes officer's rallying abilities. 0 is disabled, 1-6 is inf, off, sniper, skirm, grenadier", OnOfficerClassOverrideChange);
ConVar lb_officer_classoverride_b("lb_officer_classoverride_b", "1", CVAR_FLAGS, "Override for which British class is used as officer during linebattle mode. Includes officer's rallying abilities. 0 is disabled, 1-6 is inf, off, sniper, skirm, linf, grenadier", OnOfficerClassOverrideChange);

ConVar lb_enforce_volley_fire("lb_enforce_volley_fire", "1", CVAR_FLAGS, "Whether or not to enforce volley fire on non-officers. 0 is off, 1 is on. When on, players can only fire if their officer has ironsighted or for a short time after the officer fires");
ConVar lb_enforce_volley_fire_tolerance("lb_enforce_volley_fire_tolerance", "2", CVAR_FLAGS, "If volley fire is enforced, players are given this amount of time to fire after their officer shoots.");
ConVar lb_enforce_no_troll("lb_enforce_no_troll", "1", CVAR_FLAGS, "If on, prevents rambos from shooting or stabbing, and prevents trolls from stabbing teammates in non-melee situations. 0 is off, 1 is on.");

ConVar sv_alltalk("sv_alltalk", "0", FCVAR_NOTIFY | FCVAR_REPLICATED, "Players can hear all other players, no team restrictions");
ConVar sv_password_exists("sv_password_exists", "0", FCVAR_HIDDEN | FCVAR_REPLICATED);

// BG2 - VisualMelon - Can't find a better place to put this
int hitVerificationHairs = 0;
int hitVerificationLatency = 1.5;
#define TEAM_NONE -1

ConVar sv_maxcmdrate_auto("sv_maxcmdrate_auto", "1", CVAR_FLAGS, "Whether or not to automatically adjust sv_maxcmdrate based on player count.");

#ifndef CLIENT_DLL
void SetMaxCmdRateAuto() {
	if (sv_maxcmdrate_auto.GetBool()) {
		int numPlayers = g_pPlayerResource->GetNumAmericans() + g_pPlayerResource->GetNumBritish();
		int idealRate = 100;
		if (numPlayers > 46) {
			idealRate = 30;
		}
		else if (numPlayers > 16) {
			idealRate = 66;
		}
		ConVarRef ref("sv_maxcmdrate");
		ref.SetValue(idealRate);
	}
}
#endif
//



REGISTER_GAMERULES_CLASS( CHL2MPRules );

BEGIN_NETWORK_TABLE_NOBASE( CHL2MPRules, DT_HL2MPRules )

	#ifdef CLIENT_DLL
		RecvPropFloat( RECVINFO(m_flGameStartTime)),
		RecvPropBool( RECVINFO( m_bTeamPlayEnabled ) ),
		RecvPropFloat( RECVINFO( m_fLastRespawnWave ) ), //BG2 This needs to be here for the timer to work. -HairyPotter
		RecvPropFloat(RECVINFO(m_fLastRoundRestart)),
		RecvPropInt(RECVINFO(m_iCurrentRound)),
	#else
		SendPropFloat( SENDINFO(m_flGameStartTime)),
		SendPropBool( SENDINFO( m_bTeamPlayEnabled ) ),
		SendPropFloat(SENDINFO(m_fLastRespawnWave)), //BG2 This needs to be here for the timer to work. -HairyPotter
		SendPropFloat(SENDINFO(m_fLastRoundRestart)),
		SendPropInt(SENDINFO(m_iCurrentRound), 8, SPROP_UNSIGNED),
	#endif

END_NETWORK_TABLE()


LINK_ENTITY_TO_CLASS( hl2mp_gamerules, CHL2MPGameRulesProxy );
IMPLEMENT_NETWORKCLASS_ALIASED( HL2MPGameRulesProxy, DT_HL2MPGameRulesProxy )

static HL2MPViewVectors g_HL2MPViewVectors(
	Vector(0, 0, 60),       //VEC_VIEW (m_vView) //BG2 - Awesome[Was 64, but lowered it to match 2007 height. 
											//The 2007 height was 64 too but for some reason the 2013 one was still too high so I lowered it here]
							  
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
			extern ConVar sv_alltalk;
			extern CHL2MP_Player* g_pMicSoloPlayer;
			return (
				ToHL2MPPlayer(pTalker)->m_bMuted == ToHL2MPPlayer(pListener)->m_bMuted
				&& (sv_alltalk.GetBool() || pListener->GetTeamNumber() == pTalker->GetTeamNumber()) 
				&& (g_pMicSoloPlayer == NULL || pTalker == g_pMicSoloPlayer));
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

	//recalculate class movement speeds
	CPlayerClass::UpdateClassSpeeds();

#ifndef CLIENT_DLL
	// Create the team managers
	for ( int i = 0; i < ARRAYSIZE( sTeamNames ); i++ )
	{
		CTeam *pTeam = static_cast<CTeam*>(CreateEntityByName( "team_manager" ));
		pTeam->Init( sTeamNames[i], i );

		g_Teams.AddToTail( pTeam );
	}

	//reset the bot manager's think time
	CBotManager::SetNextBotManagerThink(gpGlobals->curtime + bot_minplayers_delay.GetFloat());

	m_bTeamPlayEnabled = teamplay.GetBool();
	m_flIntermissionEndTime = 0.0f;
	m_flGameStartTime = 0;

	m_hRespawnableItemsAndWeapons.RemoveAll();
	m_tmNextPeriodicThink = 0;
	m_flNextPeriodicMessage = 0;
	m_tmDelayedMapChangeTime = FLT_MAX;
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
	if (pVictim->GetActiveWeapon()) pVictim->GetActiveWeapon()->StopWeaponSound(RELOAD);
#ifndef CLIENT_DLL
	if ( IsIntermission() )
		return;

	//if in gun game, increment player's gun game kit
	if (NGunGame::g_bGunGameActive && info.GetAttacker() && info.GetAttacker()->IsPlayer() && info.GetAttacker() != (CBaseEntity*)pVictim) {
		CHL2MP_Player* pAttacker = (CHL2MP_Player*)info.GetAttacker();
		const int numKits = NGunGame::GetNumGunKits();
		pAttacker->m_iGunGameKit++;
		pAttacker->SetFragCount(pAttacker->m_iGunGameKit + 1);

		if (numKits - pAttacker->m_iGunGameKit == 5) {
			MSay("%s is only 5 guns away from winning!", pAttacker->GetPlayerName());
		}
		else if (pAttacker->m_iGunGameKit - 1 == numKits) {
			MSay("%s needs only one more kill to win!", pAttacker->GetPlayerName());
		}
		else if (pAttacker->m_iGunGameKit >= numKits) {
			pAttacker->m_iGunGameKit = numKits - 1; //clamp to max
			//player got kill with last weapon, game is over!
			GoToIntermission();
			CSayPlayer(pAttacker, "You win!");
			return;
		}


		CSayPlayer(pAttacker, "Gun rank-up! (%i/%i)", pAttacker->m_iGunGameKit + 1, numKits);

		//decrement vicitim's gun game kit if weapon was a bottle
		if (info.GetWeapon() && Q_strcmp(info.GetWeapon()->GetClassname(), "weapon_bottle") == 0) {
			CHL2MP_Player* pHL2MPVictim = (CHL2MP_Player*)pVictim;
			pHL2MPVictim->m_iGunGameKit--;
			if (pHL2MPVictim->m_iGunGameKit < 0) {
				pHL2MPVictim->m_iGunGameKit = 0; //clamp to min 0
			}
			//decrement victim's score too
			pHL2MPVictim->SetFragCount(pHL2MPVictim->m_iGunGameKit + 1);
			if (pHL2MPVictim->FragCount() < 1) {
				pHL2MPVictim->SetFragCount(1);
			}
			CSayPlayer(pHL2MPVictim, "%s stole your gun rank! (%i/%i)", pAttacker->GetPlayerName(), pHL2MPVictim->m_iGunGameKit + 1, numKits);
			CSayPlayer(pAttacker, "You stole %s's gun rank!", pHL2MPVictim->GetPlayerName());
		}
	}

	BaseClass::PlayerKilled( pVictim, info );
#endif
}

#ifndef CLIENT_DLL
//BG2 - This should handle all the score settings after each round, and also fire any triggers and play win music. -HairyPotter
void CHL2MPRules::HandleScores(int iTeam, int iScore, int msg_type, bool bRestart, bool bCycleRound)
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

	//dole out experience, except at end of round-based match
	if (bRestart || !mp_rounds.GetBool()) {
		EExperienceEventType event = bRestart ? EExperienceEventType::TEAM_ROUND_WIN : EExperienceEventType::MATCH_WIN;
		for (int i = 0; i < gpGlobals->maxClients; i++) {
			CHL2MP_Player* pPlayer = ToHL2MPPlayer(UTIL_PlayerByIndex(i));
			if (pPlayer && pPlayer->GetTeamNumber() == iTeam) pPlayer->m_unlockableProfile.createExperienceEvent(pPlayer, event);
		}
	}

	if (iScore > 0 && iTeam != TEAM_NONE)
		g_Teams[iTeam]->AddScore(iScore);

	if (bRestart)
	{
		int maxRounds = mp_rounds.GetInt();
		if (maxRounds && bCycleRound) {
			if (m_iCurrentRound >= maxRounds) {
				GoToIntermission();
				return;
			}
			m_iCurrentRound++;
		}

		// BG3 - Awesome - swap teams if the situation calls for it
		int iSwapTeam = mp_swapteams.GetInt();
		bool bSwapTeam = false;
		if (bCycleRound
			&& (
				iSwapTeam == 1
				|| (iSwapTeam == 0
					&& maxRounds
					//&& (UsingTickets() || IsLMS())
					&& m_iCurrentRound == maxRounds / 2 + 1
					)
				)
			)
			bSwapTeam = true;
		//
		RestartRound(bSwapTeam, bCycleRound);

		//if we're swapping teams, make a long delay
		float restartDelay = bSwapTeam ? 5.f : 1.f;

		//do not cause two simultaneous round restarts..
		m_bIsRestartingRound = false;
		m_flNextRoundRestart = gpGlobals->curtime + restartDelay;
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
	if (!pPlayer 
		|| (pPlayer->IsAlive() && skipAlive) 
		|| pPlayer->GetTeamNumber() < TEAM_AMERICANS
		|| pPlayer->GetTeamNumber() > TEAM_BRITISH)
		return;

	int iOtherTeam = pPlayer->GetTeamNumber() == TEAM_AMERICANS ? TEAM_BRITISH : TEAM_AMERICANS;

	pPlayer->m_bNoJoinMessage = true;
	const CPlayerClass* pClass = NClassQuota::FindInfiniteClassForTeam(iOtherTeam);
	pPlayer->ForceJoin(pClass, iOtherTeam, pClass->m_iClassNumber);

	//don't kill the player
	//pPlayer->ChangeTeam(iOtherTeam, false);
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
	bool	bUsingRoundSystem = mp_rounds.GetBool();

	if ( g_fGameOver )   // someone else quit the game already
	{
		if (!m_bHasDoneWinSong)
		{
			m_bHasDoneWinSong = true;
			//check for special gun game ending
			if (NGunGame::g_bGunGameActive || IsFFA()) {
				//find player with highest score
				int maxScore = -1;
				CHL2MP_Player* pWinner = NULL;
				for (int i = 0; i <= gpGlobals->maxClients; i++) {
					CHL2MP_Player* pPlayer = ToHL2MPPlayer(UTIL_PlayerByIndex(i));
					if (pPlayer && (!pWinner || pWinner->FragCount() > maxScore)) {
						pWinner = pPlayer;
						maxScore = pWinner->FragCount();
					}
				}
				if (pWinner) {
					if (IsFFA()) {
						//if we're in free for all, there's a single winner
						MSay("%s is the winner!", pWinner->GetPlayerName());
						CSay("%s is the winner!", pWinner->GetPlayerName());
						pWinner->m_unlockableProfile.createExperienceEvent(pWinner, EExperienceEventType::MATCH_WIN);
					}
					else {
						//otherwise make it a team-based win
						switch (pWinner->GetTeamNumber()) {
						case TEAM_AMERICANS:
							CSay("%s won for the American team!", pWinner->GetPlayerName());
							HandleScores(TEAM_AMERICANS, 0, AMERICAN_MAP_WIN, false);
							break;
						case TEAM_BRITISH:
							CSay("%s won for the British team!", pWinner->GetPlayerName());
							HandleScores(TEAM_BRITISH, 0, BRITISH_MAP_WIN, false);
							break;
						}
					}
				}
			}
			else if (pAmericans->GetScore() < pBritish->GetScore()) {
				//British Win
				HandleScores(TEAM_BRITISH, 0, BRITISH_MAP_WIN, false);
			}
			else if (pAmericans->GetScore() > pBritish->GetScore()) {
				//Americans Win
				HandleScores(TEAM_AMERICANS, 0, AMERICAN_MAP_WIN, false);
			}
			else if (pAmericans->GetScore() == pBritish->GetScore()) {
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
				m_iAmericanDmg += pPlayer->DamageScoreCount();
			}
			for (int x = 0; x < pBritish->GetNumPlayers(); x++)
			{
				CBasePlayer *pPlayer = pBritish->GetPlayer(x);
				if (!pPlayer)
					continue;
				m_iBritishDmg += pPlayer->DamageScoreCount();
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

	/*
	Check for end of map from timelimit if we're not using the round system
	*/
	if (!bUsingRoundSystem) {
		float flTimeLimit = GetMapRemainingTime();
		//why compare remaining time to current time? - Awesome
		if (flTimeLimit != 0 && flTimeLimit < 0) //gpGlobals->curtime >= flTimeLimit)
		{
			GoToIntermission();
			return;
		}
		//=========================
		//Time Left
		//=========================
		mp_timeleft.SetValue((flTimeLimit - gpGlobals->curtime)); //timeleft works just as well..
	}

	//BG2 - Draco - Start
	if (m_fNextFlagUpdate <= gpGlobals->curtime)
	{
		if (UsingTickets())
		{
			CheckTicketDrain();
			m_fNextFlagUpdate = gpGlobals->curtime + mp_tickets_drain_interval.GetInt();
		}
		else
		{
			CheckFullcap();
			m_fNextFlagUpdate = gpGlobals->curtime + 10; //because flags notify us for checking fullcaps, we could theoretically put FLT_MAX here, but that would break the game if the gamemode switched to tickets
		}
	}
	else if (m_fNextFlagUpdate == 0)
	{
		m_fNextFlagUpdate = gpGlobals->curtime + 1;
	}
	

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
	AutobalanceTeams();

	//=========================
	//Restart Round/Map
	//=========================
	if (sv_restartmap.GetInt() > 0) {
		m_fNextGameReset = gpGlobals->curtime + sv_restartmap.GetInt();
		sv_restartmap.SetValue(0);
	}

	//now if the time was set we can check for it, above zero means we are restarting
	if ((m_fNextGameReset > 0) && (m_fNextGameReset <= gpGlobals->curtime)) {
		RestartGame();
	}

	if (sv_restartround.GetInt() > 0) {
		m_fNextRoundReset = gpGlobals->curtime + sv_restartround.GetInt();
		sv_restartround.SetValue(0);
	}
	if ((m_fNextRoundReset > 0) && (m_fNextRoundReset <= gpGlobals->curtime)) {
		m_fNextRoundReset = 0;//dont reset again
		RestartRound(false);	//BG2 - restart round
	}

	//wave spawning 
	if (mp_respawnstyle.GetInt() == 1 || UsingTickets()) {
		if ((m_fLastRespawnWave + mp_respawntime.GetFloat()) <= gpGlobals->curtime)
		{
			RespawnWave();
			m_fLastRespawnWave = gpGlobals->curtime;
		}
	}

	//=========================
	//Round systems
	//=========================
	//don't bother if we have no players
	//if (pAmericans->GetNumPlayers() == 0 && pBritish->GetNumPlayers() == 0)
		//return;

	int aliveamericans = -1;
	int alivebritish = -1;
	if (IsLMS() || UsingTickets()) {
		aliveamericans = CountAlivePlayersAndTickets(TEAM_AMERICANS);
		alivebritish = CountAlivePlayersAndTickets(TEAM_BRITISH);
	}

	//we don't want a team to "win" by the other team not having other players
	bool bRoundVictoryPossible = (IsLMS() || UsingTickets()) && pAmericans->GetNumPlayers() != 0 && pBritish->GetNumPlayers() != 0;

	//this checks if we restart round from the time running out or from a round victory
	if ((bUsingRoundSystem && m_fLastRoundRestart + mp_roundtime.GetFloat() <= gpGlobals->curtime)
		|| (bRoundVictoryPossible && (aliveamericans == 0 || alivebritish == 0))) {

		if (!m_bIsRestartingRound)
		{
			//this block of code only happens once, at the end of the round
			m_flNextRoundRestart = gpGlobals->curtime + 5;
			m_bIsRestartingRound = true;

			//calculate the winning team
			//we do this differently depending on our gamemode
			if (IsLMS() || UsingTickets()) {
				if (aliveamericans > alivebritish)
				{
					m_iTDMTeamThatWon = TEAM_AMERICANS;
				}
				else if (aliveamericans < alivebritish)
				{
					m_iTDMTeamThatWon = TEAM_BRITISH;
				}
				else
				{
					m_iTDMTeamThatWon = TEAM_NONE;
				}
			}
			else {
				//skirmish mode
				int britScore = pBritish->GetScore();
				int amerScore = pAmericans->GetScore();
				if (britScore > amerScore)		m_iTDMTeamThatWon = TEAM_BRITISH;
				else if (amerScore > britScore) m_iTDMTeamThatWon = TEAM_AMERICANS;
				else							m_iTDMTeamThatWon = TEAM_NONE;
			}
		}
		else if (m_flNextRoundRestart < gpGlobals->curtime)
		{
			int score = 0;

			if (m_iTDMTeamThatWon != TEAM_NONE)
				score = 1;

			int msg_type = 0;
			switch (m_iTDMTeamThatWon) {
			case TEAM_AMERICANS:
				msg_type = AMERICAN_ROUND_WIN;
				break;
			case TEAM_BRITISH:
				msg_type = BRITISH_ROUND_WIN;
				break;
			case TEAM_NONE:
				msg_type = ROUND_DRAW;
				break;
			}

			HandleScores(m_iTDMTeamThatWon, score, msg_type, true);
		}
	}

	if ( gpGlobals->curtime > m_tmNextPeriodicThink )
	{		
		NScorePreserve::Think();
		m_tmNextPeriodicThink = gpGlobals->curtime + 30.0f;

		//we only need one of the two voting systems doing the automated map election
		g_pBritishVotingSystem->AutoMapchoiceElectionThink();

		SetMaxCmdRateAuto();

		if (m_flNextPeriodicMessage < gpGlobals->curtime) {
			m_flNextPeriodicMessage = gpGlobals->curtime + sv_periodic_message_interval.GetFloat();
			auto psz = sv_periodic_message.GetString();
			if (psz && psz[0]) {
				CSay(psz);
			}
		}
	}

	if (gpGlobals->curtime > m_tmDelayedMapChangeTime) {
		CElectionSystem::CancelAllElections();
		ChangeLevel();
		m_tmDelayedMapChangeTime = FLT_MAX;
	}

	g_pAmericanVotingSystem->ElectionThink();
	g_pBritishVotingSystem->ElectionThink();

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

/*void CHL2MPRules::RestartGame() {

}*/

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
	//Msg( "CLIENT DISCONNECTED, REMOVING FROM TEAM.\n" );

	CHL2MP_Player *pPlayer = (CHL2MP_Player *)CBaseEntity::Instance( pClient );
	if ( pPlayer )
	{
		// Remove the player from his team
		if ( pPlayer->GetTeam() )
		{
			pPlayer->GetTeam()->RemovePlayer( pPlayer );
		}

		//BG2 - Tjoppen - disconnecting. remove from flags
		pPlayer->RemoveSelfFromFlags();

		//BG3 - remove self from vcomm manager
		CBotComManager* pComms = CBotComManager::GetBotCommsOfPlayer(pPlayer);
		if (pComms->m_pContextPlayer == pPlayer) {
			pComms->m_pContextPlayer = nullptr;
			pComms->ResetThinkTime(bot_randfloat(4.0f, 16.0f));
		}

		//If this player is on solo mic, remove that
		extern CHL2MP_Player* g_pMicSoloPlayer;
		if (pPlayer == g_pMicSoloPlayer)
			g_pMicSoloPlayer = NULL;

		//BG3 - preserve our score
		NScorePreserve::NotifyDisconnected(pPlayer->entindex());

		//BG3 - notify class quotas that this player's class may be available
		NClassQuota::NotifyPlayerLeave(pPlayer);
	}	

	BaseClass::ClientDisconnected( pClient );

#endif
}

//-----------------------------------------------------------------------------
// Purpose: Player has just entered the game
//-----------------------------------------------------------------------------
#ifndef CLIENT_DLL
bool CHL2MPRules::ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen) {
	
	bool result = BaseClass::ClientConnected(pEntity, pszName, pszAddress, reject, maxrejectlen);

	//BG3 - Awesome - this entry point doesn't actually seem to be working, trying somewhere else
	/*if (pEntity) {
		CBaseEntity* pEnt = GetContainingEntity(pEntity);
		if (pEnt) {
			NScorePreserve::NotifyConnected(pEnt->entindex());
		}
	}*/

	return result;
}
#endif //CLIENT_DLL


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
					// // If the inflictor is the killer,  then it must be their current weapon doing the damage
					//BG3 - changed this to the damage info's weapon, less dereferencing and catches rare cases
					//where player dies after shooting
					//also helps us detect swivel gun hits
					if (info.GetWeapon())
					{
#ifdef HL1MP_DLL
						killer_weapon_name = pScorer->GetActiveWeapon()->GetClassname();
#else
						CBaseCombatWeapon* pWeapon = dynamic_cast<CBaseCombatWeapon*>(info.GetWeapon());
						if (pWeapon)
							killer_weapon_name = pWeapon->GetDeathNoticeName();
						//else if (pScorer->GetActiveWeapon())
							//killer_weapon_name = pScorer->GetActiveWeapon()->GetDeathNoticeName();
#endif
					}
					//Swivel guns and grenades at some points don't have proper weapon entities, check them specially
					else if (info.GetDamageType() & DMG_SWIVEL_GUN) {
						killer_weapon_name = "swivel_gun";
					}
					/*else if (info.GetDamageType() & DMG_TYPE_GRENADE) {
						killer_weapon_name = "weapon_frag";
					}*/
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
	return "Battle Grounds III";
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



float CHL2MPRules::GetMapElapsedTime()
{
	// if timelimit is disabled, return 0
	if (mp_timelimit.GetInt() <= 0)
		return 0;

	// timelimit is in minutes

	float timepassed = gpGlobals->curtime - m_flGameStartTime;

	return timepassed;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MPRules::Precache( void )
{
#ifdef CLIENT_DLL
#endif
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

		//return BaseClass::FShouldSwitchWeapon( pPlayer, pWeapon );

		//BG3 - Awesome - some weapons in HL2 had higher priorities than others, such that you would switch to a weapon after picking it up the first time
		//	disabling this, so that the primary weapon in the kit is always the one which is out when spawning
		return pPlayer->GetActiveWeapon() == NULL;
	}

#endif

#ifndef CLIENT_DLL

void CHL2MPRules::RestartGame()
{
	m_fNextGameReset = 0;//dont reset again

	//reset autoabalance think time
	m_flNextAutobalanceCheck = -FLT_MAX;

	//reset scores...
	NScorePreserve::Flush();

	g_Teams[TEAM_AMERICANS]->SetScore(0);//...for teams...
	g_Teams[TEAM_BRITISH]->SetScore(0);
	int x;
	for (x = 0; x < g_Teams[TEAM_AMERICANS]->GetNumPlayers(); x++) {
		CBasePlayer *pPlayer = g_Teams[TEAM_AMERICANS]->GetPlayer(x);
		if (!pPlayer)
			continue;

		pPlayer->ResetFragCount();//...for cap points...
		pPlayer->ResetDamageScoreCount();//...and damage
	}
	for (x = 0; x < g_Teams[TEAM_BRITISH]->GetNumPlayers(); x++) {
		CBasePlayer *pPlayer = g_Teams[TEAM_BRITISH]->GetPlayer(x);
		if (!pPlayer)
			continue;

		pPlayer->ResetFragCount();//...for cap points...
		pPlayer->ResetDamageScoreCount();//...and damage
	}
	m_iCurrentRound = 1;
	RestartRound(false);	//BG2 - Tjoppen - restart round

	//Reset the map time
	m_flGameStartTime = gpGlobals->curtime;

	//Gun game score reset
	if (NGunGame::g_bGunGameActive) NGunGame::ResetGunKitsOnPlayers();
}

void CHL2MPRules::AutobalanceTeams() {
	if (gpGlobals->curtime < m_flNextAutobalanceCheck)
		return;
	m_flNextAutobalanceCheck = gpGlobals->curtime + 1;

	if (mp_autobalanceteams.GetBool()) {
		CTeam* pAmericans = g_Teams[TEAM_AMERICANS];
		CTeam* pBritish = g_Teams[TEAM_BRITISH];

		//use the right sum to find diff, I don't like negative numbers...
		int iAutoTeamBalanceTeamDiff = 0;
		int iAutoTeamBalanceBiggerTeam = TEAM_BRITISH;
		if (pAmericans->GetNumPlayers() > pBritish->GetNumPlayers()) {
			iAutoTeamBalanceTeamDiff = (pAmericans->GetNumPlayers() - pBritish->GetNumPlayers());
			iAutoTeamBalanceBiggerTeam = TEAM_AMERICANS;
		}
		else {
			iAutoTeamBalanceTeamDiff = (pBritish->GetNumPlayers() - pAmericans->GetNumPlayers());
			iAutoTeamBalanceBiggerTeam = TEAM_BRITISH;
		}

		if (iAutoTeamBalanceTeamDiff >= mp_autobalancetolerance.GetInt()) {
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

void AnticheatDisabledCheck() {
	extern ConVar sv_disable_anticheat;
	if (sv_disable_anticheat.GetBool()) {
		MSay("WARNING: Anticheat is explicitly disabled on this server.");

		/*extern ConVar hostname;
		std::string serverName = hostname.GetString();

		//append warning to server name
		//first check if it's already there... assume false
		bool warningInPlace = false;
		if (serverName.length() > 6) {
			auto nameDup = serverName;
			nameDup[6] = '\0';
			if (nameDup == std::string("ANTICHEAT DISABLED")) {
				warningInPlace = true;
			}
		}
			
		if (!warningInPlace) {
			serverName = "ANTICHEAT DISABLED " + serverName;
			hostname.SetValue(serverName.c_str());
		}*/
	}
}

void CHL2MPRules::RestartRound(bool swapTeams, bool bSetLastRoundTime)
{
	AnticheatDisabledCheck();

	//restart current round. immediately.
	ResetMap();
	ResetFlags();
	CSDKBot::ResetAllBots();

	Msg("RESTARTING ROUND\n");

	if (swapTeams)
		SwapTeams();

	//as good a place as any to set this
	//used by players' classmenu to determine whether or not to show all weapons
	ConVarRef password("sv_password");
	sv_password_exists.SetValue(password.GetString() && password.GetString()[0] != '\0');

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

	if (mp_respawntime.GetInt() > 0 && bSetLastRoundTime)
		m_fLastRoundRestart = m_fLastRespawnWave = gpGlobals->curtime;
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
		pPlayer->RemoveRagdolls();
	}

	for (x = 0; x < g_Teams[TEAM_BRITISH]->GetNumPlayers(); x++)
	{
		CHL2MP_Player *pPlayer = ToHL2MPPlayer(g_Teams[TEAM_BRITISH]->GetPlayer(x));

		if (!pPlayer)
			continue;

		pPlayer->Spawn();

		//BG2 - Tjoppen - remove ragdoll - remember to change this to remove multiple ones if we decide to enable more corpses
		pPlayer->RemoveRagdolls();
	}
	if (UseLineSpawn()) {
		CLineSpawn::SpawnBothTeams();
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
		else switch (pFlag->m_iForTeam) //So the flag is set to be capped by this team.
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
			HandleScores(TEAM_AMERICANS, mp_winbonus.GetInt(), AMERICAN_ROUND_WIN, true, false);
			return;
		}
		if ((forbritish - british_flags) == 0 && forbritish != 0)
		{
			HandleScores(TEAM_BRITISH, mp_winbonus.GetInt(), BRITISH_ROUND_WIN, true, false);
			return;
		}
	}
	else
	{
		if (american_flags <= 0 && british_flags <= 0)
		{
			//draw
			HandleScores(TEAM_NONE, 0, ROUND_DRAW, true, false);
			//Msg( "draw\n" );
			return;
		}

		if (american_flags <= 0 && (forbritish - british_flags) == 0)
		{
			//british win
			//Msg( "british win\n" );
			HandleScores(TEAM_BRITISH, mp_winbonus.GetInt(), BRITISH_ROUND_WIN, true, false);
			return;
		}

		if (british_flags <= 0 && (foramericans - american_flags) == 0)
		{
			//americans win
			//Msg( "americans win\n" );
			HandleScores(TEAM_AMERICANS, mp_winbonus.GetInt(), AMERICAN_ROUND_WIN, true, false);
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

void CHL2MPRules::SetRemainingRoundTime(float flSeconds) {
	//modify m_fLastRoundRestart 
	m_fLastRoundRestart = gpGlobals->curtime - mp_roundtime.GetFloat() + flSeconds;
}

void CHL2MPRules::ChangeMapDelayed(float flDelay) {
	const char* next = nextlevel.GetString();
	if (next && next[0]) {
		MSay("Changing map to %s", nextlevel.GetString());
	}
	else {
		MSay("Changing map...");
	}
	
	m_tmDelayedMapChangeTime = gpGlobals->curtime + flDelay;
}
#endif

bool CHL2MPRules::UsingTickets()
{
	return mp_respawnstyle.GetInt() == 3 /*|| mp_respawnstyle.GetInt() == 4*/;
}
