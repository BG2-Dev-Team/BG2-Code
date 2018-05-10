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
#include "cbase.h"
#include "vguitextwindow.h"
#include <networkstringtabledefs.h>
#include <cdll_client_int.h>
#include <clientmode_shared.h>
#include "networkstringtable_clientdll.h"

#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <filesystem.h>
#include <KeyValues.h>
#include <convar.h>
#include <vgui_controls/ImageList.h>

#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Button.h>

#include <game/client/iviewport.h>
#include "bg3/bg3_classmenu.h"
#include "bg3/bg3_teammenu.h"
#include "bg3_motd.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar cl_disablehtmlmotd("cl_disablehtmlmotd", "0", FCVAR_ARCHIVE, "Disable HTML motds.");

//=============================================================================
// HPE_BEGIN:
// [Forrest] Replaced text window command string with TEXTWINDOW_CMD enumeration
// of options.  Passing a command string is dangerous and allowed a server network
// message to run arbitrary commands on the client.
//=============================================================================
CON_COMMAND(showinfo, "Shows a info panel: <type> <title> <message> [<command number>]")
{
	Warning("Calling showinfo\n");
	if (!gViewPortInterface)
		return;

	if (args.ArgC() < 4)
		return;

	//BG3 - changed this to global pointer bc we moved PANEL_INFO
	//to Teammenu! gViewPortInterface won't find it!
	//IViewPortPanel * panel = gViewPortInterface->FindPanelByName( PANEL_INFO );
	CMOTDHTML* panel = g_pMOTD;

	if (panel)
	{
		KeyValues *kv = new KeyValues("data");
		kv->SetInt("type", Q_atoi(args[1]));
		kv->SetString("title", args[2]);
		kv->SetString("message", args[3]);

		if (args.ArgC() == 5)
			kv->SetString("command", args[4]);

		panel->SetData(kv);

		//panel->ShowPanel(true);

		kv->deleteThis();
	}
	else
	{
		Warning("Couldn't find info panel.\n");
	}
}
//=============================================================================
// HPE_END
//=============================================================================


using namespace vgui;
CMOTDHTML::CMOTDHTML(Panel* parent, const char* name) : HTML(parent, name) {
	m_pViewPort = gViewPortInterface;

	m_szTitle[0] = '\0';
	m_szMessage[0] = '\0';
	m_szMessageFallback[0] = '\0';
	m_nExitCommand = TEXTWINDOW_CMD_NONE;
	m_bShownURL = false;
	m_bUnloadOnDismissal = false;

	// load the new scheme early!!
	SetScheme("ClientScheme");
	SetProportional(true);

	//BG3 - hide these
	/*m_pTextMessage = new TextEntry(this, "TextMessage");
	m_pTextMessage->SetMultiline(true);*/
	//

	m_nContentType = TYPE_TEXT;
}

void CMOTDHTML::SetData(KeyValues* data) {
	SetData(data->GetInt("type"), data->GetString("title"), data->GetString("msg"), data->GetString("msg_fallback"), data->GetInt("cmd"), data->GetBool("unload"));
}

void CMOTDHTML::SetData(int type, const char *title, const char *message, const char *message_fallback, int command, bool bUnload)
{

	Q_strncpy(m_szTitle, title, sizeof(m_szTitle));
	Q_strncpy(m_szMessage, message, sizeof(m_szMessage));
	Q_strncpy(m_szMessageFallback, message_fallback, sizeof(m_szMessageFallback));

	m_nExitCommand = command;

	m_nContentType = type;
	m_bUnloadOnDismissal = bUnload;

	ManualUpdate();
}

void CMOTDHTML::ShowFile(const char *filename)
{
	if (Q_stristr(filename, ".htm") || Q_stristr(filename, ".html"))
	{
		// it's a local HTML file
		char localURL[_MAX_PATH + 7];
		Q_strncpy(localURL, "file://", sizeof(localURL));

		char pPathData[_MAX_PATH];
		g_pFullFileSystem->GetLocalPath(filename, pPathData, sizeof(pPathData));
		Q_strncat(localURL, pPathData, sizeof(localURL), COPY_ALL_CHARACTERS);

		ShowURL(localURL);
	}
	else
	{
		// read from local text from file
		FileHandle_t f = g_pFullFileSystem->Open(m_szMessage, "rb", "GAME");

		if (!f)
			return;

		char buffer[2048];

		int size = MIN(g_pFullFileSystem->Size(f), sizeof(buffer) - 1); // just allow 2KB

		g_pFullFileSystem->Read(buffer, size, f);
		g_pFullFileSystem->Close(f);

		buffer[size] = 0; //terminate string

		ShowText(buffer);
	}
}

void CMOTDHTML::ShowText(const char *text)
{
	/*m_pTextMessage->SetVisible(true);
	m_pTextMessage->SetText(text);
	m_pTextMessage->GotoTextStart();*/
}

void CMOTDHTML::ShowURL(const char *URL, bool bAllowUserToDisable)
{

	ClientModeShared *mode = (ClientModeShared *)GetClientModeNormal();
	if ((bAllowUserToDisable && cl_disablehtmlmotd.GetBool()) || !mode->IsHTMLInfoPanelAllowed())
	{
		Warning("Blocking HTML info panel '%s'; Using plaintext instead.\n", URL);

		// User has disabled HTML TextWindows. Show the fallback as text only.
		if (g_pStringTableInfoPanel)
		{
			int index = g_pStringTableInfoPanel->FindStringIndex("motd_text");
			if (index != ::INVALID_STRING_INDEX)
			{
				int length = 0;
				const char *data = (const char *)g_pStringTableInfoPanel->GetStringUserData(index, &length);
				if (data && data[0])
				{
					SetVisible(false);
					ShowText(data);
				}
			}
		}
		return;
	}

	SetVisible(true);
	OpenURL(URL, NULL);
	m_bShownURL = true;
}

void CMOTDHTML::ShowIndex(const char *entry)
{
	const char *data = NULL;
	int length = 0;

	if (NULL == g_pStringTableInfoPanel)
		return;

	int index = g_pStringTableInfoPanel->FindStringIndex(m_szMessage);

	if (index != ::INVALID_STRING_INDEX)
		data = (const char *)g_pStringTableInfoPanel->GetStringUserData(index, &length);

	if (!data || !data[0])
		return; // nothing to show

	// is this a web URL ?
	if (!Q_strncmp(data, "http://", 7) || !Q_strncmp(data, "https://", 8))
	{
		ShowURL(data);
		return;
	}

	// try to figure out if this is HTML or not
	if (data[0] != '<')
	{
		ShowText(data);
		return;
	}

	// data is a HTML, we have to write to a file and then load the file
	FileHandle_t hFile = g_pFullFileSystem->Open(TEMP_HTML_FILE, "wb", "DEFAULT_WRITE_PATH");

	if (hFile == FILESYSTEM_INVALID_HANDLE)
		return;

	g_pFullFileSystem->Write(data, length, hFile);
	g_pFullFileSystem->Close(hFile);

	if (g_pFullFileSystem->Size(TEMP_HTML_FILE) != (unsigned int)length)
		return; // something went wrong while writing

	ShowFile(TEMP_HTML_FILE);
}

void CMOTDHTML::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	ManualReset();
}

void CMOTDHTML::ManualUpdate(void)
{
	if (m_nContentType == TYPE_INDEX)
	{
		ShowIndex(m_szMessage);
	}
	else if (m_nContentType == TYPE_URL)
	{
		if (!Q_strncmp(m_szMessage, "http://", 7) || !Q_strncmp(m_szMessage, "https://", 8) || !Q_stricmp(m_szMessage, "about:blank"))
		{
			ShowURL(m_szMessage);
		}
		else
		{
			// We should have trapped this at a higher level
			Assert(!"URL protocol is missing or blocked");
		}
	}
	else if (m_nContentType == TYPE_FILE)
	{
		ShowFile(m_szMessage);
	}
	else if (m_nContentType == TYPE_TEXT)
	{
		ShowText(m_szMessage);
	}
	else
	{
		DevMsg("CTextWindow::Update: unknown content type %i\n", m_nContentType);
	}
}

void CMOTDHTML::ManualReset(void)
{
	//=============================================================================
	// HPE_BEGIN:
	// [Forrest] Replace strange hard-coded default message with hard-coded error message.
	//=============================================================================
	V_strcpy_safe(m_szTitle, "Error loading info message.");
	V_strcpy_safe(m_szMessage, "");
	V_strcpy_safe(m_szMessageFallback, "");
	//=============================================================================
	// HPE_END
	//=============================================================================

	m_nExitCommand = TEXTWINDOW_CMD_NONE;
	m_nContentType = TYPE_TEXT;
	m_bShownURL = false;
	m_bUnloadOnDismissal = false;

	/*int length = 0;

	int type = TYPE_INDEX; 
	const char *title = ""; 
	const char *message; 
	const char *message_fallback; 
	int command; 
	bool bUnload = true;

	int index = g_pStringTableInfoPanel->FindStringIndex("motdfile");
	if (index != ::INVALID_STRING_INDEX)
		data = (const char *)g_pStringTableInfoPanel->GetStringUserData(index, &length);*/
	ManualUpdate();
}

void CMOTDHTML::SetVisible(bool bVisible) {
	if (BaseClass::IsVisible() == bVisible)
		return;

	m_pViewPort->ShowBackGround(bVisible);

	if (!bVisible)
	{
		SetVisible(false);
		SetMouseInputEnabled(false);

		if (m_bUnloadOnDismissal && m_bShownURL )//&& m_pHTMLMessage)
		{
			OpenURL("about:blank", NULL);
			m_bShownURL = false;
		}
	}
}

bool CMOTDHTML::OnStartRequest(const char *url, const char *target, const char *pchPostData, bool bIsRedirect)
{
	if (Q_strstr(url, "steam://"))
		return false;

	return BaseClass::OnStartRequest(url, target, pchPostData, bIsRedirect);
}