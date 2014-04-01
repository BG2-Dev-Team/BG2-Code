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
 
using namespace vgui;
 
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
 
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
		SetPos(vgui::scheme()->GetProportionalScaledValue(defaultX), vgui::scheme()->GetProportionalScaledValue(defaultY));
 
		BaseClass::PerformLayout();
	}
	void CMainMenu::PerformDefaultLayout()
	{
		m_pButtonBegin->SetPos(0, 0);
		m_pImgBegin->SetPos(0,0);
		m_pButtonLoad->SetPos(0, 40);
		m_pImgLoad->SetPos(0,40);
		m_pButtonOptions->SetPos(0, 80);
		m_pImgOptions->SetPos(0,80);
		m_pButtonLeave->SetPos(0, 120);
		m_pImgLeave->SetPos(0,120);
 
		m_pImgSave->SetVisible(false);
		m_pButtonSave->SetVisible(false);
 
		m_pImgResume->SetVisible(false);
		m_pButtonResume->SetVisible(false);
 
		InRolloverResume=false;
		InRolloverBegin=false;
		InRolloverLoad=false;
		InRolloverOptions=false;
		InRolloverLeave=false;
	}
 
	virtual void OnThink()
	{
		// In-game, everything will be in different places than at the root menu!
		if (InGame() && !InGameLayout) {
			DevMsg("Performing menu layout\n");
			int dy = 40; // delta y, shift value
			int x,y;
			// Resume
			m_pButtonResume->SetPos(0,0);
			m_pImgResume->SetPos(0,0);
			m_pButtonResume->SetVisible(true);
			m_pImgResume->SetVisible(true);
 
			m_pButtonBegin->GetPos(x,y);
			m_pButtonBegin->SetPos(x,y+dy);
			m_pImgBegin->GetPos(x,y);
			m_pImgBegin->SetPos(x,y+dy);
 
			m_pButtonLoad->GetPos(x,y);
			m_pButtonLoad->SetPos(x,y+dy);
			m_pImgLoad->GetPos(x,y);
			m_pImgLoad->SetPos(x,y+dy);
 
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
			m_pImgLeave->SetPos(x,y+(2*dy)); // Leave game moves under Save game, so twice as far
 
			InGameLayout = true;
		}
		if (!InGame() && InGameLayout)
		{
			PerformDefaultLayout();
			InGameLayout = false;
		}
 
		// Get mouse coords
		int x,y;
		vgui::input()->GetCursorPos(x,y);
 
		int fx,fy; // frame xpos, ypos
 
		GetPos(fx,fy);
 
		CheckRolloverBegin(x,y,fx,fy);
		CheckRolloverResume(x,y,fx,fy);
		CheckRolloverLoad(x,y,fx,fy);
		CheckRolloverSave(x,y,fx,fy);
		CheckRolloverOptions(x,y,fx,fy);
		CheckRolloverLeave(x,y,fx,fy);
 
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
				m_pImgBegin->SetImage("menu_begin_over");
				InRolloverBegin = true;
			}
		} else {
			if(InRolloverBegin) {
				m_pImgBegin->SetImage("menu_begin");
				InRolloverBegin = false;
			}
		}
	}
 
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
				if(!InRolloverResume) {
					m_pImgResume->SetImage("menu_Resume_over");
					InRolloverResume = true;
				}
			} else {
				if(InRolloverResume) {
					m_pImgResume->SetImage("menu_Resume");
					InRolloverResume = false;
				}
			}
		}
	}
	void CheckRolloverLoad(int x,int y, int fx, int fy)
	{
		int bx,by,bw,bh; // button xpos, ypos, width, height
 
		m_pButtonLoad->GetPos(bx,by);
		m_pButtonLoad->GetSize(bw,bh);
 
		bx = bx+fx; // xpos for button (rel to screen)
		by = by+fy; // ypos for button (rel to screen)
 
		// Check and see if mouse cursor is within button bounds
		if ((x > bx && x < bx+bw) && (y > by && y < by+bh))
		{
			if(!InRolloverLoad) {
				m_pImgLoad->SetImage("menu_load_over");
				InRolloverLoad = true;
			}
		} else {
			if(InRolloverLoad) {
				m_pImgLoad->SetImage("menu_load");
				InRolloverLoad = false;
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
					m_pImgSave->SetImage("menu_Save_over");
					InRolloverSave = true;
				}
			} else {
				if(InRolloverSave) {
					m_pImgSave->SetImage("menu_Save");
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
				m_pImgOptions->SetImage("menu_Options_over");
				InRolloverOptions = true;
			}
		} else {
			if(InRolloverOptions) {
				m_pImgOptions->SetImage("menu_Options");
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
				m_pImgLeave->SetImage("menu_Leave_over");
				InRolloverLeave = true;
			}
		} else {
			if(InRolloverLeave) {
				m_pImgLeave->SetImage("menu_Leave");
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
	vgui::ImagePanel *m_pImgLoad;
	vgui::ImagePanel *m_pImgSave;
	vgui::ImagePanel *m_pImgOptions;
	vgui::ImagePanel *m_pImgLeave;
	vgui::Button *m_pButtonBegin;
	vgui::Button *m_pButtonResume;
	vgui::Button *m_pButtonLoad;
	vgui::Button *m_pButtonSave;
	vgui::Button *m_pButtonOptions;
	vgui::Button *m_pButtonLeave;
 
	int defaultX;
	int defaultY;
	bool InGameLayout;
	bool InRolloverBegin;
	bool InRolloverResume;
	bool InRolloverLoad;
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
	SetKeyBoardInputEnabled( false );
	SetMouseInputEnabled( false );
	//ActivateBuildMode();
	SetScheme("MenuScheme.res");
 
        // These coords are relative to a 640x480 screen
        // Good to test in a 1024x768 resolution.
	defaultX = 60; // x-coord for our position
	defaultY = 240; // y-coord for our position
	InGameLayout = false;
 
	// Size of the panel
	SetSize(512,512);
	SetZPos(-1); // we're behind everything
 
	// Load invisi buttons
        // Initialize images
	m_pImgBegin = vgui::SETUP_PANEL(new vgui::ImagePanel(this, "Begin"));
	m_pImgResume = vgui::SETUP_PANEL(new vgui::ImagePanel(this, "Resume"));
	m_pImgLoad = vgui::SETUP_PANEL(new vgui::ImagePanel(this, "Load"));
	m_pImgSave = vgui::SETUP_PANEL(new vgui::ImagePanel(this, "Save"));
	m_pImgOptions = vgui::SETUP_PANEL(new vgui::ImagePanel(this, "Options"));
	m_pImgLeave = vgui::SETUP_PANEL(new vgui::ImagePanel(this, "Leave"));
 
	// New game
 
	m_pButtonBegin = vgui::SETUP_PANEL(new vgui::Button(this, "btnBegin", ""));	
	m_pButtonBegin->SetSize(300, 28);
	m_pButtonBegin->SetPaintBorderEnabled(false);
	m_pButtonBegin->SetPaintEnabled(false);
	m_pImgBegin->SetImage("menu_begin");
 
	// Resume
	m_pButtonResume = vgui::SETUP_PANEL(new vgui::Button(this, "btnResume", ""));	
	m_pButtonResume->SetSize(170, 28);
	m_pButtonResume->SetPaintBorderEnabled(false);
	m_pButtonResume->SetPaintEnabled(false);
	m_pImgResume->SetImage("menu_resume");
 
	// Load
	m_pButtonLoad = vgui::SETUP_PANEL(new vgui::Button(this, "btnLoad", ""));
	m_pButtonLoad->SetSize(190, 28);
	m_pButtonLoad->SetPaintBorderEnabled(false);
	m_pButtonLoad->SetPaintEnabled(false);
	m_pImgLoad->SetImage("menu_load");
 
	// Save
	m_pButtonSave = vgui::SETUP_PANEL(new vgui::Button(this, "btnSave", ""));
	m_pButtonSave->SetSize(190, 28);
	m_pButtonSave->SetPaintBorderEnabled(false);
	m_pButtonSave->SetPaintEnabled(false);
	m_pImgSave->SetImage("menu_save");
 
	// Options
	m_pButtonOptions = vgui::SETUP_PANEL(new vgui::Button(this, "btnOptions", ""));
	m_pButtonOptions->SetSize(170, 28);
	m_pButtonOptions->SetPaintBorderEnabled(false);
	m_pButtonOptions->SetPaintEnabled(false);
	m_pImgOptions->SetImage("menu_options");
 
	// Leave
	m_pButtonLeave = vgui::SETUP_PANEL(new vgui::Button(this, "btnLeave", ""));
	m_pButtonLeave->SetSize(180, 28);
	m_pButtonLeave->SetPaintBorderEnabled(false);
	m_pButtonLeave->SetPaintEnabled(false);
	m_pImgLeave->SetImage("menu_leave");
 
	PerformDefaultLayout();
}
 
void CMainMenu::OnCommand(const char *command)
{
 
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