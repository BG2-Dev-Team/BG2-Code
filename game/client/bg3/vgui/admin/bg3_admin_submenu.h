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

#ifndef ADMINSUBMENU_H
#define ADMINSUBMENU_H
#ifdef _WIN32
#pragma once
#endif

#include <string>
#include <vector>
#include "cbase.h"

class CHL2MP_Player;

#define NUM_ADMIN_MENU_LABELS 10

#define PLAYER_PER_PAGE 8
#define MAPS_PER_PAGE PLAYER_PER_PAGE

enum class AdminMenuPlayerAction {
	Mute,
	Unmute,
	Spawn,
	Slay,
	Kick,
	Ban60,
	BanIndef
};

class CAdminSubMenu {
public:
	CAdminSubMenu();

	CAdminSubMenu(const char* pszLineItemText, const char* pszTitle);

	~CAdminSubMenu() {
		extern CAdminSubMenu* g_pAdminBackButton;

		//delete children
		for (int i = 0; i < m_iNumChildren; i++) {
			if (m_aChildren[i] != g_pAdminBackButton) {
				delete m_aChildren[i];
			}
		}

		//delete the array itself
		delete[] m_aChildren;
	}

	//variable-length array of children
	//if there are no children, this is NULL and 
	//this submenu is only an option of another submenu
	//and does not have any suboptions itself.
	//i.e. if this is NULL, this is a leaf node.
	CAdminSubMenu** m_aChildren;
	int				m_iNumChildren;
	
	//Tthe text used to show this sub menu when it is shown in the given slot
	//I.e. the PARENT of this submenu uses this to get the text
	//1,2,3 etc. prefixes added automatically elsewhere;
//protected:
	const char* m_pszLineItemText;

public:
	//Title of this menu as it would appear at the top of the panel
	const char* m_pszTitle;
	
	//Usually just returns m_pszLineItemText unless this is a funky listed menu.
	void getLineItemText(uint8 iForSlot, wchar* buffer, int bufferSize) const;
	
	//Called when this submenu is opened from the parent menu
	//If returns true, then menu goes back up to parent's parent (back button)
	bool(*m_pszFunc)(uint8 iSlot, CAdminSubMenu* pSelf);
};

//Special Player entries
class CAdminPlayerSubMenu : public CAdminSubMenu {
public:
	CAdminPlayerSubMenu();

	CAdminPlayerSubMenu(const char* pszLineItemText, const char* pszTitle);
	int	m_iPlayerID;
};

//top-level singleton
class CAdminMainMenu : public CAdminSubMenu {
public:

	CAdminMainMenu();

	static CAdminMainMenu* Get();
};

//HACK HACK - using the "say" command from console doesn't interpret commands in the said text,
//so call this function to both "say" the command AND send it to the server
void SayServerCommand(const char* pszFormat, ...);

extern CAdminSubMenu* g_pAdminBackButton;

#endif //ADMINSUBMENU_H