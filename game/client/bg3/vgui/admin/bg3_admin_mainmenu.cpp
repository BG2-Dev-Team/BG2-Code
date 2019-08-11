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
#include "bg3_admin_submenu.h"

static CAdminMainMenu* g_pAdminMainMenu = NULL;

CAdminMainMenu::CAdminMainMenu() {
	g_pAdminMainMenu = this;

	//Admin menu children
	CModeMenu* pModeMenu					= new CModeMenu;
	CMapMenu* pMapMenu						= new CMapMenu;
	CPlayerActionMenu* pPlayerActionMenu	= new CPlayerActionMenu;
	CAdminSubMenu** pChildren = m_aChildren = new CAdminSubMenu*[3]; {
		pChildren[0] = pModeMenu;
		pChildren[1] = pMapMenu;
		pChildren[2] = pPlayerActionMenu;
	};

	//Mode menu children
	pChildren = pModeMenu->m_aChildren = new CAdminSubMenu*[5]; {
		pChildren[0] = new CModeMenuOption("linebattle 10", "Linebattle");
		pChildren[1] = new CModeMenuOption("skirm 20", "Skirmish");
		pChildren[2] = new CModeMenuOption("lms 10", "Last Man Standing");
		pChildren[3] = new CModeMenuOption("dm 10", "Ticket Deathmatch");
	}
}

CAdminMainMenu* CAdminMainMenu::Get() {
	if (!g_pAdminMainMenu)
		g_pAdminMainMenu = new CAdminMainMenu();
	return g_pAdminMainMenu;
}

