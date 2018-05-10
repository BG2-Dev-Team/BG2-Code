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
www.bg2mod.com

Note that because of the sheer volume of files in the Source SDK this
notice cannot be put in all of them, but merely the ones that have any
changes from the original SDK.
In order to facilitate easy searching, all changes are and must be
commented on the following form:

//BG3 - <name of contributer>[ - <small description>]
*/
#ifndef BG3_MOTD_H
#define BG3_MOTD_H

#include "cbase.h"

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/HTML.h>

#include <game/client/iviewport.h>
#include "shareddefs.h"

class CMOTDHTML;
CMOTDHTML* g_pMOTD = nullptr;
class CMOTDHTML : public vgui::HTML
{
private:
	DECLARE_CLASS_SIMPLE(CMOTDHTML, vgui::HTML);

public:
	CMOTDHTML(vgui::Panel* parent, const char* name);

	virtual const char *GetName(void) { return PANEL_INFO; }
	virtual void SetData(KeyValues *data);
	virtual void SetData(int type, const char *title, const char *message, const char *message_fallback, int command, bool bUnload);
	virtual void ShowFile(const char *filename);
	virtual void ShowText(const char *text);
	virtual void ShowURL(const char *URL, bool bAllowUserToDisable = true);
	virtual void ShowIndex(const char *entry);

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme) override;

	virtual void ManualUpdate();

	void ManualReset();

	void SetVisible(bool bVisible) override;

private:
	IViewPort	*m_pViewPort;
	char		m_szTitle[255];
	char		m_szMessage[2048];
	char		m_szMessageFallback[2048];
	//=============================================================================
	// HPE_BEGIN:
	// [Forrest] Replaced text window command string with TEXTWINDOW_CMD enumeration
	// of options.  Passing a command string is dangerous and allowed a server network
	// message to run arbitrary commands on the client.
	//=============================================================================
	int			m_nExitCommand;
	//=============================================================================
	// HPE_END
	//=============================================================================
	int			m_nContentType;
	bool		m_bShownURL;
	bool		m_bUnloadOnDismissal;

public:
	virtual bool OnStartRequest(const char *url, const char *target, const char *pchPostData, bool bIsRedirect) OVERRIDE;
};

#endif //BG3_MOTD_H