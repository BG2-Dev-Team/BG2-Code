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
#include "bg3/bg3_teammenu.h"
#include "bg3_create_server_dialog.h"

extern ConVar bot_difficulty;
extern ConVar bot_minplayers_mode;

#define NUM_BOT_BUTTONS 5

static void JoinMapFromCurrentButton() {
	/*if (!g_pSelectedMapButton)
		return;
	uint16 cmdlen = strlen(g_pSelectedMapButton->m_pMapInfo->m_pszMapName) + 6;
	char* cmd = new char[cmdlen];

	Q_snprintf(cmd, cmdlen, "map %s\0", g_pSelectedMapButton->m_pMapInfo->m_pszMapName);
	engine->ClientCmd(cmd);

	delete[] cmd;*/
}

namespace vgui {

	//---------------------------------------------------------------
	// Purpose: These buttons control the bot settings which are loaded
	//		when the player creates a local game.
	//---------------------------------------------------------------
	ConVar bot_cl_default("bot_cl_default", "1", FCVAR_ARCHIVE | FCVAR_CLIENTDLL,
		"Default setting for bot button in Create Server dialog. -1 = none, 0-3 match bot_difficulty",
		true, -1.0f, true, 3.0f);

	CBotButton*	g_pSelectedBotButton = NULL;

	CBotButton::CBotButton(Panel* parent, int buttonCode, setting botSetting) :
		Button(parent, "BotButton", ""),
		m_iButtonCode(buttonCode),
		m_bMouseOver(false),
		m_eBotSetting(botSetting) {}

	void CBotButton::OnCursorEntered() {
		m_bMouseOver = true;
	}
	void CBotButton::OnCursorExited() {
		m_bMouseOver = false;
	}
	void CBotButton::OnMousePressed(MouseCode code) {
		Select();
		ExecuteCommand();
	}
	void CBotButton::Paint() {
		int w, h; GetSize(w, h);
		if (m_bMouseOver) {
			s_pImageNormal->SetPos(0, 0);
			s_pImageNormal->SetSize(w, h);
			s_pImageNormal->Paint();
		}
		else {
			s_pImageHover->SetPos(0, 0);
			s_pImageHover->SetSize(w, h);
			s_pImageHover->Paint();
		}
	}

	void CBotButton::Select() {
		g_pSelectedBotButton = this;

		//update text for label text
		g_pCreateMapDialog->m_pBotButtonLabel->SetText(GetBotSettingLabelText());
	}

	void CBotButton::ExecuteCommand() {

		//update default value
		bot_cl_default.SetValue((int)m_eBotSetting);

		//our relationship with bot_difficulty is almost one-to-one, but not quite
		switch (m_eBotSetting) {
		case setting::NONE:
			bot_minplayers_mode.SetValue(0);
			break;
		case setting::RANDOM:
			bot_minplayers_mode.SetValue(2);
			bot_difficulty.SetValue(3);
			break;
		default:
			bot_difficulty.SetValue((int)m_eBotSetting);
			bot_minplayers_mode.SetValue(2);
		}
	}

	const wchar* CBotButton::GetBotSettingLabelText() {
		//we could do this more efficiently by indexing into an array,
		//but it's less readable from the programmer's perspective
		//and efficiency isn't important when we're just in the main menu
		const char* pszCode;

		switch (g_pSelectedBotButton->m_eBotSetting) {
		#define bcase break; case
		default:
			pszCode = "#BG3_BOTS_NONE";
		bcase setting::RANDOM:
			pszCode = "#BG3_BOTS_RANDOM";
		bcase setting::EASY:
			pszCode = "#BG3_BOTS_EASY";
		bcase setting::MED:
			pszCode = "#BG3_BOTS_MED";
		bcase setting::HARD:
			pszCode = "#BG3_BOTS_HARD";
		#undef bcase
		}

		return g_pVGuiLocalize->Find(pszCode);
	}

	void CBotButton::LoadDefaults() {
		//find bot button whose code matches default from convar
		int defaultBots = bot_cl_default.GetInt();

		//clamp value
		defaultBots = Clamp(defaultBots, (int)setting::NONE, (int)setting::HARD);

		//iterate through buttons to find match
		CBotButton** pButtons = g_pCreateMapDialog->m_pBotButtons;
		CBotButton* pDefault = NULL;
		for (int i = 0; i < NUM_BOT_BUTTONS; i++) {
			if (pButtons[i]->m_eBotSetting == (setting)defaultBots) {
				pDefault = pButtons[i];
				break;
			}
		}
		//this shouldn't happen, check anyways
		if (!pDefault) pDefault = pButtons[0];

		//select default button
		pDefault->Select();

		//load static images
		s_pImageNormal = scheme()->GetImage("bots/button_normal", false);
		s_pImageHover = scheme()->GetImage("bots/button_hover", false);
	}

	IImage* CBotButton::s_pImageNormal = NULL;
	IImage* CBotButton::s_pImageHover = NULL;

	//---------------------------------------------------------------
	// Purpose: This is the panel to which all the map buttons and other
	//	buttons and labels are parented.
	//---------------------------------------------------------------

	CCreateMapDialog* g_pCreateMapDialog = NULL;
	CCreateMapDialog::CCreateMapDialog(Panel* pParent, const char* panelName)
	: Panel(pParent, panelName)	{
		g_pCreateMapDialog = this;
		m_pMapButtons = NULL;

		//create bot buttons
		m_pBotButtons = new CBotButton*[NUM_BOT_BUTTONS];
		for (int i = 0; i < NUM_BOT_BUTTONS; i++) {
			m_pBotButtons[i] = new CBotButton(this, i - 1, (CBotButton::setting)((int)CBotButton::setting::NONE + i));
		}

		//create bot button label
		m_pBotButtonLabel = new Label(this, "BotButtonLabel", "");

		//load default bot button settings
		CBotButton::LoadDefaults();

		//create go/ok button which will actually start the server
		m_pGoButton = new FancyButton(this, "CreateMapDialogGoButton", "maps/go_normal", "maps/go_hover");
		m_pGoButton->SetCommandFunc(&JoinMapFromCurrentButton);

		//now that everything's created, let's size everything up
		CreateLayoutForResolution();
	}

	void CCreateMapDialog::RefreshMapList() {
		//LoadMaplistFromFilesystem(m_aMapNames);
	}

	void CCreateMapDialog::LoadMaplistFromFilesystem(CUtlVector<char*>& mapList) {
		mapList.RemoveAll();
		CUtlBuffer buf;
		if (!filesystem->ReadFile("maplist.txt", "GAME", buf))
			return;
		buf.PutChar(0);
		V_SplitString((char*)buf.Base(), "\n", mapList);
	}

#define MAPDIALOG_MARGIN 10
#define MAPDIALOG_MARGIN_TOP 40
	void CCreateMapDialog::CreateLayoutForResolution() {
		int screenw = ScreenWidth();
		int screenh = ScreenHeight();

		//we will assume a size of 640*400 for this panel on a 640*480 screen, then we will scale up according to screen height
		float scale = screenh / 480;
		int wide = 640 * scale;
		int tall = 400 * scale;
		SetSize(wide, tall);
		SetPos((screenw - wide) / 2, 80 * scale);

		//delete old map buttons if necessary
		if (m_pBotButtons) {
			for (int i = 0; i < m_iNumMapButtons; i++) {
				delete m_pMapButtons[i];
			}
			delete[] m_pMapButtons;
		}

		//load ideal parameters for map buttons
		LoadMapDialogIdealButtonSizes();

		//now that we know how many map buttons we can fit, reload those
		//create map buttons
		m_pMapButtons = new CMapButton*[m_iNumMapButtons];
		char buffer[8];
		for (uint16 i = 0; i < m_iNumMapButtons; i++) {
			Q_snprintf(buffer, 8, "mapb%3i", i);
			m_pMapButtons[i] = new CMapButton(this, buffer);

			//might as well set all button sizes here too
			m_pMapButtons[i]->SetSize(m_iButtonSize * scale, m_iButtonSize * scale);
		}


		//set positions of all map buttons
		uint16 xpos;
		uint16 ypos = MAPDIALOG_MARGIN_TOP;
		for (uint16 i = 0; i < m_iNumRows; i++) {
			//reset xpos
			xpos = MAPDIALOG_MARGIN;

			for (int j = 0; j < m_iButtonsPerRow; j++) {
				CMapButton* pButton = m_pMapButtons[i * m_iButtonsPerRow + j];
				pButton->SetPos(xpos * scale, ypos * scale);

				xpos += m_iButtonSize;
			}

			ypos += m_iButtonSize;
		}
	}

	void CCreateMapDialog::SetPage(int iPage) {



		LoadMapButtonImagesForCurrentPage();
	}

	void CCreateMapDialog::LoadMapButtonImagesForCurrentPage() {
		/*IImage* defaultImage = scheme()->GetImage("maps/default", false);

		char buffer[128] = "maps/";
		char* start = buffer + 5;
		for (int i = 0; i < m_iNumMapButtons; i++) {
			CMapButton* b = m_pMapButtons[i];

			//ignore hidden buttons;
			if (!b->IsVisible()) break;

			Q_snprintf(start, 128 - 5, "%s", b->m_pszMapName);
			b->m_pMapImage = scheme()->GetImage(buffer, false);

			//if image is null, use default image
			if (!b->m_pMapImage) {
				b->m_pMapImage = defaultImage;
			}
		}*/
	}

#define MAPDIALOG_MIN_BUTTON_SIZE 
	void CCreateMapDialog::LoadMapDialogIdealButtonSizes() {

	}

	ConVar cl_default_mapname("cl_default_mapname", "bg_ambush", FCVAR_CLIENTDLL | FCVAR_ARCHIVE, 
		"Default map selected when the \"Create Game\" option is chosen in the main menu.");


	CMapButton*	g_pSelectedMapButton;
	CMapButton* g_pHoveredMapButton;
	CMapButton::CMapButton(Panel* parent, const char* panelName)
		: Button(parent, panelName, "") {

	}

	void CMapButton::OnMousePressed(MouseCode code) {
		NMenuSounds::PlaySelectSound();
		Select();
	}

	void CMapButton::Select() {
		g_pSelectedMapButton = this;
		g_pHoveredMapButton = this;
		//cl_default_mapname.SetValue(m_pMapInfo->m_pszMapName);
	}

	void CMapButton::OnCursorEntered() {
		//don't do anything if we're already selected
		if (g_pSelectedMapButton == this)
			return;

		g_pHoveredMapButton = this;
		NMenuSounds::PlayHoverSound();
	}

	void CMapButton::OnCursorExited() {
		g_pHoveredMapButton = g_pSelectedMapButton;
	}

	void CMapButton::Paint() {
		int w, h, x, y;
		GetPos(x, y);
		GetSize(w, h);
		/*IImage* image = m_pMapInfo->m_pMapImage;
		image->SetPos(x, y);
		image->SetSize(w, h);
		image->Paint();*/
	}

	CMapButton* CMapButton::ButtonForMap(const char* pszMapName) {
		int index = s_mMapButtonDict.Find(pszMapName);
		if (s_mMapButtonDict.IsValidIndex(index))
			return s_mMapButtonDict.Element(index);
		else
			return NULL;
	}

	CMapButton* CMapButton::ButtonDefaultMap() {
		CMapButton* defaultMap = ButtonForMap(cl_default_mapname.GetString());
		if (defaultMap)
			return defaultMap;
		else {
			CMapButton* pMap = ButtonForMap("bg_ambush");
			return pMap ? pMap : (s_mMapButtonDict.Count() > 0 ? s_mMapButtonDict.Element(0) : NULL);
		}
			
	}

	void CMapButton::ButtonDictionaryFlush() {
		s_mMapButtonDict.RemoveAll();
	}

	CUtlDict<CMapButton*> CMapButton::s_mMapButtonDict;
	
}

static KeyValues* kv = NULL;

CON_COMMAND(keyvalues_report, "") {
	if (args.ArgC() < 1)
		return;
	if (!kv)
		kv = new KeyValues("kv");
	
	kv->Clear();
	kv->LoadFromFile(filesystem, args[1]);
	for (KeyValues *pKey = kv->GetFirstSubKey(); pKey; pKey = pKey->GetNextKey()) {
		Msg( "Key name: %s\n", pKey->GetName() );
	}
}

CON_COMMAND(searchfile, "Given a search path, lists all files matching that path.") {


	FileFindHandle_t fileSearch;
	const char* str = filesystem->FindFirst(args[1], &fileSearch);
	Msg("%s\n", str);
	while (str) {
		str = filesystem->FindNext(fileSearch);
		Msg("%s\n", str);
	}
	filesystem->FindClose(fileSearch);
}