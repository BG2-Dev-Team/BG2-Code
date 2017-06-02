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
#ifdef WIN32
#pragma once
#endif
#include "cbase.h"
#include "player.h"
#include "../shared/bg2/bg2_player_shared.h"
//#include "sdk_player.h"
#include "sdk/bg3_bot.h"
#include "bg3_bot_manager.h"

/*
CBotComManager - handles automatic context-sensitive voice commands from bots
*/
class CBotComManager;

extern CBotComManager g_BotBritComms;
extern CBotComManager g_BotAmerComms;

/*
Console Variables
*/
extern ConVar bot_vcomms; //Whether or not to allow bots to do vcomms

/*
Alias for the chars appended onto the end of vcomm commands
*/
typedef int comm_t;

class CBotComManager {
	friend class CBotManager;
	friend class CHL2MPRules;

private:
	/*
	Private variables
	*/
	int				m_iTeam;
	float			m_flNextComm; //when the next comm should be played

	BotContext		m_eContext; //this is continuously updated by this team's bots.
	CBasePlayer*	m_pContextPlayer; //the player who sent the last context

	//if not NONE , and m_pContextPlayer has a nearby bot teammate, the bot plays this context instead
	//used for bots responding with yes/no to other players' and bots' commands
	BotContext		m_eForcedContext; 


public:
	/*
	Constructors and initializers
	*/
	CBotComManager(int iTeamNumber);
	void ResetThinkTime(float flDelay);

private:
	/*
	Private helper functions
	*/
	static comm_t ParseContext(BotContext context); //parses a context into appropriate vcomm number

	void DispatchVCommToBot(); //Looks at our contexts to dispatch a voice command to a bot

public:
	/*
	Public functions and procedures
	*/
	void Think(); //checks if a bot should play a vcomm, and calls for it if so

	//notifies the vcomms manager of a new context, whether it's been created or not
	void ReceiveContext(CBasePlayer* pDispatcher, BotContext eContext, bool bSoundAlreadyPlayed = false);

	BotContext	LastContext() const { return m_eContext; }
	bool		LastContextFromRealPlayer() const { return m_pContextPlayer && !(m_pContextPlayer->GetFlags() & FL_FAKECLIENT); }

	//for interpreting comms sent from real players and other bots
	//returns default if the real context should not incite special response
	//from bots
	static BotContext		ParseIntToContext(int iCom); 
	static CBotComManager*	GetBotCommsOfPlayer(CBasePlayer* pPlayer);
};