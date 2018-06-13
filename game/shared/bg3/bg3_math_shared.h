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

#ifndef BG3_MATH
#define BG3_MATH

#include "bg3\Math\bg3_rand.h"

#ifndef CLIENT_DLL
#include "../../server/cbase.h"

#else
#include "../../client/cbase.h"

#endif 

#define SIGNOF(a) ((a) < 0 ? -1 : 1)

//Length of vector between two entities
vec_t EntDist(const CBaseEntity& ent1, const CBaseEntity& ent2);

inline void ClampVectorLength(Vector& v, vec_t max) {
	vec_t len = v.Length();
	if (len > max) {
		v *= max / len;
	}
}

inline void ScaleVectorToRandomLength(Vector& v, vec_t min, vec_t max) {
	v.NormalizeInPlace();
	v *= RndFloat(min, max);
}

inline void ScaleVectorToLength(Vector& v, vec_t len) {
	v *= len / v.Length();
}

typedef float rad_t;

/************************************************************************************
* Does the vector math in the XY plane to decide where a world location is relative
* to the player's view angles. Returned results are from the unit circle, such
* that a result of 0 is to the right of the player. pi/2 is forward, pi is left,
* 3pi/2 is behind, etc.
* One version is for where the vector is a displacement from the player,
* and the other is for where the vector is an absolute world location
************************************************************************************/
rad_t VectorAngleFromPlayerRelative(CBasePlayer* pPlayer, const Vector& vOffset);
inline rad_t VectorAngleFromPlayer(CBasePlayer* pPlayer, const Vector& vWorldLocation) {
	Vector offset = vWorldLocation - pPlayer->GetAbsOrigin(); 
	return VectorAngleFromPlayerRelative(pPlayer, offset);
}

/*
Given a sin and cos, returns the actual angle, in radians
*/
rad_t AngleFromSinCos(float sin, float cos);

//Calculates the number of radians between two angles, catching border cases
//angle is from r1 to r2, so negative return possible
rad_t AngleDiffRad(rad_t r1, rad_t r2);


/************************************************************************************
* Returns the absolute value of the distance between the given number and the
* nearest even number.
************************************************************************************/
inline float DistanceFromEven(float val) {
	int truncated = val;
	float diff = val - truncated;
	return (truncated & 1) ? 1 - diff : diff;
}

#endif //BG3_MATH