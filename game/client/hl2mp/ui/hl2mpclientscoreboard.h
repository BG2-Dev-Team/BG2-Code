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
#include "bg3/persistent/versioning.h"

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
	virtual void ShowPanel(bool bShow) override {
		NVersioning::MarkBetaTestParticipation();
		BaseClass::ShowPanel(bShow);
	}

private:
	virtual void AddHeader(); // add the start header of the scoreboard
	virtual void AddSection(int teamType, int teamNumber); // add a new section header for a team

	int GetSectionFromTeamNumber( int teamNumber );
	/*enum //BG2 - depreciated - found this while porting to 2016 engine - Awesome
	{ 
		CSTRIKE_NAME_WIDTH = 320,
		CSTRIKE_CLASS_WIDTH = 56,
		CSTRIKE_SCORE_WIDTH = 40,
		CSTRIKE_DEATH_WIDTH = 46,
		CSTRIKE_PING_WIDTH = 46,
//		CSTRIKE_VOICE_WIDTH = 40, 
//		CSTRIKE_FRIENDS_WIDTH = 24,
	};*/

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
