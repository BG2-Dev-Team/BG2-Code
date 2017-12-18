/*
The Battle Grounds 3 - A Source modification
Copyright (C) 2017, The Battle Grounds 2 Team and Contributors

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

//BG3 - <name of contributer>[ - <small description>]
*/

#ifndef BG3_BOT_INFLUENCER_H
#define BG3_BOT_INFLUENCER_H
#include "cbase.h"
#include "bg3_bot.h"
#include "bg3_bot_vcomms.h"
//#include "memory"

/**************************************************************
* Bot influencer distributes "orders" to bots to allow them
* to react to the voice commands of other players and other bots.
*
* Implemented as a friendly utility class (not a namespace) so that
*	it can have access to bot's private members
**************************************************************/
abstract_class NBotInfluencer {
private:
	//private constructor prevents instantiation
	NBotInfluencer() {}

	//Execution commands act on a single bot
	//One execution per sensible voice command
	typedef		void (*pfExecutor)(CSDKBot*);
#define DEC_EXECUTOR(name) \
	static void Execute_##name(CSDKBot* pBot);\
	static pfExecutor s_pf##name;

	DEC_EXECUTOR(Fire);
	DEC_EXECUTOR(Retreat);
	DEC_EXECUTOR(Advance);
	DEC_EXECUTOR(Halt);
	DEC_EXECUTOR(Rally_Round);

	inline static bool ShouldIgnore(CSDKBot* pBot) { return pBot->m_pCurThinker == &BotThinkers::Retreat || pBot->m_pCurThinker == &BotThinkers::Melee; }

	static void InitMap(CUtlMap<comm_t, pfExecutor>* pMap);
	static CUtlMap<comm_t, pfExecutor>* CreateMap();
	static CUtlMap<comm_t, pfExecutor>* g_pExecutorMap;

	static void		DispatchCommand(CBasePlayer* pRequester, vec_t vRange, comm_t iComm);
	inline static vec_t	GetExecutorCommandRange() { return 512.f; }
public:
	static void NotifyCommand(CBasePlayer* pPlayer, comm_t iComm);
};


#endif //BG3_BOT_INFLUENCER_H