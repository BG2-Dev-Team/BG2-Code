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
#include "fancy_button.h"
#include "bg3_fonts.h"
#include "bg3/bg3_teammenu.h"

using namespace NMenuSounds;

namespace vgui {

	FancyButton::FancyButton(Panel* parent, const char* panelName, const char* image, const char* hoverImage) : BaseClass(parent, panelName, ""){
		SetImage(image);
		SetHoverImage(hoverImage);
		m_pCurrentImage = m_pNormalImage;
		
		m_bMouseOver = false;

		//I'm using new here, woopee!
		/*int nameLength = strlen(panelName);
		int shadowNameLength = nameLength + 8;
		char* shadowName = new char[shadowNameLength];
		Q_snprintf(shadowName, shadowNameLength, "%s_shadow\0", panelName);
		m_pShadowText = new ManualLabel(parent, shadowName, "");
		delete shadowName;*/

		
		//SetText(text);
		//m_pShadowText->SetMouseInputEnabled(false);
	}

	void FancyButton::Paint() {
		//let's paint things in the right order
		//paint image first
		if (m_bMouseOver)
			m_pCurrentImage = scheme()->GetImage(m_pszHoverImage, false);
		else
			m_pCurrentImage = scheme()->GetImage(m_pszNormalImage, false);

		int x, y, w, h;
		GetPos(x, y); GetSize(w, h);
		m_pCurrentImage->SetPos(0, 0);
		m_pCurrentImage->SetSize(w, h);
		m_pCurrentImage->Paint();

		//paint shadow text manually here
		//if (m_pShadowText->IsVisible())
			//m_pShadowText->ManualPaint();

		//now paint our own text
		BaseClass::Paint();
	}

	void FancyButton::SetImage(const char* image) {
		m_pszNormalImage = image;
		m_pNormalImage = scheme()->GetImage(image, false);
		if (!m_pCurrentImage)
			m_pCurrentImage = m_pNormalImage;
	}

	void FancyButton::SetHoverImage(const char* image) {
		m_pszHoverImage = image;
		m_pHoverImage = scheme()->GetImage(image, false);
		if (!m_pCurrentImage)
			m_pCurrentImage = m_pHoverImage;
	}

	void FancyButton::ApplySchemeSettings(IScheme* scheme) {
		BaseClass::ApplySchemeSettings(scheme);
		SetPaintBorderEnabled(false);
		SetPaintBackgroundEnabled(false);

		SetColorFrontTextHighlight(g_cBG3TextHighlightColor);
		SetColorFrontText(g_cBG3TextColor);

		SetImage(m_pszNormalImage);
		SetHoverImage(m_pszHoverImage);

		//m_pShadowText->SetPaintBorderEnabled(false);
		//m_pShadowText->SetPaintBackgroundEnabled(false);

		//m_pShadowText->SetVisible(false);

		//DoShadowTextOffset();
	}

	void FancyButton::SetVisible(bool bVisible) {
		BaseClass::SetVisible(bVisible);
		SetImage(m_pszNormalImage);
		SetHoverImage(m_pszHoverImage);
		//m_pShadowText->SetVisible(bVisible);
	}

	void FancyButton::OnCursorEntered() {
		m_bMouseOver = true;
		SetFgColor(m_cHighlightColor);
		m_pCurrentImage = m_pHoverImage;
		PlayMenuSound("Mainmenu.Hover");
		//m_pShadowText->SetVisible(true);
	}

	void FancyButton::OnCursorExited() {
		m_bMouseOver = false;
		SetFgColor(m_cNormalColor);
		m_pCurrentImage = m_pNormalImage;
		//m_pShadowText->SetVisible(false);
	}

	void FancyButton::SetText(const char* text) {
		wchar * wtext = g_pVGuiLocalize->Find(text);
		BaseClass::SetText(wtext);
		//m_pShadowText->SetText(wtext);
	}

	void FancyButton::SetSizeAll(int wide, int tall) {
		BaseClass::SetSize(wide, tall);
		//m_pShadowText->SetSize(wide, tall);

		//image size is set in Paint() in case image is reused
	}

	void FancyButton::SetPosAll(int x, int y) {
		BaseClass::SetPos(x, y);

		//shadow text if offset from main text
		//DoShadowTextOffset();
		//pos of image is set in Paint() in case image is reused
	}

	void FancyButton::SetFont(HFont font) {
		BaseClass::SetFont(font);
		//m_pShadowText->SetFont(font);
	}

	void FancyButton::SetContentAlignmentAll(Label::Alignment alignment) {
		BaseClass::SetContentAlignment(alignment);
		//m_pShadowText->SetContentAlignment(alignment);
	}

	void FancyButton::SetColorFrontText(const Color& color) {
		m_cNormalColor = color;
		SetFgColor(color);
	}

	void FancyButton::SetColorFrontTextHighlight(const Color& color) {
		m_cHighlightColor = color;
		SetFgColor(color);
	}

	/*void FancyButton::SetColorShadowText(const Color& color) {
		//m_pShadowText->SetFgColor(color);
	}

	void FancyButton::DoShadowTextOffset() {
		int x, y;
		GetPos(x, y);
		m_pShadowText->SetPos(x + 1, y + 1);
	}*/
}