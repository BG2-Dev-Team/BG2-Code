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
		Tomas "Tjoppen" H�rdin		mail, in reverse: se . gamedev @ tjoppen

	You may also contact the (future) team via the Battle Grounds website and/or forum at:
		www.bgmod.com

	 Note that because of the sheer volume of files in the Source SDK this
	notice cannot be put in all of them, but merely the ones that have any
	changes from the original SDK.
	 In order to facilitate easy searching, all changes are and must be
	commented on the following form:

	//BG2 - <name of contributer>[ - <small description>]
*/

/*#ifdef CLIENT_DLL
#define CFlag C_Flag
#endif*/

#ifdef CLIENT_DLL
#error DO NOT USE ON CLIENT! c_flag.cpp/.h is for that
#endif
// #ifdef WIN32
#pragma once
// #endif
const int CFlag_START_DISABLED = 1;		// spawnflag definition

//BG2 - Tjoppen - TODO: replace the use of ClientPrintAll with custom usermessages
void ClientPrintAll( char *str, bool printfordeadplayers = false, bool forcenextclientprintall = false );
void ClientPrintAll( int msg_type, int msg_dest = HUD_PRINTCENTER, const char * param1 = 0, const char * param2 = 0 );
void ClientPrint( CBasePlayer *pPlayer, int msg_type, int msg_dest = HUD_PRINTCENTER, const char * param1 = 0, const char * param2 = 0 );

class CFlag : public CBaseAnimating
{
	DECLARE_CLASS( CFlag, CBaseAnimating );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	//BG2 - Used with the flag triggers. -HairyPotter
	bool m_bIsParent,										//Helps the flag remember it's place... as a parent.
		 m_bActive,											//Is it active? And yes, this replaces the networked version.
		 m_bInvisible;										//Is the flag model "invisible"?

	CUtlVector<CBasePlayer*>	m_vTriggerBritishPlayers,	//British players who have stepped into the trigger
								m_vTriggerAmericanPlayers;	//American players who have stepped into the trigger
	int americans,
	british,
	friendlies,
	enemies;
	//

	CNetworkVar( int, m_iLastTeam );		//which team is near this flag?
	CNetworkVar( int, m_iRequestingCappers); //yeah, extra int, im too drunk to think of a better way :o
	CNetworkVar( float, m_flNextCapture );

	CNetworkVar( int, m_iCapturePlayers );	//how many player must be nearby to capture this flag?
	CNetworkVar( int, m_iNearbyPlayers );	//how many players ARE nearby to capture this flag?
											// alternatively: how overloaded is this flag
	CNetworkVar( int, m_iForTeam );

	CNetworkVar( int, m_iHUDSlot );		//in which slot is the icon for this flag?

	//CNetworkVar( bool, m_bActive );		//BG2 - Tjoppen - adding SaintGreg's flag stuff from way back as a placeholder
										//				  until the new flag code is done.

	CNetworkVar( bool, m_bNotUncappable );	//is flag non-uncappable?
	CNetworkVar( bool, m_bUncapOnDeath );	//does this flag uncap if all overloaders die?

	//BG2 - SaintGreg - Output functions similar to BG's
	COutputEvent m_OnAmericanStartCapture;
	COutputEvent m_OnBritishStartCapture;
	COutputEvent m_OnStartCapture;
	COutputEvent m_OnAmericanCapture;
	COutputEvent m_OnBritishCapture;
	COutputEvent m_OnCapture;
	COutputEvent m_OnAmericanStopCapture;
	COutputEvent m_OnBritishStopCapture;
	COutputEvent m_OnStopCapture;
	COutputEvent m_OnAmericanLosePoint;
	COutputEvent m_OnBritishLosePoint;
	COutputEvent m_OnLosePoint;
	COutputEvent m_OnEnable;
	COutputEvent m_OnDisable;

	bool IsActive( void );
	void InputEnable( inputdata_t &inputData );
	void InputDisable( inputdata_t &inputData );
	void InputToggle( inputdata_t &inputData );
	void InputForceCap( inputdata_t &inputData );


	int		m_iTeamBonus,
			m_iTeamBonusInterval,
			m_iPlayerBonus,
			m_iAmericanFlagSkin,
			m_iBritishFlagSkin,
			m_iNeutralFlagSkin,
			m_iDisabledFlagSkin,
			m_iSavedHUDSlot,     //This is for the flag enable/disable addon. The original Hud Slot value is stored here.
			m_iFullCap,
			m_iCaptureSound,
			m_iStartingTeam,
			m_iAmericanBuff = 0,
			m_iBritishBuff = 0;

	float	m_flNextTeamBonus;

	vec_t	m_flBotNoticeRange; //Range at which bots will notice this flag

	char *SoundFile;

	string_t	m_sNeutralFlagModelName, //BG2 - These strings exist only for backwards compatibility and serve no other purpose. -HairyPotter
				m_sDisabledFlagModelName,
				m_sBritishFlagModelName,
				m_sAmericanFlagModelName; //

	int GetNeutralSkin()
	{
		//for backward compatibility without this name defined
        if( strlen(STRING(m_sNeutralFlagModelName)) != 0 || m_iNeutralFlagSkin == NULL)
			return 2;

        return m_iNeutralFlagSkin;
	}

	int GetDisabledSkin()
	{
		//for backward compatibility without this name defined
		if( strlen(STRING(m_sDisabledFlagModelName)) != 0 || m_iDisabledFlagSkin == NULL)
			return 2;

        return m_iDisabledFlagSkin;
	}

	int GetBritishSkin()
	{
		//for backward compatibility without this name defined
		if( strlen(STRING(m_sBritishFlagModelName)) != 0 || m_iBritishFlagSkin == NULL)
			return 1;

        return m_iBritishFlagSkin;
	}

	int GetAmericanSkin()
	{
		//for backward compatibility without this name defined
		if( strlen(STRING(m_sAmericanFlagModelName)) != 0 || m_iAmericanFlagSkin == NULL)
			return 0;

        return m_iAmericanFlagSkin;
	}

	CNetworkVar( float,	m_flCaptureTime );	//.. and for how long?
	float	m_flCaptureRadius;				//.. and how close?

	CNetworkVar( string_t, m_sFlagName );

public:
	CUtlVector<CBasePlayer*>	m_vOverloadingPlayers;	//nearby or overloading players
	/*
	some notes on the list:
	- when a player dies, remove from list
	- when a player changes team, remove from list
	- when a player disconnects, remove from list
	- I couldn't be arsed to make it private with accessors and shit. Java has made me quite sick of that
	*/

	void Spawn( void );
	void Precache( void );
	void Think( void );
	void ThinkUncapped( void );
	void ThinkCapped( void );
	void Capture( int iTeam );	//let iTeam have this flag now..
	void HandleLoseOutputs( void );	//let iTeam have this flag now..
	void ChangeTeam( int iTeamNum );
	void ResetFlag( void );
	virtual int UpdateTransmitState();
};
