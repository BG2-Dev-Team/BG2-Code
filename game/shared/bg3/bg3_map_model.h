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

//----------------------------------------------------------------------
// Purpose - to model maps available to the game, for the purpose
// of choosing or displaying map information.
//----------------------------------------------------------------------
#ifndef BG3_MAP_MODEL_H
#define BG3_MAP_MODEL_H
#include "cbase.h"

#ifdef CLIENT_DLL
#include <vgui_controls/ImagePanel.h>
#endif

enum class EGameMode : unsigned char {
	SKIRMISH = 1,
	CAPTURE_THE_FLAG = 2,
	LMS = 4,
	SIEGE = 8,
	LINEBATTLE = 16,
	TRAINING = 32
};

class CMapInfo {
public:
	const char*		m_pszMapName;
	uint8			m_iIdealNumBots;
	uint8			m_iModeField;
#ifdef CLIENT_DLL
	vgui::IImage*	m_pMapImage;
#endif

	CMapInfo();

	static void RefreshMapInfoList();

	static CMapInfo* GetFirstMapInfo();

	static void GetNextMapInfo(CMapInfo**);

private:
	//For building our stack representation
	CMapInfo* m_pNextInfo;
};



#endif //BG3_MAP_MODEL_H