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

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

const int buttonstarty=30;

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
		int x,y;
		//int verx;
		//verx = 296;
		x = 270;
		vgui::ImagePanel *imgarr[narr];
		vgui::Button *buttarr[narr];
		int width = this->GetWide();
		
		imgarr[0] = m_pImgBegin;
		imgarr[1] = m_pImgResume;
		imgarr[2] = m_pImgSave;
		imgarr[3] = m_pImgOptions;
		imgarr[4] = m_pImgLeave;
		buttarr[0] = m_pButtonBegin;
		buttarr[1] = m_pButtonResume;
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
			imgarr[i]->SetPos((border + (i * space) / narr + biWide * i), buttonstarty);
			buttarr[i]->SetPos((border + (i * space) / narr + biWide * i), buttonstarty);
			imgarr[i]->SetSize(biWide, biTall);
			buttarr[i]->SetSize(biWide, biTall);
		}
		
		//community->SetPos((width-community->GetWide())/2, 270); //melon
		/*m_pImgBegin->SetPos(x,200);
		m_pButtonBegin->SetPos(x,200);
		x = x +verx;
		m_pButtonResume->SetPos(x,200);
		m_pImgResume->SetPos(x,200);
		x = x +verx;
		m_pImgOptions->SetPos(x,200);
		m_pButtonOptions->SetPos(x,200);
		x = x +verx;
		m_pButtonSave->SetPos(x,200);
		m_pImgSave->SetPos(x,200);
		x = x +verx;
		m_pButtonLeave->SetPos(x,200);
		m_pImgLeave->SetPos(x, 200);*/
 
		//m_pImgSave->SetVisible(false);
		//m_pButtonSave->SetVisible(false);
 
		//m_pImgResume->SetVisible(false);
		//m_pButtonResume->SetVisible(false);

		//melon
		//m_pImgBegin->SetSize(256, 64);
		//m_pImgResume->SetSize(256, 64);
		//m_pImgSave->SetSize(256, 64);
		//m_pImgOptions->SetSize(256, 64);
		//m_pImgLeave->SetSize(256, 64);

		InRolloverBegin=false;
		InRolloverResume=false;
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
		// In-game, everything will be in different places than at the root menu!
		if (InGame() && !InGameLayout) {
			m_pCommunity->SetVisible(false);
			m_pButtonResume->SetCommand( "Disconnect" );  // Check to see if we are in game to determine the purpose of the button - Awesome - BG2
			//DevMsg("Performing menu layout\n");
			//int dy = 40; // delta y, shift value
			//int x,y;

			//m_pImgBegin->SetPos(300,200);
			//m_pButtonBegin->SetPos(300,200);
			//m_pButtonResume->SetPos(812,200);
			//m_pImgResume->SetPos(812,200);
			//m_pImgOptions->SetPos(1324,200);
			//m_pButtonOptions->SetPos(1324,200);
			//m_pButtonSave->SetPos(1836,200);
			//m_pImgSave->SetPos(1836,200);
			// Resume
			/*m_pButtonResume->SetPos(300,200);
			m_pImgResume->SetPos(300,200);
			m_pButtonResume->SetVisible(true);
			m_pImgResume->SetVisible(true);
 
			m_pButtonBegin->GetPos(x,y);
			m_pButtonBegin->SetPos(x,y+dy);
			m_pImgBegin->GetPos(x,y);
			m_pImgBegin->SetPos(x,y+dy);
 
			// Save game
			m_pButtonSave->SetPos(x,y+(2*dy));
			m_pImgSave->SetPos(x,y+(2*dy));
			m_pButtonSave->SetVisible(true);
			m_pImgSave->SetVisible(true);
 
			m_pButtonOptions->GetPos(x,y);
			m_pButtonOptions->SetPos(x,y+(2*dy));
			m_pImgOptions->GetPos(x,y);
			m_pImgOptions->SetPos(x,y+(2*dy)); // Options moves under Save game, so twice as far
 
			m_pButtonLeave->GetPos(x,y);
			m_pButtonLeave->SetPos(x,y+(2*dy));
			m_pImgLeave->GetPos(x,y);
			m_pImgLeave->SetPos(x,y+(2*dy)); // Leave game moves under Save game, so twice as far*/

 
			//InGameLayout = true; //melon - moved down
		}
		if (!InGame() && InGameLayout)
		{
			PerformDefaultLayout();
			m_pCommunity->SetVisible(true);
			m_pButtonResume->SetCommand( "OpenCreateMultiplayerGameDialog" );  // Check to see if we are in game to determine the purpose of the button - Awesome - BG2
			//InGameLayout = false; //melon - moved down
		}
 
		// Get mouse coords
		int x,y;
		vgui::input()->GetCursorPos(x,y);
 
		int fx,fy; // frame xpos, ypos
 
		GetPos(fx,fy);
 
		CheckRolloverBegin(x,y,fx,fy);
		CheckRolloverResume(x,y,fx,fy);
		CheckRolloverSave(x,y,fx,fy);
		CheckRolloverOptions(x,y,fx,fy);
		CheckRolloverLeave(x,y,fx,fy);

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
				m_pImgBegin->SetImage("find_server_over");
				InRolloverBegin = true;
			}
		} else {
			if(InRolloverBegin) {
				m_pImgBegin->SetImage("find_server_normal");
				InRolloverBegin = false;
			}
		}
	}
 
	// BG2 - VisualMelon - I've butchered this a bit so it also takes care of changing the resume image
	void CheckRolloverResume(int x,int y, int fx, int fy)
	{
		if(m_pButtonResume->IsVisible()) {
			int bx,by,bw,bh; // button xpos, ypos, width, height
 
			m_pButtonResume->GetPos(bx,by);
			m_pButtonResume->GetSize(bw,bh);
 
			bx = bx+fx; // xpos for button (rel to screen)
			by = by+fy; // ypos for button (rel to screen)
 
			// Check and see if mouse cursor is within button bounds
			if ((x > bx && x < bx+bw) && (y > by && y < by+bh))
			{
				if (!InRolloverResume || InGame() != InGameLayout) {
					if (InGame())
						m_pImgResume->SetImage("disconnect_over");
					else
						m_pImgResume->SetImage("new_game_over");
					InRolloverResume = true;
				}
			} else {
				if (InRolloverResume || InGame() != InGameLayout) {
					if (InGame())
						m_pImgResume->SetImage("disconnect_normal");
					else
						m_pImgResume->SetImage("new_game_normal");
					InRolloverResume = false;
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
					m_pImgSave->SetImage("bg2options_over");
					InRolloverSave = true;
				}
			} else {
				if(InRolloverSave) {
					m_pImgSave->SetImage("bg2options_normal");
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
				m_pImgOptions->SetImage("options_over");
				InRolloverOptions = true;
			}
		} else {
			if(InRolloverOptions) {
				m_pImgOptions->SetImage("options_normal");
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
				m_pImgLeave->SetImage("quit_over");
				InRolloverLeave = true;
			}
		} else {
			if(InRolloverLeave) {
				m_pImgLeave->SetImage("quit_normal");
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
	vgui::ImagePanel *m_pImgBegin;
	vgui::ImagePanel *m_pImgResume;
	vgui::ImagePanel *m_pImgSave;
	vgui::ImagePanel *m_pImgOptions;
	vgui::ImagePanel *m_pImgLeave;
	vgui::Button *m_pButtonBegin;
	vgui::Button *m_pButtonResume;
	vgui::Button *m_pButtonSave;
	vgui::Button *m_pButtonOptions;
	vgui::Button *m_pButtonLeave;
	//CTextWindow *community; 
	vgui::HTML *m_pCommunity;
	//CTextWindow *m_pCommunity;

	int defaultX;
	int defaultY;
	bool InGameLayout;
	bool InRolloverBegin;
	bool InRolloverResume;
	bool InRolloverSave;
	bool InRolloverOptions;
	bool InRolloverLeave;
};

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CMainMenu::CMainMenu( vgui::VPANEL parent ) : BaseClass( NULL, "CMainMenu" )
{
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
 
	// Size of the panel
	int wide,tall;
	surface()->GetScreenSize( wide, tall );
	SetSize(wide, tall);
 
	// Load invisi buttons
        // Initialize images

	m_pImgBegin = vgui::SETUP_PANEL(new vgui::ImagePanel(this, "Find Server"));
	m_pImgResume = vgui::SETUP_PANEL(new vgui::ImagePanel(this, "Create Game"));
	m_pImgOptions = vgui::SETUP_PANEL(new vgui::ImagePanel(this, "Options"));
	m_pImgSave = vgui::SETUP_PANEL(new vgui::ImagePanel(this, "BG2 Options"));
	m_pImgLeave = vgui::SETUP_PANEL(new vgui::ImagePanel(this, "Leave"));
 
	// New game
 
	m_pButtonBegin = new vgui::Button(this, "btnFind", "");
	m_pButtonBegin->AddActionSignalTarget( this );
	m_pButtonBegin->SetSize(256, 64);
	m_pButtonBegin->SetPaintBorderEnabled(false);
	m_pButtonBegin->SetPaintEnabled(false);
	m_pButtonBegin->SetCommand( "OpenServerBrowser" );
	m_pImgBegin->SetImage("find_server_normal");
 
	// Resume
	m_pButtonResume = vgui::SETUP_PANEL(new vgui::Button(this, "btnCreate", ""));	
	m_pButtonResume->SetSize(256, 64);
	m_pButtonResume->SetPaintBorderEnabled(false);
	m_pButtonResume->SetPaintEnabled(false);
	m_pButtonResume->SetCommand( "OpenCreateMultiplayerGameDialog" );
	m_pImgResume->SetImage("new_game_normal");
 
	// Save
	m_pButtonSave = vgui::SETUP_PANEL(new vgui::Button(this, "btnBG2", ""));
	m_pButtonSave->SetSize(256, 60);
	m_pButtonSave->SetPaintBorderEnabled(false);
	m_pButtonSave->SetPaintEnabled(false);
	m_pButtonSave->SetCommand("OpenBG2OptionsDialog");
	m_pImgSave->SetImage("bg2options_normal");
 
	// Options
	m_pButtonOptions = vgui::SETUP_PANEL(new vgui::Button(this, "btnOptions", ""));
	m_pButtonOptions->SetSize(256, 64);
	m_pButtonOptions->SetPaintBorderEnabled(false);
	m_pButtonOptions->SetPaintEnabled(false);
	m_pButtonOptions->SetCommand( "OpenOptionsDialog" );
	m_pImgOptions->SetImage("options_normal");
 
	// Leave
	m_pButtonLeave = vgui::SETUP_PANEL(new vgui::Button(this, "btnLeave", "Exit"));
	m_pButtonLeave->SetSize(256, 64);
	m_pButtonLeave->SetPaintBorderEnabled(false);
	m_pButtonLeave->SetPaintEnabled(false);
	m_pButtonLeave->SetCommand( "Quit" );
	m_pImgLeave->SetImage("quit_normal");
	
	// community
	//CBaseViewport* pViewPort = dynamic_cast<CBaseViewport *>( g_pClientMode->GetViewport() );
	//community = new CTextWindow(pViewPort);
	//community->SetVisible(false);
	//community->MoveToFront();
	//community->MakeReadyForUse();
	//community->ShowText("Loading...");
	//community->ShowURL("http://www.bg2mod.com/");
	
	m_pCommunity = vgui::SETUP_PANEL(new vgui::HTML(this, "CommunityHTML"));

	PerformDefaultLayout();

	m_pCommunity->OpenURL("http://www.bg2mod.com/", NULL);
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
		engine->ClientCmd("gamemenucommand OpenCreateMultiplayerGameDialog");
	}
	else if ( !Q_stricmp( command, "ResumeGame" ) )
	{
		engine->ClientCmd("gamemenucommand ResumeGame");
	}
	else if ( !Q_stricmp( command, "OpenServerBrowser" ) )
	{
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
		engine->ClientCmd("gamemenucommand Disconnect");
	}
	BaseClass::OnCommand( command );
	BaseClass::OnCommand(command);
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
