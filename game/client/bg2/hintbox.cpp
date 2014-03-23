/*
	The Battle Grounds 2 - A Source modification
	Copyright (C) 206, The Battle Grounds 2 Team and Contributors

	The Battle Grounds 2 free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The Battle Grounds 2 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

	Contact information:
		Tomas "Tjoppen" Härdin		mail, in reverse: se . gamedev @ tjoppen
		Jason "Draco" Houston		iamlorddraco@gmail.com

	You may also contact us via the Battle Grounds website and/or forum at:
		www.bgmod.com
*/
#include "cbase.h"
#include <cdll_client_int.h>
#include <globalvars_base.h>
#include <cdll_util.h>
#include <KeyValues.h>
#include "hud_macros.h"
#include "hud.h"
#include "hudelement.h"

#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/IPanel.h>
#include <vgui_controls/ImageList.h>
#include <vgui_controls/MenuItem.h>

#include <stdio.h> // _snprintf define

#include <game/client/iviewport.h>
#include "commandmenu.h"

#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Menu.h>
#include "c_hl2mp_player.h"
#include "IGameUIFuncs.h" // for key bindings
#include <imapoverview.h>
#include <shareddefs.h>
#include <igameresources.h>
#include "clientmode_hl2mpnormal.h"

#include "hintbox.h"

//memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*
##########################################################################################
==========================================================================================
GUIDE using HINTS:

To display a pre set Hint Message first add the message to hintdefs.h 
(adding an entry to the enum as well makes the usage of the message easier readable)
then to implement the hint message use:

CHintbox *hintbox = GET_HUDELEMENT( CHintbox );
hintbox->UseHint(hint, displaytime, displaymode);

or as one-liner:
(GET_HUDELEMENT( CHintbox ))->UseHint(hint, displaytime, displaymode);
e.g.:(GET_HUDELEMENT( CHintbox ))->UseHint(HINT_STAMINA, 3, DISPLAY_ALWAYS);

	hint = int or enum of the entry in hintdefs.h
	displaytime = time in seconds the message will be displayed, fades out in last second
	displaymode = 
		DISPLAY_ONCE = only show this hint once per game
		DISPLAY_ALWAYS = show this hint every time it is called but not if we already 
						 show a different hint at the time it is called
		OVERRIDE_ALL = allways show this hint, if we currently display a different 
					   hint remove the old hint and display the new one

hintbox->SetHint(hint, displaytime, displaymode);
	Not fully finished yet
==========================================================================================
##########################################################################################
*/
// not implemented yet
static ConVar cl_hintbox( "cl_hintbox", "0", FCVAR_CLIENTDLL, "0 - Off, 1 - game relevant hints, 2 -  with newbie notices" );

DECLARE_HUDELEMENT( CHintbox );
DECLARE_HUD_MESSAGE( CHintbox, Hintbox );

char *pHints[NUM_HINTS] =
{//                                       |                                   |
	"Trying to sit still and hide will \nmake you vulerable to determined \nbayonet charges!",
	"Crouching takes some stamina (heavy \ngear) but will affect your \nability to aim (watch your crosshairs size).",
	"Crouching does not increase stamina \nregeneration.",
	"Cannot use melee weapon while being \ncrouched!",
	"If these hints start to annoy you \nyou can turn them off in the \noptions dialog.",
	"Capturing all flags will end the \nround and give your team extra \nbonus points.",
	"Muskets are inaccurate! Try getting \ncloser to your enemy to get a \nbetter chance of scoring a hit.",
	"Jumping takes lots of stamina \nJump only when absolutely necessary",
	"Low Stamina Warning! Relax for a \nsecond and let your stamina fill \nup again.",
	"Melee attack is the most powerful \nmethod to kill a large amount of \nenemies in a short amount of time.",
	"While you are reloading you are an \neasy target. Be sure when to hit \nthe reload button and when not.",
	"You are in the reload process and \ndefenseless until you are done!"
};

CHint::CHint(char *input)
{
	if (!input)
		return;

	strcpy(m_text, input);
	m_shown = false;
}

using namespace vgui;

CHintbox::CHintbox( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "Hintbox" )
{
	DevMsg (2, "CHintbox::CHintbox - constructor sent %s\n", pElementName);
	
	// Here we load up our scheme and set this element to use it. Using a different scheme than ClientScheme doesn't work right off the bat anyways, so... :)
	scheme = vgui::scheme()->LoadSchemeFromFile("resource/ClientScheme.res", "ClientScheme");
	SetScheme(scheme);	
	
	// Our parent is the screen itself.
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
	
	for (int i=0; i < NUM_HINTS; i++)
	{
		m_hint[i] = new CHint(pHints[i]);
	}
	
	SetHiddenBits( HIDEHUD_ALL );	
	SetBgColor(Color( 0,0,0,100 ));
	SetPaintBackgroundEnabled( true );
	SetPos( ScreenWidth() - 300, 300);
	SetSize( 300, 120 );
	SetPaintBackgroundType (2); // Rounded corner box
	SetAlpha(255);
	
	m_hidden = true;
	m_textpos.x = 15;
	m_textpos.y = 15;
	
	m_showcountdown = gpGlobals->curtime + 50.0f;
	m_initialhintdelay = gpGlobals->curtime + 30.0f;
	
	m_preset = -1;
	
	m_label = new vgui::Label( this, "HintLabel", "hint label" );
	m_label->SetPaintBackgroundEnabled( false );
	m_label->SetPaintBorderEnabled( false );
	m_label->SetContentAlignment( vgui::Label::a_west );
	Color ColourWhite( 255, 255, 255, 230 );
	m_label->SetFgColor( ColourWhite );
	m_label->SetVisible(false);
	m_label->SetSize(280, 95);
	m_label->SetPos(m_textpos.x, m_textpos.y);
} 

void CHintbox::Init( void )
{
	//HOOK_HUD_MESSAGE( CHintbox, Hintbox ); 
	DevMsg (2, "CHintbox::Init\n" );
	Reset();
}

void CHintbox::Reset( void )
{
	m_hintshowtime = 0;
	m_hidden = true;

	m_label->SetText("");
	m_label->SetVisible(false);
}

void CHintbox::VidInit( void )
{
	DevMsg (2, "CHintbox::VidInit\n" );
}

void CHintbox::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	DevMsg (2, "CHintbox::ApplySchemeSettings\n" );
	BaseClass::ApplySchemeSettings( pScheme );
	
	m_hHintFont = pScheme->GetFont("HudHintTextSmall", true);
	m_label->SetFont(m_hHintFont);
}

bool CHintbox::SetHint( char *text, int displaytime, int displaymode )
{
	if (cl_hintbox.GetInt() == 0)
		return false;

	C_HL2MP_Player *pHL2Player = (C_HL2MP_Player*)C_HL2MP_Player::GetLocalPlayer();
	if (!pHL2Player)
		return false;

	//check wether we are already displaying a different hint
	if (!m_hidden)
		if (displaymode != OVERRIDE_ALL)
			return false;

	//we already showed this hint once
	if ((displaymode != OVERRIDE_ALL) || (displaymode != DISPLAY_ALWAYS))
		return true;

	m_label->SetText(text);

	m_hintshowtime = gpGlobals->curtime + displaytime;
	m_hidden = false;
	SetAlpha(255);

	m_label->SetVisible(true);
	m_label->SizeToContents();

	// TODO displaymode usage
	return true;
}

bool CHintbox::UseHint( int textpreset, int displaytime, int displaymode )
{
	if (cl_hintbox.GetInt() == 0)
		return false;
		
	C_HL2MP_Player *pHL2Player = (C_HL2MP_Player*)C_HL2MP_Player::GetLocalPlayer();
	if (!pHL2Player)
		return false;
	
	// no hints when the player is dead
	if (!pHL2Player->IsAlive())
		return false;
		
	if (pHL2Player->IsObserver())
		return false;

	//hack hack hack!! but considering pHL2Player->IsAlive() does not work as it is supposed to this is kinda the only way to get this to work
 	if (m_initialhintdelay > gpGlobals->curtime)
		return false;

	m_preset = textpreset;
	if (textpreset >= NUM_HINTS)
		return false;
		
	//check wether we are already displaying a different hint
	if (!m_hidden)
		if (displaymode != OVERRIDE_ALL)
			return false;

	if (m_hint == NULL)
		return false;

	//we already showed this hint once
	if (displaymode != OVERRIDE_ALL) 
		if (displaymode != DISPLAY_ALWAYS)
			if (m_hint[textpreset]->Shown())
				return true;

	Color ColourWhite( 255, 255, 255, 230 );
	m_label->SetFgColor( ColourWhite );

	m_hintshowtime = gpGlobals->curtime + displaytime;
	m_hidden = false;
	SetAlpha(255);

	m_hint[textpreset]->SetShown(true);

	wchar_t	printtext[512];
	g_pVGuiLocalize->ConvertANSIToUnicode(m_hint[textpreset]->GetText(), printtext, sizeof(printtext));
	m_label->SetText(printtext);
	m_label->SetVisible(true);
 	m_label->SizeToContents();
	
	return true;
}

void CHintbox::MsgFunc_Hintbox( bf_read &msg )
{
	int hint, time, mode;
	hint = msg.ReadByte();
	time = msg.ReadByte();
	mode = msg.ReadByte();
	DevMsg (2, "CHintbox::MsgFunc_Hintbox - got message hint: %d time: %d mode: %d\n", hint, time, mode);
	
	UseHint(hint, time, mode);
}

void CHintbox::OnThink(void)
{
	if (m_hintshowtime < gpGlobals->curtime)
	{
		SetVisible(false);
		m_hidden = true;
	}
	else
		SetVisible(true);

	//fade out
	if (m_hintshowtime - 1 < gpGlobals->curtime && !m_hidden)
	{
		float alpha = (m_hintshowtime - gpGlobals->curtime) * 255;
		if (alpha < 1)
			alpha = 0;
		SetAlpha((int)alpha);
	}

	//show various general gameplay hints from time to time
	if (m_showcountdown < gpGlobals->curtime)
	{
		int usedhint = 0;
		switch(m_showgeneralhint)
		{
		case 0: 
			usedhint = HINT_CAMPING;
			break;
		case 1: 
			usedhint = HINT_SCORE;
			break;
		case 2: 
			usedhint = HINT_MUSKET;
			break;
		case 3: 
			usedhint = HINT_MELEE;
			break;
		case 4: 
			usedhint = HINT_RELOAD;
			break;
		case 5: 
			// not implemented yet
			//usedhint = HINT_HINT;
			break;
		default:
			usedhint = HINT_CAMPING;
			m_showgeneralhint = 0;
			break;
		}			

		if (UseHint(usedhint, 6, DISPLAY_ONCE))
			m_showgeneralhint++;
		
		m_showcountdown = gpGlobals->curtime + 45.0f;
	}
}

/*static void hintbox_f( void ) //This needs fixing. -HairyPotter
{
	if( ArgC() == 2 )
	{
		int hint = atoi( args[ 1 ] /*engine->Cmd_Argv( 1 )*/// );
		/*CHintbox *hintbox = GET_HUDELEMENT( CHintbox );
		int displaytime = 8;
		int displaymode = DISPLAY_ALWAYS;
		hintbox->UseHint(hint, displaytime, displaymode);
	}
}

static ConCommand hintbox( "hintbox", hintbox_f );*/