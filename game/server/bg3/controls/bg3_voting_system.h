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
#ifndef BG3_VOTING_SYSTEM_H
#define BG3_VOTING_SYSTEM_H

#include "cbase.h"
#include "../shared/bg3/controls/bg3_voting_system_shared.h"
#include <vector>

class CHL2MP_Player;

//The voting will be made of two CElectionSystem objects, one for each team
class CElectionSystem {
private:
	uint8						m_iTeamNumber;

	//Data for the current issue up for vote
	struct SVote {
		uint8 m_chosenSlot;
		CSteamID m_votedID;
	};
	std::vector<SVote>			m_votes; //current votes being polled from players
	float						m_flElectionEndTime; //when the current Election ends
	bool						m_bElectionIsUniversal; //whether this Election is accessible to all non-spectators
	bool						m_bHasActiveElection;
	bool						m_bRtvComplete; //used as a flag to start a map election from RTV

	//These parameters are setup when a Election is created, such that when the Election is over 
	//the callback can have access to all data it needs
	struct SResultCommandParameters {
		bool						m_bResultValid;
		uint8						m_iSlot; //winning slot
		std::vector<std::string>	m_options; //list of options 
		std::vector<int>			m_optionCounts; //list of counts for each option
		CHandle<CHL2MP_Player>		m_target; //subject of the Election - who we trying to kick, mute etc.?
		CHandle<CHL2MP_Player>		m_requester; //who created the Election?
		std::string					m_sMapName; //map name to switch to, if applicable
		std::string					m_sVoteHeader; //vote title that pops up
	} m_resultCommandParameters;

	void						(*m_pResultCommand)(SResultCommandParameters*); //function to call when a Election is finished
	

	static bool NewElectionAllowedForTeam(uint8 iTeam, bool bUniversal);
	
public:
	static CElectionSystem* GetElectionSystemForPlayer(CHL2MP_Player*);
	static CElectionSystem* GetElectionSystemForTeam(uint8 iTeam); //ignores universal votes
	static CElectionSystem* GetActiveElectionSystemForTeam(uint8 iTeam); //does not ignore universal votes
	
	inline bool HasActiveElection() const { return m_bHasActiveElection; }

private:

	bool AllPlayersVoted() const;

	bool PlayerCanVote(CHL2MP_Player* pPlayer, CSteamID& id);

public:
	void SubmitPlayerVote(CHL2MP_Player* pPlayer, uint8 slot);
private:

	void CalculateResults(bool& resultValid, uint8& resultSlot, std::vector<int>& resultCounts);

	//Master Election creation function
	//Other Election creation functions just call this one
	bool CreateElection(
		bool universal,
		CHL2MP_Player* pRequester,
		CHL2MP_Player* pTarget, //for kick/mute
		std::string sMapName, //map name to switch to, if applicable, assumed to be valid
		const char* pszHeader, //election title that pops up
		std::vector<std::string>& options,
		EVoteMessageType creationType,
		ConVar* pThresholdVar,
		void(*pResultCommand)(SResultCommandParameters*));

	void FinishElection(); //executes the results of the election and calls ClearElection()

public:
	void CancelElection() { 
		CSay("The vote has been cancelled");
		BroadcastElectionCancellationToClients(); 
		ClearElection(); 
	}
private:

	void ClearElection(); //resets the election system to a electionless state, ready for a new vote

	void BroadcastElectionCreationToClients(EVoteMessageType type, const char* pszHeader, ConVar* pThresholdVar);

	void BroadcastElectionOptionsToClients(CRecipientFilter& filter);

	void BroadcastElectionCancellationToClients();

	void BroadcastElectionTallyToClients(uint8 slot); //called when a tally is made, so that players know the results realtime as they come in

	void BroadcastElectionEndToClients();

	void GetElectionFilter(CRecipientFilter& filter);

public:

	CElectionSystem(uint8 _iTeamNumber);

	void NotifyPlayerDisconnected(CHL2MP_Player* pPlayer); //votes targeting players need to be cancelled if the player leaves

	void ElectionThink();

	void AutoMapchoiceElectionThink();

	void CreateKickElection(CHL2MP_Player* pRequester, const char* pszPlayerSelector);

	void CreateMuteElection(CHL2MP_Player* pRequester, const char* pszPlayerSelector);

	void CreateScrambleTeamsElection(CHL2MP_Player* pRequester);

	void CreateGunGameElection(CHL2MP_Player* pRequester);

	void CreateMapChangeElection(CHL2MP_Player* pRequester, const char* pszMapName);

	void CreateActionlessMultichoiceElection(CHL2MP_Player* pRequester, const CCommand& args);

	void CreateMultichoiceMapElection(CHL2MP_Player* pRequester, const CCommand& args);
	void CreateMultichoiceMapElection(CHL2MP_Player* pRequester, std::vector<std::string>& options);

	inline void FlagForRtvMapElection(bool flag) { m_bRtvComplete = flag; }

	static void CancelAllElections();
};

extern CElectionSystem* g_pBritishVotingSystem;
extern CElectionSystem* g_pAmericanVotingSystem;

#endif //BG3_VOTING_SYSTEM_H