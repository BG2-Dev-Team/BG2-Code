//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "vgui_int.h"
#include "ienginevgui.h"
#include "itextmessage.h"
#include "vguicenterprint.h"
#include "iloadingdisc.h"
#include "ifpspanel.h"
#include "imessagechars.h"
#include "inetgraphpanel.h"
#include "idebugoverlaypanel.h"
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui/IInput.h>
#include "tier0/vprof.h"
#include "iclientmode.h"
#include <vgui_controls/Panel.h>
#include <KeyValues.h>
#include "filesystem.h"
#include "matsys_controls/matsyscontrols.h"


#include "../bg2/vgui_Panel_MainMenu.h"
//BG2 - Tjoppen - options panel
#include "../bg2/vgui_bg2_options.h"
#include "bg3/bg3_classmenu.h"
//
/*class CBG2Version : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CBG2Version, vgui::Panel);
	CBG2Version(vgui::Panel parent) : BaseClass(NULL, "BG2Version")
	{
		vgui::Panel *pParent = g_pClientMode->GetViewport();
		SetParent(pParent);

		//BG2 - Print out the BG2 version number in the main menu panel. -HairyPotter
		Color ColourWhite(255, 255, 255, 255);
		vgui::Label *m_pLabelBGVersion = NULL;
		m_pLabelBGVersion = new vgui::Label(this, "RoundState_warmup", "");
		m_pLabelBGVersion->SetPaintBackgroundEnabled(false);
		m_pLabelBGVersion->SetPaintBorderEnabled(false);
		m_pLabelBGVersion->SetText("1.3.00b (SVN)");
		m_pLabelBGVersion->SizeToContents();
		m_pLabelBGVersion->SetContentAlignment(vgui::Label::a_west);
		m_pLabelBGVersion->SetFgColor(ColourWhite);
		m_pLabelBGVersion->SetPos(GetWide() - 20, GetTall() - 20);
		//

		SetVisible(true);
	}

	~CBG2Version()
	{
	}
};
extern CBG2Version *bg2version;*/

#ifdef SIXENSE
#include "sixense/in_sixense.h"
#endif

#if defined( TF_CLIENT_DLL )
#include "tf_gamerules.h"
#endif

using namespace vgui;

void MP3Player_Create( vgui::VPANEL parent );
void MP3Player_Destroy();

#include <vgui/IInputInternal.h>
vgui::IInputInternal *g_InputInternal = NULL;

#include <vgui_controls/Controls.h>
#include "game_controls/vguitextwindow.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//BG2 - Tjoppen - options panel
CBG2OptionsPanel *bg2options = NULL;
//

void GetVGUICursorPos( int& x, int& y )
{
	vgui::input()->GetCursorPos(x, y);
}

void SetVGUICursorPos( int x, int y )
{
	if ( !g_bTextMode )
	{
		vgui::input()->SetCursorPos(x, y);
	}
}

class CHudTextureHandleProperty : public vgui::IPanelAnimationPropertyConverter
{
public:
	virtual void GetData( Panel *panel, KeyValues *kv, PanelAnimationMapEntry *entry )
	{
		void *data = ( void * )( (*entry->m_pfnLookup)( panel ) );
		CHudTextureHandle *pHandle = ( CHudTextureHandle * )data;

		// lookup texture name for id
		if ( pHandle->Get() )
		{
			kv->SetString( entry->name(), pHandle->Get()->szShortName );
		}
		else
		{
			kv->SetString( entry->name(), "" );
		}
	}
	
	virtual void SetData( Panel *panel, KeyValues *kv, PanelAnimationMapEntry *entry )
	{
		void *data = ( void * )( (*entry->m_pfnLookup)( panel ) );
		
		CHudTextureHandle *pHandle = ( CHudTextureHandle * )data;

		const char *texturename = kv->GetString( entry->name() );
		if ( texturename && texturename[ 0 ] )
		{
			CHudTexture *currentTexture = gHUD.GetIcon( texturename );
			pHandle->Set( currentTexture );
		}
		else
		{
			pHandle->Set( NULL );
		}
	}

	virtual void InitFromDefault( Panel *panel, PanelAnimationMapEntry *entry )
	{
		void *data = ( void * )( (*entry->m_pfnLookup)( panel ) );

		CHudTextureHandle *pHandle = ( CHudTextureHandle * )data;

		const char *texturename = entry->defaultvalue();
		if ( texturename && texturename[ 0 ] )
		{
			CHudTexture *currentTexture = gHUD.GetIcon( texturename );
			pHandle->Set( currentTexture );
		}
		else
		{
			pHandle->Set( NULL );
		}
	}
};

static CHudTextureHandleProperty textureHandleConverter;

static void VGui_VideoMode_AdjustForModeChange( void )
{
	// Kill all our panels. We need to do this in case any of them
	//	have pointers to objects (eg: iborders) that will get freed
	//	when schemes get destroyed and recreated (eg: mode change).
	netgraphpanel->Destroy();
	debugoverlaypanel->Destroy();
#if defined( TRACK_BLOCKING_IO )
	iopanel->Destroy();
#endif
	fps->Destroy();
	messagechars->Destroy();
	loadingdisc->Destroy();

	// Recreate our panels.
	VPANEL gameToolParent = enginevgui->GetPanel( PANEL_CLIENTDLL_TOOLS );
	VPANEL toolParent = enginevgui->GetPanel( PANEL_TOOLS );
#if defined( TRACK_BLOCKING_IO )
	VPANEL gameDLLPanel = enginevgui->GetPanel( PANEL_GAMEDLL );
#endif

	loadingdisc->Create( gameToolParent );
	messagechars->Create( gameToolParent );

	// Debugging or related tool
	fps->Create( toolParent );
#if defined( TRACK_BLOCKING_IO )
	iopanel->Create( gameDLLPanel );
#endif
	netgraphpanel->Create( toolParent );
	debugoverlaypanel->Create( gameToolParent );
}

static void VGui_OneTimeInit()
{
	static bool initialized = false;
	if ( initialized )
		return;
	initialized = true;

	vgui::Panel::AddPropertyConverter( "CHudTextureHandle", &textureHandleConverter );


    g_pMaterialSystem->AddModeChangeCallBack( &VGui_VideoMode_AdjustForModeChange );
}

bool VGui_Startup( CreateInterfaceFn appSystemFactory )
{
	if ( !vgui::VGui_InitInterfacesList( "CLIENT", &appSystemFactory, 1 ) )
		return false;

	if ( !vgui::VGui_InitMatSysInterfacesList( "CLIENT", &appSystemFactory, 1 ) )
		return false;

	g_InputInternal = (IInputInternal *)appSystemFactory( VGUI_INPUTINTERNAL_INTERFACE_VERSION,  NULL );
	if ( !g_InputInternal )
	{
		return false; // c_vguiscreen.cpp needs this!
	}

	VGui_OneTimeInit();

	// Create any root panels for .dll
	VGUI_CreateClientDLLRootPanel();

	// Make sure we have a panel
	VPANEL root = VGui_GetClientDLLRootPanel();
	if ( !root )
	{
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void VGui_CreateGlobalPanels( void )
{
	VPANEL gameToolParent = enginevgui->GetPanel( PANEL_CLIENTDLL_TOOLS );
	VPANEL toolParent = enginevgui->GetPanel( PANEL_TOOLS );
#if defined( TRACK_BLOCKING_IO )
	VPANEL gameDLLPanel = enginevgui->GetPanel( PANEL_GAMEDLL );
#endif
	// Part of game
	internalCenterPrint->Create( gameToolParent );
	//loadingdisc->Create( gameToolParent ); //BG2 - found this commented out while porting to 2016 - Awesome
	messagechars->Create( gameToolParent );

	// Debugging or related tool
	fps->Create( toolParent );
#if defined( TRACK_BLOCKING_IO )
	iopanel->Create( gameDLLPanel );
#endif
	netgraphpanel->Create( toolParent );
	debugoverlaypanel->Create( gameToolParent );

	// Create mp3 player off of tool parent panel
	MP3Player_Create( toolParent );

	//BG2 - Tjoppen - options panel
	VPANEL GameUiDll = enginevgui->GetPanel( PANEL_GAMEUIDLL );
	bg2options = new CBG2OptionsPanel(GameUiDll);	//make into main menu panel
	SMenu->Create(GameUiDll); //new Main menu
	//
}

void VGui_Shutdown()
{
	VGUI_DestroyClientDLLRootPanel();

	MP3Player_Destroy();

	netgraphpanel->Destroy();
	debugoverlaypanel->Destroy();
#if defined( TRACK_BLOCKING_IO )
	iopanel->Destroy();
#endif
	fps->Destroy();
	SMenu->Destroy(); // to VGui_Shutdown().

	messagechars->Destroy();
	//loadingdisc->Destroy(); //BG2 - found this commented out while porting to 2016 - Awesome
	internalCenterPrint->Destroy();

	//BG2 - Tjoppen - options panel
	if (bg2options)
	{
		delete bg2options;
		bg2options = NULL;
	}
	//

	if ( g_pClientMode )
	{
		g_pClientMode->VGui_Shutdown();
	}

	// Make sure anything "marked for deletion"
	//  actually gets deleted before this dll goes away
	vgui::ivgui()->RunFrame();
}

static ConVar cl_showpausedimage( "cl_showpausedimage", "1", 0, "Show the 'Paused' image when game is paused." );

//-----------------------------------------------------------------------------
// Things to do before rendering vgui stuff...
//-----------------------------------------------------------------------------
void VGui_PreRender()
{
	VPROF( "VGui_PreRender" );
	tmZone( TELEMETRY_LEVEL0, TMZF_NONE, "%s", __FUNCTION__ );

	// 360 does not use these plaques
	if ( IsPC() )
	{
		loadingdisc->SetLoadingVisible( engine->IsDrawingLoadingImage() && !engine->IsPlayingDemo() );
#if !defined( TF_CLIENT_DLL )
		loadingdisc->SetPausedVisible( !enginevgui->IsGameUIVisible() && cl_showpausedimage.GetBool() && engine->IsPaused() && !engine->IsTakingScreenshot() && !engine->IsPlayingDemo() );
#else
		bool bShowPausedImage = cl_showpausedimage.GetBool() && ( TFGameRules() && !TFGameRules()->IsInTraining() );
		loadingdisc->SetPausedVisible( !enginevgui->IsGameUIVisible() && bShowPausedImage && engine->IsPaused() && !engine->IsTakingScreenshot() && !engine->IsPlayingDemo() );
#endif
	}
}

void VGui_PostRender()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : cl_panelanimation - 
//-----------------------------------------------------------------------------
CON_COMMAND( cl_panelanimation, "Shows panel animation variables: <panelname | blank for all panels>." )
{
	if ( args.ArgC() == 2 )
	{
		PanelAnimationDumpVars( args[1] );
	}
	else
	{
		PanelAnimationDumpVars( NULL );
	}
}

void GetHudSize( int& w, int &h )
{
	vgui::surface()->GetScreenSize( w, h );

	VPANEL hudParent = enginevgui->GetPanel( PANEL_CLIENTDLL );
	if ( hudParent )
	{
		vgui::ipanel()->GetSize( hudParent, w, h );
	}
}
