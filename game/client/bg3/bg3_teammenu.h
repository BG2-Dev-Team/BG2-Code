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

#ifndef BG3_TEAMMENU_H
#define BG3_TEAMMENU_H

#ifdef _WIN32
#pragma once
#endif

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
//#include <game_controls/vguitextwindow.h>
#include "vgui/bg3_motd.h"
#include "vgui/timed_label.h"
#include <game/client/iviewport.h>

#include "hl2mp_gamerules.h" // BG2 - VisualMelon - moved from classmenu.cpp so we can get the ammo counts

#define TEMP_HTML_FILE	"textwindow_temp.html"

class KeyValues;

namespace vgui
{
	class TextEntry;
	class Button;
	class Panel;
	class ImagePanel;
	class ComboBox;
}

class IBaseFileSystem;

//-----------------------------------------------------------------------------
// Purpose: Performs operations and overlays for the two big team buttons
//-----------------------------------------------------------------------------
class CTeamButton : public vgui::Button {
	DECLARE_CLASS_SIMPLE(CTeamButton, vgui::Button);

	CTeamButton(Panel* parent, const char* panelName, const char* text, int team) : Button(parent, panelName, text), m_iTeam(team) {}

	void SimulateMousePressed() { OnMousePressed(MOUSE_LEFT); }
	void OnMousePressed(vgui::MouseCode code)	override;
	void OnCursorEntered(void)					override;
	void OnCursorExited(void)					override;

	//Function overrides from Button
	//virtual void Paint() override;
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme)	override;

	void PerformCommand();

private:
	friend class CTeamMenu;
	//The paintings are in the background, we only need overlays to highlight/shadow them
	int			m_iTeam;

	bool		m_bEnabled = true;
	bool		m_bMouseOver = false;

public:
	void UpdateGameRulesData();
};

//-----------------------------------------------------------------------------
// Purpose: This little label just says "Spectate" and calls the spectate command.
//-----------------------------------------------------------------------------
class CSpectateButton : public vgui::Button {
public:
	DECLARE_CLASS_SIMPLE(CSpectateButton, vgui::Button);

	CSpectateButton(Panel *parent, const char *panelName, const char *text) : Button(parent, panelName, text) { }
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

	void OnMousePressed(vgui::MouseCode code);
	void OnCursorEntered(void);
	//void OnCursorExited(void);

private:
};

//-----------------------------------------------------------------------------
// Purpose: Randomly picks a valid team for the player
//-----------------------------------------------------------------------------
class CConscriptButton : public vgui::Button {
public:
	DECLARE_CLASS_SIMPLE(CConscriptButton, vgui::Button);

	CConscriptButton(Panel *parent, const char *panelName, const char *text) : Button(parent, panelName, text) { }

	void OnMousePressed(vgui::MouseCode code)					override;
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme)	override;
	void OnCursorEntered(void)									override;
	//void OnCursorExited(void)									override;
private:
	friend class CTeamMenu;
};

//-----------------------------------------------------------------------------
// Purpose: Wraps together all of the team menu buttons
//-----------------------------------------------------------------------------
class CTeamMenu : public vgui::Frame, public IViewPortPanel {
	DECLARE_CLASS_SIMPLE(CTeamMenu, vgui::Frame);

public:
	CTeamMenu(vgui::VPANEL pParent);
	~CTeamMenu();

	virtual const char* GetName(void)		override { return PANEL_TEAMS; }
	virtual void SetVisible(bool bVisible)	override ;
	virtual void Reset(void)				override { m_pMOTD->ManualReset(); }
	virtual void Update(void)				override ;
	virtual bool NeedsUpdate(void)			override { return m_flNextGameRulesUpdate < gpGlobals->curtime; }
	virtual bool HasInputElements(void)		override { return true; }
	virtual void ShowPanel(bool bShow)		override ;
	virtual void Paint(void)				override ;
	virtual void SetData(KeyValues* kv)		override ;
	// both vgui::Frame and IViewPortPanel define these, so explicitly define them here as passthroughs to vgui
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	vgui::VPANEL GetVPanel(void) { return BaseClass::GetVPanel(); }
	virtual void SetParent(vgui::VPANEL parent) { BaseClass::SetParent(parent); }

	virtual void OnKeyCodePressed(vgui::KeyCode code) override {
		if (code == KEY_ESCAPE)
			ShowPanel(false);
		else
			BaseClass::OnKeyCodePressed(code);
	}

	inline void	 SwitchToClassmenu() {
		//ShowPanel(false); //this should be done at the class menu command
							//because class menu needs to know whether or not the teammenu
							//was just visible
		engine->ClientCmd("classmenu");
	}
	//Checks if we can join the team OPPOSITE to the one given.
	inline bool	 MayJoinOtherTeam(int iCurentTeam) {
		if (iCurentTeam == TEAM_AMERICANS)
			return s_pBritishButton->m_bEnabled;
		else
			return s_pAmericanButton->m_bEnabled;
	}

	//choose team label
	vgui::Label*		m_pChooseTeamLabel;

	vgui::Label*		m_pAmericanLabel;
	vgui::Label*		m_pBritishLabel;

	vgui::TimedLabel*	m_pTeamFullLabel;

	//Team selection buttons
	static CTeamButton	*s_pBritishButton,
						*s_pAmericanButton;
	CSpectateButton		*m_pSpectateButton;
	CConscriptButton	*m_pConscriptButton;

	CMOTDHTML			*m_pMOTD;

	vgui::IImage		*m_pBackground,
						*m_pLeftHighlight,
						*m_pRightHighlight,
						*m_pLeftDarken,
						*m_pRightDarken;

private:
	// VGUI2 overrides
	//virtual void OnKeyCodePressed(vgui::KeyCode code);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void PerformLayout();

	float m_flNextGameRulesUpdate = 0.0f;
};

extern int					g_iCurrentTeammenuTeam;
extern CTeamMenu*			g_pTeamMenu;

namespace NMenuSounds {
	void PlayMenuSound(const char* pszSound);

	void PlayHoverSound();
	void PlaySelectSound();
}

#endif //BG3_TEAMMENU_H