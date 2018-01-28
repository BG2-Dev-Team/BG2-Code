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
#include "bg3_math_shared.h"



vec_t EntDist(const CBaseEntity& ent1, const CBaseEntity& ent2) {
	return (ent1.GetAbsOrigin() - ent2.GetAbsOrigin()).Length();
}


/************************************************************************************
* Does the vector math in the XY plane to decide where a world location is relative
* to the player's view angles. Returned results are from the unit circle, such
* that a result of 0 is to the right of the player. pi/2 is forward, pi is left,
* 3pi/2 is behind, etc.
************************************************************************************/
rad_t VectorAngleFromPlayerRelative(CBasePlayer* pPlayer, const Vector& vOffset) {
	Vector eyesright; {
		AngleVectors(pPlayer->EyeAngles(), nullptr, &eyesright, nullptr);
		eyesright.z = 0;
		eyesright.NormalizeInPlace();
	}
	Vector offset = vOffset;
	offset.z = 0;
	offset.NormalizeInPlace();
	
	//because the vectors are unit vectors, the results of these are equivelant to sin and cos
	float cos = DotProduct(eyesright, offset);
	Vector cross = CrossProduct(eyesright, offset);
	float sin = cross.Length() * SIGNOF(cross.z); //this should work with the right-hand rule, try it yourself at home

	return AngleFromSinCos(sin, cos);
}

rad_t AngleFromSinCos(float sin, float cos) {
	//from these we derive what the angle actually is
	//acos gives us angles between 0 and pi, so check if the sin tells us that we're actually on the other side
	if (sin < 0) {
		if (cos < 0) {
			//quadrant 3
			float acos = acosf(cos);

			//flip accross x axis
			return acos + 2 * (M_PI_F - acos);
		}
		else {
			//quadrant 4
			//just flip to quadrant 2 then add pi
			return acosf(-cos) + M_PI_F;
		}
	}
	else {
		//quadrants 1 and 2
		return acosf(cos);;
	}
}

rad_t AngleDiffRad(rad_t r1, rad_t r2) {
	//make both positive
	while (SIGNOF(r1) < 0) r1 += 2 * M_PI_F;
	while (SIGNOF(r2) < 0) r2 += 2 * M_PI_F;
	//both are now between 0 and 2pi

	//calc angle, then check for other direction
	rad_t offset = r2 - r1;

	if (offset > M_PI_F)
		//negative direction instead
		offset -= 2 * M_PI_F;

	return offset;
}