#include "cbase.h"
#include "bg3/bg3_teammenu.h"
#include "bg3_create_server_dialog.h"

namespace vgui {
	//---------------------------------------------------------------
	// Purpose: These buttons control the bot settings which are loaded
	//		when the player creates a local game.
	//---------------------------------------------------------------
	ConVar bot_cl_default("bot_cl_default", "1", FCVAR_ARCHIVE | FCVAR_CLIENTDLL,
		"Default setting for bot button in Create Server dialog. -1 = none, 0-3 match bot_difficulty",
		true, -1.0f, true, 3.0f);


	CBotButton::CBotButton(Panel* parent, int buttonCode) :
		Button(parent, "BotButton", ""),
		m_iButtonCode(buttonCode),
		m_bMouseOver(false) {}

	void CBotButton::OnCursorEntered() {
		m_bMouseOver = true;
	}
	void CBotButton::OnCursorExited() {
		m_bMouseOver = false;
	}
	void CBotButton::OnMousePressed(MouseCode code) {
		bot_cl_default.SetValue(m_iButtonCode);
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
	IImage* CBotButton::s_pImageNormal = NULL;
	IImage* CBotButton::s_pImageHover = NULL;

	//---------------------------------------------------------------
	// Purpose: This is the panel to which all the map buttons and other
	//	buttons and labels are parented.
	//---------------------------------------------------------------
#define NUM_BOT_BUTTONS 5
	CCreateMapDialog::CCreateMapDialog(Panel* pParent, const char* panelName, const CUtlVector<char*>& mapNames)
	: Panel(pParent, panelName)	{

		//create map buttons
		m_pMapButtons = new CMapButton*[mapNames.Count()];
		m_iNumMapButtons = mapNames.Count();
		char buffer[8];
		for (int i = 0; i < m_iNumMapButtons; i++) {
			Q_snprintf(buffer, 8, "mapb%3i", i);
			m_pMapButtons[i] = new CMapButton(this, buffer, mapNames[i]);
		}

		//select default map
		CMapButton* pDefaultMap = CMapButton::ButtonDefaultMap();
		if (pDefaultMap)
			pDefaultMap->Select();

		//create bot buttons
		m_pBotButtons = new CBotButton*[NUM_BOT_BUTTONS];
		for (int i = 0; i < NUM_BOT_BUTTONS; i++) {
			m_pBotButtons[i] = new CBotButton(this, i - 1);
		}
	}

	void CCreateMapDialog::LoadMaplistFromFilesystem(CUtlVector<char*>& mapList) {
		mapList.RemoveAll();
		CUtlBuffer buf;
		if (!filesystem->ReadFile("maplist.txt", "GAME", buf))
			return;
		buf.PutChar(0);
		V_SplitString((char*)buf.Base(), "\n", mapList);
	}

	void CCreateMapDialog::CreateLayoutForResolution() {

	}

	void CCreateMapDialog::LoadButtonImages() {
		char buffer[128] = "maps/";
		char* start = buffer + 5;
		for (int i = 0; i < m_iNumMapButtons; i++) {
			CMapButton* b = m_pMapButtons[i];
			Q_snprintf(start, 128 - 5, "%s", b->m_pszMapName);
			b->m_pMapImage = scheme()->GetImage(buffer, false);
		}
	}

	ConVar cl_default_mapname("cl_default_mapname", "bg_ambush", FCVAR_CLIENTDLL | FCVAR_ARCHIVE, 
		"Default map selected when the \"Create Game\" option is chosen in the main menu.");


	CMapButton*	g_pSelectedMapButton;
	CMapButton* g_pHoveredMapButton;
	CMapButton::CMapButton(Panel* parent, const char* panelName, const char* pszMapName)
		: Button(parent, panelName, "") {

	}

	void CMapButton::OnMousePressed(MouseCode code) {
		NMenuSounds::PlaySelectSound();
		Select();
	}

	void CMapButton::Select() {
		g_pSelectedMapButton = this;
		g_pHoveredMapButton = this;
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
		m_pMapImage->SetPos(x, y);
		m_pMapImage->SetSize(w, h);
		m_pMapImage->Paint();
	}

	const char* CMapButton::GetMapName() {
		return m_pszMapName;
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
		else
			return ButtonForMap("bg_ambush");
	}

	CUtlDict<CMapButton*> CMapButton::s_mMapButtonDict;
	
}