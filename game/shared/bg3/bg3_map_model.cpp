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
#include "filesystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//For building a stack of CMapInfo
static CMapInfo* g_pTopMapInfo = NULL;

CMapInfo::CMapInfo() {
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

	FileFindHandle_t fileSearch;
	const char* pszMapName = filesystem->FindFirst("maps/*.bsp", &fileSearch);
	g_pTopMapInfo = new CMapInfo();
	g_pTopMapInfo->SetName(pszMapName);
	pszMapName = filesystem->FindNext(fileSearch);

	while (pszMapName) {
		//create new CMapInfo will push itself onto stack
		CMapInfo* info = new CMapInfo();
		info->SetName(pszMapName);
		pszMapName = filesystem->FindNext(fileSearch);
	}
	filesystem->FindClose(fileSearch);
}

CMapInfo* CMapInfo::GetFirstMapInfo() {
	return g_pTopMapInfo;
}

void CMapInfo::GetNextMapInfo(CMapInfo** ppMapInfo) {
	CMapInfo* curInfo = *ppMapInfo;
	*ppMapInfo = curInfo->m_pNextInfo;
}

bool CMapInfo::MapExists(const char* pszMapName) {
	CMapInfo::RefreshMapInfoList();

	CMapInfo* pMap = GetFirstMapInfo();
	bool bFound = false;
	while (pMap && !bFound) {
		bFound = Q_strcmp(pszMapName, pMap->m_pszMapName) == 0;
		CMapInfo::GetNextMapInfo(&pMap);
	}
	return bFound;
}

void CMapInfo::SetName(const char* pszName) {
	strcpy(m_pszMapName, pszName);

	//Find the . and set it to null, simplifies later searches
	bool bFound = false;
	for (int i = 0; i < MAX_MAP_NAME && !bFound; i++) {
		if (m_pszMapName[i] == '.') {
			m_pszMapName[i] = 0;
			bFound = true;
		}
	}

	//update mode field based on name
	m_iModeField = 0;
	if (m_pszMapName[0] == 'b' && m_pszMapName[1] == 'g')
		m_iModeField |= (uint8)EGameMode::SKIRMISH | (uint8)EGameMode::LMS;
	else if (m_pszMapName[0] == 'c' && m_pszMapName[1] == 't' && m_pszMapName[2] == 'f')
		m_iModeField |= (uint8)EGameMode::CAPTURE_THE_FLAG | (uint8)EGameMode::LMS;
	else if (m_pszMapName[0] == 'l' && m_pszMapName[1] == 'b')
		m_iModeField |= (uint8)EGameMode::LINEBATTLE;
	else if (m_pszMapName[0] == 's' && m_pszMapName[1] == 'g')
		m_iModeField |= (uint8)EGameMode::SIEGE;
	else
		m_iModeField |= (uint8)EGameMode::TRAINING;
}

CON_COMMAND(maplistupdate, "") {
	CMapInfo::RefreshMapInfoList();
}

CON_COMMAND(maptest, "") {
	if (args.ArgC() == 2)
		Msg("%i\n", CMapInfo::MapExists(args[1]));
}
