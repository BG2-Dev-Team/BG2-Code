/*
	The Battle Grounds 2 - A Source modification
	Copyright (C) 2005, The Battle Grounds 2 Team and Contributors

	The Battle Grounds 2 free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The Battle Grounds 2 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

	Contact information:
		Tomas "Tjoppen" Härdin		mail, in reverse: se . gamedev @ tjoppen

	You may also contact the (future) team via the Battle Grounds website and/or forum at:
		www.bgmod.com

	 Note that because of the sheer volume of files in the Source SDK this
	notice cannot be put in all of them, but merely the ones that have any
	changes from the original SDK.
	 In order to facilitate easy searching, all changes are and must be
	commented on the following form:

	//BG2 - <name of contributer>[ - <small description>]
*/

class C_Flag : public C_BaseAnimating
{
	DECLARE_CLASS( C_Flag, C_BaseAnimating );
public:
	DECLARE_CLIENTCLASS();

					C_Flag();
	virtual			~C_Flag();

	//virtual void	PreDataUpdate( DataUpdateType_t updateType );
	//virtual	void	ClientThink();

public:

	int		m_iLastTeam;
	int     m_iRequestingCappers;
	float	m_flNextCapture;

	int		m_iCapturePlayers;
	int		m_iNearbyPlayers;
	int		m_iForTeam;
	float	m_flCaptureTime;
	bool	m_pOverloading[MAX_PLAYERS];	//which players are overloading this flag?
	
	char	m_sFlagName[256];

	int		m_iHUDSlot;		//in which slot is the icon for this flag?
	//bool	m_bActive; //Tweaked. See flag.cpp in the server project. -HairyPotter
	
	bool	m_bNotUncappable,
			m_bUncapOnDeath;
};

extern CUtlVector< C_Flag * > g_Flags;