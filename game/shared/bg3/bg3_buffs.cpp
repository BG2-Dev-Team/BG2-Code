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

//BG3 - <name of contributer>[ - <small description>]
*/

#include "cbase.h"
#include "bg3_buffs.h"
#include "bg3_player_shared.h"
#include "effect_dispatch_data.h"
#ifndef CLIENT_DLL
#include "te_effect_dispatch.h"
#endif

#ifdef CLIENT_DLL
#define CVAR_FLAGS	(FCVAR_REPLICATED | FCVAR_NOTIFY)
#define CVAR_FLAGS_HIDDEN (FCVAR_REPLICATED)
#define FCVAR_RALLY_TIME (FCVAR_REPLICATED /*| FCVAR_HIDDEN*/)
#else
#define CVAR_FLAGS	(FCVAR_GAMEDLL | FCVAR_REPLICATED | FCVAR_NOTIFY)
#define CVAR_FLAGS_HIDDEN (FCVAR_GAMEDLL | FCVAR_REPLICATED)
#define FCVAR_RALLY_TIME (FCVAR_GAMEDLL | FCVAR_REPLICATED /*| FCVAR_HIDDEN*/)
#endif

extern ConVar mp_respawnstyle;
extern ConVar lb_officer_classoverride_a;
extern ConVar lb_officer_classoverride_b;


static ConVar mp_nextrally_amer("mp_nextrally_amer", "0", FCVAR_RALLY_TIME | FCVAR_HIDDEN | FCVAR_REPLICATED);
static ConVar mp_nextrally_brit("mp_nextrally_brit", "0", FCVAR_RALLY_TIME | FCVAR_HIDDEN | FCVAR_REPLICATED);

static ConVar mp_endrally_amer("mp_endrally_amer", "0", FCVAR_RALLY_TIME | FCVAR_HIDDEN | FCVAR_REPLICATED);
static ConVar mp_endrally_brit("mp_endrally_brit", "0", FCVAR_RALLY_TIME | FCVAR_HIDDEN | FCVAR_REPLICATED);

inline ConVar* NextRallyTimeCvarFor(int iTeam) { return iTeam == TEAM_AMERICANS ? &mp_nextrally_amer : &mp_nextrally_brit; }
inline ConVar* EndRallyTimeCvarFor(int iTeam) { return iTeam == TEAM_AMERICANS ? &mp_endrally_amer : &mp_endrally_brit; }

#define MAX_LEN_FLOAT 10
char flBuffer[MAX_LEN_FLOAT];

namespace BG3Buffs {
#ifndef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: Interprets and handles a player's voice command for rallying ability
//-----------------------------------------------------------------------------
	bool RallyRequest(int vcommCommand, CHL2MP_Player* pRequester) {
		//first check whether or not we're currently allowed to rally
		if (!PlayerCanRally(pRequester)) {
			return false;
		}

		//then check that the vcomm is registered with a rallying bonus
		int newRallyFlags = ParseRallyCommand(vcommCommand);
		if (newRallyFlags == -1)
			return false;

		//Okay, so we can rally our fellow players now...
		//First determine our buff radius
		extern ConVar mp_respawnstyle;
		float buffRadius;
		if (IsLinebattle())
			buffRadius = RALLY_RADIUS_LINEBATTLE;
		else
			buffRadius = RALLY_RADIUS_SKIRM;

		//Soon we're going to iterate through other players. Let's count how many are affected.
		int affectedCount = 0;
		int iTeam = pRequester->GetTeamNumber();

		CRecipientFilter filter;

		//Iterate through them and rally them if they're close enough
		CHL2MP_Player * curPlayer = nullptr;
		Vector ourLocation = pRequester->GetAbsOrigin();
		for (int i = 1; i <= gpGlobals->maxClients; i++) {
			curPlayer = static_cast<CHL2MP_Player*>(UTIL_PlayerByIndex(i));

			//check for valid player that isn't us
			if (curPlayer && curPlayer->IsAlive() && curPlayer->GetTeamNumber() == iTeam && curPlayer != pRequester) {
				Vector theirLocation = curPlayer->GetAbsOrigin();
				if ((ourLocation - theirLocation).Length() < buffRadius){
					RallyPlayer(newRallyFlags, curPlayer);
					affectedCount++;
					filter.AddRecipient(curPlayer);
				}
			}
		}

		//check against wasted buffs, and only rally ourself if we had rallied someone else
		if (affectedCount == 0) {
			return false;
		} else {
			//if we've rallied other players, rally ourselves to
			RallyPlayer(newRallyFlags, pRequester);
			filter.AddRecipient(pRequester);

			//Set end and next rally times
			ConVar* pcvEndTime = EndRallyTimeCvarFor(iTeam);
			pcvEndTime->SetValue(gpGlobals->curtime + GetRallyDuration(newRallyFlags));

			SetNextRallyTime(iTeam, gpGlobals->curtime + RALLY_INTERVAL);

			//notify clients of rallying, activating HUD events
			CEffectData data;
			DispatchEffect("RalEnab", data, filter);
		}

		//return true to indicate request as succesful
		return true;
	}

//-----------------------------------------------------------------------------
// Purpose: Rallys a player and calls for its OnEnabled / OnDisabled functions
//-----------------------------------------------------------------------------
	void	RallyPlayer(int rallyFlags, CHL2MP_Player* pPlayer) {
		//First see if we're just clearing out rally flags
		if (rallyFlags == 0){
			pPlayer->m_iCurrentRallies = 0;
			return;
		}

		//Don't let us be rallied again if we're already being rallied
		if (pPlayer->m_iCurrentRallies)
			return;

		//Okay now actually set the rally flags
		pPlayer->m_iCurrentRallies = rallyFlags;

		pPlayer->OnRallyEffectEnable();
	}

//-----------------------------------------------------------------------------
// Purpose: Sets the next time the given team can rally, for external control
//-----------------------------------------------------------------------------
	void	SetNextRallyTime(int iTeam, float flTime) {
		ConVar* pcvNextTime = NextRallyTimeCvarFor(iTeam);
		pcvNextTime->SetValue(flTime);
	}

//-----------------------------------------------------------------------------
// Purpose: Resets rally times to appropriate values
//-----------------------------------------------------------------------------
	void	Reset() {
		mp_nextrally_amer.SetValue(gpGlobals->curtime);
		mp_nextrally_brit.SetValue(gpGlobals->curtime);
	}

#else

	inline bool IsLinebattle()	{ return mp_respawnstyle.GetInt() == 4; }

#define bcase break; case
	CHudTexture* RallyIconFrom(int rallyFlags) {
		CHudTexture* pIcon;
		switch (rallyFlags) {
		default:
			pIcon = g_ppIcons[NONE];
		bcase RALLY_ADVANCE:
			pIcon = g_ppIcons[ADVANCE];
		bcase RALLY_FIRE:
			pIcon = g_ppIcons[FIRE];
		bcase RALLY_RALLY_ROUND:
			pIcon = g_ppIcons[RALLY_ROUND];
		bcase RALLY_RETREAT:
			pIcon = g_ppIcons[RETREAT];
		}
		
		return pIcon;
	}


	CHudTexture* g_ppIcons[5];
	void InitializeIcons() {
		g_ppIcons[ADVANCE]		= gHUD.GetIcon("buff_advance");
		g_ppIcons[FIRE]			= gHUD.GetIcon("buff_fire");
		g_ppIcons[RALLY_ROUND]	= gHUD.GetIcon("buff_rally_round");
		g_ppIcons[RETREAT]		= gHUD.GetIcon("buff_retreat");
		g_ppIcons[NONE]			= gHUD.GetIcon("buff_empty");
	}

	void SendRallyRequestFromSlot(int iSlot) {
		const char* pszCommand = "";
		switch (iSlot) {
		default:
			bcase 1 :
				pszCommand = "voicecomm 9"; //advance command
			bcase 2:
				pszCommand = "voicecomm 16"; //fire command
			bcase 3:
				pszCommand = "voicecomm 11"; //rally round command
			bcase 4:
				pszCommand = "voicecomm 10"; //retreat command
		}
		engine->ServerCmd(pszCommand);
	}
#undef bcase

#endif //CLIENT_DLL

//-----------------------------------------------------------------------------
// Purpose: Gets the time the current rally for the team will end, or otherwise
//		when the last one ended.
//-----------------------------------------------------------------------------
	float	GetEndRallyTime(int iTeam) {
		return EndRallyTimeCvarFor(iTeam)->GetFloat();
	}

//-----------------------------------------------------------------------------
// Purpose: Gets the next time this team can rally
//-----------------------------------------------------------------------------
	float	GetNextRallyTime(int iTeam) {
		return NextRallyTimeCvarFor(iTeam)->GetFloat();
	}

	float	GetRallyDuration(int rallyFlags) {
		Assert(rallyFlags);
		float duration = 0.0f;

		switch (rallyFlags){
		case RALLY_ADVANCE:
			duration = RALLY_ADVANCE_DURATION;
			break;
		case RALLY_RALLY_ROUND:
			duration = RALLY_RALLY_ROUND_DURATION;
			break;
		case RALLY_FIRE:
			duration = RALLY_FIRE_DURATION;
			break;
		case RALLY_RETREAT:
			duration = RALLY_RETREAT_DURATION;
		default:
			Assert("Failed to get duration of a rally flag!" && 0);
			break;
		}

		//Lengthen the time if we're in linebattle mode
		
		if (IsLinebattle())
			duration *= RALLY_DURATION_LB_MOD;

		return duration;
	}

	//Checks that we're of the right class
	//@requires pPlayer is non-null
	bool	PlayersClassHasRallyAbility(const CHL2MP_Player* pPlayer) {
		ConVar * officerClass = pPlayer->GetTeamNumber() == TEAM_AMERICANS ? &lb_officer_classoverride_a : &lb_officer_classoverride_b;

		bool bAllow; 

		if (IsLinebattle() && officerClass->GetBool()) {
			bAllow = pPlayer->GetTeamNumber() == officerClass->GetInt();
		} else {
			bAllow = pPlayer->GetPlayerClass()->m_bCanDoVcommBuffs;
		}
		return bAllow;
	}

	//Checks against times and player status
	//@requires pRequester is non-null
	bool	PlayerCanRally(const CHL2MP_Player* pRequester) {
		if (!pRequester->IsAlive())
			return false;

		if (!PlayersClassHasRallyAbility(pRequester))
			return false;

		int iTeam = pRequester->GetTeamNumber();
		if (!(iTeam == TEAM_AMERICANS || iTeam == TEAM_BRITISH))
			return false;

		//check against times
		if (gpGlobals->curtime < NextRallyTimeCvarFor(iTeam)->GetFloat())
			return false;

		//we've passed all checks, return true
		return true;
	}
	//Given a vcomm command, parses it into rally flags. Returns -1 if its an unusable vcomm
	int		ParseRallyCommand(int vcommCommand) {
		int newRallyFlags;
		switch (vcommCommand)
		{
		case VCOMM2_ADVANCE:
			newRallyFlags = RALLY_ADVANCE;
			break;
		case VCOMM2_RALLY_ROUND:
			newRallyFlags = RALLY_RALLY_ROUND;
			break;
		case VCOMM2_FIRE:
			newRallyFlags = RALLY_FIRE;
			break;
		case VCOMM2_RETREAT:
			newRallyFlags = RALLY_RETREAT;
			break;
		default:
			newRallyFlags = -1;
		}
		return newRallyFlags;
	}
}