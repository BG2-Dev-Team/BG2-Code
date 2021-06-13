//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include <cdll_client_int.h>
#include <globalvars_base.h>
#include <cdll_util.h>
#include <KeyValues.h>

#include <vgui/ILocalize.h>

#include <stdio.h> // _snprintf define

//#include "spectatorgui.h"
#include "vgui_adminmenu.h"
#include "vguicenterprint.h"

#include <game/client/iviewport.h>
#include "commandmenu.h"
#include "hltvcamera.h"	

#include "IGameUIFuncs.h" // for key bindings

//BG2 - HairyPotter
//#include "classmenu.h"
#include "bg3/bg3_classmenu.h"
#include "bg3/vgui/bg3_vgui_shared.h"
#include "bg2/commmenu2.h"
//

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;


CAdminMenu* g_pAdminMenu = NULL;
CAdminMenu::CAdminMenu(vgui::Panel* parent) : Panel(parent, PANEL_ADMIN)
{
	SetSize(640, 320);
	g_pAdminMenu = this;

	m_pTitleLabel = new Label(this, "adminmenutitle", "Admin Menu Title");
	m_pTitleLabel->SetSize(640, 60);
	m_pTitleLabel->SetPos(0, 0);

	int x = 10;
	int y = 55;
	for (int i = 0; i < NUM_ADMIN_MENU_LABELS; i++) {
		m_pLabels[i] = new Label(this, "adminmenuslot", "Dummy text");
		m_pLabels[i]->SetSize(620, 42);
		m_pLabels[i]->SetPos(x, y);
		y += 26;
	}
}

void CAdminMenu::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CAdminMenu::SetFontsAllLabels(HFont font, HFont titleFont) {
	m_pTitleLabel->SetFont(titleFont);
	for (int i = 0; i < NUM_ADMIN_MENU_LABELS; i++) {
		m_pLabels[i]->SetFont(font);
	}
}

void CAdminMenu::ResetToMainMenu() {
	//clear stack
	m_menuStack.clear();

	//set main menu as current
	m_menuStack.push_back(CAdminMainMenu::Get());//hook into main menu logic/page system);
	m_pCurrentMenu = CAdminMainMenu::Get();

	UpdateLabelsToMatchCurrentMenu();
}

void CAdminMenu::ForwardKeyToSubmenu(uint8 slot) {
	//ignore invalid slots
	if (slot >= 0 && m_pCurrentMenu && slot < m_pCurrentMenu->m_iNumChildren && m_pCurrentMenu->m_aChildren) {
		//perform the chosen slot's function
		bool goBack = false;
		if (m_pCurrentMenu->m_pszFunc) {
			auto func = m_pCurrentMenu->m_aChildren[slot]->m_pszFunc;
			goBack = func(slot, m_pCurrentMenu->m_aChildren[slot]);
		}
		
		if (goBack) {
			//pop top menu off stack
			m_menuStack.pop_back();

			//if we were in main menu and "going back", hide the menu
			if (m_menuStack.size() == 0) {
				SetVisible(false);
			}
		}
		else {
			m_menuStack.push_back(m_pCurrentMenu->m_aChildren[slot]);
		}

		//update to match end of stack
		m_pCurrentMenu = m_menuStack.back();
		UpdateLabelsToMatchCurrentMenu();
	}
}

void CAdminMenu::UpdateLabelsToMatchCurrentMenu() {
	//if current label has now children, then hide the menu
	if (m_pCurrentMenu->m_iNumChildren == 0) {
		SetVisible(false);
		return;
	}

	//set title
	m_pTitleLabel->SetText(m_pCurrentMenu->m_pszTitle);

	const int LABEL_BUFFER_SIZE = 128;
	wchar labelBuffer[LABEL_BUFFER_SIZE];

	//set slot labels
	int numLabelsVisible = m_pCurrentMenu->m_iNumChildren;
	for (int i = 0; i < NUM_ADMIN_MENU_LABELS; i++) {
		if (i < numLabelsVisible) {
			m_pLabels[i]->SetVisible(true);
			m_pCurrentMenu->m_aChildren[i]->getLineItemText(i, labelBuffer, LABEL_BUFFER_SIZE);
			m_pLabels[i]->SetText(labelBuffer);
		}
		else {
			m_pLabels[i]->SetVisible(false);
		}
	}
}

void CAdminMenu::SetVisible(bool bVisible) {
	//ignore non-changes
	if (bVisible == IsVisible())
		return;

	if (engine->IsPlayingDemo()) //Don't display when playing demos.
		return;

	//show
	if (bVisible) {
		//reset to main menu
		ResetToMainMenu();

		//hide other menus
		g_pCommMenu->SetVisible(false);
		g_pCommMenu2->SetVisible(false);
	}
	//hide
	else {
		//no special actions necessary... yet
	}

	BaseClass::SetVisible(bVisible);
}



