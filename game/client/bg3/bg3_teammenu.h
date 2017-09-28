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

#include <game/client/iviewport.h>

#include "hl2mp_gamerules.h" // BG2 - VisualMelon - moved from classmenu.cpp so we can get the ammo counts

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

	CTeamButton(Panel* parent, const char* panelName, const char* text, int team);

	void SetCommand(int team);
	void OnMousePressed(vgui::MouseCode code)	override;
	void OnCursorEntered(void)					override;

	//Function overrides from Button
	virtual void Paint() override;
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme)	override;
	virtual void ApplySettings(KeyValues* inResourceData)		override;

	//The paintings are in the background, we only need overlays to highlight/shadow them
	int			m_iTeam;
	const char* m_pszDarkenOverlay;
	const char* m_pszHighlightOverlay;
	bool		m_bMouseOver;

	inline vgui::IImage* GetImage() {
		const char* m_pszImage = m_bMouseOver ? m_pszHighlightOverlay : m_pszDarkenOverlay;
		return vgui::scheme()->GetImage(m_pszImage, false);
	}
};

//-----------------------------------------------------------------------------
// Purpose: This little label just says "Spectate" and calls the spectate command.
//-----------------------------------------------------------------------------
class CSpectateButton : public vgui::Button {
public:
	DECLARE_CLASS_SIMPLE(CSpectateButton, vgui::Button);

	CSpectateButton(Panel *parent, const char *panelName, const char *text) : Button(parent, panelName, text) { }

	void SetCommand(int command){}
	void OnMousePressed(vgui::MouseCode code);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	void OnCursorEntered(void){}
	void PerformCommand(void);
};

//-----------------------------------------------------------------------------
// Purpose: Randomly picks a valid team for the player
//-----------------------------------------------------------------------------
class CConscriptButton : public vgui::Button {
public:
	DECLARE_CLASS_SIMPLE(CConscriptButton, vgui::Button);

	CConscriptButton(Panel *parent, const char *panelName, const char *text) : Button(parent, panelName, text) { }

	void SetCommand(int command){}
	void OnMousePressed(vgui::MouseCode code);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	void OnCursorEntered(void){}
	void PerformCommand(void);
};

class CTeamMenu : public vgui::Frame, public IViewPortPanel {
	DECLARE_CLASS_SIMPLE(CTeamMenu, vgui::Frame);

public:
	CTeamMenu(IViewPort* pViewPort);
	~CTeamMenu();

	virtual const char* GetName(void)		override { return PANEL_TEAMS; }
	virtual void SetData(KeyValues* data)	override {}
	virtual void SetVisible(bool bVisible)	override ;
	virtual void Reset(void)				override {}
	virtual void Update(void)				override ;
	virtual bool NeedsUpdate(void)			override { return false; }
	virtual bool HasInputElements(void)		override { return true; }
	virtual void ShowPanel(bool bShow)		override ;
	virtual void Paint(void)				override ;

	// both vgui::Frame and IViewPortPanel define these, so explicitly define them here as passthroughs to vgui
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	vgui::VPANEL GetVPanel(void) { return BaseClass::GetVPanel(); }
	virtual void SetParent(vgui::VPANEL parent) { BaseClass::SetParent(parent); }

	//Team selection buttons
	CTeamButton			*m_pBritishButton,
						*m_pAmericanButton;
	CSpectateButton		*m_pSpectateButton;
	CConscriptButton	*m_pConscriptButton;

	vgui::HTML*			*m_pInfoHTML;
};

#endif //BG3_TEAMMENU_H