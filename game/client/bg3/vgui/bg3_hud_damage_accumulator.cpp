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
#include "bg3_hud_damage_accumulator.h"

static const char* HitgroupName(int hitgroup)
{
	//BG2 - Tjoppen - TODO: localize
	switch (hitgroup)
	{
	case HITGROUP_GENERIC:
		return NULL;
	case HITGROUP_HEAD:
		return "head";
	case HITGROUP_CHEST:
		return "chest";
	case HITGROUP_STOMACH:
		return "stomach";
	case HITGROUP_LEFTARM:
		return "left arm";
	case HITGROUP_RIGHTARM:
		return "right arm";
	case HITGROUP_LEFTLEG:
		return "left leg";
	case HITGROUP_RIGHTLEG:
		return "right leg";
	default:
		return "unknown default case";
	}
}

CDamagePlayerAccumulator::CDamagePlayerAccumulator(bool isVictim)
{
	m_bIsVictim = isVictim;
	m_flLastAttack = 0;
	m_iTotalDamage = 0;
}

std::string CDamagePlayerAccumulator::GetPlayerName(int player) const
{
	player_info_t info;

	if (engine->GetPlayerInfo(player, &info))
		return info.name;
	else
		return "";
}

//accumulates the specified damage and returns a proper message
std::string CDamagePlayerAccumulator::Accumulate(int damage, int player, int hitgroup)
{
	if (m_flLastAttack + ACCUMULATE_LIMIT > gpGlobals->curtime)
	{
		//accumulate
		m_iTotalDamage += damage;
		m_sPlayers.insert(player);

		//only override hitgroup if it's a headshot
		if (hitgroup == HITGROUP_HEAD)
			m_iHitgroup = hitgroup;
	}
	else
	{
		//reset
		m_flLastAttack = gpGlobals->curtime;
		m_iTotalDamage = damage;
		m_iHitgroup = hitgroup;
		m_sPlayers.clear();
		m_sPlayers.insert(player);
	}

	/**
	Generate hit verification message. We have five cases:
	* we're the victim and were hit by one attacker in no particular hitgroup
	* we're the victim and were hit by one attacker in some hitgroup (head, chest etc.)
	* we're the attacker and hit one victim in no particular hitgroup
	* we're the attacker and hit one victim in some hitgroup
	* we're the attacker and hit multiple victims

	This means we don't care about being hit by multiple attackers (just print the first one)
	or things like headshoting multiple victims
	*/
	//use m_iHitgroup instead of hitgroup so headshots show even though hitgroup might be something else
	const char *hitgroupname = HitgroupName(m_iHitgroup);
	std::ostringstream oss;

	//TODO: localize
	if (m_bIsVictim)
	{
		oss << "You were hit";

		if (hitgroupname)
		{
			//specific hitgroup
			oss << " in the " << hitgroupname;
		}

		oss << " by " << GetPlayerName(player);
	}
	else
	{
		oss << "You hit ";

		if (m_sPlayers.size() == 1)
		{
			//single victim
			oss << GetPlayerName(player);

			if (hitgroupname)
			{
				//specific hitgroup
				oss << " in the " << hitgroupname;
			}
		}
		else
		{
			//multiple victims
			//desired format: "player1[, player2[, player3...]] and playerN"
			size_t numLeft = m_sPlayers.size() - 1;

			for (std::set<int>::iterator it = m_sPlayers.begin(); it != m_sPlayers.end(); it++, numLeft--)
			{
				oss << GetPlayerName(*it);

				if (numLeft > 1)
					oss << ", ";
				else if (numLeft > 0)
					oss << " and ";
			}
		}
	}

	oss << " for " << m_iTotalDamage << " points of damage";

	return oss.str();
}

