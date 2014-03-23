//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef CHL2MPCLIENTSCOREBOARDDIALOG_H
#define CHL2MPCLIENTSCOREBOARDDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include <clientscoreboarddialog.h>

//-----------------------------------------------------------------------------
// Purpose: Game ScoreBoard
//-----------------------------------------------------------------------------
class CHL2MPClientScoreBoardDialog : public CClientScoreBoardDialog
{
private:
	DECLARE_CLASS_SIMPLE(CHL2MPClientScoreBoardDialog, CClientScoreBoardDialog);
	
public:
	CHL2MPClientScoreBoardDialog(IViewPort *pViewPort);
	~CHL2MPClientScoreBoardDialog();


protected:
	// scoreboard overrides
	virtual void InitScoreboardSections();
	virtual void UpdateTeamInfo();
	virtual bool GetPlayerScoreInfo(int playerIndex, KeyValues *outPlayerInfo);
	virtual void UpdatePlayerInfo();

	// vgui overrides for rounded corner background
	virtual void PaintBackground();
	virtual void PaintBorder();
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );

private:
	virtual void AddHeader(); // add the start header of the scoreboard
	virtual void AddSection(int teamType, int teamNumber); // add a new section header for a team

	int GetSectionFromTeamNumber( int teamNumber );

	// rounded corners
	Color					 m_bgColor;
	Color					 m_borderColor;

	int iAmericanDmg, iBritishDmg, iSpecDmg,
		NAME_WIDTH, //Modded for BG2. -HairyPotter
		CLASS_WIDTH,
		SCORE_WIDTH,
		DEATH_WIDTH,
		PING_WIDTH; //BG2 - For the team damage totals. -HairyPotter

	//BG2 - For background image - HairyPotter
	vgui::IImage *m_pImage;
	//
};


#endif // CHL2MPCLIENTSCOREBOARDDIALOG_H
