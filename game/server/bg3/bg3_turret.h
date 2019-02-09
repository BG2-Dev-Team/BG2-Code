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
battlegrounds3.com

Note that because of the sheer volume of files in the Source SDK this
notice cannot be put in all of them, but merely the ones that have any
changes from the original SDK.
In order to facilitate easy searching, all changes are and must be
commented on the following form:

//BG3 - <name of contributer>[ - <small description>]
*/

#ifndef BG3_TURRET_H
#define BG3_TURRET_H

#include "cbase.h"

/*****************************************************************
* Entity handles fire events for swivel gun
*****************************************************************/
class CTurretFire : public CServerOnlyPointEntity {
	DECLARE_CLASS(CTurretFire, CServerOnlyPointEntity);
	DECLARE_DATADESC();

private:

	//handles bullet & damage logic, higher accuracy radius is less accurate
	static void DoFireEvent(CBasePlayer* pAttacker, const Vector& vMuzzle, Vector vDirection, float flAccuracyConeRadius, 
		vec_t vRange, byte iBaseDamage, byte iNumBullets);

public:
	float	m_flAccuracyConeRadius = 0.03f;
	vec_t	m_flRange = 2048.f;
	int		m_iBaseDamage = 128;
	int		m_iNumBullets = 8;

	Vector	m_vOffsetFromParent; //the entity we're based on doesn't do parenting properly

	//string_t	m_iszParent;

	//because the entity we're based on doesn't do parenting properly
	//this is technically more efficient too, because we're only
	//doing the parenting-like calculations when we fire, but whatever
	float	m_flMuzzleHeight;
	float	m_flMuzzleForward;

	void	InputFire(inputdata_t& inputdata);

	void	Spawn() override;
};


#endif //BG3_TURRET_H