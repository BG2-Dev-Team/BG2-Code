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
*/

#ifndef BG3_FONTS_H
#define BG3_FONTS_H

#include <vgui/IScheme.h>
#include <vgui_controls/Label.h>

namespace vgui {
	extern Color				g_cBG3TextColor;
	extern Color				g_cBG3TextHighlightColor;
	 
	HFont GetDefaultBG3Font(IScheme* pScheme);

	HFont GetDefaultBG3FontScaledHorizontal(IScheme* pScheme, Label* pForLabel);
	HFont GetDefaultBG3FontScaledVertical(IScheme* pScheme, Label* pForLabel);

	inline void SetDefaultBG3FontScaled(IScheme* pScheme, Label* pForLabel) {
		pForLabel->SetFont(GetDefaultBG3FontScaledHorizontal(pScheme, pForLabel));
	}

	void MsgW(wchar* text);
}

#endif