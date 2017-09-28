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
/*
BG3 - A lot of this was declared and defined in CHL2MP_Player but seeing that the client needs some of it too
I'm moving a lot of the related functions to here - Awesome
*/
#ifndef BG3_BUFFS_H
#define BG3_BUFFS_H

#include "cbase.h"

#ifdef CLIENT_DLL
#include "../../client/hl2mp/c_hl2mp_player.h"
#else
#include "../../server/hl2mp/hl2mp_player.h"
#endif //CLIENT_DLL

namespace BG3Buffs {
#ifndef CLIENT_DLL
	bool	RallyRequest(int vcommCommand, CHL2MP_Player* pRequester);

	void	RallyPlayer(int rallyFlags, CHL2MP_Player* pPlayer);

	void	SetNextRallyTime(int iTeam, float flTime); //For when round restarts etc.
#else
	CHudTexture* RallyIconFrom(int rallyFlags);

#endif //CLIENT_DLL
	float	GetEndRallyTime(int iTeam);
	float	GetNextRallyTime(int iTeam);


	inline float GetTimeUntilNextRally(int iTeam) {
		float tTime = GetNextRallyTime(iTeam) - gpGlobals->curtime;
		return MAX(0.0, tTime);
	}

	float	GetRallyDuration(int rallyFlags);

	bool	PlayersClassHasRallyAbility(const CHL2MP_Player* pPlayer); //Checks that we're of the right class
	bool	PlayerCanRally(const CHL2MP_Player* pRequester); //Checks against times

	int		ParseRallyCommand(int vcommCommand); //Given a vcomm command, parses it into rally flags. Returns -1 if its an unusable vcomm
}

#endif //BG3_BUFFS_H