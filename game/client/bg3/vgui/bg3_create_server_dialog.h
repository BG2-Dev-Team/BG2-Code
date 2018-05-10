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

//---------------------------------------------------------------
// Purpose: These are the definitions of classes and functions
//		used in the dialog which creates a new local server
//---------------------------------------------------------------
namespace vgui {

	//---------------------------------------------------------------
	// Purpose: These buttons control the bot settings which are loaded
	//		when the player creates a local game.
	//---------------------------------------------------------------
	class	CBotButton : public ::vgui::Button {
	public:
		CBotButton(Panel* parent, int buttonCode);

		void OnCursorEntered(void) override;
		void OnCursorExited(void) override;
		void OnMousePressed(MouseCode code) override;
		void Paint() override;

	private:
		bool m_bMouseOver;
		int m_iButtonCode;

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
			
		CCreateMapDialog(Panel* pParent, const char* panelName, const CUtlVector<char*>& mapNames);
		void LoadMaplistFromFilesystem(CUtlVector<char*>&);
		void CreateLayoutForResolution();

	private:
		void LoadButtonImages();

		CMapButton**	m_pMapButtons;
		int				m_iNumMapButtons;
		CBotButton**	m_pBotButtons;
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
		CMapButton(Panel* parent, const char* panelName, const char* pszMapName);

		inline void	SimulateMousePressed() { OnMousePressed(MOUSE_LEFT); }
		void		OnMousePressed(MouseCode code); //selects this button, plays sound
		void		Select(); //does not play a sound
		void		OnCursorEntered(void);
		void		OnCursorExited(void);
		
		void		Paint() override;
		
		const char*	GetMapName();
		static CMapButton* ButtonForMap(const char* pszMapName); //Finds the button for given map
		static CMapButton* ButtonDefaultMap();

	private:
		const char* m_pszMapName;
		IImage*		m_pMapImage;
		static CUtlDict<CMapButton*> s_mMapButtonDict;
	};
}

#endif //BG3_CREATE_SERVER_DIALOG_H