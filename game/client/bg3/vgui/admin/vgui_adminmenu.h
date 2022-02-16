//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef ADMINMENU_H
#define ADMINMENU_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui/IScheme.h>
#include <vgui/KeyCode.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ComboBox.h>

#include <game/client/iviewport.h>
#include <../../shared/bg3/bg3_player_shared.h>
#include "../shared/bg3/controls/bg3_voting_system_shared.h"
#include "bg3_admin_submenu.h"
#include <vector>

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

//------------------------------------------------------------------------------------------------------------------------
// Begin comm menu base code.
//------------------------------------------------------------------------------------------------------------------------
class CAdminMenu : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CAdminMenu, vgui::Panel);

public:
	CAdminMenu(vgui::Panel* parent);
	~CAdminMenu() {}

	virtual const char *GetName(void) { return PANEL_ADMIN; }

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme) override;
	virtual bool IsVisible() override { return BaseClass::IsVisible(); }
	vgui::VPANEL GetVPanel(void) override { return BaseClass::GetVPanel(); }
	virtual void SetParent(vgui::VPANEL parent) override { BaseClass::SetParent(parent); }

	vgui::Label*	m_pTitleLabel;
	vgui::Label*	m_pLabels[NUM_ADMIN_MENU_LABELS];
	vgui::Label*	m_pElectionThresholdLabel;
	void			SetFontsAllLabels(vgui::HFont font, vgui::HFont titleFont);

	//And here is the actual important BG3/Admin menu stuff
	CAdminSubMenu*					m_pCurrentMenu;
	CAdminSubMenu*					m_pElectionMenu; // a separate menu not part of the admin menu tree
	std::vector<CAdminSubMenu*>		m_menuStack; //so we can go back with the back button
	void ResetToMainMenu();
	void ForwardKeyToSubmenu(uint8 slot); //handles input. 0 = slot1, 1 = slot2, etc. 0 = slot 10
	void UpdateLabelsToMatchCurrentMenu();
	void SetVisible(bool bVisible) override;

	void PerformLayoutDefault();
	void PerformLayoutBinaryElection();
	void PerformLayoutMapChoice();

	void ResetLabelColors();

	void Paint() override;

	void ReceiveElectionMsg(bf_read& bf);

private:
	void ReceiveElectionOption(bf_read& bf, bool bTopic = false);
	void CreateElectionSubmenu();
	void ResetElection();
	void SetBinaryVotingOptions();
	void CalcWidestLabelWidth();
	void ScaleMenuSizeToContent(); //only use this when m_bElectionActive is true

	inline bool IsMapChoiceElection() const { return m_eLastElectionType == EVoteMessageType::TOPIC_MAPCHOICE; }

	int m_iNumElectionOptionsReceived;
	int m_iNumElectionOptions;

	float m_flNextAutoHide;
	bool m_bElectionActive;

	char m_electionTopic[128];
	char m_electionOptions[NUM_ADMIN_MENU_LABELS][128];
	uint8 m_electionVoteTallies[NUM_ADMIN_MENU_LABELS];
	uint8 m_iNumVotesReceived;
	uint8 m_iMinVotePercentage; //used for UI only on yes/no votes

	EVoteMessageType m_eLastElectionType; //used to determine which election GUI to use

	int m_iWidestLabelWidth; //used by vote tallies to determine base X position
	vgui::IImage* m_pTallymarkImage;

	bool m_bAdvancedMapElectionMenu; //whether or not to use the pictured menu; let smaller screens use text-based one
	int m_iHeightPerMapImage;
	inline int WidthPerMapImage() const { return 16 * m_iHeightPerMapImage / 9; }
	vgui::IImage* m_aMapImages[MAX_NUM_MULTICHOICE_MAP_OPTIONS];
	vgui::IImage* m_pTallyBgImage;
	vgui::IImage* m_pDefaultImage;

	vgui::HFont m_fontTitle;
	vgui::HFont m_fontLabel;
};

extern CAdminMenu* g_pAdminMenu;

#endif // ADMINMENU_H
