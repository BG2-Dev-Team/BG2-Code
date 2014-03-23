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


// This point of this is to have something that takes care of business after some of the other events in BG2. Such as map and round wins. -HairyPotter
class CMapTrigger : public CBaseAnimating
{
	DECLARE_CLASS( CMapTrigger, CBaseAnimating );
	DECLARE_DATADESC();

	COutputEvent m_OnAmericanWinRound;
	COutputEvent m_OnBritishWinRound;
	COutputEvent m_OnAmericanWinMap;
	COutputEvent m_OnBritishWinMap;
	COutputEvent m_OnDraw;

	void InputForceAmericanWinRound( inputdata_t &inputData );
	void InputForceBritishWinRound( inputdata_t &inputData );
	void InputForceDrawRound( inputdata_t &inputData );

public:

	void AmericanRoundWin( void );
	void BritishRoundWin( void );
	void AmericanMapWin( void );
	void BritishMapWin( void );
	void Draw( void );
	void Spawn( void );
};