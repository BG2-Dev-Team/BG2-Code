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
#include <vgui_controls/Panel.h>
#include <vgui/ILocalize.h>
#include "bg3_admin_submenu.h"

using namespace vgui;

static bool defaultSubMenuFunction(uint8 slot, CAdminSubMenu*) { return false; }

//DEFAULT SUBMENU CONSTRUCTORS
CAdminSubMenu::CAdminSubMenu(){
	m_aChildren = NULL;
	m_iNumChildren = 0;
	m_pszLineItemText = "DEFAULT LINE ITEM TEXT";
	m_pszTitle = "DEFAULT SUBMENU TITLE";
	m_pszFunc = defaultSubMenuFunction;
}

CAdminSubMenu::CAdminSubMenu(const char* pszLineItemText, const char* pszTitle) {
	m_aChildren = NULL;
	m_iNumChildren = 0;
	m_pszLineItemText = pszLineItemText;
	m_pszTitle = pszTitle;
	m_pszFunc = defaultSubMenuFunction;
}

void CAdminSubMenu::getLineItemText(uint8 iForSlot, wchar* buffer, int bufferSize) const {
	wchar localBuffer[128];
	if (m_pszLineItemText[0] == '#')
		wcscpy_s(localBuffer, g_pVGuiLocalize->Find(m_pszLineItemText));
	else
		g_pVGuiLocalize->ConvertANSIToUnicode(m_pszLineItemText, localBuffer, 128);

	int displayedInt = iForSlot + 1;
	if (displayedInt == 10) displayedInt = 0;

	V_snwprintf(buffer, bufferSize, L"  %i. %s", displayedInt, localBuffer);
}

void SayServerCommand(const char* pszFormat, ...) {
	va_list args;
	va_start(args, pszFormat);
	char buffer[256];
	char buffer2[256];
	vsnprintf_s(buffer, 256, pszFormat, args);
	sprintf_s(buffer2, "say \"!%s\"", buffer);

	engine->ClientCmd(buffer2);
	engine->ServerCmd(buffer);

	va_end(args);
}

CAdminSubMenu* g_pAdminBackButton;