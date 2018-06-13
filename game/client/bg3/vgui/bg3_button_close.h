#ifndef BG3_BUTTON_CLOSE
#define BG3_BUTTON_CLOSE

#include <vgui/IScheme.h>
#include <vgui/KeyCode.h>
#include <vgui/MouseCode.h>
#include <vgui_controls/HTML.h>
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


namespace vgui {
	class CCloseButton : public ::vgui::Button {
	public:
		CCloseButton(Panel* pParent, const char* buttonName, const char* pszImgName);

		void OnMousePressed(MouseCode code) override;
		void Paint() override;
		void OnCursorEntered() override { m_bMouseOver = true; }
		void OnCursorExited() override { m_bMouseOver = false; }

	private:
		union {
			IViewPortPanel* m_pViewportParent;
			Panel* m_pPanelParent;
		};
		bool	m_bParentIsViewport;
		IImage* m_pImage;
		bool	m_bMouseOver;
	};
}

#endif //BG3_BUTTON_CLOSE