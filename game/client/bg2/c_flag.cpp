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

#include "cbase.h"
#include "c_flag.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

void RecvProxy_IsOverloading_Bit( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	C_Flag *pFlag = (C_Flag*)pStruct;
	pFlag->m_pOverloading[pData->m_iElement] = pData->m_Value.m_Int;
}


IMPLEMENT_CLIENTCLASS_DT( C_Flag, DT_Flag, CFlag )
	RecvPropInt( RECVINFO( m_iLastTeam ) ),
	RecvPropInt( RECVINFO( m_iRequestingCappers ) ),
	RecvPropFloat( RECVINFO( m_flNextCapture ) ),
	RecvPropInt( RECVINFO( m_iCapturePlayers ) ),
	RecvPropInt( RECVINFO( m_iNearbyPlayers ) ),
	RecvPropInt( RECVINFO( m_iForTeam ) ),	
	RecvPropFloat( RECVINFO( m_flCaptureTime ) ),
	RecvPropString( RECVINFO( m_sFlagName ) ),		
	RecvPropInt( RECVINFO( m_iHUDSlot ) ),			
	//RecvPropBool( RECVINFO( m_bActive ) ), //Tweaked. See flag.cpp in the server project. -HairyPotter
	RecvPropBool( RECVINFO( m_bNotUncappable ) ),
	RecvPropBool( RECVINFO( m_bUncapOnDeath ) ),

	RecvPropArray2( 
		NULL, //we don't need array legth..
		RecvPropInt("IsOverloading_Bit", 0, SIZEOF_IGNORE, 0, RecvProxy_IsOverloading_Bit),
		MAX_PLAYERS, 
		0,
		"IsOverloading"
		),

END_RECV_TABLE()

// Global list of client side team entities
CUtlVector< C_Flag * > g_Flags;

//=================================================================================================
// C_Team functionality

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_Flag::C_Flag()
{
	/*m_iScore = 0;
	memset( m_szTeamname, 0, sizeof(m_szTeamname) );

	m_iDeaths = 0;
	m_iPing = 0;
	m_iPacketloss = 0;*/

	m_sFlagName[0] = 0;	//nullterm just in case

	// Add myself to the global list of team entities
	g_Flags.AddToTail( this );
}

C_Flag::~C_Flag()
{
	g_Flags.FindAndRemove( this );
}
