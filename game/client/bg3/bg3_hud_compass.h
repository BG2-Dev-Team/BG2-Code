/*
The Battle Grounds 3 - A Source modification
Copyright (C) 2017, The Battle Grounds 2 Team and Contributors

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

#ifndef BG3_HUD_COMPASS_H
#define BG3_HUD_COMPASS_H

#include "cbase.h"

#include <vgui/IScheme.h>
#include <vgui/KeyCode.h>
#include <vgui/MouseCode.h>
#include <vgui_controls/HTML.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/ToggleButton.h>
#include <vgui_controls/CheckButton.h>
#include "vgui_bitmapimage.h"
#include "vgui_bitmapbutton.h"
#include <vgui_controls/ImagePanel.h>
#include <imagemouseoverbutton.h>

#include <game/client/iviewport.h>

#include "hl2mp_gamerules.h"
#include "../shared/bg3/bg3_math_shared.h"

/***********************************************************
* Interface for drawing a rotated compass within the current
* VGUI Panel
***********************************************************/
/*abstract_class NHudCompass{
public:
	//Public interface
	static void Init(); //only call this once
	static void Paint();
	static void SetPos(int x, int y) { g_pImage->SetPos(x, y); }
	static void SetSize(int wide, int tall) { g_pImage->SetSize(wide, tall); }
	static void SetNorth(const Vector& vDirection);

	//for debuggin
	static void PrintAngle() { Msg("%i, %f\n", g_iAngle, g_flAngle); }
	
private:
	NHudCompass() {} //private constructor prevents instantiation

	static vgui::IImage* g_pImage;
	static void CalcIdealAngle();

	//this makes the compass a little floaty, like a real compass
	static rad_t	g_flAngle; //from 0 to 2pi, with 0 pointing north
	static int		g_iAngle; //the VGUI uses degrees, so we convert to this
	static float	g_flAngularVelocity;
	static float	g_flAngularAcceleration;

	static Vector g_vNorth;
};*/

#endif //BG3_HUD_COMPASS_H