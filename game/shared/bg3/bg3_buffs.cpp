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
battlegrounds3.com

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
#include "../shared/bg3/vgui/bg3_buff_sprite.h"
#else
#include <vgui_controls/controls.h>
#include <vgui/ILocalize.h>
#endif
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

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

	void	CreateRallyEffectAtEntity(CBaseEntity* pParent, int iForTeam) {
//		Msg(__FUNCTION__ "\n");
		//Create the buff sprite
		CBaseEntity* pEntity = CreateEntityByName("buff_sprite");
		CBuffSprite* pBuffSprite = static_cast<CBuffSprite*>(pEntity);
		if (pEntity && pBuffSprite) {
			pBuffSprite->m_iForTeam = iForTeam;
			Vector offset = pParent->IsPlayer() ? Vector(0, 0, 30) : Vector(0, 0, 60);
			//Msg("offset %f\n", offset.z);
			pBuffSprite->SetAbsOrigin(pParent->GetAbsOrigin() + offset);
			pBuffSprite->SetParent(pParent);
			pBuffSprite->Spawn();
			//Msg(" Created sprite entity!\n");
		}
	}

//-----------------------------------------------------------------------------
// Purpose: Interprets and handles a player's voice command for rallying ability
//-----------------------------------------------------------------------------
	bool RallyRequest(int vcommCommand, CBaseEntity* pRequester, float flRadiusOverride) {
		//first check whether or not we're currently allowed to rally
		CHL2MP_Player* pPlayer = dynamic_cast<CHL2MP_Player*>(pRequester);
		if (pPlayer && !PlayerCanRally(pPlayer)) {
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
		if (flRadiusOverride)
			buffRadius = flRadiusOverride;
		else if (IsLinebattle())
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
			if (pPlayer) {
				RallyPlayer(newRallyFlags, pPlayer);
				filter.AddRecipient(pPlayer);
			}
			

			//Set end and next rally times
			ConVar* pcvEndTime = EndRallyTimeCvarFor(iTeam);
			pcvEndTime->SetValue(gpGlobals->curtime + GetRallyDuration(newRallyFlags));

			//If our requester was a player, reset the next time
			if (pPlayer)
				SetNextRallyTime(iTeam, gpGlobals->curtime + GetRallyInterval(newRallyFlags));

			//notify clients of rallying, activating HUD events
			CEffectData data;
			DispatchEffect("RalEnab", data, filter);

			CreateRallyEffectAtEntity(pRequester, iTeam);
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
			pPlayer->m_flEndRallyTime = -FLT_MAX;
			return;
		}

		//Don't let us be rallied again if we're already being rallied
		//but let rally-round cancel the slow
		if (pPlayer->m_iCurrentRallies && (pPlayer->m_iCurrentRallies != NERF_SLOW || rallyFlags != RALLY_RALLY_ROUND))
			return;

		if (rallyFlags == RALLY_RALLY_ROUND)
			pPlayer->m_iStamina = 100;

		//Okay now actually set the rally flags
		pPlayer->m_iCurrentRallies = rallyFlags;
		pPlayer->m_flEndRallyTime = gpGlobals->curtime + BG3Buffs::GetRallyDuration(rallyFlags);

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

		//slow nerf suppresses officer rally
		if (pRequester->m_iCurrentRallies == NERF_SLOW)
			return false;

		//we've passed all checks, return true
			return true;
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
		bcase NERF_SLOW:
			pIcon = g_ppIcons[SLOW];
		}
		
		return pIcon;
	}


	CHudTexture* g_ppIcons[6];
	void InitializeIcons() {
		g_ppIcons[ADVANCE]		= gHUD.GetIcon("buff_advance");
		g_ppIcons[FIRE]			= gHUD.GetIcon("buff_fire");
		g_ppIcons[RALLY_ROUND]	= gHUD.GetIcon("buff_rally_round");
		g_ppIcons[RETREAT]		= gHUD.GetIcon("buff_retreat");
		g_ppIcons[SLOW]			= gHUD.GetIcon("nerf_slow");
		g_ppIcons[NONE]			= gHUD.GetIcon("buff_empty");
	}

	static wchar_t g_aaBuffTexts[5][64];
	//RALLY_SPEED_RELOAD is an appoximation
	//normally while reloading we move at 50% of top speed.
	//with the buff, we move at 25% of top speed
	//this is good enough anyways, players get the message
	#define RALLY_SPEED_RELOAD_APPROX 25
	void InitializeTexts() {
		V_snwprintf(g_aaBuffTexts[0], 64, g_pVGuiLocalize->Find("#BG3_Effect_Advance"),
			(int)(RALLY_SPEED_MOD * 100 - 100), RALLY_SPEED_RELOAD_APPROX);
		V_snwprintf(g_aaBuffTexts[1], 64, g_pVGuiLocalize->Find("#BG3_Effect_Fire"),
			(int)(((1.0f - Sqr(RALLY_ACCURACY_MOD)) * 100)), (int)(RALLY_DAMAGE_MOD * 100 - 100));
		V_snwprintf(g_aaBuffTexts[2], 64, g_pVGuiLocalize->Find("#BG3_Effect_Rally"),
			(int)(RALLY_STAMINA_MOD * 100 - 100), (int)((1.0f - RALLY_ARMOR_MOD) * 100));
		V_snwprintf(g_aaBuffTexts[3], 64, g_pVGuiLocalize->Find("#BG3_Effect_Retreat"),
			(int)((1.0f - RALLY_ARMOR_MOD) * 100), RALLY_SPEED_RELOAD_APPROX);
		V_snwprintf(g_aaBuffTexts[4], 64, g_pVGuiLocalize->Find("#BG3_Effect_Slow"),
			(int)((1.0f - NERF_SLOW_MOD) * 100));
	}

	wchar_t* GetTextForBuff(RallyAsInt buff) {
		if (buff >= 0 && buff < NONE)
			return g_aaBuffTexts[buff];
		else
			return L"";
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

	float	GetRallyInterval(int rallyFlags) {
		float interval = RALLY_INTERVAL;
		if (rallyFlags == RALLY_RALLY_ROUND) interval = RALLY_ROUND_INTERVAL;
		return interval;
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
			break;
		case NERF_SLOW:
			duration = NERF_SLOW_DURATION;
			break;
		default:
			Assert("Failed to get duration of a rally flag!" && 0);
			break;
		}

		//Lengthen the time if we're in linebattle mode
		
		if (IsLinebattle() && rallyFlags != NERF_SLOW)
			duration *= RALLY_DURATION_LB_MOD;

		return duration;
	}

	//Checks that we're of the right class
	//@requires pPlayer is non-null
	bool	PlayersClassHasRallyAbility(const CHL2MP_Player* pPlayer) {
		ConVar * officerClass = pPlayer->GetTeamNumber() == TEAM_AMERICANS ? &lb_officer_classoverride_a : &lb_officer_classoverride_b;

		bool bAllow; 

		if (IsLinebattle()) {
			bAllow = pPlayer->GetClass() == officerClass->GetInt();
		} else {
			bAllow = pPlayer->GetPlayerClass()->m_bCanDoVcommBuffs;
		}
		return bAllow;
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

#ifndef CLIENT_DLL
CON_COMMAND_F(rallyme, "", FCVAR_CHEAT | FCVAR_HIDDEN) {
	CHL2MP_Player* pPlayer = ToHL2MPPlayer(UTIL_GetCommandClient());
	if (pPlayer) {
		BG3Buffs::RallyPlayer(NERF_SLOW, pPlayer);

		//Set end and next rally times
		ConVar* pcvEndTime = EndRallyTimeCvarFor(pPlayer->GetTeamNumber());
		pcvEndTime->SetValue(gpGlobals->curtime + BG3Buffs::GetRallyDuration(NERF_SLOW));
	}
}
#endif