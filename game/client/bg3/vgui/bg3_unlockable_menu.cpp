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

using namespace vgui;

extern IImage* g_pHoverIcon;
extern IImage* g_pSelectionIcon;

CUnlockableButton::CUnlockableButton(Panel* pParent, const char* buttonName, UnlockableBit ulk) : Button(pParent, buttonName, "") {

	//load assigned static data
	m_pProfile = UnlockableProfile::get();
	m_pUnlockable = Unlockable::fromBitIndex(ulk);

	m_bMouseOver = false;
	
	UpdateUnlockableState();

	//load asset images
	char buffer[64];
	//Q_snprintf(buffer, sizeof(buffer), "unlockmenu/%s", m_pUnlockable->m_pszImagePath);
	Q_snprintf(buffer, sizeof(buffer), "unlockmenu/item_default");
	m_pUnlockableImage = scheme()->GetImage(buffer, false);
}

void CUnlockableButton::OnCursorEntered() {
	m_bMouseOver = true;
	g_pHoveredUnlockable = this;
}
void CUnlockableButton::OnCursorExited() {
	m_bMouseOver = false;
	g_pHoveredUnlockable = g_pSelectedUnlockable;
}

void CUnlockableButton::OnMousePressed(MouseCode code) {

	//Tell parent menu to focus on this unlockable
	CUnlockableMenu* pParent = (CUnlockableMenu*)GetParent();
	pParent->UnlockableButtonFocus(this);

	//if we're unlocked already, toggle ourselves
	if (m_bUnlocked) {
		//if unlockable is toggleable, toggle it
		if (m_pUnlockable->m_bToggleable) {
			m_bEnabled = m_pProfile->toggleItemActivation(m_pUnlockable);
		}
	}
}

void CUnlockableButton::ManualPaint() {
	m_pUnlockableImage->SetPos(GetXPos(), GetYPos());
	m_pUnlockableImage->SetSize(GetWide(), GetTall());
	m_pUnlockableImage->Paint();
}

void CUnlockableButton::UpdateUnlockableState() {
	m_bUnlocked = m_pProfile->isUnlockableUnlocked(m_pUnlockable);
	m_bEnabled = m_pProfile->isUnlockableActivated(m_pUnlockable);
}

CUnlockableButton* g_pHoveredUnlockable = NULL;
CUnlockableButton* g_pSelectedUnlockable = NULL;


CUnlockableMenu* g_pUnlockMenu = NULL;
CUnlockableMenu::CUnlockableMenu(Panel* parent, const char* panelName) : Panel(parent, panelName) {
	g_pUnlockMenu = this;
	m_pProfile = UnlockableProfile::get();

	//create unlockable children buttons
	for (uint8 i = 0; i < NUM_UNLOCKABLES; i++) {
		m_aButtons[i] = new CUnlockableButton(this, "ulkButton", (UnlockableBit)i);
	}

	//labels
	m_pUnlockableTitle = new Label(this, "unlockableTitle", "Unlockable Menu");
	m_pUnlockableDesc = new Label(this, "unlockableDesc", "Select an item to unlock or toggle!");
	m_pPointsRemainingCount = new Label(this, "unlockablePointCount", "");
	m_pPointsRemainingText = new Label(this, "unlockablePointText", "");

	/*m_pUnlockButton = new FancyButton(this, "unlockButton", "", "");
	m_pUnlockButton->SetCommandFunc([]() {
		g_pUnlockMenu->UnlockButtonPressed();
	});*/

	m_pBackground = scheme()->GetImage("unlockmenu/background", false);
}

void CUnlockableMenu::ApplySchemeSettings(IScheme* pScheme) {
	SetPaintBorderEnabled(false);

	//apply text colors
	m_pUnlockableTitle->SetFgColor(g_cBG3TextColor);
	m_pUnlockableDesc->SetFgColor(g_cBG3TextColor);
	m_pPointsRemainingCount->SetFgColor(g_cBG3TextColor);
	m_pPointsRemainingText->SetFgColor(g_cBG3TextColor);

	//TODO set fonts!
}

void CUnlockableMenu::Paint() {
	//paint background
	m_pBackground->Paint();

	//paint child buttons
	for (uint8 i = 0; i < (uint8)NUM_UNLOCKABLES; i++) {
		m_aButtons[i]->ManualPaint();
	}

	//paint hover/select overlays
	if (g_pHoveredUnlockable) {
		g_pHoverIcon->SetPos(g_pHoveredUnlockable->GetXPos(), g_pHoveredUnlockable->GetYPos());
		g_pHoverIcon->SetSize(g_pHoveredUnlockable->GetWide(), g_pHoveredUnlockable->GetTall());
		g_pHoverIcon->Paint();
	}
	if (g_pSelectedUnlockable) {
		g_pSelectionIcon->SetPos(g_pSelectedUnlockable->GetXPos(), g_pSelectedUnlockable->GetYPos());
		g_pSelectionIcon->SetSize(g_pSelectedUnlockable->GetWide(), g_pSelectedUnlockable->GetTall());
		g_pSelectionIcon->Paint();
	}
}

void CUnlockableMenu::PerformLayout() {
	//center on screen, 800x600 fixed for now
	const int width = 800;
	const int height = 600;

	SetPos(ScreenWidth() / 2 - width / 2, ScreenHeight() / 2 - height / 2);
	SetSize(width, height);

	//Buttons
	for (uint8 i = 0; i < (uint8)NUM_UNLOCKABLES; i++) {
		//uniform size
		m_aButtons[i]->SetSize(100, 60);

		//assuming unlockables are already sorted tier-wise
		uint8 tier = i / 5; //5 items per tier, tiers are 0,1,2
		uint8 rowPos = i - (tier * 5); //are we first in the row or last?

		m_aButtons[i]->SetPos(50 + rowPos * 120, 120 + 80 * tier);
	}

	//Labels
	m_pUnlockableTitle->SetPos(0, 0);
	m_pUnlockableTitle->SetSize(width, 50);
	m_pUnlockableDesc->SetPos(0, 50);
	m_pUnlockableDesc->SetSize(width, 50);

	m_pPointsRemainingCount->SetPos(0, height - 50);
	m_pPointsRemainingCount->SetSize(50, 50);
	m_pPointsRemainingText->SetPos(50, height - 50);
	m_pPointsRemainingText->SetSize(300, 50);
}

void CUnlockableMenu::UnlockableButtonFocus(CUnlockableButton* pButton) {
	g_pSelectedUnlockable = pButton;
	
	m_pUnlockableTitle->SetText(pButton->GetUnlockable()->m_pszImagePath);
	m_pUnlockableDesc->SetText(pButton->GetUnlockable()->m_pszImagePath);

	//TODO decide whether or not to show unlock button
}

void CUnlockableMenu::UnlockButtonPressed() {
	//attempt unlocking the selected item
	m_pProfile->unlockItem(g_pSelectedUnlockable->GetUnlockable());
}