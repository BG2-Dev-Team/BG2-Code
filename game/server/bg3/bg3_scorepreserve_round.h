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

namespace NScorePreserveRound {


	//---------------------------------------------------------------------
	// Purpose: Stores and remembers the TEAMS' total scores AND the
	//				score, damage count, team, class number
	//			of all players currently playing the game.
	//			Spectators are ignored and none of their data is stored.
	//			This list of scores is put into a list such that
	//			it can be retrieved later.
	//			
	//			ALSO, UNDER THESE CONDITIONS:
	//				-we have data for rounds whose round number is greater
	//					than or equal to the given round number
	//				-we did not just restore scores to the given round
	//					number with RestoreRoundScore(...)
	//			THE DATA FOR ROUNDS > iRound WILL BE CLEARED
	//			(this is such that new future round data won't conflict with
	//				the old future round data...yeah... but we can
	//				still go back to even earlier rounds)
	//
	//	@param iRound - the round number we're preserving - index starts
	//					at 1 AND NOT AT ZERO
	//---------------------------------------------------------------------
	void PreserveRoundScore(uint8 iRound);

	//---------------------------------------------------------------------
	// Purpose: Restores the TEAMS' total scores AND the
	//				score, damage count, team, class number
	//			of all players currently playing the game
	//			to that of the given round number.
	//	
	//	@param iRound - the round number to restore to - index STARTS AT 1
	//			and NOT at 0. The first round hence corresponds with index
	//			1, etc...
	//	@ensures - current players have the
	//				score, damage count, team, class number
	//			  that they had at the beginning of the given round.
	//			 - the TEAMs' score total scores are restored to that of the
	//			 - the given round.
	//			 - POLICY FOR PLAYERS WHO WEREN"T 
	//				PLAYING DURING THE GIVEN ROUND:
	//					-set scores to 0.
	//					-change their team if the majority of their teammates
	//						are switching teams.
	//					-if they switch teams, preserve the CLASS NUMBER
	//						they currently have.
	//			 - this function does NOT change the CPlayerClass of each
	//				player and does NOT respawn players - let GameRules
	//				handle that.
	//	@return - whether or not the MAJORITY of players switched teams
	//---------------------------------------------------------------------
	bool RestoreRoundScore(uint8 iRound) {
		return true;
	}

	
	//---------------------------------------------------------------------
	// Purpose: Removes all round score data from memory.
	//			This is called when the match restarts
	//			and when a new map starts.
	//			This way, data from previous matches won't conflict with
	//			the current match.
	//---------------------------------------------------------------------
	void FlushRoundScoreData();

}
