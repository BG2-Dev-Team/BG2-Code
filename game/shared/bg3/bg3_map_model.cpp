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
#include "bg3_map_model.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//For building a stack of CMapInfo
static CMapInfo* g_pTopMapInfo = NULL;

CMapInfo::CMapInfo() {
	m_pszMapName = NULL;
	m_iIdealNumBots = 0;
	m_iModeField = 0;
#ifdef CLIENT_DLL
	m_pMapImage = NULL;
#endif
	m_pNextInfo = NULL;

	//build mapinfo stack
	m_pNextInfo = g_pTopMapInfo;
	g_pTopMapInfo = this;
}

void CMapInfo::RefreshMapInfoList() {
	//delete existing map info
	while (g_pTopMapInfo) {
		CMapInfo* deleteMe = g_pTopMapInfo;
		g_pTopMapInfo = g_pTopMapInfo->m_pNextInfo;
		delete deleteMe;
	}
}

CMapInfo* CMapInfo::GetFirstMapInfo() {
	return g_pTopMapInfo;
}

void CMapInfo::GetNextMapInfo(CMapInfo** ppMapInfo) {
	CMapInfo* curInfo = *ppMapInfo;
	*ppMapInfo = curInfo->m_pNextInfo;
}