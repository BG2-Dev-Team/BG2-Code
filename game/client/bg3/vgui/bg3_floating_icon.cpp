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

Outside contributer - Tingtom - assisted Ricochet with floating flag icon hud

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
#include "bg2/c_flag.h"
#include "bg3_floating_icon.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DECLARE_HUDELEMENT(CHudFloatingIcon);

#define FLAG_HEIGHT 160

//==============================================
// CHudFlags's CHudFlags
// Constructor
//==============================================
CHudFloatingIcon::CHudFloatingIcon(const char *pElementName) :
CHudElement(pElementName), BaseClass(NULL, "HudFloatingIcon") //BG3 - Tingtom and Ricochet - defined in scripts\HudLayout.res
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent(pParent);
	Color ColourWhite(255, 255, 255, 255);
	SetHiddenBits(HIDEHUD_MISCSTATUS);
	//vgui::HFont font = pScheme->GetFont( "DracoLucidaRawks" );
	for (int i = 0; i < MAX_FLAGS; i++)
	{
		/*m_IconFlag[i] = NULL;
		m_IconCover[i] = NULL;*/
		m_pLabelFlag[i] = new vgui::Label(this, "RoundState_warmup", "omg r teh lolzors" /*vgui::localize()->Find( "#Clan_warmup_mode" )*/);
		m_pLabelFlag[i]->SetPaintBackgroundEnabled(false);
		m_pLabelFlag[i]->SetPaintBorderEnabled(false);
		m_pLabelFlag[i]->SizeToContents();
		m_pLabelFlag[i]->SetContentAlignment(vgui::Label::a_west);
		m_pLabelFlag[i]->SetFgColor(ColourWhite);
		m_pLabelFlag[i]->SetVisible(false);
		//m_pLabelFlag[i]->SetFont(font);
	}

	m_pIconBlank = m_pIconRed = m_pIconBlue;
}

//==============================================
// CHudFlags's ApplySchemeSettings
// applies the schemes
//==============================================
void CHudFloatingIcon::ApplySchemeSettings(IScheme *scheme)
{
	BaseClass::ApplySchemeSettings(scheme);
	vgui::HFont font = scheme->GetFont("DracoLucidaRawks");
	for (int i = 0; i < MAX_FLAGS; i++)
	{
		m_pLabelFlag[i]->SetFont(font);
	}

	SetPaintBackgroundEnabled(false);
	SetSize(ScreenWidth(), ScreenHeight());
}

//==============================================
// CHudFlags's Init
// Inits any vars needed
//==============================================
void CHudFloatingIcon::Init(void)
{
}

//==============================================
// CHudFlags's VidInit
// Inits any textures needed
//==============================================
void CHudFloatingIcon::VidInit(void)
{
	m_pIconBlank = gHUD.GetIcon("hud_flagicon_blank");
	m_pIconRed = gHUD.GetIcon("hud_flagicon_red");
	m_pIconBlue = gHUD.GetIcon("hud_flagicon_blue");
}

bool CHudFloatingIcon::ShouldDraw(void)
{

	if (!g_Flags.Count()) //No flags? Die here. -HairyPotter
		return false;

	C_HL2MP_Player *pHL2Player = dynamic_cast<C_HL2MP_Player*>(C_HL2MP_Player::GetLocalPlayer());
	C_BaseCombatWeapon *wpn = pHL2Player->GetActiveWeapon();
	// Don't draw hud if we're using Iron Sights. -HairyPotter
	if (wpn && wpn->m_bIsIronsighted)
		return false;

	return CHudElement::ShouldDraw();
}

float easeIn(float t, float b, float c, float d) {
	return c * (t /= d)*t*t + b;
}

float easeOut(float t, float b, float c, float d) {
	return c * ((t = t / d - 1)*t*t + 1) + b;
}

float easeInOut(float t, float b, float c, float d) {
	if ((t /= d / 2) < 1) return c / 2 * t*t*t + b;
	return c / 2 * ((t -= 2)*t*t + 2) + b;
}

//==============================================
// CHudFlags's Paint
// errr... paints the panel
//==============================================
void CHudFloatingIcon::Paint()
{
	int m_iFlagCount = g_Flags.Count();

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if (!pPlayer)
		return;

	char text[512];
	Color ColourWhite(255, 255, 255, 255);
	//Color ColourRed(255, 0, 0, 255);
	//Color ColourBlue(0, 0, 255, 255);

	int screenWidth, screenHeight;
	vgui::surface()->GetScreenSize(screenWidth, screenHeight);

	CUtlVector<Vector2D> flagPositions;
	int minY = screenHeight;

	for (int i = 0; i < m_iFlagCount; i++)
	{
		//BG3 - Tingtom and Ricochet - For crosshair to show floating icon above flag entity
		float x, y;
		x = screenWidth / 2;
		y = screenHeight / 2;

		Vector screen;
		ScreenTransform(g_Flags[i]->GetAbsOrigin() + Vector(0, 0, FLAG_HEIGHT), screen); //BG3 - Tingtom and Ricochet - Vector for moving origin up a bit on flag, third value is height on flag entity pole
		//Msg("%i%i\n", x, y); //BG3 - Tingtom and Ricochet - was used for debugging

		//BG3 - Tingtom and Ricochet - To keep the floating flag icon at the top of the flag pole
		x += 0.5f * screen[0] * screenWidth + 0.5f;
		y -= 0.5f * screen[1] * screenHeight + 0.5f;

		//BG3 - Tingtom and Ricochet - Distance check to not draw icons if the length between player minus the length of the flag is greater than a value
		if ((pPlayer->GetAbsOrigin() - g_Flags[i]->GetAbsOrigin()).Length() > 2048)
		{
			if (y > 0 && y < screenHeight)
				minY = min(y, minY);
			continue;
		}

		flagPositions.AddToTail(Vector2D(x, y));
	}

	for (int i = 0; i < flagPositions.Size(); i++)
	{
		Vector2D pos = flagPositions[i];

		//float alpha = (((pPlayer->GetAbsOrigin() - g_Flags[i]->GetAbsOrigin()).Length() + 256.0f) / 400.0f) * 255.0f;

		//Don't allow alpha to go over 255
		//alpha = min(alpha, 255.0f);

		//Msg("%f\n", alpha);

		//Color ColourWhite(255, 255, 255, alpha);

		if (pos.x <= 0 || pos.x >= screenWidth || minY == screenHeight)
		{
			m_pLabelFlag[i]->SetVisible(false);
			continue;
		}

		//Set all y positions to the minimum value
		pos.y = minY;

		switch (g_Flags[i]->GetTeamNumber())
		{
		case TEAM_UNASSIGNED:
			switch (g_Flags[i]->m_iForTeam)
			{
			case 0:
				m_pIconBlank->DrawSelf(pos.x - m_pIconBlank->Width() / 2, pos.y - m_pIconBlank->Height() / 2, ColourWhite); //BG3 - Tingtom and Ricochet - Width() and Height() to center icon on flag entity pole
				break;
			case 1:
				m_pIconRed->DrawSelf(pos.x - m_pIconBlank->Width() / 2, pos.y - m_pIconBlank->Height() / 2, ColourWhite);
				break;
			case 2:
				m_pIconBlue->DrawSelf(pos.x - m_pIconBlank->Width() / 2, pos.y - m_pIconBlank->Height() / 2, ColourWhite);
				break;
			}
			break;
		case TEAM_AMERICANS:
			m_pIconBlue->DrawSelf(pos.x - m_pIconBlank->Width() / 2, pos.y - m_pIconBlank->Height() / 2, ColourWhite);
			break;
		case TEAM_BRITISH:
			m_pIconRed->DrawSelf(pos.x - m_pIconBlank->Width() / 2, pos.y - m_pIconBlank->Height() / 2, ColourWhite);
			break;
		}

		//BG3 - Tingtom and Ricochet - copied from bg2_hud_flags.cpp to get the player amount for capping, and flag name

		//BG2 - Tjoppen - first draw the color of the team holding the flag, then progress bar of capture
		float fTimeToCap = g_Flags[i]->m_flNextCapture - gpGlobals->curtime;
		int iTimeToCap = (int)fTimeToCap;

		if (iTimeToCap >= 0)
		{
			//stepwise transition
			//int coverw = (64 * (iTimeToCap+1)) / (int)(g_Flags[i]->m_flCaptureTime + 1);

			//dod:s style smooth transition
			int coverw = (int)(128.f * (fTimeToCap + 1) / (g_Flags[i]->m_flCaptureTime + 1));

			if (coverw < 0)
				coverw = 0;

			switch (g_Flags[i]->m_iLastTeam)
			{
			case TEAM_UNASSIGNED:
				//is this needed? unassigned can't capture flags..
				//m_IconCover[i]->DrawSelfCropped(x_offset, 0, x_offset, 0, 32 - coverw, 32, ColourWhite);
				m_pIconBlank->DrawSelfCropped(pos.x - m_pIconBlank->Width() / 2, pos.y - m_pIconBlank->Height() / 2, 0, 0, 128 - coverw, 64, ColourWhite);
				break;
			case TEAM_BRITISH:
				//m_IconCover[i]->DrawSelfCropped(x_offset, 0, x_offset, 0, 32 - coverw, 32, ColourRed);
				m_pIconRed->DrawSelfCropped(pos.x - m_pIconRed->Width() / 2, pos.y - m_pIconRed->Height() / 2, 0, 0, 128 - coverw, 64, ColourWhite);
				break;
			case TEAM_AMERICANS:
				//m_IconCover[i]->DrawSelfCropped(x_offset, 0, x_offset, 0, 32 - coverw, 32, ColourBlue);
				m_pIconBlue->DrawSelfCropped(pos.x - m_pIconBlue->Width() / 2, pos.y - m_pIconBlue->Height() / 2, 0, 0, 128 - coverw, 64, ColourWhite);
				break;
			}
		}

		int r = 0, g = 0, b = 0;
		switch (g_Flags[i]->m_iLastTeam)
		{
		case TEAM_AMERICANS:
			r = 66;
			g = 115;
			b = 247;
			r += 188 * (sin(fTimeToCap * 4) + 1) / 2;
			g += 139 * (sin(fTimeToCap * 4) + 1) / 2;
			b += 7 * (sin(fTimeToCap * 4) + 1) / 2;
			break;
		case TEAM_BRITISH:
			r = 255;
			g = 16;
			b = 16;
			g += 238 * (sin(fTimeToCap * 4) + 1) / 2;
			b += 238 * (sin(fTimeToCap * 4) + 1) / 2;
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
					g += 238 * (int)((sin(fTimeToCap * 8) + 2.7f) / 2);
					b += 238 * (int)((sin(fTimeToCap * 8) + 2.7f) / 2);
					break;
				case TEAM_AMERICANS:
					r = 66;
					g = 115;
					b = 247;
					r += 188 * (int)((sin(fTimeToCap * 8) + 2.7f) / 2);
					g += 139 * (int)((sin(fTimeToCap * 8) + 2.7f) / 2);
					b += 7 * (int)((sin(fTimeToCap * 8) + 2.7f) / 2);
					break;
				case TEAM_UNASSIGNED:
					switch (g_Flags[i]->m_iForTeam)
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

		Q_snprintf(text, sizeof(text), "%s %i/%i", g_Flags[i]->m_sFlagName, g_Flags[i]->m_iNearbyPlayers,
			g_Flags[i]->m_iCapturePlayers);
		if (iTimeToCap > 0)
		{
			char text_add[128];
			Q_snprintf(text_add, sizeof(text_add), " - %i", (int)iTimeToCap);
			strcat(text, text_add);
		}
		m_pLabelFlag[i]->SetText(text);
		m_pLabelFlag[i]->SizeToContents();
		//m_pLabelFlag[i]->SetVisible( true );

		//m_pLabelFlag[i]->SetPos( (x_offset + 32), 64 );
		//center on icon
		int w, h;
		m_pLabelFlag[i]->GetSize(w, h);
		//BG3 - Tingtom and Ricochet - where the text is on the floating icon
		m_pLabelFlag[i]->SetPos((pos.x) - w / 2, (pos.y - 45) - h / 2); //BG3 - Ricochet - originally SetPos((x + 100) - w / 2, 40 - h / 2);

		m_pLabelFlag[i]->SetFgColor(Color(r, g, b, 255));
		m_pLabelFlag[i]->SetVisible(true);

		//x_offset += 80;

	}

}
