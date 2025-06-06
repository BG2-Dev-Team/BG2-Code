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
#include "bg3_weapon_def.h"
#include "../../shared/bg2/weapon_bg2base.h"
#include "../../shared/bg3/math/bg3_rand.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//Maps weapon names to their weapon defs - for external usage
CUtlDict<CWeaponDef*> g_dictWeaponDefs;

//-----------------------------------------------------------------------------
// Purpose: Puts any weapon def into the dictionary
//-----------------------------------------------------------------------------
CWeaponDef::CWeaponDef(const char* pszWeaponName) {
	//m_bQuickdraw = false;
	//m_bSlowDraw = false;
	m_flIronsightsTime = 0.3f;
	m_flRandomAdditionalLockTimeMax = 0.0f;

	m_bDontAutoreload = true;
	m_bCantAbortReload = true;
	m_flReloadMovementSpeedModifier = 1.0f;
	m_flDamageDropoffMultiplier = 1.0f;
	m_bPenetrateFlesh = false;
	m_iOwnerSpeedModOnKill = m_iAerialDamageMod = 0;
	m_flVerticalAccuracyScale = 0.5;
	m_bDemoteNonHeadhitsToSecondaryDamage = false;

	m_eWeaponType = GENERIC;
	m_flLockTime = 0.135f;

	m_flIronsightFOVOffset = 0.0f;
	m_bWeaponHasSights = false;
	m_bBreakable = false;

	m_Attackinfos[0].m_flRetraceDelay = m_Attackinfos[1].m_flRetraceDelay = 0;

	m_bFiresUnderwater = false;
	m_bAltFiresUnderwater = false;

	g_dictWeaponDefs.Insert(pszWeaponName, this);
	m_pszWeaponDefName = pszWeaponName;

	m_bShotOnly = false;
	m_bSlowNerf = false;

	m_iExtraDamageTypes = 0;

#ifdef CLIENT_DLL
	//generate unique color based on weapon name
	int nameSum = 0;
	int len = strlen(pszWeaponName);
	for (int i = 6; i < len; i++) {
		nameSum += ~pszWeaponName[i];
	}
	RndSeed(nameSum);
	int mainColor = RndInt(0, 2);
	int rmin = mainColor == 0 ? 200 : 0;
	int gmin = mainColor == 1 ? 200 : 0;
	int bmin = mainColor == 2 ? 200 : 0;
	m_graphColor = Color(RndInt(rmin, 255), RndInt(gmin, 255), RndInt(bmin, 255), 255);
#endif
}

//Default weapon def used by non-BG3 weapons
DEC_BG3_WEAPON_DEF(weapon_default); //Use this to declare a weapon definiton
DEF_BG3_WEAPON_DEF(weapon_default) {} //Use this to define a weapon definitions constructor

//-----------------------------------------------------------------------------
// Purpose: Gets default weapon def for weapons
//-----------------------------------------------------------------------------
const CWeaponDef* CWeaponDef::GetDefault() {
	return &g_Def_weapon_default;
}

//-----------------------------------------------------------------------------
// Purpose: Given a weapon name, returns the weapon def used by it.
//		For external usage; instantiated weapons should get their weapon def in
//		their own constructors.
//-----------------------------------------------------------------------------
const CWeaponDef* CWeaponDef::GetDefForWeapon(const char* pszWeaponName) {
	int index = g_dictWeaponDefs.Find(pszWeaponName);
	return g_dictWeaponDefs[index];
}

#ifdef CLIENT_DLL
CON_COMMAND(weapon_list, "Lists all standard BG3 weapons, excluding grenades") {
	for (int i = 0; i < g_dictWeaponDefs.MaxElement(); i++) {
		if (g_dictWeaponDefs.IsValidIndex(i)) {
			Msg("%s\n", g_dictWeaponDefs[i]->m_pszWeaponDefName);
		}
	}
}
#endif