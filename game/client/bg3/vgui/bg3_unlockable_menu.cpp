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
#include "bg3_unlockable_menu.h"

#include "bg3/vgui/bg3_fonts.h"
#include "bg3/bg3_teammenu.h"

using namespace vgui;
using namespace NMenuSounds;

extern IImage* g_pHoverIcon;
extern IImage* g_pSelectionIcon;
IImage* g_pDarkenTexture;
IImage* g_pLockIcon;
static IImage* g_pCheckIcon;
static IImage* g_pXIcon;
static IImage* g_pRationsIcon;

CUnlockableButton::CUnlockableButton(Panel* pParent, const char* buttonName, UnlockableBit ulk) : Button(pParent, buttonName, "") {
	SetPaintBorderEnabled(false);

	//load assigned static data
	m_pProfile = UnlockableProfile::get();
	m_pUnlockable = Unlockable::fromBitIndex(ulk);

	m_bMouseOver = false;
	
	UpdateUnlockableState();

	//load asset images
	char buffer[64];
	Q_snprintf(buffer, sizeof(buffer), "unlockmenu/%s", m_pUnlockable->m_pszImagePath);
	//Q_snprintf(buffer, sizeof(buffer), "unlockmenu/item_default");
	m_pUnlockableImage = scheme()->GetImage(buffer, false);
}

void CUnlockableButton::OnCursorEntered() {
	PlayHoverSound();
	m_bMouseOver = true;
	g_pHoveredUnlockable = this;
}
void CUnlockableButton::OnCursorExited() {
	m_bMouseOver = false;
	g_pHoveredUnlockable = g_pSelectedUnlockable;
}

void CUnlockableButton::OnMousePressed(MouseCode code) {
	bool bPlaySelectSound = true;
	

	//Tell parent menu to focus on this unlockable
	CUnlockableMenu* pParent = (CUnlockableMenu*)GetParent();
	pParent->UnlockableButtonFocus(this);

	//if we're unlocked already, toggle ourselves
	if (m_bUnlocked) {
		//if unlockable is toggleable, toggle it
		if (m_pUnlockable->m_bToggleable) {
			m_bEnabled = m_pProfile->toggleItemActivation(m_pUnlockable);
			bPlaySelectSound = m_bEnabled;
			UnlockableProfile::get()->sendSettingsToServer();
		}
	}

	if (bPlaySelectSound) PlaySelectSound();
	else PlayDenySound();
}

void CUnlockableButton::ManualPaint() {
	m_pUnlockableImage->SetPos(GetXPos(), GetYPos());
	m_pUnlockableImage->SetSize(GetWide(), GetTall());
	m_pUnlockableImage->Paint();
}

void CUnlockableButton::OverlayPaint() {
	IImage* statusIcon = null;

	//lock icon
	if (!m_bUnlocked) {
		//darkening
		if (g_pHoveredUnlockable != this && g_pSelectedUnlockable != this) {
			g_pDarkenTexture->SetPos(GetXPos(), GetYPos());
			g_pDarkenTexture->SetSize(GetWide(), GetTall());
			g_pDarkenTexture->Paint();
		}
		statusIcon = g_pLockIcon;
	}
	else if (m_pUnlockable->m_bToggleable) {
		if (m_bEnabled) {
			statusIcon = g_pCheckIcon;
		}
		else {
			statusIcon = g_pXIcon;
		}
	}
	
	if (statusIcon) {
		int h = GetTall() * .35f;
		int w = statusIcon == g_pLockIcon ? (h * .6f) : h;
		statusIcon->SetSize(w, h);
		statusIcon->SetPos(GetXPos() + h * .6f, GetYPos() + GetTall() - h * .8f);
		statusIcon->Paint();
	}
	
}

void CUnlockableButton::UpdateUnlockableState() {
	m_bUnlocked = m_pProfile->isUnlockableUnlocked(m_pUnlockable);
	m_bEnabled = m_pProfile->isUnlockableActivated(m_pUnlockable);
}

CUnlockableButton* g_pHoveredUnlockable = NULL;
CUnlockableButton* g_pSelectedUnlockable = NULL;


//-----------------------------------------------------------------------------
// Unlock button unlocks the selected unlockable
//-----------------------------------------------------------------------------
CUnlockActionButton::CUnlockActionButton(vgui::Panel* pParent, const char* buttonName) : Button(pParent, buttonName, "") {
	SetPaintBorderEnabled(false);
	SetPaintBackgroundEnabled(false);
	SetContentAlignment(Label::Alignment::a_center);

	m_pProfile = UnlockableProfile::get();

	SetText("Unlock this item by trading in 1");
	m_bEnabled = false;
	m_bHover = false;
	m_pBackground = scheme()->GetImage("unlockmenu/background", false);
}

void CUnlockActionButton::OnCursorEntered() {
	m_bHover = true;
	if (m_bEnabled) PlayHoverSound();
}
void CUnlockActionButton::OnCursorExited() {
	m_bHover = false;
}
void CUnlockActionButton::OnMousePressed(vgui::MouseCode code) {
	if (!m_bEnabled || !m_bHover || !g_pSelectedUnlockable) {
		PlayDenySound();
		return;
	}

	if (code == MouseCode::MOUSE_LEFT) {
		PlayDrumSound();

		Unlockable* pUnlockable = g_pSelectedUnlockable->GetUnlockable();
		m_pProfile->unlockItem(pUnlockable);

		//update data everywhere here to reflect the change
		UpdateData();
		g_pSelectedUnlockable->UpdateUnlockableState();
		g_pUnlockableMenu->UpdateToMatchProfile();
		UnlockableProfile::get()->sendSettingsToServer();
	}
}
void CUnlockActionButton::ManualPaint() {
	m_pBackground->SetSize(GetWide(), GetTall());
	m_pBackground->SetPos(GetXPos(), GetYPos());
	m_pBackground->Paint();

	if (m_bEnabled) {
		if (m_bHover) {
			g_pHoverIcon->SetSize(GetWide(), GetTall());
			g_pHoverIcon->SetPos(GetXPos(), GetYPos());
			g_pHoverIcon->Paint();
		}
	}
	else {
		g_pDarkenTexture->SetPos(GetXPos(), GetYPos());
		g_pDarkenTexture->SetSize(GetWide(), GetTall());
		g_pDarkenTexture->Paint();
	}
	if (!m_bComingSoon) {
		g_pRationsIcon->SetPos(rationx, rationy);
		g_pRationsIcon->Paint();
	}
}

void CUnlockActionButton::UpdateData() {
	if (g_pSelectedUnlockable) {
		Unlockable* ulk = g_pSelectedUnlockable->GetUnlockable();
		m_bEnabled = (m_pProfile->getPointsAvailable() >= ulk->getCost()
			&& !m_pProfile->isUnlockableUnlocked(ulk)
			&& !ulk->isDisabled());

		m_bComingSoon = ulk->isDisabled();

		if (m_bComingSoon) {
			SetText("Coming soon!");
		}
		else {
			char buffer[64];
			Q_snprintf(buffer, sizeof(buffer), "Unlock this item by trading in %i", (int)ulk->getCost());
			SetText(buffer);
			CalculateRationPos();
		}
	}
	else {
		m_bEnabled = false;
	}
}

void CUnlockActionButton::CalculateRationPos() {
	//do a temporary size to contents to determine our right side limit
	int x, y, w, h;
	GetSize(w, h);
	GetPos(x, y);
	SizeToContents();
	rationx = GetXPos() + GetWide() * 1.5f;
	rationy = GetYPos() - GetTall() * .05f;
	SetSize(w, h);
	SetPos(x, y);
}

CUnlockableMenu* g_pUnlockableMenu = NULL;
CUnlockableMenu::CUnlockableMenu(Panel* parent, const char* panelName) : Panel(parent, panelName) {
	g_pUnlockableMenu = this;
	m_pProfile = UnlockableProfile::get();

	//create unlockable children buttons
	for (uint8 i = 0; i < NUM_UNLOCKABLES; i++) {
		m_aButtons[i] = new CUnlockableButton(this, "ulkButton", (UnlockableBit)i);
	}

	m_pUnlockActionButton = new CUnlockActionButton(this, "ulkActionButton");

	//labels
	m_pUnlockableTitle = new Label(this, "unlockableTitle", "Unlockable Menu");
	m_pUnlockableDesc = new Label(this, "unlockableDesc", "Select an item to unlock or toggle!");
	m_pPointsRemainingCount = new Label(this, "unlockablePointCount", "7");
	m_pPointsRemainingText = new Label(this, "unlockablePointText", "");

	/*m_pUnlockButton = new FancyButton(this, "unlockButton", "", "");
	m_pUnlockButton->SetCommandFunc([]() {
		g_pUnlockMenu->UnlockButtonPressed();
	});*/

	m_pBackground = scheme()->GetImage("unlockmenu/background", false);
	m_pBackgroundOverlay = scheme()->GetImage("unlockmenu/tier_overlay", false);
	g_pDarkenTexture = scheme()->GetImage("locked_overlay", false);

	g_pLockIcon = scheme()->GetImage("lock_icon", false);
	g_pCheckIcon = scheme()->GetImage("checkmark", false);
	g_pXIcon = scheme()->GetImage("xmark", false);
	g_pRationsIcon = scheme()->GetImage("unlockmenu/rations", false);

	SetPaintBorderEnabled(false);

	//apply text colors
	m_pUnlockableTitle->SetFgColor(g_cBG3TextColor);
	m_pUnlockableDesc->SetFgColor(g_cBG3TextColor);
	m_pPointsRemainingCount->SetFgColor(g_cBG3TextColor);
	m_pPointsRemainingText->SetFgColor(g_cBG3TextColor);
	m_pPointsRemainingText->SetText("#BG3_UnlockMenu_Points_Remaining");
	m_pPointsRemainingText->SetContentAlignment(Label::Alignment::a_west);
}

void CUnlockableMenu::ApplySchemeSettings(IScheme* pScheme) {
	
	//TODO set fonts!
	SetDefaultBG3FontScaled(pScheme, m_pUnlockableTitle);

	SetDefaultBG3FontScaled(pScheme, m_pPointsRemainingCount);
	//SetDefaultBG3FontScaledVertical(pScheme, m_pUnlockableDesc);
	SetDefaultBG3FontScaled(pScheme, m_pPointsRemainingText);
	SetDefaultBG3FontScaled(pScheme, m_pUnlockActionButton);
}

void CUnlockableMenu::Paint() {
	//paint background
	m_pBackgroundOverlay->Paint();

	//paint child buttons
	for (uint8 i = 0; i < (uint8)NUM_UNLOCKABLES; i++) {
		m_aButtons[i]->ManualPaint();
	}

	m_pUnlockActionButton->ManualPaint();

	//paint hover/select overlays
	if (g_pHoveredUnlockable) {
		g_pHoverIcon->SetPos(g_pHoveredUnlockable->GetXPos(), g_pHoveredUnlockable->GetYPos());
		g_pHoverIcon->SetSize(g_pHoveredUnlockable->GetWide(), g_pHoveredUnlockable->GetTall());
		g_pHoverIcon->Paint();
	}
	if (g_pSelectedUnlockable) {
		g_pHoverIcon->SetPos(g_pSelectedUnlockable->GetXPos(), g_pSelectedUnlockable->GetYPos());
		g_pHoverIcon->SetSize(g_pSelectedUnlockable->GetWide(), g_pSelectedUnlockable->GetTall());
		g_pHoverIcon->Paint();
	}

	//paint per-button things that go over the hover/selection overlays, ex. lock icon
	for (uint8 i = 0; i < (uint8)NUM_UNLOCKABLES; i++) {
		m_aButtons[i]->OverlayPaint();
	}

	//ration icon next to count
	Label* p = m_pPointsRemainingCount;
	g_pRationsIcon->SetPos(p->GetXPos() + p->GetWide() / 2, p->GetYPos());
	g_pRationsIcon->Paint();
}

void CUnlockableMenu::PerformLayout() {

	//assume 1024 pixels wide, then scale based on that
	const int width = ScreenWidth();
	const int height = ScreenHeight() - 50;

	//use the smaller of the two scales to ensure everything fits
	float scale = 1.f;
	{
		const float scalex = ScreenWidth() / 1365.f;
		//const float scaley = ScreenHeight() / 768.f;
		//scale = scalex < scaley ? scaley : scalex;
		scale = scalex;
	}

	SetPos(0,0);
	SetSize(width, height);

	
	const int bw = 180 * scale;
	const int bh = 90 * scale;
	const int spacingx = bw + (25 * scale);
	const int spacingy = bh + (25 * scale);
	const int bmy = 170 * scale;

	//Buttons
	for (uint8 i = 0; i < (uint8)NUM_UNLOCKABLES; i++) {
		//uniform size
		m_aButtons[i]->SetSize(bw, bh);

		//assuming unlockables are already sorted tier-wise
		uint8 tier = i / 5; //5 items per tier, tiers are 0,1,2
		uint8 rowPos = i - (tier * 5); //are we first in the row or last?

		m_aButtons[i]->SetPos(rowPos * spacingx + spacingx, bmy + spacingy * tier);
	}

	//const int completewidth = spacingx * 5;

	//unlock action button
	m_pUnlockActionButton->SetSize(bw * 3, bh * 0.8f);
	m_pUnlockActionButton->SetPos(spacingx * 2, bmy + 3 * spacingy);
	m_pUnlockActionButton->CalculateRationPos();
	g_pRationsIcon->SetSize(bh * .8f, bh);

	//Labels
	const int mx = 20 * scale;
	const int my = 20 * scale;
	m_pUnlockableTitle->SetPos(mx, my);
	m_pUnlockableTitle->SetSize(width, 70 * scale);
	m_pUnlockableDesc->SetPos(mx, my + 70 * scale);
	m_pUnlockableDesc->SetSize(width, 50 * scale);

	m_pPointsRemainingCount->SetPos(mx, bmy + 3 * spacingy);
	m_pPointsRemainingCount->SetSize(80 * scale, 80 * scale);
	m_pPointsRemainingText->SetPos(mx + 140 * scale, bmy + 3 * spacingy + 15 * scale);
	m_pPointsRemainingText->SetSize(300, 60);

	m_pBackgroundOverlay->SetPos(mx, my + 75 * scale);
	int w = 1250 * scale;
	m_pBackgroundOverlay->SetSize(w, w * 0.38f);

	
}

void CUnlockableMenu::UnlockableButtonFocus(CUnlockableButton* pButton) {
	g_pSelectedUnlockable = pButton;

	char buffer[32];
	Q_snprintf(buffer, sizeof(buffer), "#%s_Title", pButton->GetUnlockable()->m_pszImagePath);
	m_pUnlockableTitle->SetText(buffer);

	Q_snprintf(buffer, sizeof(buffer), "#%s_Desc", pButton->GetUnlockable()->m_pszImagePath);
	m_pUnlockableDesc->SetText(buffer);

	//decide whether or not to enable unlock button
	m_pUnlockActionButton->UpdateData();
}

void CUnlockableMenu::UnlockButtonPressed() {
	//attempt unlocking the selected item
	m_pProfile->unlockItem(g_pSelectedUnlockable->GetUnlockable());
	UpdateToMatchProfile();
}

void CUnlockableMenu::UpdateToMatchProfile() {
	char buffer[5];
	Q_snprintf(buffer, sizeof(buffer), "%i", m_pProfile->getPointsAvailable());
	//Msg("Setting points remaining to %s\n", buffer);
	m_pPointsRemainingCount->SetText(buffer);
	//m_pPointsRemainingCount->SizeToContents();
}

void CUnlockableMenu::SetVisible(bool bVisible) {
	BaseClass::SetVisible(bVisible);
	if (bVisible) {
		UpdateToMatchProfile();
		m_pUnlockActionButton->UpdateData();
		for (int i = 0; i < (int)NUM_UNLOCKABLES; i++) {
			m_aButtons[i]->UpdateUnlockableState();
		}
	}
}