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

#include "cbase.h"
#include "bg3_hud_compass.h"

#define SIGNOF(x) ((x) < 0 ? -1 : 1)

using namespace vgui;

/*
void NHudCompass::Init() {
	g_pImage = scheme()->GetImage("hud/compass", false);
}

namespace { bool g_bEveryOtherFrame = false; }

void NHudCompass::Paint() {
	g_bEveryOtherFrame = !g_bEveryOtherFrame;
	if (g_bEveryOtherFrame)
		CalcIdealAngle();

	//g_iAngle++;

	g_pImage->SetRotation(g_iAngle);
	g_pImage->Paint();
}

void NHudCompass::SetNorth(const Vector& vDirection) {
	g_vNorth = vDirection;
}

IImage* NHudCompass::g_pImage = nullptr;

void NHudCompass::CalcIdealAngle() {
	//get local player's view offset from North
	rad_t absAngle = VectorAngleFromPlayerRelative(CBasePlayer::GetLocalPlayer(), g_vNorth);
	//this angle is between 0 and 2pi

	//get offset from current angle
	rad_t diff = AngleDiffRad(g_flAngle, absAngle); //angle from current compass direction to North
	//should be between +pi and -pi

	//don't bother if our current angle is close enough
	if (abs(diff) < 0.06f && g_flAngularVelocity < 0.1f)
		return;

	//assume constant acceleration - don't want to be too much of a jerk!
	g_flAngularAcceleration *= SIGNOF(diff);

	//adjust velocity based on acceleration
	g_flAngularVelocity += g_flAngularAcceleration * gpGlobals->frametime;

	//perform angular damping, otherwise we'll just be a perfect pendulum
	g_flAngularVelocity *= 0.7f;

	//modify angle based on velocity
	g_flAngle += g_flAngularVelocity * gpGlobals->frametime;

	//ensure angle bounds
	//we should technically use a while loop,
	//but with how fast these frames are we shouldn't need it
	if (g_flAngle < 0) g_flAngle += 2 * M_PI_F;
	else if (g_flAngle > 2 * M_PI_F) g_flAngle -= 2 * M_PI_F;

	g_flAngle += 0.1f;

	//covert to degress for painting
	g_iAngle = RAD2DEG(g_flAngle);

	//we're done, paint will do rest
}

rad_t	NHudCompass::g_flAngle = 0.0f;
int		NHudCompass::g_iAngle = 0;
float	NHudCompass::g_flAngularVelocity = 0.0f;
float	NHudCompass::g_flAngularAcceleration = 1.2f;

Vector	NHudCompass::g_vNorth = Vector(0.0f, 1.0f, 0.0f);

CON_COMMAND(compass, "Prints current compass angle to console") {
	NHudCompass::PrintAngle();
}*/