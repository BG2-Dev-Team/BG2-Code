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
#include "team.h"
#include "hl2mp/hl2mp_player.h"
#include "bg3_voting_system.h"

using namespace std;

static CVotingSystem* g_pBritishVotingSystem;
static CVotingSystem* g_pAmericanVotingSystem;


ConVar sv_vote_duration("sv_vote_duration", "30", FCVAR_GAMEDLL, "This time, in seconds, is how long a vote will last.", true, 5.f, false, 0.f);
ConVar sv_vote_min_turnout("sv_vote_min_turnout", "0.4", FCVAR_GAMEDLL, "Minimum fraction of votes required for a vote to be valid", true, 0.f, true, 1.0f);


bool CVotingSystem::NewVoteAllowedForTeam(uint8 iTeam, bool bUniversal) {
	//If either one has a universal vote, cancel
	//also cancel if one has a vote and we're asking for a universal vote
	if (
		(g_pAmericanVotingSystem->HasActiveVote() && (bUniversal || g_pAmericanVotingSystem->m_bVoteIsUniversal))
		|| (g_pBritishVotingSystem->HasActiveVote() && (bUniversal || g_pBritishVotingSystem->m_bVoteIsUniversal))
		)
		return false;

	//otherwise check if voting system already has a vote
	return !GetVotingSystemForTeam(iTeam)->HasActiveVote();
}

CVotingSystem* CVotingSystem::GetVotingSystemForPlayer(CHL2MP_Player* pPlayer) {
	return GetVotingSystemForTeam(pPlayer->GetTeamNumber());
}

CVotingSystem* CVotingSystem::GetVotingSystemForTeam(uint8 iTeam) {
	CVotingSystem* pResult = NULL;
	if (iTeam == TEAM_AMERICANS) pResult = g_pAmericanVotingSystem;
	if (iTeam == TEAM_BRITISH) pResult = g_pBritishVotingSystem;
	return pResult;
}


bool CVotingSystem::CreateVote(
	bool universal,
	CHL2MP_Player* pRequester,
	CHL2MP_Player* pTarget,
	string sVoteHeader,
	vector<string>& options,
	void(*pResultCommand)(SResultCommandParameters*)) {

	//First check if we're allowed to create a vote
	if (!NewVoteAllowedForTeam(m_iTeamNumber, universal)) return false;

	m_bVoteIsUniversal = universal;

	m_resultCommandParameters.m_requester = pRequester;
	m_resultCommandParameters.m_target = pTarget;
	m_resultCommandParameters.m_options = options;
	
	m_pResultCommand = pResultCommand;

	m_flVoteEndTime = gpGlobals->curtime + sv_vote_duration.GetFloat();

	BroadcastVoteCreationToClients();
	return true;
}

bool CVotingSystem::AllPlayersVoted() const {
	if (m_bVoteIsUniversal) {
		return m_votes.size() == (size_t) (g_Teams[TEAM_AMERICANS]->GetNumPlayers() + g_Teams[TEAM_BRITISH]->GetNumPlayers());
	}
	else {
		return m_votes.size() == (size_t) g_Teams[m_iTeamNumber]->GetNumPlayers();
	}
}


bool CVotingSystem::PlayerCanVote(CHL2MP_Player* pPlayer, CSteamID& id) {
	if (!pPlayer
		|| !HasActiveVote()
		|| (!m_bVoteIsUniversal && pPlayer->GetTeamNumber() != m_iTeamNumber)
		|| (pPlayer->GetTeamNumber() < TEAM_AMERICANS)) return false;

	pPlayer->GetSteamID(&id);

	//now check that this player hasn't already voted
	bool bAlreadyVoted = false;
	for (size_t i = 0; i < m_votes.size() && !bAlreadyVoted; i++) {
		bAlreadyVoted = m_votes[i].m_votedID == id;
	}
	return !bAlreadyVoted;

}

void CVotingSystem::SubmitPlayerVote(CHL2MP_Player* pPlayer, uint8 iSlot) {
	//make sure no funny business is going on
	CSteamID id;
	if (!PlayerCanVote(pPlayer, id)) return;

	//don't allow out-of-bounds votes
	if (iSlot >= m_resultCommandParameters.m_options.size()) return;

	SVote vote {
		iSlot,
		id,
	};

	m_votes.push_back(vote);
	if (AllPlayersVoted())
		EndVote();
}

void CVotingSystem::CalculateResults(bool& resultValid, uint8& resultSlot) {
	int totalPlayers = g_Teams[TEAM_AMERICANS]->GetNumPlayers() + g_Teams[TEAM_BRITISH]->GetNumPlayers();
	resultValid = 1.0f * m_votes.size() / totalPlayers >= sv_vote_min_turnout.GetFloat();

	uint8 slotCounters[9];
	memset(slotCounters, 0, sizeof(slotCounters));
	for (size_t i = 0; i < m_votes.size(); i++) {
		slotCounters[m_votes[i].m_chosenSlot]++;
	}

	//now find which slot is max
	uint8 maxSlot = 0;
	uint8 maxSlotValue = 0;
	for (size_t i = 0; i < 9; i++) {
		if (slotCounters[i] > maxSlotValue) {
			maxSlot = i;
			maxSlotValue = slotCounters[i];
		}
	}
	resultSlot = maxSlot;
}

void CVotingSystem::EndVote() {
	//calculate results
	uint8 resultSlot;
	CalculateResults(m_resultCommandParameters.m_bResultValid, resultSlot);
	m_resultCommandParameters.m_iSlot = resultSlot;

	m_pResultCommand(&m_resultCommandParameters);

	BroadcastVoteEndToClients();
	ClearVote();
}

void CVotingSystem::ClearVote() {
	m_flVoteEndTime = -FLT_MAX;
	m_bVoteIsUniversal = false;

	m_resultCommandParameters.m_iSlot = 0;
	m_resultCommandParameters.m_options = vector<string>();
	m_resultCommandParameters.m_target = NULL;
	m_resultCommandParameters.m_requester = NULL;

	m_pResultCommand = NULL;
	m_votes = vector<SVote>();
}

void CVotingSystem::BroadcastVoteCreationToClients() {

}

void CVotingSystem::BroadcastVoteEndToClients() {
	for (int i = 0; i <= gpGlobals->maxClients; i++) {
		CHL2MP_Player* pPlayer = ToHL2MPPlayer(UTIL_PlayerByIndex(i));
		if (pPlayer && (m_bVoteIsUniversal || GetVotingSystemForPlayer(pPlayer) == this)) {
			
		}
	}
}

void CVotingSystem::Think() {
	//we could check if this is our next time to think,
	//but that would take as much performance time as actually thinking
	if (gpGlobals->curtime > m_flVoteEndTime)
		EndVote();
}


CVotingSystem::CVotingSystem(uint8 _iTeamNumer) {
	m_iTeamNumber = _iTeamNumer;

	ClearVote();
}