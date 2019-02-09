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

#include "cbase.h"
#include "bg3_turret.h"
#include "bg3\bg3_math_shared.h"

BEGIN_DATADESC(CTurretFire)
	DEFINE_KEYFIELD(m_flAccuracyConeRadius, FIELD_FLOAT, "accuracy"),
	DEFINE_KEYFIELD(m_flRange, FIELD_FLOAT, "range"),
	DEFINE_KEYFIELD(m_iBaseDamage, FIELD_INTEGER, "damage"),
	DEFINE_KEYFIELD(m_iNumBullets, FIELD_INTEGER, "numBullets"),

	//DEFINE_KEYFIELD(m_flMuzzleHeight, FIELD_FLOAT, "muzzleHeight"),
	//DEFINE_KEYFIELD(m_flMuzzleForward, FIELD_FLOAT, "muzzleForward"),

	//DEFINE_KEYFIELD(m_iszParent, FIELD_STRING, "parent"),

	DEFINE_INPUTFUNC(FIELD_VOID, "Fire", InputFire),
END_DATADESC()

LINK_ENTITY_TO_CLASS(info_swivel_muzzle, CTurretFire);

void CTurretFire::DoFireEvent(CBasePlayer* pAttacker, const Vector& vMuzzle, Vector vDirection, float flAccuracyConeRadius, 
	vec_t vRange, byte iBaseDamage, byte iNumBullets) {

	//Make the direction unit length
	vDirection.NormalizeInPlace();

	for (; iNumBullets > 0; iNumBullets--) {
		//This will actually give us a cone with a spherical end (like an ice cream cone with the ice cream in it)
		//rather than a cone with a flat end, but it doesn't matter because we're scaling the length anyways
		float& a = flAccuracyConeRadius;
		Vector bubble = Vector(RndFloat(-a, a), RndFloat(-a, a), RndFloat(-a, a));

		if (bubble.Length() > a) {
			ScaleVectorToRandomLength(bubble, 0, a);
		}

		//calculate trajectory
		Vector vBubbledDirection = vDirection + bubble;
		Vector endpos = vBubbledDirection; {
			ScaleVectorToLength(endpos, vRange);
			endpos += vMuzzle;
		}

		trace_t tr;

		UTIL_TraceLine(vMuzzle, endpos, MASK_SHOT, NULL, COLLISION_GROUP_NONE, &tr);
		UTIL_ImpactTrace(&tr, DMG_BULLET);

		extern ConVar sv_simulatedbullets_show_trajectories;
		extern ConVar sv_simulatedbullets_show_trajectories_timeout;

		if (sv_simulatedbullets_show_trajectories.GetBool())
			NDebugOverlay::Line(tr.startpos, tr.endpos, tr.DidHit() ? 0 : 255, tr.DidHitWorld() ? 0 : 255, tr.DidHitWorld() ? 255 : 0, true, sv_simulatedbullets_show_trajectories_timeout.GetFloat());

		CBaseEntity* pVictim = tr.m_pEnt;
		if (tr.DidHitNonWorldEntity() && pVictim) {


			//linear ramp from full damage down to half damage
			byte iDamage = (iBaseDamage / 2) + (iBaseDamage / 2) * (1.0f - tr.fraction);

			CTakeDamageInfo info = CTakeDamageInfo(pAttacker, pAttacker, iDamage, DMG_BULLET | DMG_CRUSH | DMG_NEVERGIB);
			info.SetDamagePosition(tr.endpos);

			pVictim->DispatchTraceAttack(info, vBubbledDirection, &tr);
		}
	}

	//apply any damage that's been accumulated
	ApplyMultiDamage();
}

void CTurretFire::InputFire(inputdata_t& inputdata) {
	CBasePlayer* pPlayer = ToBasePlayer(inputdata.pActivator);
	CBaseEntity* pParent = GetParent();
	if (pPlayer && pParent) {

		//get firing vector from angles
		Vector forward;
		Vector pseudoup;
		AngleVectors(pParent->GetAbsAngles(), &forward, nullptr, &pseudoup);
		ScaleVectorToLength(forward, m_flMuzzleForward);
		ScaleVectorToLength(pseudoup, m_flMuzzleHeight);

		Vector muzzle = pParent->GetAbsOrigin() + pseudoup + forward;

		//NDebugOverlay::Line(pParent->Get, tr.endpos, 122, 122, 255, true, 2.f);

		//dispatch fire event
		DoFireEvent(pPlayer, muzzle, forward, m_flAccuracyConeRadius, m_flRange, m_iBaseDamage, m_iNumBullets);
	}
}

void CTurretFire::Spawn() {
	BaseClass::Spawn();

	/*CBaseEntity* pParent = gEntList.FindEntityByName(NULL, m_iszParent);
	if (pParent) {
		SetParent(pParent);
	}
	else {
		Warning("Couldn't find parent \"%s\" for an instance of info_swivel_muzzle, tell the mapper!\n", m_iszParent.ToCStr());
	}*/
	if (!GetParent())
		Warning("Couldn't find parent for an instance of info_swivel_muzzle, tell the mapper!\n");
	else {
		Warning("info_swivel_muzzle parented to a %s\n", GetParent()->GetClassname());
		m_vOffsetFromParent = GetAbsOrigin() - GetParent()->GetAbsOrigin();

		m_flMuzzleHeight = m_vOffsetFromParent.z;
		Vector flatOffset = m_vOffsetFromParent; {
			flatOffset.z = 0;
		}

		m_flMuzzleForward = flatOffset.Length();
	}
	//clamp bullet count
	Clamp(m_iNumBullets, 1, 32);

	//Clamp accuracy
	Clamp(m_flAccuracyConeRadius, 0.0f, 0.5f);

	//Clamp damage - no healing!
	Clamp(m_iBaseDamage, 0, 1024);

	
}