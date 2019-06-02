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

#ifndef BG3_HUD_DAMAGE_ACCUMULATOR_H
#define BG3_HUD_DAMAGE_ACCUMULATOR_H
#include "cbase.h"
#include <sstream>
#include <set>

//BG2 - Tjoppen - used for accumulated damage cauased by or inflicted on the player, over a short period of time (buckshot)
#define ACCUMULATE_LIMIT	0.5	//accumulate damages for this long. should be enough for all shot to hit



class CDamagePlayerAccumulator
{

public:

	bool m_bIsVictim;			//governs what kind of message is produced
	float m_flLastAttack;
	int m_iTotalDamage;
	int m_iHitgroup;			//only used if only one player was hit
	std::set<int> m_sPlayers;

	CDamagePlayerAccumulator(bool isVictim);
	std::string GetPlayerName(int player) const;

	//accumulates the specified damage and returns a proper message
	std::string Accumulate(int damage, int player, int hitgroup);
};

#endif //BG3_HUD_DAMAGE_ACCUMULATOR_H