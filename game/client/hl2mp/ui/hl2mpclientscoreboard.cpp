//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "hud.h"
#include "hl2mpclientscoreboard.h"
#include "c_team.h"
#include "c_playerresource.h"
#include "c_hl2mp_player.h"
#include "hl2mp_gamerules.h"

#include <KeyValues.h>

#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui_controls/SectionedListPanel.h>

#include "voice_status.h"

#include <FileSystem.h> //BG2 - HairyPotter

using namespace vgui;

#define TEAM_MAXCOUNT			5

extern ConVar sv_show_damages;
ConVar cl_scoreboard("cl_scoreboard", "3", FCVAR_ARCHIVE, "Scoreboard setting for BG2. 1 = Old Scoreboard, 2 = 1.5a Scoreboard, 3 = 2.0 scoreboard.");
// id's of sections used in the scoreboard


const int NumSegments = 7;
static int coord[NumSegments + 1] = {
	0,
	1,
	2,
	3,
	4,
	6,
	9,
	10
};

//-----------------------------------------------------------------------------
// Purpose: Konstructor
//-----------------------------------------------------------------------------
CHL2MPClientScoreBoardDialog::CHL2MPClientScoreBoardDialog(IViewPort *pViewPort) :CClientScoreBoardDialog(pViewPort)
{
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CHL2MPClientScoreBoardDialog::~CHL2MPClientScoreBoardDialog()
{
}

//-----------------------------------------------------------------------------
// Purpose: Paint background for rounded corners
//-----------------------------------------------------------------------------
void CHL2MPClientScoreBoardDialog::PaintBackground()
{
	m_pPlayerList->SetBgColor(Color(0, 0, 0, 0));
	m_pBritishPlayerList->SetBgColor(Color(0, 0, 0, 0));

	m_pPlayerList->SetBorder(NULL);
	m_pBritishPlayerList->SetBorder(NULL);

	surface()->DrawSetColor(m_bgColor);
	surface()->DrawSetTextColor(m_bgColor);

	int wide, tall;
	GetSize(wide, tall);

	//BG2 - Background Image - HairyPotter
	if (cl_scoreboard.GetInt() == 3 && m_pImage)
	{
		m_pImage->SetSize(wide, tall);
		m_pImage->Paint();
		//
	}
	else //Older scoreboards.
	{
		int x1, x2, y1, y2;
		surface()->DrawSetColor(m_bgColor);
		surface()->DrawSetTextColor(m_bgColor);

		int i;

		// top-left corner --------------------------------------------------------
		int xDir = 1;
		int yDir = -1;
		int xIndex = 0;
		int yIndex = NumSegments - 1;
		int xMult = 1;
		int yMult = 1;
		int x = 0;
		int y = 0;
		for (i = 0; i<NumSegments; ++i)
		{
			x1 = min(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
			x2 = max(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
			y1 = max(y + coord[yIndex] * yMult, y + coord[yIndex + 1] * yMult);
			y2 = y + coord[NumSegments];
			surface()->DrawFilledRect(x1, y1, x2, y2);

			xIndex += xDir;
			yIndex += yDir;
		}

		// top-right corner -------------------------------------------------------
		xDir = 1;
		yDir = -1;
		xIndex = 0;
		yIndex = NumSegments - 1;
		x = wide;
		y = 0;
		xMult = -1;
		yMult = 1;
		for (i = 0; i<NumSegments; ++i)
		{
			x1 = min(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
			x2 = max(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
			y1 = max(y + coord[yIndex] * yMult, y + coord[yIndex + 1] * yMult);
			y2 = y + coord[NumSegments];
			surface()->DrawFilledRect(x1, y1, x2, y2);
			xIndex += xDir;
			yIndex += yDir;
		}

		// bottom-right corner ----------------------------------------------------
		xDir = 1;
		yDir = -1;
		xIndex = 0;
		yIndex = NumSegments - 1;
		x = wide;
		y = tall;
		xMult = -1;
		yMult = -1;
		for (i = 0; i<NumSegments; ++i)
		{
			x1 = min(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
			x2 = max(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
			y1 = y - coord[NumSegments];
			y2 = min(y + coord[yIndex] * yMult, y + coord[yIndex + 1] * yMult);
			surface()->DrawFilledRect(x1, y1, x2, y2);
			xIndex += xDir;
			yIndex += yDir;
		}

		// bottom-left corner -----------------------------------------------------
		xDir = 1;
		yDir = -1;
		xIndex = 0;
		yIndex = NumSegments - 1;
		x = 0;
		y = tall;
		xMult = 1;
		yMult = -1;
		for (i = 0; i<NumSegments; ++i)
		{
			x1 = min(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
			x2 = max(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
			y1 = y - coord[NumSegments];
			y2 = min(y + coord[yIndex] * yMult, y + coord[yIndex + 1] * yMult);
			surface()->DrawFilledRect(x1, y1, x2, y2);
			xIndex += xDir;
			yIndex += yDir;
		}

		// paint between top left and bottom left ---------------------------------
		x1 = 0;
		x2 = coord[NumSegments];
		y1 = coord[NumSegments];
		y2 = tall - coord[NumSegments];
		surface()->DrawFilledRect(x1, y1, x2, y2);

		// paint between left and right -------------------------------------------
		x1 = coord[NumSegments];
		x2 = wide - coord[NumSegments];
		y1 = 0;
		y2 = tall;
		surface()->DrawFilledRect(x1, y1, x2, y2);

		// paint between top right and bottom right -------------------------------
		x1 = wide - coord[NumSegments];
		x2 = wide;
		y1 = coord[NumSegments];
		y2 = tall - coord[NumSegments];
		surface()->DrawFilledRect(x1, y1, x2, y2);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Paint border for rounded corners
//-----------------------------------------------------------------------------
void CHL2MPClientScoreBoardDialog::PaintBorder()
{
	if (cl_scoreboard.GetInt() == 3)
		return;

	int x1, x2, y1, y2;
	surface()->DrawSetColor(m_borderColor);
	surface()->DrawSetTextColor(m_borderColor);

	int wide, tall;
	GetSize(wide, tall);

	int i;

	// top-left corner --------------------------------------------------------
	int xDir = 1;
	int yDir = -1;
	int xIndex = 0;
	int yIndex = NumSegments - 1;
	int xMult = 1;
	int yMult = 1;
	int x = 0;
	int y = 0;
	for (i = 0; i<NumSegments; ++i)
	{
		x1 = MIN(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
		x2 = MAX(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
		y1 = MIN(y + coord[yIndex] * yMult, y + coord[yIndex + 1] * yMult);
		y2 = MAX(y + coord[yIndex] * yMult, y + coord[yIndex + 1] * yMult);
		surface()->DrawFilledRect(x1, y1, x2, y2);

		xIndex += xDir;
		yIndex += yDir;
	}

	// top-right corner -------------------------------------------------------
	xDir = 1;
	yDir = -1;
	xIndex = 0;
	yIndex = NumSegments - 1;
	x = wide;
	y = 0;
	xMult = -1;
	yMult = 1;
	for (i = 0; i<NumSegments; ++i)
	{
		x1 = MIN(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
		x2 = MAX(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
		y1 = MIN(y + coord[yIndex] * yMult, y + coord[yIndex + 1] * yMult);
		y2 = MAX(y + coord[yIndex] * yMult, y + coord[yIndex + 1] * yMult);
		surface()->DrawFilledRect(x1, y1, x2, y2);
		xIndex += xDir;
		yIndex += yDir;
	}

	// bottom-right corner ----------------------------------------------------
	xDir = 1;
	yDir = -1;
	xIndex = 0;
	yIndex = NumSegments - 1;
	x = wide;
	y = tall;
	xMult = -1;
	yMult = -1;
	for (i = 0; i<NumSegments; ++i)
	{
		x1 = MIN(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
		x2 = MAX(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
		y1 = MIN(y + coord[yIndex] * yMult, y + coord[yIndex + 1] * yMult);
		y2 = MAX(y + coord[yIndex] * yMult, y + coord[yIndex + 1] * yMult);
		surface()->DrawFilledRect(x1, y1, x2, y2);
		xIndex += xDir;
		yIndex += yDir;
	}

	// bottom-left corner -----------------------------------------------------
	xDir = 1;
	yDir = -1;
	xIndex = 0;
	yIndex = NumSegments - 1;
	x = 0;
	y = tall;
	xMult = 1;
	yMult = -1;
	for (i = 0; i<NumSegments; ++i)
	{
		x1 = MIN(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
		x2 = MAX(x + coord[xIndex] * xMult, x + coord[xIndex + 1] * xMult);
		y1 = MIN(y + coord[yIndex] * yMult, y + coord[yIndex + 1] * yMult);
		y2 = MAX(y + coord[yIndex] * yMult, y + coord[yIndex + 1] * yMult);
		surface()->DrawFilledRect(x1, y1, x2, y2);
		xIndex += xDir;
		yIndex += yDir;
	}

	// top --------------------------------------------------------------------
	x1 = coord[NumSegments];
	x2 = wide - coord[NumSegments];
	y1 = 0;
	y2 = 1;
	surface()->DrawFilledRect(x1, y1, x2, y2);

	// bottom -----------------------------------------------------------------
	x1 = coord[NumSegments];
	x2 = wide - coord[NumSegments];
	y1 = tall - 1;
	y2 = tall;
	surface()->DrawFilledRect(x1, y1, x2, y2);

	// left -------------------------------------------------------------------
	x1 = 0;
	x2 = 1;
	y1 = coord[NumSegments];
	y2 = tall - coord[NumSegments];
	surface()->DrawFilledRect(x1, y1, x2, y2);

	// right ------------------------------------------------------------------
	x1 = wide - 1;
	x2 = wide;
	y1 = coord[NumSegments];
	y2 = tall - coord[NumSegments];
	surface()->DrawFilledRect(x1, y1, x2, y2);
}

//-----------------------------------------------------------------------------
// Purpose: Apply scheme settings
//-----------------------------------------------------------------------------
void CHL2MPClientScoreBoardDialog::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	m_bgColor = GetSchemeColor("SectionedListPanel.BgColor", GetBgColor(), pScheme);
	m_borderColor = pScheme->GetColor("FgColor", Color(0, 0, 0, 0));

	SetBgColor(Color(0, 0, 0, 0));
	SetBorder(pScheme->GetBorder("BaseBorder"));
}


//-----------------------------------------------------------------------------
// Purpose: sets up base sections
//-----------------------------------------------------------------------------
void CHL2MPClientScoreBoardDialog::InitScoreboardSections()
{
	m_pPlayerList->SetBgColor(Color(0, 0, 0, 0));
	m_pBritishPlayerList->SetBgColor(Color(0, 0, 0, 0));

	m_pPlayerList->SetBorder(NULL);
	m_pBritishPlayerList->SetBorder(NULL);

	// fill out the structure of the scoreboard
	AddHeader();

	// add the team sections
	AddSection(TYPE_TEAM, TEAM_AMERICANS);
	AddSection(TYPE_TEAM, TEAM_BRITISH);
	AddSection(TYPE_TEAM, TEAM_SPECTATOR);

	//Put this here because it updates only when you first bring up the scoreboard. -HairyPotter
	if (cl_scoreboard.GetInt() == 3)
	{
		m_pImage = NULL;
		char temp[160] = "",
			mapimage[128] = "",
			mapname[64] = "";

		Q_FileBase(engine->GetLevelName(), mapname, sizeof(mapname)); //Get the map name.
		Q_snprintf(mapimage, sizeof(mapimage), "scoreboard/maps/%s", mapname); // This is for the GetImage function.
		Q_snprintf(temp, sizeof(temp), "materials/VGUI/%s.vmt", mapimage); // This is used to see if the file exists.

		if (g_pFullFileSystem->FileExists(temp))
			m_pImage = scheme()->GetImage(mapimage, false);
		else
			m_pImage = scheme()->GetImage("scoreboard/scoreboard", false);
	}
	//
}

//-----------------------------------------------------------------------------
// Purpose: resets the scoreboard team info
//-----------------------------------------------------------------------------
void CHL2MPClientScoreBoardDialog::UpdateTeamInfo()
{
	if (g_PR == NULL)
		return;

	int iNumPlayersInGame = 0;

	for (int j = 1; j <= gpGlobals->maxClients; j++)
	{
		if (UTIL_PlayerByIndex(j))
		{
			iNumPlayersInGame++;
		}
	}

	//BG2 - Get the total team damage points. -HairyPotter
	iAmericanDmg = 0;
	iBritishDmg = 0;
	iSpecDmg = 0;

	for (int j = 1; j <= gpGlobals->maxClients; j++)
	{
		C_BasePlayer *pPlayer = UTIL_PlayerByIndex(j);
		if (pPlayer)
		{
			switch (pPlayer->GetTeamNumber())
			{
			case TEAM_AMERICANS:
				iAmericanDmg += g_PR->GetDeaths(j);
				break;
			case TEAM_BRITISH:
				iBritishDmg += g_PR->GetDeaths(j);
				break;
			case TEAM_SPECTATOR:
				iSpecDmg += g_PR->GetDeaths(j);
				break;
			}
		}
	}
	//

	// update the team sections in the scoreboard
	for (int i = TEAM_SPECTATOR; i < TEAM_MAXCOUNT; i++)
	{
		wchar_t *teamName = NULL;
		int sectionID = 0;
		C_Team *team = GetGlobalTeam(i);

		if (team)
		{
			sectionID = GetSectionFromTeamNumber(i);

			// update team name
			wchar_t name[64];
			wchar_t string1[1024];
			wchar_t wNumPlayers[6];

			_snwprintf(wNumPlayers, 6, L"%i", team->Get_Number_Players());

			if (!teamName && team)
			{
				g_pVGuiLocalize->ConvertANSIToUnicode(team->Get_Name(), name, sizeof(name));
				teamName = name;
			}

			if (team->Get_Number_Players() == 1)
			{
				g_pVGuiLocalize->ConstructString(string1, sizeof(string1), g_pVGuiLocalize->Find("#ScoreBoard_Player"), 2, teamName, wNumPlayers);
			}
			else
			{
				g_pVGuiLocalize->ConstructString(string1, sizeof(string1), g_pVGuiLocalize->Find("#ScoreBoard_Players"), 2, teamName, wNumPlayers);
			}

			// update stats
			wchar_t deaths[8];
			wchar_t val[6];
			swprintf(val, L"%d", team->Get_Score());

			switch (i)
			{
			case TEAM_AMERICANS:
				swprintf(deaths, L"%d", iAmericanDmg);
				break;
			case TEAM_BRITISH:
				swprintf(deaths, L"%d", iBritishDmg);
				break;
			case TEAM_SPECTATOR:
				swprintf(deaths, L"%d", iSpecDmg);
				break;
			}

			if (cl_scoreboard.GetInt() == 1) //Old scoreboard.
			{
				m_pPlayerList->ModifyColumn(sectionID, "deaths", deaths);
				m_pPlayerList->ModifyColumn(sectionID, "frags", val);
				if (team->Get_Ping() < 1)
				{
					m_pPlayerList->ModifyColumn(sectionID, "ping", L"");
				}
				else
				{
					swprintf(val, L"%d", team->Get_Ping());
					m_pPlayerList->ModifyColumn(sectionID, "ping", val);
				}
				m_pPlayerList->ModifyColumn(sectionID, "name", string1);
			}
			else //New Scoreboard.
			{
				switch (i)
				{
				case TEAM_BRITISH:	//British is always "sectionID" 1 in new scoreboard, because the british team has their own section.
					//swprintf(deaths, L"%d", iBritishDmg);
					m_pBritishPlayerList->ModifyColumn(1, "deaths", deaths);
					m_pBritishPlayerList->ModifyColumn(1, "frags", val);
					if (team->Get_Ping() < 1)
					{
						m_pBritishPlayerList->ModifyColumn(1, "ping", L"");
					}
					else
					{
						swprintf(val, L"%d", team->Get_Ping());
						m_pBritishPlayerList->ModifyColumn(1, "ping", val);
					}
					m_pBritishPlayerList->ModifyColumn(1, "name", string1);
					break;
				default: //If it's not British just stick everything in the "default" (American) section.
					m_pPlayerList->ModifyColumn(sectionID, "deaths", deaths);
					m_pPlayerList->ModifyColumn(sectionID, "frags", val);
					if (team->Get_Ping() < 1)
					{
						m_pPlayerList->ModifyColumn(sectionID, "ping", L"");
					}
					else
					{
						swprintf(val, L"%d", team->Get_Ping());
						m_pPlayerList->ModifyColumn(sectionID, "ping", val);
					}
					m_pPlayerList->ModifyColumn(sectionID, "name", string1);
					break;
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: adds the top header of the scoreboars
//-----------------------------------------------------------------------------
void CHL2MPClientScoreBoardDialog::AddHeader()
{
	HFont hFallbackFont = scheme()->GetIScheme(GetScheme())->GetFont("DefaultVerySmallFallBack", false);

	//BG2 - Set up the second section here. -HairyPotter
	switch (cl_scoreboard.GetInt())
	{
	case 1: //Old Scoreboard.
		NAME_WIDTH = 220, //Modded for BG2. -HairyPotter
			CLASS_WIDTH = 56,
			SCORE_WIDTH = 40,
			DEATH_WIDTH = 46,
			PING_WIDTH = 46;
		break;

	default: //If not 1, we'll activate new scoreboard.
		NAME_WIDTH = 145, //BG2 - Modded for new scoreboard. -HairyPotter
			CLASS_WIDTH = 35,
			SCORE_WIDTH = 32,
			DEATH_WIDTH = 35,
			PING_WIDTH = 30;

		//Since we're using the new scoreboard, set up the british section next to the default (american) section.
		m_pBritishPlayerList->AddSection(0, "");
		m_pBritishPlayerList->SetSectionAlwaysVisible(0);
		m_pBritishPlayerList->AddColumnToSection(0, "name", "", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), NAME_WIDTH), hFallbackFont);
		m_pBritishPlayerList->AddColumnToSection(0, "class", "", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), CLASS_WIDTH));
		m_pBritishPlayerList->AddColumnToSection(0, "frags", "#PlayerScore", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), SCORE_WIDTH));
		m_pBritishPlayerList->AddColumnToSection(0, "deaths", "Damage", 0, scheme()->GetProportionalScaledValue(DEATH_WIDTH));
		m_pBritishPlayerList->AddColumnToSection(0, "ping", "#PlayerPing", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), PING_WIDTH));
		break;
	}
	//

	// add the top header
	m_pPlayerList->AddSection(0, "");
	m_pPlayerList->SetSectionAlwaysVisible(0);
	m_pPlayerList->AddColumnToSection(0, "name", "", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), NAME_WIDTH), hFallbackFont);
	m_pPlayerList->AddColumnToSection(0, "class", "", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), CLASS_WIDTH));
	m_pPlayerList->AddColumnToSection(0, "frags", "#PlayerScore", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), SCORE_WIDTH));
	//BG2 - Tjoppen - Where is #PlayerDeath defined? HACKHACK for now..
	//m_pPlayerList->AddColumnToSection(0, "deaths", "#PlayerDeath", 0 | SectionedListPanel::COLUMN_RIGHT, scheme()->GetProportionalScaledValue( CSTRIKE_DEATH_WIDTH ) );
	m_pPlayerList->AddColumnToSection(0, "deaths", "Damage", 0, scheme()->GetProportionalScaledValue(DEATH_WIDTH));
	//
	m_pPlayerList->AddColumnToSection(0, "ping", "#PlayerPing", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), PING_WIDTH));
}

//-----------------------------------------------------------------------------
// Purpose: Adds a new section to the scoreboard (i.e the team header)
//-----------------------------------------------------------------------------
void CHL2MPClientScoreBoardDialog::AddSection(int teamType, int teamNumber)
{
	HFont hFallbackFont = scheme()->GetIScheme(GetScheme())->GetFont("DefaultVerySmallFallBack", false);

	int sectionID = GetSectionFromTeamNumber(teamNumber);
	if (teamNumber != TEAM_BRITISH || cl_scoreboard.GetInt() == 1) //BG2 - We go by team numbers now, unless we're using the old scoreboard. -HairyPotter
	{
		m_pPlayerList->AddSection(sectionID, "", StaticPlayerSortFunc);
		// setup the columns
		m_pPlayerList->AddColumnToSection(sectionID, "name", "", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), NAME_WIDTH), hFallbackFont);
		m_pPlayerList->AddColumnToSection(sectionID, "class", "", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), CLASS_WIDTH));
		m_pPlayerList->AddColumnToSection(sectionID, "frags", "", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), SCORE_WIDTH));
		m_pPlayerList->AddColumnToSection(sectionID, "deaths", "", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), DEATH_WIDTH));
		m_pPlayerList->AddColumnToSection(sectionID, "ping", "", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), PING_WIDTH));

		// set the section to have the team color

		if (teamNumber != TEAM_SPECTATOR || TEAM_UNASSIGNED)
		{
			if (GameResources())
				m_pPlayerList->SetSectionFgColor(sectionID, GameResources()->GetTeamColor(teamNumber));

			m_pPlayerList->SetSectionAlwaysVisible(sectionID);
		}
	}
	else if (teamNumber == TEAM_BRITISH)
	{
		m_pBritishPlayerList->AddSection(1, "", StaticPlayerSortFunc);
		// setup the columns
		m_pBritishPlayerList->AddColumnToSection(1, "name", "", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), NAME_WIDTH), hFallbackFont);
		m_pBritishPlayerList->AddColumnToSection(1, "class", "", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), CLASS_WIDTH));
		m_pBritishPlayerList->AddColumnToSection(1, "frags", "", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), SCORE_WIDTH));
		m_pBritishPlayerList->AddColumnToSection(1, "deaths", "", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), DEATH_WIDTH));
		m_pBritishPlayerList->AddColumnToSection(1, "ping", "", 0, scheme()->GetProportionalScaledValueEx(GetScheme(), PING_WIDTH));

		// set the section to have the team color
		if (teamNumber)
		{
			if (GameResources())
				m_pBritishPlayerList->SetSectionFgColor(1, GameResources()->GetTeamColor(teamNumber));
		}

		m_pBritishPlayerList->SetSectionAlwaysVisible(1);
	}
}

int CHL2MPClientScoreBoardDialog::GetSectionFromTeamNumber(int teamNumber)
{
	switch (teamNumber)
	{
	case TEAM_AMERICANS:
		return 1;
	case TEAM_BRITISH:
		return 2;
	case TEAM_SPECTATOR:
		return 3;
	case TEAM_UNASSIGNED: //BG2 - Added back in so you can see all players in game, even if unassigned. -HairyPotter
		return 3;
	default:
		return 3;
	}
	return 3;
}

//-----------------------------------------------------------------------------
// Purpose: Adds a new row to the scoreboard, from the playerinfo structure
//-----------------------------------------------------------------------------
bool CHL2MPClientScoreBoardDialog::GetPlayerScoreInfo(int playerIndex, KeyValues *kv)
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if (!pPlayer)
		return false;

	kv->SetInt("playerIndex", playerIndex);
	kv->SetInt("team", g_PR->GetTeam(playerIndex));
	kv->SetString("name", g_PR->GetPlayerName(playerIndex));
	//BG2 - Tjoppen - hide non-local player damage
	//kv->SetInt("deaths", g_PR->GetDeaths( playerIndex ));
	if (sv_show_damages.GetBool())
	{
		kv->SetInt("deaths", g_PR->GetDeaths(playerIndex));
	}
	else
	{
		if (playerIndex == pPlayer->entindex())
			kv->SetInt("deaths", g_PR->GetDeaths(playerIndex));
		else
			kv->SetString("deaths", "");
	}
	//
	kv->SetInt("frags", g_PR->GetPlayerScore(playerIndex));
	//BG2 - Tjoppen - dead column
	//kv->SetString("class", "");
	//display for dead participating players
	kv->SetWString("class", !g_PR->IsAlive(playerIndex) && g_PR->GetTeam(playerIndex) >= TEAM_AMERICANS ? g_pVGuiLocalize->Find("#DEAD") : g_pVGuiLocalize->Find("#ALIVE"));
	//

	if (g_PR->GetPing(playerIndex) < 1)
	{
		if (g_PR->IsFakePlayer(playerIndex))
		{
			kv->SetString("ping", "BOT");
		}
		else
		{
			kv->SetString("ping", "");
		}
	}
	else
	{
		kv->SetInt("ping", g_PR->GetPing(playerIndex));
	}

	return true;
}

enum {
	//BG2 - Increased for maxplayer increase. -HairyPotter
	MAX_PLAYERS_PER_TEAM = 32,
	MAX_SCOREBOARD_PLAYERS = 64
};
struct PlayerScoreInfo
{
	int index;
	int frags;
	int deaths;
	bool important;
	bool alive;
};

int PlayerScoreInfoSort(const PlayerScoreInfo *p1, const PlayerScoreInfo *p2)
{
	// check local
	if (p1->important)
		return -1;
	if (p2->important)
		return 1;

	// check alive
	if (p1->alive && !p2->alive)
		return -1;
	if (p2->alive && !p1->alive)
		return 1;

	// check frags
	if (p1->frags > p2->frags)
		return -1;
	if (p2->frags > p1->frags)
		return 1;

	// check deaths
	if (p1->deaths < p2->deaths)
		return -1;
	if (p2->deaths < p1->deaths)
		return 1;

	// check index
	if (p1->index < p2->index)
		return -1;

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MPClientScoreBoardDialog::UpdatePlayerInfo()
{
	m_iSectionId = 0; // 0'th row is a header
	int selectedRow = -1;
	int i;

	CBasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if (!pPlayer || !g_PR)
		return;

	// walk all the players and make sure they're in the scoreboard
	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		bool shouldShow = g_PR->IsConnected(i);
		if (shouldShow)
		{
			// add the player to the list
			KeyValues *playerData = new KeyValues("data");
			GetPlayerScoreInfo(i, playerData);
			int itemID = FindItemIDForPlayerIndex(i);
			int sectionID = GetSectionFromTeamNumber(g_PR->GetTeam(i));

			if (cl_scoreboard.GetInt() != 1) //New Scoreboard
			{
				switch (g_PR->GetTeam(i))
				{
				case TEAM_BRITISH: //British SectionID is always 1, because british have their own section.
					if (itemID == -1)
						itemID = m_pBritishPlayerList->AddItem(1, playerData);
					else
						m_pBritishPlayerList->ModifyItem(itemID, 1, playerData);

					// set the row color based on the players team
					m_pBritishPlayerList->SetItemFgColor(itemID, g_PR->GetTeamColor(TEAM_BRITISH));
					break;
				default: //If not british, put into the "default" (American) section.
					if (itemID == -1)
						itemID = m_pPlayerList->AddItem(sectionID, playerData);
					else
						m_pPlayerList->ModifyItem(itemID, sectionID, playerData);

					// set the row color based on the players team
					m_pPlayerList->SetItemFgColor(itemID, g_PR->GetTeamColor(g_PR->GetTeam(i)));
					break;

				}
			}
			else //Old Scoreboard.
			{
				if (itemID == -1)
					itemID = m_pPlayerList->AddItem(sectionID, playerData);
				else
					m_pPlayerList->ModifyItem(itemID, sectionID, playerData);

				m_pPlayerList->SetItemFgColor(itemID, g_PR->GetTeamColor(g_PR->GetTeam(i)));
			}

			playerData->deleteThis();
		}
		else
		{
			// remove the player
			int itemID = FindItemIDForPlayerIndex(i);
			if (itemID != -1)
			{
				if (cl_scoreboard.GetInt() == 1)
					m_pPlayerList->RemoveItem(itemID);
				else
				{
					switch (g_PR->GetTeam(i))
					{
					case TEAM_BRITISH:
						m_pBritishPlayerList->RemoveItem(itemID);
						break;
					default:
						m_pPlayerList->RemoveItem(itemID);
						break;
					}
				}
			}
		}
	}

	if (selectedRow != -1)
	{
		m_pPlayerList->SetSelectedItem(selectedRow);
		m_pBritishPlayerList->SetSelectedItem(selectedRow);
	}


}
