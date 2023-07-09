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
#include "../shared/bg3/bg3_map_model.h"
#include "bg3_voting_system.h"
#include "player_resource.h"
#include "../Permissions/bg3_player_locator.h"
#include "bg3_map_nomination.h"
#include "bg3_gungame.h"

CHL2MP_Player* GetSinglePlayerFromSelector(const char* pszSelector);

using namespace std;

static CElectionSystem g_AmericanVotingSystemActual(TEAM_AMERICANS);
static CElectionSystem g_BritishVotingSystemActual(TEAM_BRITISH);

CElectionSystem* g_pBritishVotingSystem = &g_BritishVotingSystemActual;
CElectionSystem* g_pAmericanVotingSystem = &g_AmericanVotingSystemActual;

extern char* sTeamNames[];



extern ConVar sv_vote_duration;
ConVar sv_vote_min_turnout("sv_vote_min_turnout", "0.4", FCVAR_GAMEDLL, "Minimum fraction of votes required for a vote to be valid", true, 0.f, true, 1.0f);

ConVar sv_vote_kick_enabled("sv_vote_kick_enabled", "1", FCVAR_GAMEDLL);
ConVar sv_vote_kick_ratio("sv_vote_kick_ratio", "0.8", FCVAR_GAMEDLL);
ConVar sv_vote_mute_enabled("sv_vote_mute_enabled", "1", FCVAR_GAMEDLL);
ConVar sv_vote_mute_ratio("sv_vote_mute_ratio", "0.7", FCVAR_GAMEDLL);
ConVar sv_vote_scramble_enabled("sv_vote_scramble_enabled", "1", FCVAR_GAMEDLL);
ConVar sv_vote_scramble_ratio("sv_vote_scramble_ratio", "0.7", FCVAR_GAMEDLL);
ConVar sv_vote_gungame_enabled("sv_vote_gungame_enabled", "1", FCVAR_GAMEDLL);
ConVar sv_vote_gungame_ratio("sv_vote_gungame_ratio", "0.7", FCVAR_GAMEDLL);
ConVar sv_vote_mapchange_enabled("sv_vote_mapchange_enabled", "1", FCVAR_GAMEDLL);
ConVar sv_vote_mapchange_ratio("sv_vote_mapchange_ratio", "0.6", FCVAR_GAMEDLL);

ConVar sv_vote_mapchoice_enabled("sv_vote_mapchoice_enabled", "1", FCVAR_GAMEDLL);
ConVar sv_vote_mapchoice_instant("sv_vote_mapchoice_instant", "0", FCVAR_GAMEDLL);
ConVar sv_vote_mapchoice_start_time("sv_vote_mapchoice_start_time", "120", FCVAR_GAMEDLL, "How much time in seconds before map end to show the automated voting menu");




bool CElectionSystem::NewElectionAllowedForTeam(uint8 iTeam, bool bUniversal) {
	//If either one has a universal vote, cancel
	//also cancel if one has a vote and we're asking for a universal vote
	if (
		g_fGameOver
		|| (g_pAmericanVotingSystem->HasActiveElection() && (bUniversal || g_pAmericanVotingSystem->m_bElectionIsUniversal))
		|| (g_pBritishVotingSystem->HasActiveElection() && (bUniversal || g_pBritishVotingSystem->m_bElectionIsUniversal))
		) {
		//CSay("%i %i %i", g_pAmericanVotingSystem->HasActiveElection(), bUniversal, g_pAmericanVotingSystem->m_bElectionIsUniversal);
		//CSay("%i %i %i", g_pBritishVotingSystem->HasActiveElection(), bUniversal, g_pBritishVotingSystem->m_bElectionIsUniversal);
		CSay("There is already an ongoing election.");
		return false;
	}
		

	//otherwise check if our election system already has a vote
	return !GetElectionSystemForTeam(iTeam)->HasActiveElection();
}

CElectionSystem* CElectionSystem::GetElectionSystemForPlayer(CHL2MP_Player* pPlayer) {
	return GetElectionSystemForTeam(pPlayer->GetTeamNumber());
}

CElectionSystem* CElectionSystem::GetElectionSystemForTeam(uint8 iTeam) {
	CElectionSystem* pResult = NULL;
	if (iTeam == TEAM_AMERICANS) pResult = g_pAmericanVotingSystem;
	if (iTeam == TEAM_BRITISH) pResult = g_pBritishVotingSystem;
	return pResult;
}

CElectionSystem* CElectionSystem::GetActiveElectionSystemForTeam(uint8 iTeam) {
	CElectionSystem* pResult = NULL;
	
	if (g_pAmericanVotingSystem->m_bElectionIsUniversal && g_pAmericanVotingSystem->HasActiveElection())
		pResult = g_pAmericanVotingSystem;
	else if (g_pBritishVotingSystem->m_bElectionIsUniversal && g_pBritishVotingSystem->HasActiveElection())
		pResult = g_pBritishVotingSystem;
	else
		pResult = GetElectionSystemForTeam(iTeam);

	return pResult;
}


bool CElectionSystem::AllPlayersVoted() const {
	int numBrit = g_pPlayerResource->GetNumBritish();
	int numAmer = g_pPlayerResource->GetNumAmericans();
	if (m_bElectionIsUniversal) {
		return (int)m_votes.size() == (numBrit + numAmer);
	}
	else {
		return (int)m_votes.size() == (m_iTeamNumber == TEAM_AMERICANS ? numAmer : numBrit);
	}
}


bool CElectionSystem::PlayerCanVote(CHL2MP_Player* pPlayer, CSteamID& id) {
	if (!pPlayer
		|| !HasActiveElection()
		|| (!m_bElectionIsUniversal && pPlayer->GetTeamNumber() != m_iTeamNumber)
		|| (pPlayer->GetTeamNumber() < TEAM_AMERICANS)) return false;

	pPlayer->GetSteamID(&id);

	//now check that this player hasn't already voted
	bool bAlreadyVoted = false;
	for (size_t i = 0; i < m_votes.size() && !bAlreadyVoted; i++) {
		bAlreadyVoted = m_votes[i].m_votedID == id;
	}
	return !bAlreadyVoted;
}

void CElectionSystem::SubmitPlayerVote(CHL2MP_Player* pPlayer, uint8 iSlot) {
	//make sure no funny business is going on
	CSteamID id;
	//the check below also acquires the steam id for us
	if (!PlayerCanVote(pPlayer, id)) return;

	//don't allow out-of-bounds votes
	if (iSlot >= m_resultCommandParameters.m_options.size()) return;

	SVote vote {
		iSlot,
		id,
	};

	m_votes.push_back(vote);

	BroadcastElectionTallyToClients(iSlot); //inform players of the vote received

	if (AllPlayersVoted())
		FinishElection();
}

void CElectionSystem::CalculateResults(bool& resultValid, uint8& resultSlot, std::vector<int>& resultCounts) {
	int totalPlayers = g_pPlayerResource->GetNumNonSpectators();
	resultValid = (1.0f * m_votes.size() / totalPlayers) >= sv_vote_min_turnout.GetFloat();

	int numOptions = m_resultCommandParameters.m_options.size();
	

	resultCounts = vector<int>();
	for (int i = 0; i < numOptions; i++) {
		resultCounts.emplace_back(0);
	}

	for (size_t i = 0; i < m_votes.size(); i++) {
		resultCounts[m_votes[i].m_chosenSlot]++;
	}

	//now find which slot is max
	uint8 maxSlot = 0;
	uint8 maxSlotValue = 0;
	for (int i = 0; i < numOptions; i++) {
		if (resultCounts[i] > maxSlotValue) {
			maxSlot = i;
			maxSlotValue = resultCounts[i];
		}
	}
	resultSlot = maxSlot;
}

bool CElectionSystem::CreateElection(
	bool universal,
	CHL2MP_Player* pRequester,
	CHL2MP_Player* pTarget, //for kick/mute
	std::string sMapName, //map name to switch to, if applicable, assumed to be valid
	const char* pszVoteHeader, //vote title that pops up
	std::vector<std::string>& options,
	EVoteMessageType creationType,
	ConVar* pThresholdVar,
	void(*pResultCommand)(SResultCommandParameters*)) {

	if (pRequester && pRequester->GetTeamNumber() < TEAM_AMERICANS)
		return false;

	//First check if we're allowed to create a vote
	if (!NewElectionAllowedForTeam(m_iTeamNumber, universal)) {
		return false;
	}

	//ensure enough time has passed after map start
	if (HL2MPRules()->GetMapElapsedTime() < 60) {
		if (pRequester) CSayPlayer(pRequester, "Not enough time has passed for new vote, wait a bit then try again!");
		return false;
	}

	//ensure we have enough time for the vote
	if (sv_vote_duration.GetFloat() + 5 > HL2MPRules()->GetMapRemainingTime()) {
		CSay("Not enough time for vote");
		return false;
	}

	m_bElectionIsUniversal = universal;
	m_flElectionEndTime = gpGlobals->curtime + sv_vote_duration.GetFloat();
	m_bHasActiveElection = true;

	//save data for the results later
	m_resultCommandParameters.m_requester = pRequester;
	m_resultCommandParameters.m_target = pTarget;
	m_resultCommandParameters.m_options = options; //copying by value to simplify earlier memory management
	m_resultCommandParameters.m_sMapName = sMapName;
	m_pResultCommand = pResultCommand;

	//CSay("Creating election with %u options\n", m_resultCommandParameters.m_options.size());
	

	BroadcastElectionCreationToClients(creationType, pszVoteHeader, pThresholdVar);
	return true;
}

void CElectionSystem::FinishElection() {
	CSay("Finishing election...");

	//calculate results
	uint8 resultSlot;
	CalculateResults(m_resultCommandParameters.m_bResultValid, resultSlot, m_resultCommandParameters.m_optionCounts);
	m_resultCommandParameters.m_iSlot = resultSlot;

	m_pResultCommand(&m_resultCommandParameters);

	BroadcastElectionEndToClients();
	ClearElection();
}

void CElectionSystem::ClearElection() {
	m_flElectionEndTime = FLT_MAX;
	m_bElectionIsUniversal = false;
	m_bHasActiveElection = false;
	//m_bRtvComplete = false;

	m_resultCommandParameters.m_iSlot = 0;
	m_resultCommandParameters.m_options = vector<string>();
	m_resultCommandParameters.m_target = NULL;
	m_resultCommandParameters.m_requester = NULL;

	m_pResultCommand = NULL;
	m_votes = vector<SVote>();
}

void CElectionSystem::BroadcastElectionCreationToClients(EVoteMessageType type, const char* pszHeader, ConVar* pThresholdVar) {
	//determine min vote percentage for yes/no elections
	uint8 minVotePercentage = 0;
	if (pThresholdVar) {
		minVotePercentage = (uint8)(pThresholdVar->GetFloat() * 100);
	}


	//decide filter type based on whether we are global or just for our team rn
	CRecipientFilter filter;
	GetElectionFilter(filter);

	//customize message based on vote type
	UserMessageBegin(filter, "BG3Election");
	WRITE_BYTE((int)type); //start with type so client knows how to interpret the rest of the message
	if (type == EVoteMessageType::TOPIC || type == EVoteMessageType::TOPIC_MAPCHOICE) {
		//generic vote with customizable options
		//first send the number of options, then the topic/question text
		WRITE_BYTE(m_resultCommandParameters.m_options.size());
		WRITE_STRING(pszHeader);
		MessageEnd();

		//options sent separately due to 255 byte payload limit
		BroadcastElectionOptionsToClients(filter);
	}
	else if (type == EVoteMessageType::TOPIC_BINARY) {
		WRITE_BYTE(minVotePercentage);
		WRITE_STRING(pszHeader);
		MessageEnd();

		//let everyone know in chat too
		/*if (m_bElectionIsUniversal) {
			CSay("Beginning vote for %s", pszHeader);
		}*/
	}
	else if (type == EVoteMessageType::TOPIC_PLAYER) {
		WRITE_BYTE(minVotePercentage);
		WRITE_BYTE(m_resultCommandParameters.m_target.Get()->GetClientIndex() + 1);
		WRITE_STRING(pszHeader);
		MessageEnd();
	}
	else if (type == EVoteMessageType::TOPIC_MAP) {
		WRITE_BYTE(minVotePercentage);
		//CSay("Beginning vote for map change.");
		WRITE_STRING(m_resultCommandParameters.m_sMapName.c_str());
		MessageEnd();
	}
}

void CElectionSystem::BroadcastElectionOptionsToClients(CRecipientFilter& filter) {
	vector<string>& opts = m_resultCommandParameters.m_options;

	//send all the options, let client sort them out
	for (size_t i = 0; i < opts.size(); i++) {
		UserMessageBegin(filter, "BG3Election");
		WRITE_BYTE((int)EVoteMessageType::SEND_OPTION); //inform we're sending an option
		WRITE_BYTE(i); //inform which option we're sending
		WRITE_STRING(opts[i].c_str()); //the text of the option itself
		MessageEnd();
	}
}

void CElectionSystem::BroadcastElectionCancellationToClients() {
	CSay("The vote has been cancelled.");

	CRecipientFilter filter;
	GetElectionFilter(filter);
	UserMessageBegin(filter, "BG3Election");
	WRITE_BYTE((int)EVoteMessageType::CANCEL);
	MessageEnd();
}

void CElectionSystem::BroadcastElectionTallyToClients(uint8 slot) {
	CRecipientFilter filter;
	GetElectionFilter(filter);
	UserMessageBegin(filter, "BG3Election");
	WRITE_BYTE((int)EVoteMessageType::VOTE_TALLY);
	WRITE_BYTE(slot); //inform players which slots other players vote for
	MessageEnd();
}

void CElectionSystem::BroadcastElectionEndToClients() {
	CRecipientFilter filter;
	GetElectionFilter(filter);
	UserMessageBegin(filter, "BG3Election");
	WRITE_BYTE((int)EVoteMessageType::RESULT);
	WRITE_BYTE(m_resultCommandParameters.m_iSlot); //inform players which slots won
	MessageEnd();
}

void CElectionSystem::GetElectionFilter(CRecipientFilter& filter) {
	if (m_bElectionIsUniversal) {
		filter.AddAllPlayers();
	}
	else {
		filter.AddRecipientsByTeam(g_Teams[m_iTeamNumber]);
	}
	filter.MakeReliable();
}

float GetBinaryResultsRatio(std::vector<int>& optionCounts) {
	if (optionCounts.size() < 2) {
		CSay("Error calculating binary results - less than 2 options");
		return 0.f;
	}

	int yes = optionCounts[0];
	int no = optionCounts[1];
	int total = yes + no;
	return 1.f * yes / total;
}

void CElectionSystem::ElectionThink() {
	if (gpGlobals->curtime > m_flElectionEndTime) //not really precise timing here but that doesn't matter
		FinishElection();
}

void CElectionSystem::AutoMapchoiceElectionThink() {
	extern ConVar nextlevel;

	if (!m_bHasActiveElection 
		&& !mp_competitive.GetBool() 
		&& sv_vote_mapchoice_enabled.GetBool() 
		&& !nextlevel.GetString()[0]) {
		bool create = false;

		if (m_bRtvComplete) {
			create = true;
		}
		else if (mp_rounds.GetBool() && HL2MPRules()->m_iCurrentRound == mp_rounds.GetInt()) {
			create = true;
		}
		else {
			float remain = HL2MPRules()->GetMapRemainingTime();
			create = remain < sv_vote_mapchoice_start_time.GetFloat();
		}

		if (create) {
			vector<string> randomOptions;
			NMapNomination::GetRandomPublicMapList(randomOptions);
			CreateMultichoiceMapElection(NULL, randomOptions);
		}
	}
}


CElectionSystem::CElectionSystem(uint8 _iTeamNumer) {
	m_iTeamNumber = _iTeamNumer;

	ClearElection();
}

static std::vector<std::string> g_booleanElectionOptions = { "Yes", "No" };


//----------------------------------------------------------------------------
// Individual vote types
//----------------------------------------------------------------------------
void CElectionSystem::CreateKickElection(CHL2MP_Player* pRequester, const char* pszPlayerSelector) {
	if (!sv_vote_kick_enabled.GetBool()) {
		if (pRequester) CSayPlayer(pRequester, "Votekick disabled on server.");
		return;
	}

	//only allow actions for players on your own team
	CHL2MP_Player* pTarget = GetSinglePlayerFromSelector(pszPlayerSelector);
	if (!pRequester || !pTarget || pRequester->GetTeamNumber() != pTarget->GetTeamNumber()) {
		if (pRequester) CSayPlayer(pRequester, "Player not found or not on your team.");
		return;
	}

	CSay("%s wants to votekick %s", pRequester->GetPlayerName(), pszPlayerSelector);


	if (pTarget) {
		CreateElection(
			false, //not universal;
			pRequester, //who is requesting the vote
			pTarget, //targeted player
			"", //map name, if applicable
			"#BG3_Vote_Create_Kick", //header message sent to client
			g_booleanElectionOptions, //options
			EVoteMessageType::TOPIC_PLAYER, //type of creation message to send to client
			&sv_vote_kick_ratio,
			[](SResultCommandParameters* pResults) {
				//verify that target player is still on server
				if (!pResults->m_target)
					return;

				if (GetBinaryResultsRatio(pResults->m_optionCounts) >= sv_vote_kick_ratio.GetFloat()) {
					char buffer[MAX_PLAYER_NAME_LENGTH + 8];
					Q_snprintf(buffer, sizeof(buffer), "kick %s\n", pResults->m_target->GetPlayerName());
					engine->ServerCommand(buffer);
				}
				else {
					CSay("Kick vote failed to pass");
				}
			}
		);
	}
}

extern ConVar sv_alltalk;
extern ConVar mp_competitive;
void CElectionSystem::CreateMuteElection(CHL2MP_Player* pRequester, const char* pszPlayerSelector) {
	
	if (!sv_vote_mute_enabled.GetBool()) {
		if (pRequester) CSayPlayer(pRequester, "Votemute disabled on server.");
		return;
	}
	
	CHL2MP_Player* pTarget = GetSinglePlayerFromSelector(pszPlayerSelector);
	if (!pTarget) {
		if (pRequester) CSayPlayer(pRequester, "Player not found");
		return;
	}

	CSay("%s wants to votemute %s", pRequester->GetPlayerName(), pszPlayerSelector);

	if (pTarget) {
		CreateElection(
			!(mp_competitive.GetBool() && !sv_alltalk.GetBool()), //universal most of the time
			pRequester, //who is requesting the vote
			pTarget, //targeted player
			"", //map name, if applicable
			"#BG3_Vote_Create_Mute", //header message sent to client
			g_booleanElectionOptions, //options
			EVoteMessageType::TOPIC_PLAYER, //type of creation message to send to client
			&sv_vote_mute_ratio,
			[](SResultCommandParameters* pResults) {
				//verify that target player is still on server
				if (!pResults->m_target)
					return;

				if (GetBinaryResultsRatio(pResults->m_optionCounts) >= sv_vote_mute_ratio.GetFloat()) {
					char buffer[MAX_PLAYER_NAME_LENGTH + 8];
					Q_snprintf(buffer, sizeof(buffer), "silence %s\n", pResults->m_target->GetPlayerName());
					engine->ServerCommand(buffer);
				}
				else {
					CSay("Mute vote failed to pass");
				}
			}
		);
	}
}

void ScrambleTeams();
void CElectionSystem::CreateScrambleTeamsElection(CHL2MP_Player* pRequester) {
	CSay("%s wants to scramble the teams", pRequester->GetPlayerName());

	if (!sv_vote_scramble_enabled.GetBool()) {
		CSay("Votescramble disabled on server.");
		return;
	}

	CreateElection(
		true, //universal
		pRequester, //who is requesting the vote
		NULL, //targeted player
		"", //map name, if applicable
		"#BG3_Vote_Create_Scramble", //header message sent to client
		g_booleanElectionOptions, //options
		EVoteMessageType::TOPIC_BINARY, //type of creation message to send to client
		&sv_vote_scramble_ratio,
		[](SResultCommandParameters* pResults) {
		
			// 2/3 of yeses required
			if (GetBinaryResultsRatio(pResults->m_optionCounts) >= sv_vote_scramble_ratio.GetFloat()) {
				ScrambleTeams();
			}
			else {
				CSay("Scramble vote failed to pass");
			}
		}
	);
}

void CElectionSystem::CreateGunGameElection(CHL2MP_Player* pRequester) {
	CSay("%s wants to turn on gungame mode", pRequester->GetPlayerName());

	if (!sv_vote_gungame_enabled.GetBool()) {
		CSay("Gungame disabled on server.");
		return;
	}

	CreateElection(
		true, //universal
		pRequester, //who is requesting the vote
		NULL, //targeted player
		"", //map name, if applicable
		"Launch gun game mode?", //header message sent to client
		g_booleanElectionOptions, //options
		EVoteMessageType::TOPIC_BINARY, //type of creation message to send to client
		&sv_vote_gungame_ratio,
		[](SResultCommandParameters* pResults) {

			// 2/3 of yeses required
			if (GetBinaryResultsRatio(pResults->m_optionCounts) >= sv_vote_gungame_ratio.GetFloat()) {
				MSay("Gungame vote succeeded!");
				engine->ServerCommand("gungame\n");
				engine->ServerExecute();
			}
			else {
				CSay("Gungame vote failed to pass");
			}
		}
	);
}

void CElectionSystem::CreateMapChangeElection(CHL2MP_Player* pRequester, const char* pszMapName) {
	CSay("%s wants to change the map to %s", pRequester->GetPlayerName(), pszMapName);
	if (!sv_vote_mapchange_enabled.GetBool()) {
		CSayPlayer(pRequester, "Votemap disabled on server.");
		return;
	}

	//ensure enough time has passed after map start
	if (HL2MPRules()->GetMapElapsedTime() < 240) {
		if (pRequester) CSayPlayer(pRequester, "Not enough time has passed for a map vote, wait a bit!");
		return;
	}

	//verify that the map exists
	if (!CMapInfo::MapExists(pszMapName)) {
		CSayPlayer(pRequester, "The map %s does not exist", pszMapName);
		return;
	}

	if (!NMapNomination::MapIsNominatable(pszMapName)) {
		CSayPlayer(pRequester, "The map %s exists but is not available for voting", pszMapName);
		return;
	}

	CreateElection(
		true, //universal
		pRequester, //who is requesting the vote
		NULL, //targeted player
		pszMapName, //map name, if applicable
		"#BG3_Vote_Create_ChangeMap", //header message sent to client
		g_booleanElectionOptions, //options
		EVoteMessageType::TOPIC_MAP, //type of creation message to send to client
		&sv_vote_mapchange_ratio,
		[](SResultCommandParameters* pResults) {

			if (GetBinaryResultsRatio(pResults->m_optionCounts) >= sv_vote_mapchange_ratio.GetFloat()) {
				nextlevel.SetValue(pResults->m_sMapName.c_str());
				HL2MPRules()->ChangeMapDelayed(5.f);
			}
			else {
				CSay("Map vote failed to pass");
			}
		}
	);
}

void CElectionSystem::CreateActionlessMultichoiceElection(CHL2MP_Player* pRequester, const CCommand& args) {
	
	//create vector of options from args
	std::vector<std::string> options;

	//the first option is the third arg so start there
	for (int i = 2; i < args.ArgC(); i++) {
		options.push_back(args[i]);
	}

	CreateElection(
		true, //universal
		pRequester, //who is requesting the vote
		NULL, //targeted player
		"", //map name, if applicable
		args[1], //header message sent to client
		options, //options
		EVoteMessageType::TOPIC, //type of creation message to send to client
		NULL,
		[](SResultCommandParameters* pResults) {
		//nothing to do, this is actionless vote to inform admins of player's views on a custom election
		}
	);
}

void CElectionSystem::CreateMultichoiceMapElection(CHL2MP_Player* pRequester, const CCommand& args) {

	//create vector of options from args
	std::vector<std::string> options;

	if (args.ArgC() - 1 > MAX_NUM_MULTICHOICE_MAP_OPTIONS) {
		CSay("Map vote is limited to " V_STRINGIFY(MAX_NUM_MULTICHOICE_MAP_OPTIONS) " options");
	}

	//the first option is the second arg so start there
	for (int i = 1; i < args.ArgC() && i < MAX_NUM_MULTICHOICE_MAP_OPTIONS + 1; i++) {
		options.push_back(args[i]);
	}

	CreateMultichoiceMapElection(pRequester, options);
}

void CElectionSystem::CancelAllElections() {
	//cancel any ongoing elections
	if (g_pAmericanVotingSystem->HasActiveElection()) {
		g_pAmericanVotingSystem->CancelElection();
	}
	if (g_pBritishVotingSystem->HasActiveElection()) {
		g_pBritishVotingSystem->CancelElection();
	}
}

void CElectionSystem::CreateMultichoiceMapElection(CHL2MP_Player* pRequester, std::vector<std::string>& options) {
	if (options.size() > MAX_NUM_MULTICHOICE_MAP_OPTIONS) {
		CSay("Map vote is limited to " V_STRINGIFY(MAX_NUM_MULTICHOICE_MAP_OPTIONS) " options");
		return;
	}

	for (size_t i = 0; i < options.size() && i < MAX_NUM_MULTICHOICE_MAP_OPTIONS; i++) {

		//verify that the map exists
		if (!CMapInfo::MapExists(options[i].c_str())) {
			CSay("The map %s does not exist", options[i].c_str());
			return;
		}
	}

	//customize function based on instantaneous setting
	void(*mapResultFunc)(SResultCommandParameters*);
	if (sv_vote_mapchoice_instant.GetBool() || m_bRtvComplete) {
		m_bRtvComplete = false;
		mapResultFunc = [](SResultCommandParameters* pResults) {
			nextlevel.SetValue(pResults->m_options[pResults->m_iSlot].c_str());
			engine->ServerCommand(((string) "sm_nextmap " + pResults->m_options[pResults->m_iSlot]).c_str());
			HL2MPRules()->ChangeMapDelayed(5.f);
		};
	}
	else {
		mapResultFunc = [](SResultCommandParameters* pResults) {
			nextlevel.SetValue(pResults->m_options[pResults->m_iSlot].c_str());
			engine->ServerCommand(((string) "sm_nextmap " + pResults->m_options[pResults->m_iSlot]).c_str());
		};
	}

	CreateElection(
		true, //universal
		pRequester, //who is requesting the vote
		NULL, //targeted player
		"", //map name, if applicable
		"#BG3_Vote_Create_MapChoice", //header message sent to client
		options, //options
		EVoteMessageType::TOPIC_MAPCHOICE, //type of creation message to send to client
		NULL,
		mapResultFunc
		);
}

CON_COMMAND_F(votecancel, "Cancels the ongoing vote", FCVAR_GAMEDLL) {
	if (verifyMapModePermissions(__FUNCTION__)) {
		CBasePlayer* requester = UTIL_GetCommandClient();
		if (requester) {
			CElectionSystem* sys = CElectionSystem::GetActiveElectionSystemForTeam(requester->GetTeamNumber());
			if (sys && sys->HasActiveElection()) {
				sys->CancelElection();
				CSay("%s cancelled an ongoing vote", requester->GetPlayerName());
			}
			else {
				CSayPlayer((CHL2MP_Player*)requester, "There is no election to cancel");
			}
		}
		else {
			CElectionSystem::CancelAllElections();
		}
	}
}