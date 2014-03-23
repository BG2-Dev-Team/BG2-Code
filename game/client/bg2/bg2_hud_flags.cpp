/*
	The Battle Grounds 2 - A Source modification
	Copyright (C) 2005, The Battle Grounds 2 Team and Contributors

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

	You may also contact the (future) team via the Battle Grounds website and/or forum at:
		www.bgmod.com

	 Note that because of the sheer volume of files in the Source SDK this
	notice cannot be put in all of them, but merely the ones that have any
	changes from the original SDK.
	 In order to facilitate easy searching, all changes are and must be
	commented on the following form:

	//BG2 - <name of contributer>[ - <small description>]
*/
#include "cbase.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "c_playerresource.h"
#include "clientmode_hl2mpnormal.h"
#include <vgui_controls/Controls.h>
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <KeyValues.h>
#include "c_baseplayer.h"
#include "c_hl2mp_player.h"
#include "c_team.h"
#include "c_flag.h"


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar cl_flagstatus( "cl_flagstatus", "1", FCVAR_ARCHIVE, "0 - Off, 1 - Text, 2 - Icons" );
ConVar cl_flagstatusdetail( "cl_flagstatusdetail", "2", FCVAR_ARCHIVE, "0 - No Details, 1 - Compact Details, 2 - Full Details" );

enum
{
	TEAM_AMERICANS = 2,
	TEAM_BRITISH,
	//BG2 - Tjoppen - NUM_TEAMS is useful
	NUM_TEAMS,	//!! must be last !!
};

#define MAX_FLAGS	12

//==============================================
// CHudFlags
// Displays flag status
//==============================================
class CHudFlags : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHudFlags, vgui::Panel );
public:
	CHudFlags( const char *pElementName );
	void Init( void );
	void VidInit( void );
	virtual bool ShouldDraw( void );
	virtual void Paint( void );
	virtual void ApplySchemeSettings( vgui::IScheme *scheme );
	//void MsgFunc_flagstatus( bf_read &msg );

private:


	CHudTexture		*m_pIconBlank,
					*m_pIconRed,
					*m_pIconBlue;

	vgui::Label * m_pLabelFlag[MAX_FLAGS]; 
	
	//flag myflags[MAX_FLAGS];
	
	//int iNumFlags;
};

using namespace vgui;

DECLARE_HUDELEMENT( CHudFlags );
//DECLARE_HUD_MESSAGE( CHudFlags, flagstatus );

//==============================================
// CHudFlags's CHudFlags
// Constructor
//==============================================
CHudFlags::CHudFlags( const char *pElementName ) :
	CHudElement( pElementName ), BaseClass( NULL, "HudFlags" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
	Color ColourWhite( 255, 255, 255, 255 );
	SetHiddenBits( HIDEHUD_MISCSTATUS );
	//vgui::HFont font = pScheme->GetFont( "DracoLucidaRawks" );
	for( int i = 0; i < MAX_FLAGS; i++ )
	{
		/*m_IconFlag[i] = NULL;
		m_IconCover[i] = NULL;*/
		m_pLabelFlag[i] = new vgui::Label( this, "RoundState_warmup", "omg r teh lolzors" /*vgui::localize()->Find( "#Clan_warmup_mode" )*/ );
		m_pLabelFlag[i]->SetPaintBackgroundEnabled( false );
		m_pLabelFlag[i]->SetPaintBorderEnabled( false );
		m_pLabelFlag[i]->SizeToContents();
		m_pLabelFlag[i]->SetContentAlignment( vgui::Label::a_west );
		m_pLabelFlag[i]->SetFgColor( ColourWhite );
		m_pLabelFlag[i]->SetVisible(false);
		//m_pLabelFlag[i]->SetFont(font);
	}

	m_pIconBlank = m_pIconRed = m_pIconBlue;
}

//==============================================
// CHudFlags's ApplySchemeSettings
// applies the schemes
//==============================================
void CHudFlags::ApplySchemeSettings( IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme );
	vgui::HFont font = scheme->GetFont( "DracoLucidaRawks" );
	for( int i = 0; i < MAX_FLAGS; i++ )
	{
		m_pLabelFlag[i]->SetFont(font);
	}
	SetPaintBackgroundEnabled( false );
}

//==============================================
// CHudFlags's Init
// Inits any vars needed
//==============================================
void CHudFlags::Init( void )
{	
}

//==============================================
// CHudFlags's VidInit
// Inits any textures needed
//==============================================
void CHudFlags::VidInit( void )
{
	m_pIconBlank	= gHUD.GetIcon( "hud_flagicon_blank" );
	m_pIconRed		= gHUD.GetIcon( "hud_flagicon_red" );
	m_pIconBlue		= gHUD.GetIcon( "hud_flagicon_blue" );
}

//==============================================
// CHudFlags's ShouldDraw
// whether the panel should be drawing
//==============================================
bool CHudFlags::ShouldDraw( void )
{

	if ( !g_Flags.Count() ) //No flags? Die here. -HairyPotter
		return false;

	C_HL2MP_Player *pHL2Player = dynamic_cast<C_HL2MP_Player*>(C_HL2MP_Player::GetLocalPlayer());
	C_BaseCombatWeapon *wpn = pHL2Player->GetActiveWeapon();
	// Don't draw hud if we're using Iron Sights. -HairyPotter
	if ( wpn && wpn->m_bIsIronsighted )
		return false;
	//

	return CHudElement::ShouldDraw();
}

//==============================================
// CHudFlags's Paint
// errr... paints the panel
//==============================================
void CHudFlags::Paint()
{
	int m_iFlagCount = g_Flags.Count();

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if( !pPlayer )
		return;

	char text[512];
	int i;// = 0;
	int x_offset = 5;
	int y_offset = 5;
	Color ColourWhite( 255, 255, 255, 255 );
	Color ColourRed( 255, 0, 0, 255 );
	Color ColourBlue( 0, 0, 255, 255 );
	switch (cl_flagstatus.GetInt())
	{
	case 0: // not display anything
		for( i = 0; i < MAX_FLAGS; i++ )
		{
			m_pLabelFlag[i]->SetVisible(false);
		}
		break;
	case 1: // display flag status as text 
		for( i = 0; i < MAX_FLAGS; i++ )
		{
			m_pLabelFlag[i]->SetVisible(false);
		}

		for( i = 0; i < m_iFlagCount && i < 12; i++ )
		{
			if( !g_Flags[i] )
			{
				break;
			}
			//BG2 - Tjoppen - obey HUDSlot
			if( g_Flags[i]->m_iHUDSlot < 0 )
			{
				m_pLabelFlag[i]->SetVisible( false );
				continue;
			}
			//

			float iTimeToCap = g_Flags[i]->m_flNextCapture - gpGlobals->curtime;

			const char *pOLString = NULL;	//describes overload status - "Overloaded", "Not overloaded", empty string of not applicable

			//empty string if..
			if( g_Flags[i]->GetTeamNumber() == TEAM_UNASSIGNED ||				//flag not held by any team
					g_Flags[i]->m_bNotUncappable ||								//flag can't be uncapped
					!g_Flags[i]->m_bUncapOnDeath ||								//flag doesn't uncap when overloaders die
					g_Flags[i]->GetTeamNumber() != pPlayer->GetTeamNumber() )	//flag held by other team
			{
				pOLString = "";
			}
			else if( g_Flags[i]->m_pOverloading[pPlayer->GetClientIndex()] )
			{
				//player has overloading this flag
				if( cl_flagstatusdetail.GetInt() <= 1 )
					pOLString = "- OL:ed";
				else
					pOLString = "- Overloaded";
				
			}
			else
			{
				//player has not overloaded this flag yet
				if( cl_flagstatusdetail.GetInt() <= 1 )
					pOLString = "- Not OL:ed";
				else
					pOLString = "- Not overloaded";
			}

			//team specific flag?
			switch( g_Flags[i]->m_iForTeam )
			{
				case 0:
					//flag can be taken by any team
					Q_snprintf( text, sizeof(text), "%s", g_Flags[i]->m_sFlagName);

					switch (cl_flagstatusdetail.GetInt())
					{
						case 0: // No Details
							if( iTimeToCap > 0.1f )
							{
								char text_add[128];
								Q_snprintf( text_add, sizeof(text_add), "- %i", (int)iTimeToCap);
								strcat(	text, text_add);
							}
							break;
						case 1: // Compact Details
							if( iTimeToCap > 0 )
							{
								char text_add[128];
								Q_snprintf( text_add, sizeof(text_add), " - %i/%i - %i", g_Flags[i]->m_iNearbyPlayers, 
									g_Flags[i]->m_iCapturePlayers, (int)iTimeToCap );

								strcat(	text, text_add);
							}
							else
							{
								char text_add[128];
								Q_snprintf( text_add, sizeof(text_add), " - %i/%i %s", g_Flags[i]->m_iNearbyPlayers, 
									g_Flags[i]->m_iCapturePlayers, pOLString );

								strcat(	text, text_add);
							}
							break;
						case 2: // Full Details
							if( iTimeToCap > 0 )
							{
								char text_add[128];
								Q_snprintf( text_add, sizeof(text_add), " - %i/%i Players - Time %i", 
									g_Flags[i]->m_iNearbyPlayers, g_Flags[i]->m_iCapturePlayers, (int)iTimeToCap );

								strcat(	text, text_add);
							}
							else
							{
								char text_add[128];
								Q_snprintf( text_add, sizeof(text_add), " - %i/%i Players %s", 
									g_Flags[i]->m_iNearbyPlayers, g_Flags[i]->m_iCapturePlayers, pOLString );

								strcat(	text, text_add);
							}
							break;
					}
					break;

				case 1:
					//flag can only be taken by the americans
					if( iTimeToCap > 0 )
					{
						Q_snprintf( text, sizeof(text), "%s - American Only Target - %i/%i Players - Time %i", 
							g_Flags[i]->m_sFlagName, g_Flags[i]->m_iNearbyPlayers, g_Flags[i]->m_iCapturePlayers, 
							(int)iTimeToCap );
					}
					else
					{
						Q_snprintf( text, sizeof(text), "%s - American Only Target - %i/%i Players %s", 
							g_Flags[i]->m_sFlagName, g_Flags[i]->m_iNearbyPlayers, g_Flags[i]->m_iCapturePlayers,
							pOLString );
					}
					break;

				case 2:
					//flag can only be taken by the british
					if( iTimeToCap > 0 )
					{
						Q_snprintf( text, sizeof(text), "%s - British Only Target - %i/%i Players - Time %i", 
							g_Flags[i]->m_sFlagName, g_Flags[i]->m_iNearbyPlayers, g_Flags[i]->m_iCapturePlayers, 
							(int)iTimeToCap );
					}
					else
					{
						Q_snprintf( text, sizeof(text), "%s - British Only Target - %i/%i Players %s", 
							g_Flags[i]->m_sFlagName, g_Flags[i]->m_iNearbyPlayers, g_Flags[i]->m_iCapturePlayers, 
							pOLString );
					}
					break;
			}

			//figure out which colors to use
			float r = 0;
			float g = 0;
			float b = 0;

			/*if( !g_Flags[i]->m_bActive ) //Tweaked. See flag.cpp in server project. -HairyPotter
			{
				//BG2 - Tjoppen - inactive flags simply have grey text for now
				r = g = b = 96;
			}
			else*/
				switch( g_Flags[i]->m_iLastTeam )
				{
					case TEAM_AMERICANS:
						r = 66;
						g = 115;
						b = 247;
						r += 188 * (sin(iTimeToCap*4) + 1)/2;
						g += 139 * (sin(iTimeToCap*4) + 1)/2;
						b += 7 * (sin(iTimeToCap*4) + 1)/2;
						break;
					case TEAM_BRITISH:
						r = 255;
						g = 16;
						b = 16;
						g += 238 * (sin(iTimeToCap*4) + 1)/2;
						b += 238 * (sin(iTimeToCap*4) + 1)/2;
						break;
					default:
					case TEAM_UNASSIGNED:
						switch (g_Flags[i]->GetTeamNumber())
						{
							case TEAM_AMERICANS:
								r = 66;
								g = 115;
								b = 247;
								break;
							case TEAM_BRITISH:
								r = 255;
								g = 16;
								b = 16;
								break;
							case TEAM_UNASSIGNED:
								switch (g_Flags[i]->m_iRequestingCappers)
								{
									case TEAM_BRITISH:
										r = 255;
										g = 16;
										b = 16;
										g += 238 * (int)((sin(iTimeToCap*8) + 2.7f)/2);
										b += 238 * (int)((sin(iTimeToCap*8) + 2.7f)/2);
										break;
									case TEAM_AMERICANS:
										r = 66;
										g = 115;
										b = 247;
										r += 188 * (int)((sin(iTimeToCap*8) + 2.7f)/2);
										g += 139 * (int)((sin(iTimeToCap*8) + 2.7f)/2);
										b += 7 * (int)((sin(iTimeToCap*8) + 2.7f)/2);
										break;
									case TEAM_UNASSIGNED:
										switch( g_Flags[i]->m_iForTeam )
										{
										case 0:
											r = 255;
											g = 255;
											b = 255;
											break;
										case 1:
											r = 255;
											g = 16;
											b = 16;
											break;
										case 2:
											r = 66;
											g = 115;
											b = 247;
											break;
										}
										break;
								}
								break;
						}
						break;
				}
			m_pLabelFlag[i]->SetFgColor(Color(r,g,b,255));
			m_pLabelFlag[i]->SetText( text );
			m_pLabelFlag[i]->SizeToContents();
			
			//BP hack to widen the label so no cropping can occur
			m_pLabelFlag[i]->SetWide(m_pLabelFlag[i]->GetWide() + 5);

			m_pLabelFlag[i]->SetVisible( true );

			//BG2 - Tjoppen - obey HUDSlot
			if( g_Flags[i]->m_iHUDSlot == 0 )
			{
				m_pLabelFlag[i]->SetPos(x_offset,y_offset);
				y_offset += 20;
			}
			else
			{
				m_pLabelFlag[i]->SetPos(x_offset, 20*(g_Flags[i]->m_iHUDSlot - 1) + 5 );
			}
			//
		}
		break;
	case 2: // Display Flag status as Icons
		i = 0;
		while (i < MAX_FLAGS)
		{
			m_pLabelFlag[i]->SetVisible(false);
			i++;
		}
		int xinc = 0;	//incremental x.. for old flags
		for( i = 0; i < m_iFlagCount; i++ )
		{
			int x_offset;
			if( g_Flags[i]->m_iHUDSlot < 0 )
			{
				m_pLabelFlag[i]->SetVisible( false );
				continue;
			}
			else if( g_Flags[i]->m_iHUDSlot == 0 )
			{
				x_offset = xinc;
				xinc += 80;
			}
			else
				x_offset = (g_Flags[i]->m_iHUDSlot - 1) * 80;

			//BG2 - Tjoppen - first draw the color of the team holding the flag, then progress bar of capture
			float fTimeToCap = g_Flags[i]->m_flNextCapture - gpGlobals->curtime;
			int iTimeToCap = (int)fTimeToCap;
			//switch( g_Flags[i]->m_iLastTeam )
			switch( g_Flags[i]->GetTeamNumber() )
			{
			case TEAM_UNASSIGNED:
				switch( g_Flags[i]->m_iForTeam )
				{
				case 0:
					m_pIconBlank->DrawSelf( x_offset, 0, ColourWhite );
					break;
				case 1:
					m_pIconRed->DrawSelf( x_offset, 0, ColourWhite );
					break;
				case 2:
					m_pIconBlue->DrawSelf( x_offset, 0, ColourWhite );
					break;
				}
				break;
			case TEAM_AMERICANS:
				m_pIconBlue->DrawSelf( x_offset, 0, ColourWhite );
				break;
			case TEAM_BRITISH:
				m_pIconRed->DrawSelf( x_offset, 0, ColourWhite );
				break;
			}
			
			if( iTimeToCap >= 0 )
			{
				//stepwise transition
				//int coverw = (64 * (iTimeToCap+1)) / (int)(g_Flags[i]->m_flCaptureTime + 1);

				//dod:s style smooth transition
				int coverw = (int)(64.f * (fTimeToCap + 1) / (g_Flags[i]->m_flCaptureTime + 1));

				if( coverw < 0 )
					coverw = 0;

				switch( g_Flags[i]->m_iLastTeam )
				{
				case TEAM_UNASSIGNED:
					//is this needed? unassigned can't capture flags..
					//m_IconCover[i]->DrawSelfCropped(x_offset, 0, x_offset, 0, 32 - coverw, 32, ColourWhite);
					m_pIconBlank->DrawSelfCropped(x_offset, 0, 0, 0, 64 - coverw, 64, ColourWhite);
					break;
				case TEAM_BRITISH:
					//m_IconCover[i]->DrawSelfCropped(x_offset, 0, x_offset, 0, 32 - coverw, 32, ColourRed);
					m_pIconRed->DrawSelfCropped(x_offset, 0, 0, 0, 64 - coverw, 64, ColourWhite);
					break;
				case TEAM_AMERICANS:
					//m_IconCover[i]->DrawSelfCropped(x_offset, 0, x_offset, 0, 32 - coverw, 32, ColourBlue);
					m_pIconBlue->DrawSelfCropped(x_offset, 0, 0, 0, 64 - coverw, 64, ColourWhite);
					break;
				}
			}

            int r=0,g=0,b=0;
			switch( g_Flags[i]->m_iLastTeam )
			{
				case TEAM_AMERICANS:
					r = 66;
					g = 115;
					b = 247;
					r += 188 * (sin(fTimeToCap*4) + 1)/2;
					g += 139 * (sin(fTimeToCap*4) + 1)/2;
					b += 7 * (sin(fTimeToCap*4) + 1)/2;
					break;
				case TEAM_BRITISH:
					r = 255;
					g = 16;
					b = 16;
					g += 238 * (sin(fTimeToCap*4) + 1)/2;
					b += 238 * (sin(fTimeToCap*4) + 1)/2;
					break;
				default:
				case TEAM_UNASSIGNED:
					switch (g_Flags[i]->GetTeamNumber())
					{
						case TEAM_AMERICANS:
							r = 66;
							g = 115;
							b = 247;
							break;
						case TEAM_BRITISH:
							r = 255;
							g = 16;
							b = 16;
							break;
						case TEAM_UNASSIGNED:
							switch (g_Flags[i]->m_iRequestingCappers)
							{
								case TEAM_BRITISH:
									r = 255;
									g = 16;
									b = 16;
									g += 238 * (int)((sin(fTimeToCap*8) + 2.7f)/2);
									b += 238 * (int)((sin(fTimeToCap*8) + 2.7f)/2);
									break;
								case TEAM_AMERICANS:
									r = 66;
									g = 115;
									b = 247;
									r += 188 * (int)((sin(fTimeToCap*8) + 2.7f)/2);
									g += 139 * (int)((sin(fTimeToCap*8) + 2.7f)/2);
									b += 7 * (int)((sin(fTimeToCap*8) + 2.7f)/2);
									break;
								case TEAM_UNASSIGNED:
									switch( g_Flags[i]->m_iForTeam )
									{
									case 0:
										r = 255;
										g = 255;
										b = 255;
										break;
									case 1:
										r = 255;
										g = 16;
										b = 16;
										break;
									case 2:
										r = 66;
										g = 115;
										b = 247;
										break;
									}
									break;
							}
							break;
					}
					break;
			}

			Q_snprintf( text, 512, "%i/%i", g_Flags[i]->m_iNearbyPlayers, g_Flags[i]->m_iCapturePlayers );
			m_pLabelFlag[i]->SetText( text );
			m_pLabelFlag[i]->SizeToContents();
			//m_pLabelFlag[i]->SetVisible( true );

			//m_pLabelFlag[i]->SetPos( (x_offset + 32), 64 );
			//center on icon
			int w,h;
			m_pLabelFlag[i]->GetSize( w, h );
			m_pLabelFlag[i]->SetPos( (x_offset + 32) - w/2, 32 - h/2 );

			m_pLabelFlag[i]->SetFgColor( Color(r,g,b,255) );
			m_pLabelFlag[i]->SetVisible(true);

			//x_offset += 80;
		}
		break;
	}
}