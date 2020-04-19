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
#include "cbase.h"
#include <vector>


//Whether or not colorblind mode is desired
ConVar cl_colorblind_mode("cl_colorblind_mode", "0", FCVAR_ARCHIVE);

//Whether or not colorblind mode is active
ConVar cl_colorblind_mode_active("cl_colorblind_mode_active", "0", FCVAR_ARCHIVE | FCVAR_HIDDEN);

namespace a11y {

	class ColorblindMaterialProfile {
		string_t m_activeMaterialName; //name of the material the game actually loads up
		string_t m_originalMaterialName; //name of the original non-colorblind materials
		string_t m_accessibleMaterialName; //name of the colorblind-corrected material
	};

	std::vector<ColorblindMaterialProfile> g_colorblindProfiles;
	
	void InitColorblindModeParams() {
		//if profile already loaded, skip
		if (g_colorblindProfiles.size() > 0) 
			return;


	}

	void ApplyCurrentColorblindMode() {
		//if desired mode is already active, skip
		if (cl_colorblind_mode.GetBool() == cl_colorblind_mode_active.GetBool())
			return;


	}
}