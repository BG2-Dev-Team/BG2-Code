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
#include "bg3_fonts.h"

namespace vgui {
	Color				g_cBG3TextColor(229, 228, 178, 255);
	Color				g_cBG3TextHighlightColor(253, 252, 201, 255);

	HFont GetDefaultBG3Font(IScheme* pScheme) {
		return pScheme->GetFont("BG3Default30");
	}
	
	static int g_iGlobalTextScale = 55;
	HFont GetDefaultBG3FontScaled(IScheme* pScheme, Label* pForLabel) {
		char buffer[32];
		wchar wbuffer[256];

		int width = pForLabel->GetWide();
		pForLabel->GetText(wbuffer, ARRAYSIZE(wbuffer));
		int numChars = wcslen(wbuffer);

		if (numChars > 0) {
			int pixelsPerChar = width / numChars;

			//do a lerp onto a 5-14 range, those are our base font sizes
			float scale = 1.0f * pixelsPerChar / g_iGlobalTextScale;
			int baseSize = 5 + (scale * 9 + 0.1f);

			//clamp sizes
			baseSize = Clamp(baseSize, 5, 14);

			//search for appropriate size
			int fontSize = baseSize * 5;
			Q_snprintf(buffer, ARRAYSIZE(buffer), "BG3Default%i\0", fontSize);
			return pScheme->GetFont(buffer);
		}
		return 0;
	}
}