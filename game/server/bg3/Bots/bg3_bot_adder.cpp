/*
The Battle Grounds 3 - A Source modification
Copyright (C) 2017, The Battle Grounds 3 Team and Contributors

The Battle Grounds 3 free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

The Battle Grounds 3 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Contact information:
Chel "Awesome" Trunk		mail, in reverse: com . gmail @ latrunkster

You may also contact the (future) team via the Battle Grounds website and/or forum at:
www.bg2mod.com

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
/*
Contrib by Awesome: Overhauled bots to use an intelligent strategy pattern. Also added additional functionalities
in separate files for bot communication, map navigation, etc.
*/

#include "cbase.h"
#include "player.h"
//#include "sdk_player.h"
#include "in_buttons.h"
#include "movehelper_server.h"
#include "gameinterface.h"
#include "team.h"
#include "hl2mp_gamerules.h"
#include "bg3_bot.h"
#include "hl2mp_player.h"
#include "../bg2/flag.h"
#include "../bg2/weapon_bg2base.h"
#include "../shared/bg3/bg3_class_quota.h"

//bot manager manages bot population
#include "bg3_bot_manager.h"
#include "bg3_bot_navpoint.h"
#include "bg3_bot_vcomms.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar bot_forceclass("bot_forceclass", "0", FCVAR_GAMEDLL | FCVAR_HIDDEN, "Force bots to spawn as given class. 0 = Infantry, 1 = Officer, 2 = Sniper, 3 = Skirmisher");
ConVar bot_limitclass("bot_limitclass", "1", 0, "Force bots to conform to class regulations.");

const char* g_ppszEasyBotPrefixes[] = { "Pte.", "Pte.", "LCpl.", };

const char* g_ppszMedBotPrefixes[] = { "Pte.", "Pte.", "LCpl.", "LCpl.", "Cpl.", };

const char* g_ppszHardBotPrefixes[] = { "Cpl.", "Cpl.", "Sjt.", "Sjt.", };

const char* g_ppszRareBotPrefixes[] = { "Lt.", "Capt.", "Maj.", "LtCol.", };

const char* g_ppszBotNames[] = {
	"Martin", "Smith", "Williams", "Jones", "Coleman", "Jenkins", "Adams", "Clark",
	"Hall", "Nelson", "Campbell", "Cox", "Cook", "Washington", "Cooper", "Harris", "Rogers",
	"Torres", "Ward", "Murphy", "Long", "Alexander", "Thomas", "Wright", "Bell", "Richardson",
	"Evans", "Butler", "Kelly", "Lee", "Rivera", "Miller", "Davis", "Garcia", "Griffin",
	"Bailey", "Powell", "Ross", "Wood", "Reed", "Lewis", "Taylor", "Perry", "Young", "Brewster",
	"Loomis",

	"Atkey", "Brown", "Cogswell", "Dutton", "Eplett", "Francis", "Glanville", "Hurst",
	"Jackson", "Kent", "Lloyd", "Macey", "Noyes", "Olivey", "Perry", "Rosser",
	"Sanders", "Tuck", "Unsworth", "Varley", "Whitworth", "Yeo",
};

const char* g_ppszBotRareNames[] = {
	"Karpinsky", "Allen", "Hale", "Revere", "Tallmadge", "Gates", "Morgan",
	"Arnold", "Greene", "Howe", "Prescott", "Tarleton", "Klif", "Hawke", "Roob",
	"Chris", "Emosewa", "Fox"

};

//-----------------------------------------------------------------------------
// Purpose: Create a new Bot and put it in the game.
// Output : Pointer to the new Bot, or NULL if there's no free clients.
//-----------------------------------------------------------------------------
CBasePlayer *BotPutInServer(int iAmount, bool bFrozen)
{
	int i = 1;
	while (i <= iAmount)
	{
		char botname[32];
		const char * pRank;
		const char * pName;

		//First choose a difficulty
		BotDifficulty* pDifficulty = CSDKBot::GetDifficulty();

		//randomly get a name
		if (pDifficulty == &BotDiffEasy) {
			pRank = g_ppszEasyBotPrefixes[RandomInt(0, ARRAYSIZE(g_ppszEasyBotPrefixes) - 1)];
			pName = g_ppszBotNames[RandomInt(0, ARRAYSIZE(g_ppszBotNames) - 1)];
		} else if (pDifficulty == &BotDiffNorm) {
			pRank = g_ppszMedBotPrefixes[RandomInt(0, ARRAYSIZE(g_ppszMedBotPrefixes) - 1)];
			pName = g_ppszBotNames[RandomInt(0, ARRAYSIZE(g_ppszBotNames) - 1)];
		} else {
			if (RandomFloat() > 0.95f) {
				pRank = g_ppszRareBotPrefixes[RandomInt(0, ARRAYSIZE(g_ppszRareBotPrefixes) - 1)];
				pName = g_ppszBotRareNames[RandomInt(0, ARRAYSIZE(g_ppszBotRareNames) - 1)];
			} else {
				pRank = g_ppszHardBotPrefixes[RandomInt(0, ARRAYSIZE(g_ppszHardBotPrefixes) - 1)];
				pName = g_ppszBotNames[RandomInt(0, ARRAYSIZE(g_ppszBotNames) - 1)];
			}
		}

		Q_snprintf(botname, sizeof(botname), "%s %s", pRank, pName);

		// This trick lets us create a CSDKBot for this client instead of the CSDKPlayer
		// that we would normally get when ClientPutInServer is called.
		//ClientPutInServerOverride( &CBotManager::ClientPutInServerOverride_Bot );
		edict_t *pEdict = engine->CreateFakeClient(botname);
		//ClientPutInServerOverride( NULL );

		if (!pEdict)
		{
			Msg("Failed to create Bot(%s).\n", botname);
			return NULL;
		}

		// Allocate a player entity for the bot, and call spawn
		CSDKPlayer *pPlayer = ((CSDKPlayer *)CBaseEntity::Instance(pEdict));

		if (!pPlayer)
		{
			Msg("Couldn't cast bot to player\n");
			return NULL;
		}
		
		//initialize the bot's AI, class, and team
		CSDKBot::Init(pPlayer, pDifficulty);

		i++;
		//return pPlayer;
	}
	return NULL;
}

int  g_iWaitingAmount = 0; //This is just a temp int really.
int  g_iNextBotTeam = TEAM_BRITISH;

//ConCommand  cc_Bot( "bot_add", BotAdd_f, "Add a bot", FCVAR_CHEAT );
CON_COMMAND_F(bot_add, "Creates bot(s)in the server. <Bot Count>", FCVAR_GAMEDLL)
{
	int m_iCount;
	if (args.ArgC() > 1)
		m_iCount = atoi(args[1]); //Spawn given number of bots.
	else
		m_iCount = 1; //Just spawn 1 bot.

	g_iNextBotTeam = g_iNextBotTeam == TEAM_AMERICANS ? TEAM_BRITISH : TEAM_AMERICANS;

	if (g_bServerReady) //Server is already loaded, just do it.
		CBotManager::AddBotOfTeam(g_iNextBotTeam, m_iCount);
}

CON_COMMAND_F(bot_add_a, "Creates bot(s)in the server. <Bot Count>", FCVAR_GAMEDLL)
{
	int m_iCount;
	if (args.ArgC() > 1)
		m_iCount = atoi(args[1]); //Spawn given number of bots.
	else
		m_iCount = 1; //Just spawn 1 bot.

	g_iNextBotTeam = TEAM_AMERICANS;

	if (g_bServerReady) //Server is already loaded, just do it.
		CBotManager::AddBotOfTeam(g_iNextBotTeam, m_iCount);
}

CON_COMMAND_F(bot_add_b, "Creates bot(s)in the server. <Bot Count>", FCVAR_GAMEDLL)
{
	int m_iCount;
	if (args.ArgC() > 1)
		m_iCount = atoi(args[1]); //Spawn given number of bots.
	else
		m_iCount = 1; //Just spawn 1 bot.

	g_iNextBotTeam = TEAM_BRITISH;

	if (g_bServerReady) //Server is already loaded, just do it.
		CBotManager::AddBotOfTeam(g_iNextBotTeam, m_iCount);
}

//called when game server is created
CSDKBot::CSDKBot() {
	m_bLastThinkWasStateChange = false;
	m_bUpdateFlags = false;
	m_bTowardFlag = false;
	m_pPlayer = NULL;
	m_flNextFireTime = FLT_MAX;
	m_flNextStrafeTime = FLT_MAX;
	m_flEndRetreatTime = FLT_MAX;

	m_bLastThinkWasInFlag = false;

	m_pCurThinker = m_pPrevThinker = s_pDefaultThinker;

}

//called when a bot is put in the server
void CSDKBot::Init(CBasePlayer* pPlayer, BotDifficulty* pDifficulty) {
	CSDKBot& curBot = *ToBot(pPlayer);
	curBot.m_pPlayer = pPlayer;
	curBot.m_flNextStrafeTime = 0;
	curBot.m_flForceNoRetreatTime = 0;

	//default to death thinker or else bad things happen!
	curBot.m_pPrevThinker = &BotThinkers::Death;
	curBot.m_pCurThinker = &BotThinkers::Death;
	curBot.m_bLastThinkWasStateChange = false;
	curBot.m_bLastThinkWasInFlag = false;

	curBot.m_flLastFollowTime = FLT_MIN;

	curBot.m_pDifficult = pDifficulty;
	curBot.m_flNextThink = gpGlobals->curtime + 0.1f;
	pPlayer->ClearFlags();

	//lastClass = (lastClass + 1) % 4; //4 classes now.
	int lastClass = RandomInt(0, 3);

	pPlayer->ChangeTeam(g_iNextBotTeam);

	//BG2 - Obey Class Limits now. -HairyPotter
	if (bot_forceclass.GetInt() > -1) //Force the bot to spawn as the given class.
		((CHL2MP_Player*)pPlayer)->SetNextClass(bot_forceclass.GetInt());

	else if (bot_limitclass.GetBool() && bot_forceclass.GetInt() < 0) //Just make sure we're not trying to force a class that has a limit.
	{
		int limit = NClassQuota::GetLimitTeamClass(g_iNextBotTeam, lastClass);
		if (limit >= 0 && g_Teams[g_iNextBotTeam]->GetNumOfNextClass(lastClass) >= limit)
		{
			Msg("Tried to spawn too much of class %i. Spawning as infantry. \n", lastClass);
			((CHL2MP_Player*)pPlayer)->SetNextClass(0); //Infantry by default
		} else
			((CHL2MP_Player*)pPlayer)->SetNextClass(lastClass);
	} else //Otherwise just spawn as whatever RandomInt() turns up.
		((CHL2MP_Player*)pPlayer)->SetNextClass(lastClass);

	pPlayer->AddFlag(FL_CLIENT | FL_FAKECLIENT);

	//only spawn if we're not in LMS
	if (!IsLMS())
		pPlayer->Spawn();

	curBot.m_PlayerSearchInfo.Init(pPlayer);
}

void CSDKBot::ResetAllBots() {
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CSDKPlayer *pPlayer = UTIL_PlayerByIndex(i);// );

		if (pPlayer && pPlayer->IsFakeClient())
		{
			CSDKBot *pBot = ToBot(pPlayer);
			if (pBot) //Do most of the "filtering" here.
				pBot->ScheduleThinker(&BotThinkers::Death, 1.0f);
		}
	}
}