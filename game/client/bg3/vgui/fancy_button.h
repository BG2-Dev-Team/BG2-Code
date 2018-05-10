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

#ifndef BG3_FANCY_LABEL_H
#define BG3_FANCY_LABEL_H

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

	class ManualLabel;

	/************************************************************************
	* This class is a inherits from button to provide a fancy display with
	* a background image, a text shadow, and text
	************************************************************************/
	class FancyButton : public Button {
	public:
		DECLARE_CLASS_SIMPLE(FancyButton, Button);

		FancyButton(Panel *parent, const char *panelName, const char* image = "", const char* hoverImage = "");

		void Paint() override;
		virtual void ApplySchemeSettings(IScheme* scheme) override;

		virtual void SetVisible(bool bVisible) override;

		void SetCommandFunc(void(* pCommand)()) {
			m_pfCommand = pCommand;
		}
		virtual void OnMousePressed(MouseCode code);
		void OnCursorEntered() override;
		void OnCursorExited() override;

		void SetImage(const char* pszImage);
		void SetHoverImage(const char* pszImage);
		void SetText(const char* text) override;
		
		void SetSizeAll(int wide, int tall);
		void SetPosAll(int x, int y);
		void SetFont(HFont font) override;
		void SetContentAlignmentAll(Label::Alignment alignment);

		//for simplicity, these override current colors regardless
		//of whether the mouse is over us
		void SetColorFrontText(const Color& color);
		void SetColorFrontTextHighlight(const Color& color);
		//void SetColorShadowText(const Color& color);

		bool HasMouseOver() const { return m_bMouseOver; }
	private:
		IImage*			m_pCurrentImage = nullptr;
		IImage*			m_pNormalImage = nullptr;
		IImage*			m_pHoverImage;
		const char*		m_pszNormalImage;
		const char*		m_pszHoverImage;

		bool			m_bMouseOver;

		//ManualLabel*	m_pShadowText;

		Color m_cNormalColor;
		Color m_cHighlightColor;

		void(* m_pfCommand)() = nullptr;
		//void DoShadowTextOffset();
	};

	/************************************************************************
	* This class is identical to a label except that its paint function does
	* nothing; instead one must manully call ManualPaint() . This is done to
	* bypass the default vgui scheme, allowing for labels to be painted in
	* the proper order
	************************************************************************/
	class ManualLabel : public Label {
	public:
		ManualLabel(Panel* parent, const char* panelName, const char* text) : Label(parent, panelName, text) {}

		void Paint() override {}
		void ManualPaint() { Label::Paint(); }
	};
}

#endif