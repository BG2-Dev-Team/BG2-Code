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
#include "cbase.h"

class CHL2MP_Player;

class CAdminSubMenu {
public:
	CAdminSubMenu();

	//variable-length array of children
	//if there are no children, this is NULL and 
	//this submenu is only an option of another submenu
	//and does not have any suboptions itself.
	//i.e. if this is NULL, this is a leaf node.
	CAdminSubMenu** m_aChildren = NULL;
	
	//Gets the text used to show how this sub menu would appear if it is shown in the given slot
	//I.e. the PARENT of this submenu uses this to get the text
	//1,2,3 etc. prefixes added automatically;
	virtual std::string getLineItemText(uint8 slot) = 0;
	
	//Called when this submenu is opened from the parent menu
	virtual void onOpenedFromParent(uint8 slot) = 0;
};

class CAdminMainMenu : public CAdminSubMenu {
public:
	std::string getLineItemText(uint8 slot) override { return "Admin Menu"; }
	void onOpenedFromParent(uint8 slot) override {}

	CAdminMainMenu();

	static CAdminMainMenu* Get();
};

/*****************************************************************************************************
* Generalized list menu, so that we don't have to code a pagination system multiple times.
*****************************************************************************************************/
class CListedMenuOption {
	virtual std::string getForIndex(int index) = 0;
	std::string getLineItemText(uint8 slot);
	virtual void onOpenedFromParent(uint8 slot);
};

class CListedMenu : public CAdminSubMenu {
	void onOpenedFromParent(uint8 slot) override;
};

/*****************************************************************************************************
* Map menu and map menu options
*****************************************************************************************************/
class CMapMenu : public CListedMenu {
	std::string getLineItemText(uint8 slot) override { return "Change Map"; }
	void onOpenedFromParent(uint8 slot) override;
};

class CMapMenuOption : public CListedMenuOption {
public:
	std::string getForIndex(int index) override;
	void onOpenedFromParent(uint8 slot) override;
};

/*****************************************************************************************************
* Player menu and player menu options
*****************************************************************************************************/
class CPlayerMenu : public CAdminSubMenu {
	std::string getLineItemText(uint8 slot) override;
	void onOpenedFromParent(uint8 slot) override;
	void(*m_pPerPlayerAction)(CHL2MP_Player*);
};

class CPlayerMenuOption : public CListedMenuOption {
	std::string getForIndex(int index) override;
	void onOpenedFromParent(uint8 slot) override;
	void(*m_pPerPlayerAction)(CHL2MP_Player*);
};

class CPlayerActionMenu : public CAdminSubMenu {
	std::string getLineItemText(uint8 slot) override { return "Player Commands"; }
	void onOpenedFromParent(uint8 slot) override;
};

/*****************************************************************************************************
* Gamemode menu and gamemode menu options
*****************************************************************************************************/
class CModeMenu : public CAdminSubMenu {
	std::string getLineItemText(uint8 slot) override { return "Game Modes"; }
	void onOpenedFromParent(uint8 slot) override {}
};

class CModeMenuOption : public CAdminSubMenu {
public:
	CModeMenuOption(const char* command, const char* name) {
		m_pszCommand = command; m_pszCommandName = name;
	}
	const char* m_pszCommand;
	const char* m_pszCommandName;
	std::string getLineItemText(uint8 slot) override { return m_pszCommandName; }
	void onOpenedFromParent(uint8 slot) override {
		engine->ServerCmd(m_pszCommand);
	}
};

#endif //ADMINSUBMENU_H