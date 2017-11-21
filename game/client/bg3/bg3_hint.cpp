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
www.bg2mod.com

Note that because of the sheer volume of files in the Source SDK this
notice cannot be put in all of them, but merely the ones that have any
changes from the original SDK.
In order to facilitate easy searching, all changes are and must be
commented on the following form:

//BG3 - <name of contributer>[ - <small description>]
*/

#include "cbase.h"
#include "bg3_hint.h"
#include "bg2/bg2_hud_main.h"
#include "../shared/bg3/Math/bg3_rand.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

namespace BG3Hint {
	//Translators may not want to translate everything, so we have to calculate
	//how many hints and quotes we have
	int g_iNumBasicHints = -1;
	int g_iNumIntermediateHints = -1;
	int g_iNumAdvancedHints = -1;
	int g_iNumQuotes = -1;

	inline bool HaveCalculatedCounts() { return g_iNumBasicHints != -1; }

	ConVar cl_hintlevel("cl_hintlevel", "1", FCVAR_ARCHIVE, "Determines what setting of hints to display.\n0 - Basic\n1 - Intermediate\n2 - Advanced\n");

	void CalculateCounts() {
		wchar_t* pText;

		pText = g_pVGuiLocalize->Find("#HINT_BSC_NUM");
		g_iNumBasicHints = _wtoi(pText);

		pText = g_pVGuiLocalize->Find("#HINT_INT_NUM");
		g_iNumIntermediateHints = _wtoi(pText);

		pText = g_pVGuiLocalize->Find("#HINT_ADV_NUM");
		g_iNumAdvancedHints = _wtoi(pText);

		pText = g_pVGuiLocalize->Find("#HINT_HIST_NUM");
		g_iNumQuotes = _wtoi(pText);
	}

	char g_pszLocalizedToken[32];
	wchar_t g_pszLocalizedText[256];

	wchar_t* GetHint() {
		if (!HaveCalculatedCounts())
			CalculateCounts();

		int count;

		const char* baseHintText;
		switch (cl_hintlevel.GetInt()) {
		default:
		case 0:
			count = g_iNumBasicHints;
			baseHintText = "#HINT_BSC"; break;
		case 1:
			count = g_iNumIntermediateHints;
			baseHintText = "#HINT_INT"; break;
		case 2:
			count = g_iNumAdvancedHints;
			baseHintText = "#HINT_ADV"; break;
		}

		//Now get a random hint for us
		//reuse stack variable
		count = RndInt(0, count - 1);

		V_snprintf(g_pszLocalizedToken, ARRAYSIZE(g_pszLocalizedToken),
				   "%s%i\0", baseHintText, count);
		//Msg(g_pszLocalizedToken);

		wchar_t* hintPrefix = g_pVGuiLocalize->Find("#HINT_PREFIX");
		wchar_t* rawHint = g_pVGuiLocalize->Find(g_pszLocalizedToken);
		V_snwprintf(g_pszLocalizedText, ARRAYSIZE(g_pszLocalizedText),
					L"%s%s\0", hintPrefix, rawHint);

		return g_pszLocalizedText;
	}

	wchar_t* GetHistoricalQuote() {
		if (!HaveCalculatedCounts())
			CalculateCounts();

		int randomQuoteIndex = RndInt(0, g_iNumQuotes - 1);
		//Msg("%i\n", g_iNumQuotes);

		V_snprintf(g_pszLocalizedToken, ARRAYSIZE(g_pszLocalizedToken),
				   "#HINT_HIST%i", randomQuoteIndex);

		return g_pVGuiLocalize->Find(g_pszLocalizedToken);
	}

	void UpdateLoadingDialogueHintDisplay() {
		//vgui::Panel* pViewPort = g_pClientMode->GetViewport();
		//vgui::Label* pLabel = pViewPort->FindControl<Label>(pszLabelName, true);

		wchar_t* pHintText = g_pVGuiLocalize->Find("#HINT_SPACE");
		static int hintTextLength = wcslen(pHintText);
		wchar_t* hintTextToLoad;

		RndSeed(gpGlobals->curtime);
		if (RndBool(0.5f))
			hintTextToLoad = GetHint();
		else
			hintTextToLoad = GetHistoricalQuote();

		//HACK HACK - this dumps the text straight into the localizer!
		V_snwprintf(pHintText, hintTextLength, hintTextToLoad);

		/*Msg("\nUpdating hint display...");
		if (pLabel) {
			Msg("found %s...", pLabel->GetName());
			char text[128];
			pLabel->GetText(text, 128);
			Msg("its text is %s", text);
			wchar_t* pText;
			if (RndBool(0.5)) {
				pText = GetHistoricalQuote();
			} else {
				pText = GetHint();
			}
			pLabel->SetText(pText, true);
			Msg("...its height is %i", pLabel->GetTall());
		} else {
			Msg("%s could not be found\n", pszLabelName);
		}*/
	}
}

CON_COMMAND(vgui_updateHintDisplay, "Updates the hint display as seen in the loading dialog") {
	BG3Hint::UpdateLoadingDialogueHintDisplay();
}
