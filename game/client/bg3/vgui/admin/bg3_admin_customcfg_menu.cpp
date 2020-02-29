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
#include "bg3_admin_customcfg_menu.h"


//Lots of forward declarations
static std::vector<std::string> g_aCfgNames;
/*void LoadStringsFromCVar(ConVar* pCVar, std::vector<std::string>& strList);

static void OnCfgListChanged(IConVar* pCvar, const char* pszOldValue, float flOldValue);
ConVar adm_cfg_list = ConVar("adm_cfg_list", "", FCVAR_GAMEDLL | FCVAR_REPLICATED | FCVAR_HIDDEN, "", &OnCfgListChanged);
static void OnCfgListChanged(IConVar* pCvar, const char* pszOldValue, float flOldValue) {
	LoadStringsFromCVar(&adm_cfg_list, g_aCfgNames);
}*/

//Define conversion from CVar list
//Using cvars because I don't want to mess with networked arrays of strings
/*void LoadStringsFromCVar(ConVar* pCVar, std::vector<std::string>& strList) {
	//clear existing list
	strList.clear();

	//make a mutable copy of string
	int len = strlen(pCVar->GetString());
	char* buffer = new char[len];
	strcpy_s(buffer, len, pCVar->GetString());
	
	//go character by character until we reach the end of buffer
	int previousNullIndex = -1;
	for (int i = 0; i < len; i++) {
		if (buffer[i] == ';') {
			buffer[0];

			//if previous semicolon was more than one character away, then add the CFG name to list
			if (i - previousNullIndex > 1) {
				strList
			}
		}
	}
}*/

CCustomCFGMenu::CCustomCFGMenu(int cfgIndex, CAdminSubMenu* parent) {
	//assign static stuff
	m_pszTitle = "Choose Custom CFG";

	//HACK HACK if cfgIndex==-1, then this menu is a leaf that actually runs the CFG
	if (cfgIndex == -1) {
		m_pszFunc = [](uint8, CAdminSubMenu* pSelf)->bool {
			SayServerCommand("rc exec %s", pSelf->m_pszLineItemText);
			return false;
		};
	}
	else {
		//calculate number of children
		int remainingCFGs = g_aCfgNames.size() - cfgIndex;

		m_iNumChildren = min(10, remainingCFGs + 1); //+1 for back button

		bool bHaveNextButton = remainingCFGs > 9; //sometimes we can fit all 9 remaining instead of having a 'next' button to a one-item page.
		int numCFGLabels; //figure out how many labels will actually execute CFGs
		if (bHaveNextButton) numCFGLabels = 8;
		else numCFGLabels = remainingCFGs;

		//created children

		//assign back button

		//assign labels

		//assign next button
		if (bHaveNextButton) {

			//make recursive call here
		}
	}
	


}