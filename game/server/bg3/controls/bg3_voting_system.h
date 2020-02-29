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
#include <vector>

class CHL2MP_Player;

//The voting will be made of two CVotingSystem objects, one for each team
class CVotingSystem {
private:
	uint8						m_iTeamNumber;

	//Data for the current issue up for vote
	struct SVote {
		uint8 m_chosenSlot;
		CSteamID m_votedID;
	};
	std::vector<SVote>			m_votes; //current votes being polled from players
	float						m_flVoteEndTime; //when the current vote ends
	bool						m_bVoteIsUniversal; //whether this vote is accessible to all non-spectators

	//These parameters are setup when a vote is created, such that when the vote is over 
	//the callback can have access to all data it needs
	struct SResultCommandParameters {
		bool						m_bResultValid;
		uint8						m_iSlot; //
		std::vector<std::string>	m_options; //list of options 
		CHandle<CHL2MP_Player>		m_target; //subject of the vote - who we trying to kick, mute etc.?
		CHandle<CHL2MP_Player>		m_requester; //how created the vote?
	} m_resultCommandParameters;
	void						(*m_pResultCommand)(SResultCommandParameters*); //function to call when a vote is finished
	
	static bool NewVoteAllowedForTeam(uint8 iTeam, bool bUniversal);
	static CVotingSystem* GetVotingSystemForPlayer(CHL2MP_Player*);
	static CVotingSystem* GetVotingSystemForTeam(uint8 iTeam);

	//Master vote creation function
	//Other vote creation functions just call this one
	bool CreateVote(
		bool universal,
		CHL2MP_Player* pRequester, 
		CHL2MP_Player* pTarget,
		std::string sVoteHeader, 
		std::vector<std::string>& options,
		void(*pResultCommand)(SResultCommandParameters*));

	inline bool HasActiveVote() const { return m_flVoteEndTime < gpGlobals->curtime; }

	bool AllPlayersVoted() const;

	bool PlayerCanVote(CHL2MP_Player* pPlayer, CSteamID& id);

	void SubmitPlayerVote(CHL2MP_Player* pPlayer, uint8 slot);

	void CalculateResults(bool& resultValid, uint8& resultSlot);

	void EndVote();

	void ClearVote();

	void BroadcastVoteCreationToClients();

	void BroadcastVoteEndToClients();

	void Think();

public:

	CVotingSystem(uint8 _iTeamNumber);

	void CreateKickVote(CHL2MP_Player* pRequester, const char* pszPlayerSelector, std::string sVoteHeader);


};

#endif //BG3_VOTING_SYSTEM_H