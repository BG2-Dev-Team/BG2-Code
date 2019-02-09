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

#ifndef BG3_CREATE_SERVER_DIALOG_H
#define BG3_CREATE_SERVER_DIALOG_H

#include <vgui/IScheme.h>
#include <vgui/KeyCode.h>
#include <vgui/MouseCode.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/ToggleButton.h>
#include <vgui_controls/CheckButton.h>
#include "vgui_bitmapimage.h"
#include "vgui_bitmapbutton.h"
#include <vgui_controls/ImagePanel.h>
#include <imagemouseoverbutton.h>

#include <game/client/iviewport.h>

#include "fancy_button.h"

//---------------------------------------------------------------
// Purpose: These are the definitions of classes and functions
//		used in the dialog which creates a new local server
//---------------------------------------------------------------
namespace vgui {

	

	//---------------------------------------------------------------
	// Purpose: These buttons control the bot settings which are loaded
	//		when the player creates a local game.
	//---------------------------------------------------------------
	class	CBotButton;
	extern	CBotButton* g_pSelectedBotButton;
	class	CBotButton : public ::vgui::Button {
	public:
		enum class setting {
			NONE = -2,
			RANDOM,
			EASY,
			MED,
			HARD,

			NUM_SETTINGS = HARD - NONE + 1,
		};

		CBotButton(Panel* parent, int buttonCode, setting botSetting);

		void OnCursorEntered(void) override;
		void OnCursorExited(void) override;
		void OnMousePressed(MouseCode code) override;
		void Paint() override;

		void Select();
		void ExecuteCommand();

		static const wchar* GetBotSettingLabelText();
		static void			LoadDefaults();
	private:
		bool	m_bMouseOver;
		int		m_iButtonCode;
		setting m_eBotSetting;

		static IImage* s_pImageNormal;
		static IImage* s_pImageHover;
	};

	//---------------------------------------------------------------
	// Purpose: This is the panel to which all the map buttons and other
	//	buttons and labels are parented.
	//---------------------------------------------------------------
	class	CCreateMapDialog;
	class	CMapButton;
	extern	CCreateMapDialog* g_pCreateMapDialog;
	class	CCreateMapDialog : public ::vgui::Panel {
		
		friend class CBotButton;

	public:
		//constructor loads everything and sets us to the first page
		//calls CreateLayoutForResolution()
		CCreateMapDialog(Panel* pParent, const char* panelName);

		//Refreshes the map list according to current settings, nothing more (does no GUI operations)
		void RefreshMapList();

		//loads the positions and sizes of everything, calls SetupPages(), and sets current page to the first page
		void CreateLayoutForResolution();

		//sets the page to the given page
		//@param iPage - page to set to.
		//@requires 0 <= iPage < NumPages()
		void SetPage(int iPage);
		inline int NumPages() const { return m_iNumPages; }

	private:
		//loads map list into given vector
		static void LoadMaplistFromFilesystem(CUtlVector<char*>&);

		void LoadMapButtonImagesForCurrentPage();
		void LoadMapDialogIdealButtonSizes();

		uint8				FindPageForDefaultMap() const;

		CMapButton**		m_pMapButtons;
		CBotButton**		m_pBotButtons;
		Label*				m_pBotButtonLabel;

		//CUtlVector<char*>	m_aMapNames;

		//page system
		uint8				m_iCurrentPage;
		uint8				m_iNumPages;
		uint8				m_iButtonsPerRow;
		uint8				m_iNumRows;
		uint8				m_iNumMapButtons;
		uint16				m_iButtonSize;

		FancyButton*		m_pGoButton;
	};


	//---------------------------------------------------------------
	// Purpose: These are the buttons which display & referene each
	//		map in the game.
	//---------------------------------------------------------------
	class	CMapButton;
	extern	CMapButton* g_pSelectedMapButton;
	extern	CMapButton* g_pHoveredMapButton; //just storing one reference will improve performance, rather than having each button check on its own
	extern	ConVar		cl_default_mapname;
	class	CMapButton : public ::vgui::Button {
		friend class CCreateMapDialog;
	public:
		CMapButton(Panel* parent, const char* panelName);

		inline void	SimulateMousePressed() { OnMousePressed(MOUSE_LEFT); }
		void		OnMousePressed(MouseCode code); //selects this button, plays sound
		void		Select(); //does not play a sound
		void		OnCursorEntered(void);
		void		OnCursorExited(void);
		
		void		Paint() override;
		

		static void			ButtonDictionaryLoad(CUtlVector<char*>& aMapNames, uint8 start, uint8 xEns);
		static CMapButton*	ButtonForMap(const char* pszMapName); //Finds the button for given map
		static CMapButton*	ButtonDefaultMap();
		static void			ButtonDictionaryFlush();
		

	private:
		static CUtlDict<CMapButton*> s_mMapButtonDict;
	};
}

#endif //BG3_CREATE_SERVER_DIALOG_H