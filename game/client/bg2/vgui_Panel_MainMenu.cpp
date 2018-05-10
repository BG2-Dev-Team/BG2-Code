//========= Copyright © 2006, Valve Productions, All rights reserved. ============//
//
// Purpose: Display Main Menu images, handles rollovers as well
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "vgui_Panel_MainMenu.h"
#include "vgui_controls/Frame.h"
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui/IInput.h>

#include "vgui_controls/Button.h"
#include "vgui_controls/ImagePanel.h"
#include "iclientmode.h"
#include "baseviewport.h"
#include "game_controls/vguitextwindow.h"
#include "../bg3/bg3_hint.h"
#include "../../shared/bg3/Math/bg3_rand.h"
#include "bg3/vgui/fancy_button.h"
#include "bg3/vgui/bg3_fonts.h"
#include "bg3/bg3_teammenu.h"

using namespace vgui;
using namespace NMenuSounds;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

const int buttonstarty=30;

class CMainMenu;
CMainMenu* g_pMainMenu = nullptr;

//-----------------------------------------------------------------------------
// Purpose: Displays the logo panel
//-----------------------------------------------------------------------------
class CMainMenu : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CMainMenu, vgui::Frame);
 
public:
	CMainMenu( vgui::VPANEL parent );
	~CMainMenu();
 
	virtual void OnCommand(const char *command);
 
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme )
	{
		BaseClass::ApplySchemeSettings( pScheme );

#define set(a) a->SetFont(GetDefaultBG3FontScaledHorizontal(pScheme, a))
		set(m_pButtonBegin);
		set(m_pButtonCreateDisconnect);
		set(m_pButtonOptions);
		set(m_pButtonSave);
		set(m_pButtonLeave);
#undef set
	}
 
	// The panel background image should be square, not rounded.
	virtual void PaintBackground()
	{
		SetBgColor(Color(0,0,0,0));
		SetPaintBackgroundType( 0 );
		BaseClass::PaintBackground();
	}
	virtual void PerformLayout()
	{
		// re-position
		//SetPos(vgui::scheme()->GetProportionalScaledValue(defaultX), vgui::scheme()->GetProportionalScaledValue(defaultY));
 
		BaseClass::PerformLayout();

		// BG2 - VisualMelon - Update on resolution change
		PerformDefaultLayout();
		//
	}
	void CMainMenu::PerformDefaultLayout()
	{
		const int narr = 5;
		int x;
		//int verx;
		//verx = 296;
		x = 270;
		vgui::FancyButton *buttarr[narr];
		int width = this->GetWide();
		
		/*imgarr[0] = m_pImgBegin;
		imgarr[1] = m_pImgCreateDisconnect;
		imgarr[2] = m_pImgSave;
		imgarr[3] = m_pImgOptions;
		imgarr[4] = m_pImgLeave;*/
		buttarr[0] = m_pButtonBegin;
		buttarr[1] = m_pButtonCreateDisconnect;
		buttarr[2] = m_pButtonSave;
		buttarr[3] = m_pButtonOptions;
		buttarr[4] = m_pButtonLeave;
		
		// BG2 - VisualMelon - fudge button/img widths for small screens
		int biWide = (width * 2) / (narr * 2 + 1); // fit narr things in space of narr+0.5
		if (biWide > 256)
			biWide = 256;

		int biTall = 64;
		int space = width - (narr * biWide);
		int border = space / (2*narr);
		for(int i = 0; i < narr; i++)
		{
			buttarr[i]->SetPosAll((border + (i * space) / narr + biWide * i), buttonstarty);
			buttarr[i]->SetSizeAll(biWide, biTall);
		}

		InRolloverBegin=false;
		InRolloverCreateDisconnect=false;
		InRolloverSave=false;
		InRolloverOptions=false;
		InRolloverLeave=false;

		// community feed
		int communityBorderX = 40; // left, right
		int communityBorderY = 20; // bottom
		int communityY = buttonstarty+70;

		m_pCommunity->SetSize(this->GetWide() - communityBorderX * 2, this->GetTall() - (communityY + communityBorderY));
		m_pCommunity->SetPos(communityBorderX, communityY);
	}
 
	virtual void OnThink()
	{
		//Warning("Performing main menu layout!\n");
		// In-game, everything will be in different places than at the root menu!
		if (InGame() && !InGameLayout) {
			m_pButtonCreateDisconnect->SetCommand("Disconnect");
			PerformDefaultLayout();
			m_pCommunity->SetVisible(false);
		}
		if (!InGame() && InGameLayout)
		{
			m_pButtonCreateDisconnect->SetCommand("OpenCreateMultiplayerGameDialog");
			PerformDefaultLayout();
			m_pCommunity->SetVisible(true);
		}
 
		// Get mouse coords
		int x,y;
		vgui::input()->GetCursorPos(x,y);
 
		int fx,fy; // frame xpos, ypos
 
		GetPos(fx,fy);
 
		if (InGame())
			m_pButtonCreateDisconnect->SetText("#BG3_Main_Disconnect");
		else
			m_pButtonCreateDisconnect->SetText("#BG3_Main_Create_Server");

		/*CheckRolloverBegin(x,y,fx,fy);
		CheckRolloverCreateDisconnect(x,y,fx,fy);
		CheckRolloverSave(x,y,fx,fy);
		CheckRolloverOptions(x,y,fx,fy);
		CheckRolloverLeave(x,y,fx,fy);*/

		InGameLayout = InGame();
 
		BaseClass::OnThink();		
	}
 
	void CheckRolloverBegin(int x,int y, int fx, int fy)
	{
		int bx,by,bw,bh; // button xpos, ypos, width, height
 
		m_pButtonBegin->GetPos(bx,by);
		m_pButtonBegin->GetSize(bw,bh);
 
		bx = bx+fx; // xpos for button (rel to screen)
		by = by+fy; // ypos for button (rel to screen)
 
		// Check and see if mouse cursor is within button bounds
		if ((x > bx && x < bx+bw) && (y > by && y < by+bh))
		{
			if(!InRolloverBegin) {
				//m_pImgBegin->SetImage("find_server_over");
				InRolloverBegin = true;
			}
		} else {
			if(InRolloverBegin) {
				//m_pImgBegin->SetImage("find_server_normal");
				InRolloverBegin = false;
			}
		}
	}
 
	// BG2 - VisualMelon - I've butchered this a bit so it also takes care of changing the disconnect image
	void CheckRolloverCreateDisconnect(int x,int y, int fx, int fy)
	{
		if(m_pButtonCreateDisconnect->IsVisible()) {
			int bx,by,bw,bh; // button xpos, ypos, width, height
 
			m_pButtonCreateDisconnect->GetPos(bx,by);
			m_pButtonCreateDisconnect->GetSize(bw,bh);
 
			bx = bx+fx; // xpos for button (rel to screen)
			by = by+fy; // ypos for button (rel to screen)
 
			// Check and see if mouse cursor is within button bounds
			if ((x > bx && x < bx+bw) && (y > by && y < by+bh))
			{
				if (!InRolloverCreateDisconnect || InGame() != InGameLayout) {
					if (InGame())
						m_pButtonCreateDisconnect->SetText("#BG3_Main_Disconnect");
					else
						m_pButtonCreateDisconnect->SetImage("#BG3_Main_Create_Server");
					InRolloverCreateDisconnect = true;
				}
			} else {
				if (InRolloverCreateDisconnect || InGame() != InGameLayout) {
					/*if (InGame()
						//m_pImgCreateDisconnect->SetImage("disconnect_normal");
					else
						//m_pImgCreateDisconnect->SetImage("new_game_normal");*/
					InRolloverCreateDisconnect = false;
				}
			}
		}
	}
	void CheckRolloverSave(int x,int y, int fx, int fy)
	{
		if(m_pButtonSave->IsVisible()) {
			int bx,by,bw,bh; // button xpos, ypos, width, height
 
			m_pButtonSave->GetPos(bx,by);
			m_pButtonSave->GetSize(bw,bh);
 
			bx = bx+fx; // xpos for button (rel to screen)
			by = by+fy; // ypos for button (rel to screen)
 
			// Check and see if mouse cursor is within button bounds
			if ((x > bx && x < bx+bw) && (y > by && y < by+bh))
			{
				if(!InRolloverSave) {
					//m_pImgSave->SetImage("bg2options_over");
					InRolloverSave = true;
				}
			} else {
				if(InRolloverSave) {
					//m_pImgSave->SetImage("bg2options_normal");
					InRolloverSave = false;
				}
			}
		}
	}
	void CheckRolloverOptions(int x,int y, int fx, int fy)
	{
		int bx,by,bw,bh; // button xpos, ypos, width, height
 
		m_pButtonOptions->GetPos(bx,by);
		m_pButtonOptions->GetSize(bw,bh);
 
		bx = bx+fx; // xpos for button (rel to screen)
		by = by+fy; // ypos for button (rel to screen)
 
		// Check and see if mouse cursor is within button bounds
		if ((x > bx && x < bx+bw) && (y > by && y < by+bh))
		{
			if(!InRolloverOptions) {
				//m_pImgOptions->SetImage("options_over");
				InRolloverOptions = true;
			}
		} else {
			if(InRolloverOptions) {
				//m_pImgOptions->SetImage("options_normal");
				InRolloverOptions = false;
			}
		}
	}
	void CheckRolloverLeave(int x,int y, int fx, int fy)
	{
		int bx,by,bw,bh; // button xpos, ypos, width, height
 
		m_pButtonLeave->GetPos(bx,by);
		m_pButtonLeave->GetSize(bw,bh);
 
		bx = bx+fx; // xpos for button (rel to screen)
		by = by+fy; // ypos for button (rel to screen)
 
		// Check and see if mouse cursor is within button bounds
		if ((x > bx && x < bx+bw) && (y > by && y < by+bh))
		{
			if(!InRolloverLeave) {
				//m_pImgLeave->SetImage("quit_over");
				InRolloverLeave = true;
			}
		} else {
			if(InRolloverLeave) {
				//m_pImgLeave->SetImage("quit_normal");
				InRolloverLeave = false;
			}
		}
	}
	bool CMainMenu::InGame()
	{
		C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
 
		if(pPlayer && IsVisible())
		{
			return true;
		} else {
			return false;
		}
	}

private:
	//vgui::ImagePanel *m_pImgBegin;
	//vgui::ImagePanel *m_pImgCreateDisconnect;
	//vgui::ImagePanel *m_pImgSave;
	//vgui::ImagePanel *m_pImgOptions;
	//vgui::ImagePanel *m_pImgLeave;
	vgui::FancyButton *m_pButtonBegin;
	vgui::FancyButton *m_pButtonCreateDisconnect;
	vgui::FancyButton *m_pButtonSave;
	vgui::FancyButton *m_pButtonOptions;
	vgui::FancyButton *m_pButtonLeave;
	//CTextWindow *community; 
	vgui::HTML *m_pCommunity;
	//CTextWindow *m_pCommunity;

	int defaultX;
	int defaultY;
	bool InGameLayout;
	bool InRolloverBegin;
	bool InRolloverCreateDisconnect;
	bool InRolloverSave;
	bool InRolloverOptions;
	bool InRolloverLeave;
};

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CMainMenu::CMainMenu( vgui::VPANEL parent ) : BaseClass( NULL, "CMainMenu" )
{
	g_pMainMenu = this;

	LoadControlSettings( "resource/UI/MainMenu.res" ); // Optional, don't need this

	SetParent( parent );
	SetTitleBarVisible( false );
	SetMinimizeButtonVisible( false );
	SetMaximizeButtonVisible( false );
	SetCloseButtonVisible( false );
	SetSizeable( false );
	SetMoveable( false );
	SetProportional( true );
	SetVisible( true );
	//SetKeyBoardInputEnabled( true );
	//SetMouseInputEnabled( true );
	//ActivateBuildMode();
	//SetScheme("MenuScheme.res");
	
        // These coords are relative to a 640x480 screen
        // Good to test in a 1024x768 resolution.
	defaultX = 60; // x-coord for our position
	defaultY = 240; // y-coord for our position
	//InGameLayout = false;
 
	SetScheme("ClientScheme");

	// Size of the panel
	int wide,tall;
	surface()->GetScreenSize( wide, tall );
	SetSize(wide, tall);

#define setup(a) \
	a->SetSize(256, 64);\
	a->SetContentAlignmentAll(Label::a_center);\
	a->SetColorFrontTextHighlight(g_cBG3TextHighlightColor);\
	a->SetColorFrontText(g_cBG3TextColor);\
	a->SetImage("menu_icon"); \
	a->SetHoverImage("menu_icon_dark");\
	/*a->SetColorShadowText(g_cBG3TextShadowColor);*/ \
	a->SetVisible(true); \
	a->MakeReadyForUse();


	// New game
	m_pButtonBegin = new vgui::FancyButton(this, "btnFind", "");
	m_pButtonBegin->AddActionSignalTarget( this );
	setup(m_pButtonBegin);
	m_pButtonBegin->SetCommand( "OpenServerBrowser" );
	m_pButtonBegin->SetText("#BG3_Main_Find_Server");
 
	// Create / Disconnect
	m_pButtonCreateDisconnect = vgui::SETUP_PANEL(new vgui::FancyButton(this, "btnCreate", ""));	
	setup(m_pButtonCreateDisconnect);
	m_pButtonCreateDisconnect->SetCommand( "OpenCreateMultiplayerGameDialog" );
	m_pButtonCreateDisconnect->SetText("#BG3_Main_Create_Server");
 
	// Save
	m_pButtonSave = vgui::SETUP_PANEL(new vgui::FancyButton(this, "btnBG2", ""));
	setup(m_pButtonSave);
	m_pButtonSave->SetCommand("OpenBG2OptionsDialog");
	m_pButtonSave->SetText("#BG3_Main_BG3_Options");
 
	// Options
	m_pButtonOptions = vgui::SETUP_PANEL(new vgui::FancyButton(this, "btnOptions", ""));
	setup(m_pButtonOptions);
	m_pButtonOptions->SetCommand( "OpenOptionsDialog" );
	m_pButtonOptions->SetText("#BG3_Main_Options");

	// Leave
	m_pButtonLeave = vgui::SETUP_PANEL(new vgui::FancyButton(this, "btnLeave", "Exit"));
	setup(m_pButtonLeave);
	m_pButtonLeave->SetCommand( "Quit" );
	m_pButtonLeave->SetText("#BG3_Main_Quit");
#undef setup
	
	m_pCommunity = vgui::SETUP_PANEL(new vgui::HTML(this, "CommunityHTML"));

	PerformDefaultLayout();

	m_pCommunity->OpenURL("https://battlegrounds3.com", NULL);
	m_pCommunity->SetVisible(true);
}
 
void CMainMenu::OnCommand(const char *command)
{
	if ( !Q_stricmp( command, "Quit" ) )
	{
		engine->ClientCmd("gamemenucommand Quit");
	}
	else if ( !Q_stricmp( command, "OpenCreateMultiplayerGameDialog" ) )
	{
		BG3Hint::UpdateLoadingDialogueHintDisplay();
		engine->ClientCmd("gamemenucommand OpenCreateMultiplayerGameDialog");
	}
	else if ( !Q_stricmp( command, "ResumeGame" ) )
	{
		engine->ClientCmd("gamemenucommand ResumeGame");
	}
	else if ( !Q_stricmp( command, "OpenServerBrowser" ) )
	{
		BG3Hint::UpdateLoadingDialogueHintDisplay();
		engine->ClientCmd("gamemenucommand OpenServerBrowser");
	}
	else if ( !Q_stricmp( command, "OpenBG2OptionsDialog" ) )
	{
		engine->ClientCmd("OpenBG2OptionsDialog");
	}
	else if ( !Q_stricmp( command, "OpenOptionsDialog" ) )
	{
		engine->ClientCmd("gamemenucommand OpenOptionsDialog");
	}
	else if ( !Q_stricmp( command, "Disconnect" ) )
	{
		engine->ClientCmd("Disconnect");
	}
	BaseClass::OnCommand( command );
	//BaseClass::OnCommand(command);
}

 
//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CMainMenu::~CMainMenu()
{
}
 
// Class
// Change CSMenu to CModMenu if you want. Salient is the name of the source mod, 
// hence SMenu. If you change CSMenu, change ISMenu too where they all appear.
class CSMenu : public ISMenu
{
private:
	CMainMenu *MainMenu;
	vgui::VPANEL m_hParent;
 
public:
	CSMenu( void )
	{
		MainMenu = NULL;
	}
 
	void Create( vgui::VPANEL parent )
	{
		// Create immediately
		MainMenu = new CMainMenu(parent);
	}
 
	void Destroy( void )
	{
		if ( MainMenu )
		{
			MainMenu->SetParent( (vgui::Panel *)NULL );
			delete MainMenu;
		}
	}
 
};
 
static CSMenu g_SMenu;
ISMenu *SMenu = ( ISMenu * )&g_SMenu;