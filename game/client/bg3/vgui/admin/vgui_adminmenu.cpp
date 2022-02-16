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
#include "../shared/bg3/bg3_map_model.h"

#include "bg2/commmenu2.h"
#include "bg3/bg3_teammenu.h"
//
#include "bg3/vgui/bg3_fonts.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


using namespace vgui;

#define ELECTION_OPTION_LABEL_X_COORD 10
#define ADMIN_LABEL_HEIGHT 42


CAdminMenu* g_pAdminMenu = NULL;
CAdminMenu::CAdminMenu(vgui::Panel* parent) : Panel(parent, PANEL_ADMIN)
{
	g_pAdminMenu = this;

	ResetElection();

	m_pTitleLabel = new Label(this, "adminmenutitle", "Admin Menu Title");

	m_pElectionThresholdLabel = new Label(this, "voteThreshold", "Min Vote %");

	for (int i = 0; i < NUM_ADMIN_MENU_LABELS; i++) {
		m_pLabels[i] = new Label(this, "adminmenuslot", "Dummy text");
	}

	m_pElectionMenu = new CAdminSubMenu("", m_electionTopic);
	m_bAdvancedMapElectionMenu = false;

	PerformLayoutDefault();
}

void CAdminMenu::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	m_pTallymarkImage = scheme()->GetImage("checkmark", false);
	SetBgColor(Color(0,0,0, 160));
}

void CAdminMenu::SetFontsAllLabels(HFont font, HFont titleFont) {
	m_fontTitle = titleFont;
	m_fontLabel = font;
	m_pTitleLabel->SetFont(titleFont);
	m_pElectionThresholdLabel->SetFont(font);
	for (int i = 0; i < NUM_ADMIN_MENU_LABELS; i++) {
		m_pLabels[i]->SetFont(font);
	}
}

void CAdminMenu::ResetToMainMenu() {
	ResetLabelColors();

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
		else if (!m_bElectionActive) {
			m_menuStack.push_back(m_pCurrentMenu->m_aChildren[slot]);
		}

		//update to match end of stack, but only if we're not in an election
		//bcs in an election we don't go into any submenus, and we don't close
		//until told to by server
		if (!m_bElectionActive) {
			m_pCurrentMenu = m_menuStack.back();
			UpdateLabelsToMatchCurrentMenu();
		}
	}
}

void CAdminMenu::UpdateLabelsToMatchCurrentMenu() {
	//if current label has no children, then hide the menu
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
		m_bAdvancedMapElectionMenu = false; //default to false, then check otherwise

		//decide on layout
		switch (m_eLastElectionType) {
		case EVoteMessageType::TOPIC_MAPCHOICE:
			//base layout on resolution
			if (ScreenHeight() < 768 || ScreenWidth() < 1024) {
				PerformLayoutDefault();
			}
			else {
				PerformLayoutMapChoice();
				m_bAdvancedMapElectionMenu = true;
			}

			break;
		case EVoteMessageType::TOPIC_BINARY:
		case EVoteMessageType::TOPIC_MAP:
		case EVoteMessageType::TOPIC_PLAYER:
			PerformLayoutBinaryElection();
			break;
		default:
			PerformLayoutDefault();
			break;
		}

		if (!m_bElectionActive) {
			//reset to main menu if we're not voting
			ResetToMainMenu();
		}

		//hide other menus
		g_pCommMenu->SetVisible(false);
		g_pCommMenu2->SetVisible(false);
	}
	//hide
	else {
		//if the menu was hidden for an active election, for any reason, reset the election
		if (m_bElectionActive) {
			m_bElectionActive = false; //WARNING, we MUST set this to false first, or else ResetElection() will call this function with SetVisible(false), entering a recursion loop, be careful!
			ResetElection();
		}
			

		//no special actions necessary... yet
	}

	BaseClass::SetVisible(bVisible);
}

void CAdminMenu::PerformLayoutDefault() {
	SetFontsAllLabels(m_fontLabel, m_fontTitle);
	SetPos(100, 180);
	SetSize(640, 320);

	m_pTitleLabel->SetSize(640, 60);
	m_pTitleLabel->SetPos(0, 0);

	m_pElectionThresholdLabel->SetVisible(false);

	int x = ELECTION_OPTION_LABEL_X_COORD;
	int y = 55;
	for (int i = 0; i < NUM_ADMIN_MENU_LABELS; i++) {
		m_pLabels[i]->SetSize(620, ADMIN_LABEL_HEIGHT);
		m_pLabels[i]->SetPos(x, y);
		y += 26;
	}
}

void CAdminMenu::PerformLayoutBinaryElection() {
	PerformLayoutDefault(); //this will only be slightly different from default

	m_pElectionThresholdLabel->SetSize(640, ADMIN_LABEL_HEIGHT);
	m_pElectionThresholdLabel->SetPos(0, 55);
	m_pElectionThresholdLabel->SetVisible(true);

	Q_snprintf(g_hudBuffer, sizeof(g_hudBuffer), "(%i%% required)", m_iMinVotePercentage);
	m_pElectionThresholdLabel->SetText(g_hudBuffer);

	int x = ELECTION_OPTION_LABEL_X_COORD;
	int y = 90;
	for (int i = 0; i < 2; i++) {
		m_pLabels[i]->SetPos(x, y);
		m_pLabels[i]->SetSize(620, 60);
		m_pLabels[i]->SetFont(m_fontTitle);
		y += 50;
	}

	ScaleMenuSizeToContent();
}

void CAdminMenu::PerformLayoutMapChoice() {
	SetPos(90, 100);

	//determine ideal map image size based on screen height and lots of constants...
	int margintop; //how much space between top of screen and where the first image can be
	int marginbottom; //how much space between bottom of screen and where fifth image can end
	int marginbetween; //how much space between images for all the labels


	margintop = 100 + m_pTitleLabel->GetYPos() + m_pTitleLabel->GetTall();
	marginbottom = 195 + ADMIN_LABEL_HEIGHT; //hardcoded based on CHudBG2::ApplySettings
	marginbetween = 3 * ADMIN_LABEL_HEIGHT; //total of three labels in between 5 images, the last one is beneath the last image

	m_iHeightPerMapImage = 
		(ScreenHeight() - (margintop + marginbottom + marginbetween)) 
		/ MAX_NUM_MULTICHOICE_MAP_OPTIONS;

	m_pElectionThresholdLabel->SetVisible(false);

	//calculate map images based on map names
	m_pDefaultImage = scheme()->GetImage("mapvote/default", false);
	for (int i = 0; i < m_iNumElectionOptions; i++) {
		if (!CMapInfo::MapImageExists(m_electionOptions[i])) {
			m_aMapImages[i] = m_pDefaultImage;
		}
		else {
			Q_snprintf(g_hudBuffer, sizeof(g_hudBuffer), "mapvote/%s", m_electionOptions[i]);
			m_aMapImages[i] = scheme()->GetImage(g_hudBuffer, false);
		}
	}
	m_pTallyBgImage = scheme()->GetImage("mapvote/tally_bg", false);
	

	//more spaced-out-coodinates
	int x = 0;
	int y = m_pTitleLabel->GetYPos() + m_pTitleLabel->GetTall() + m_iHeightPerMapImage;
	for (int i = 0; i < MAX_NUM_MULTICHOICE_MAP_OPTIONS; i++) {
		//double column layout
		if (i == MAX_NUM_MULTICHOICE_MAP_OPTIONS / 2) {
			x = ELECTION_OPTION_LABEL_X_COORD + WidthPerMapImage();
			y = m_pTitleLabel->GetYPos() + m_pTitleLabel->GetTall() + m_iHeightPerMapImage;
		}

		m_pLabels[i]->SetFont(m_fontLabel);
		m_pLabels[i]->SetSize(WidthPerMapImage() + ELECTION_OPTION_LABEL_X_COORD * 2, ADMIN_LABEL_HEIGHT);
		m_pLabels[i]->SetPos(x, y);
		y += m_iHeightPerMapImage + ADMIN_LABEL_HEIGHT;
		SetDefaultBG3FontScaled(scheme()->GetIScheme(GetScheme()), m_pLabels[i]);
	}
	
	//limit height based on number of options
	int gridHeight = min(3, m_iNumElectionOptions);
	int gridWidth = 1 + (m_iNumElectionOptions > (MAX_NUM_MULTICHOICE_MAP_OPTIONS / 2));
	SetSize(gridWidth * ELECTION_OPTION_LABEL_X_COORD + gridWidth * WidthPerMapImage(),
		m_pTitleLabel->GetYPos() + m_pTitleLabel->GetTall() + gridHeight * (m_iHeightPerMapImage + ADMIN_LABEL_HEIGHT));

	//now set title label width and scale its font too
	m_pTitleLabel->SetWide(GetWide());
	SetDefaultBG3FontScaled(scheme()->GetIScheme(GetScheme()), m_pTitleLabel);
}

void CAdminMenu::ResetLabelColors() {
	for (int i = 0; i < NUM_ADMIN_MENU_LABELS; i++) {
		m_pLabels[i]->SetFgColor(COLOR_OFF_WHITE);
	}
}

void CAdminMenu::Paint() {
	//determine whether or not we need to delayed-hide ourselves
	if (gpGlobals->curtime > m_flNextAutoHide) {
		SetVisible(false);
		m_flNextAutoHide = FLT_MAX;
		ResetElection();
	}
	else {
		//if we're showing the election menu, show tally marks and possibly map images
		if (m_pCurrentMenu == m_pElectionMenu) {
			int xBase, xSpace;
			xBase = ELECTION_OPTION_LABEL_X_COORD + m_iWidestLabelWidth;

			//calculate tally mark size based on label size
			int tsize = m_pLabels[0]->GetTall() * .48f;
			m_pTallymarkImage->SetSize(tsize, tsize);

			//map choice voting layout, paint these first so tally marks can go on top
			if (m_bAdvancedMapElectionMenu) {
				int width = xSpace = WidthPerMapImage();

				for (int i = 0; i < m_iNumElectionOptions; i++) {
					m_aMapImages[i]->SetSize(width, m_iHeightPerMapImage);
					m_aMapImages[i]->SetPos(m_pLabels[i]->GetXPos() + ELECTION_OPTION_LABEL_X_COORD, m_pLabels[i]->GetYPos() - m_iHeightPerMapImage);
					m_aMapImages[i]->Paint();
					m_pTallyBgImage->SetSize(width, tsize);
					m_pTallyBgImage->SetPos(m_pLabels[i]->GetXPos() + ELECTION_OPTION_LABEL_X_COORD, m_pLabels[i]->GetYPos() - tsize);
					m_pTallyBgImage->Paint();
				}
			}
			//default voting layout
			else {
				xSpace = GetWide() - xBase;
			}
			
			
			
			for (int i = 0; i < m_iNumElectionOptions; i++) {
			
				int y;
				if (m_bAdvancedMapElectionMenu)
					y = m_pLabels[i]->GetYPos() - tsize / 2;
				else
					y = m_pLabels[i]->GetYPos() + tsize / 2;

				//x icrement between tallies is dependent on the amount of space we have in this row
				//if there's not enough space for full spacing, we'll cram them on top of each other
				int tallyTotal = m_electionVoteTallies[i];
				int xIncrement = tsize;
				if (xIncrement * tallyTotal > xSpace) {
					xIncrement = xSpace / tallyTotal;
				}

				for (int tally = 0; tally < tallyTotal; tally++) {
					if (m_bAdvancedMapElectionMenu)
						m_pTallymarkImage->SetPos(m_pLabels[i]->GetXPos() + tally * xIncrement, y);
					else
						m_pTallymarkImage->SetPos(xBase + tally * xIncrement, y);
					m_pTallymarkImage->Paint();
				}	
			}
		}
			
		BaseClass::Paint();
	}
}

void CAdminMenu::ReceiveElectionMsg(bf_read& bf) {
	
	EVoteMessageType type = (EVoteMessageType)bf.ReadByte();

	//remember our last new election type
	if (type >= EVoteMessageType::TOPIC) {
		m_eLastElectionType = type;
	}
	
	uint8 slot;
	int playerSlot;
	switch (type) {
	case EVoteMessageType::CANCEL:
		ResetElection();
		break;

	case EVoteMessageType::SEND_OPTION:
		ReceiveElectionOption(bf);
		break;

	//both of these below are handled the same at logical level, only different graphically later on
	case EVoteMessageType::TOPIC_MAPCHOICE:
	case EVoteMessageType::TOPIC:
		ReceiveElectionOption(bf, true);
		break;

	case EVoteMessageType::TOPIC_BINARY:
		m_bElectionActive = true;
		m_iMinVotePercentage = bf.ReadByte();
		bf.ReadString(m_electionTopic, sizeof(m_electionTopic), true);
		SetBinaryVotingOptions();
		CreateElectionSubmenu();
		break;

	case EVoteMessageType::TOPIC_PLAYER:
		m_bElectionActive = true;
		m_iMinVotePercentage = bf.ReadByte();
		playerSlot = bf.ReadByte();
		bf.ReadString(m_electionTopic, sizeof(m_electionTopic), true);
		if (g_PR->IsConnected(playerSlot)) {
			Q_snprintf(g_hudBuffer, sizeof(m_electionTopic), g_pVGuiLocalize->FindAsUTF8(m_electionTopic), g_PR->GetPlayerName(playerSlot));
			strcpy_s(m_electionTopic, g_hudBuffer);
		}
		else {
			Warning("Voting system could not find player to match slot %i\n", playerSlot);
			return;
		}
		SetBinaryVotingOptions();
		CreateElectionSubmenu();
		break;

	case EVoteMessageType::TOPIC_MAP:
		m_bElectionActive = true;
		m_iMinVotePercentage = bf.ReadByte();
		bf.ReadString(m_electionTopic, sizeof(m_electionTopic), true); //get map name from buffer
		Q_snprintf(g_hudBuffer, sizeof(m_electionTopic), g_pVGuiLocalize->FindAsUTF8("#BG3_Vote_Create_ChangeMap"), m_electionTopic);
		strcpy_s(m_electionTopic, g_hudBuffer);
		SetBinaryVotingOptions();
		CreateElectionSubmenu();
		break;

	case EVoteMessageType::RESULT:
		slot = bf.ReadByte();
		m_pLabels[slot]->SetFgColor(COLOR_GREEN);
		m_flNextAutoHide = gpGlobals->curtime + 4.f;
		NMenuSounds::PlayDrumSound();
		break;

	case EVoteMessageType::VOTE_TALLY:
		slot = bf.ReadByte();
		m_iNumVotesReceived++;
		m_electionVoteTallies[slot]++;
		NMenuSounds::PlaySelectSound();

	default:
		break;
	}
}

void CAdminMenu::ReceiveElectionOption(bf_read& bf, bool bTopic) {
	//if we're not in an active election, put us in one
	if (!m_bElectionActive) {
		m_bElectionActive = true;
		m_iNumElectionOptionsReceived = 0;
		memset(m_electionOptions, 0, sizeof(m_electionOptions));
	}

	//either we're receiving an option, or we're being notified that options are to come
	if (bTopic) {
		m_iNumElectionOptions = bf.ReadByte();
		bf.ReadString(m_electionTopic, sizeof(m_electionTopic), true);
	}
	else {
		m_iNumElectionOptionsReceived++;

		//copy our received option into the buffer
		uint8 slot = bf.ReadByte();
		bf.ReadString(m_electionOptions[slot], 128, true);
	}

	//if we've received them all, create the submenu and show the user
	if (m_iNumElectionOptionsReceived == m_iNumElectionOptions) {
		CreateElectionSubmenu();
	}
}

static char g_pszVoteCommand[8] = "vote 0\0";


void CAdminMenu::CreateElectionSubmenu() {
	//if we haven't created the menu yet, create a raw version we can then fill in with specifics
	//for the current vote
	//the children aren't deleted until the player exits the game, so the below code will only ever happen once
	if (m_pElectionMenu->m_aChildren == NULL) {
		//per-line-item voting function
		auto voteFunc = [](uint8 slot, CAdminSubMenu*){
			//the server side vote command expects "vote 1" to be voting for the 'first' option
			//so we have to bump up "vote 0" to "vote 1" etc., and "vote 9" becomes "vote 0"
			//this system is done this way so that players can execute the commands directly
			//with "vote 1" in chat etc.

			slot++;
			if (slot == 10) slot = 0; //0 serves as the tenth slot at index 9
			g_pszVoteCommand[5] = '0' + slot; //set correct slot in vote command
			engine->ClientCmd(g_pszVoteCommand);
			return false;
		};

		//create functional menu to be interpreted in UpdateLabelsToMatchCurrentMenu
		CAdminSubMenu** pChildren = m_pElectionMenu->m_aChildren = new CAdminSubMenu*[NUM_ADMIN_MENU_LABELS];
		m_pElectionMenu->m_iNumChildren = NUM_ADMIN_MENU_LABELS;
		for (int i = 0; i < NUM_ADMIN_MENU_LABELS; i++) {
			pChildren[i] = new CAdminSubMenu(m_electionOptions[i], ""); //HACK HACK election option text is already persistent here, so might as well point to it instead of copying around
			pChildren[i]->m_pszFunc = voteFunc;
		}
	}

	//limit the menu to thinking it only has as many options as we've received
	//for this election, even if junk options from prior elections still exist
	//in later options
	m_pElectionMenu->m_iNumChildren = m_iNumElectionOptions;
	//for the text for each option, each submenu already points to its assigned position in m_electionOPtions, see several lines above

	ResetLabelColors();

	//clear stack
	m_menuStack.clear();

	//set voting menu as current
	m_menuStack.push_back(m_pElectionMenu);//hook into main menu logic/page system);
	m_pCurrentMenu = m_pElectionMenu;

	UpdateLabelsToMatchCurrentMenu();

	//everything should be ready, now show ourselves
	SetVisible(true);
	CalcWidestLabelWidth(); //setting to visible can change layout, so only calc this after

	//play sound
	NMenuSounds::PlayHoverSound();

	//set the auto-hide to a time in the future, in case we don't receive the message telling us the election is over
	extern ConVar sv_vote_duration;
	m_flNextAutoHide = gpGlobals->curtime + 5.f + sv_vote_duration.GetFloat();
}

void CAdminMenu::ResetElection() {
	if (m_bElectionActive) {
		m_bElectionActive = false;
		SetVisible(false); //WARNING! SetVisible(...) CAN call this function, ResetElection, so be careful when modifying these calls
	}

	m_flNextAutoHide = FLT_MAX;
	m_iNumElectionOptions = -1;
	m_iNumElectionOptionsReceived = 0;
	m_iNumVotesReceived = 0;
	m_iMinVotePercentage = 0;
	m_eLastElectionType = EVoteMessageType::NONE;
	m_iWidestLabelWidth = 0;

	memset(m_electionTopic, 0, sizeof(m_electionTopic));
	memset(m_electionOptions, 0, sizeof(m_electionOptions));
	memset(m_electionVoteTallies, 0, sizeof(m_electionVoteTallies));
}

void CAdminMenu::SetBinaryVotingOptions() {
	strcpy_s(m_electionOptions[0], "#BG3_VoiceComm_A1"); //"Aye!"
	strcpy_s(m_electionOptions[1], "#BG3_VoiceComm_A2"); //"No!"
	m_iNumElectionOptions = m_iNumElectionOptionsReceived = 2;
}

void CAdminMenu::CalcWidestLabelWidth() {
	m_iWidestLabelWidth = 0;
	for (int i = 0; i < m_iNumElectionOptions; i++) {
		int w, h;
		m_pLabels[i]->GetContentSize(w, h);
		if (w > m_iWidestLabelWidth) {
			m_iWidestLabelWidth = w;
		}
	}
}

void CAdminMenu::ScaleMenuSizeToContent() {
	//title label and option labels - determine which extends farthest to the left, excluding unfilled space
	if (m_iWidestLabelWidth == 0)
		CalcWidestLabelWidth();

	int widestLabel = ELECTION_OPTION_LABEL_X_COORD + m_iWidestLabelWidth + 200; //leave 200 pixels for vote tallies

	int w, h;
	m_pTitleLabel->GetContentSize(w, h);
	int titleLabelWidth = w + m_pTitleLabel->GetXPos();

	if (titleLabelWidth > widestLabel)
		widestLabel = titleLabelWidth;

	//final, calculate height
	int maxy = 
		m_pLabels[m_iNumElectionOptions - 1]->GetYPos() 
		+ m_pLabels[m_iNumElectionOptions - 1]->GetTall() 
		+ 10;

	SetSize(widestLabel, maxy);
}
