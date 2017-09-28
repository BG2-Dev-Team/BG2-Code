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
#include "bg3_weapon_def.h"
#include "../../server/bg2/weapon_bg2base.h"

//Maps weapon names to their weapon defs - for external usage
CUtlDict<CWeaponDef*> g_dictWeaponDefs;

//-----------------------------------------------------------------------------
// Purpose: Puts any weapon def into the dictionary
//-----------------------------------------------------------------------------
CWeaponDef::CWeaponDef(const char* pszWeaponName) {
	m_bDontAutoreload = true;
	m_bCantAbortReload = true;

	m_eWeaponType = GENERIC;

	m_flIronsightFOVOffset = 0.0f;
	m_bWeaponHasSights = false;

	m_bFiresUnderwater = false;
	m_bAltFiresUnderwater = false;

	g_dictWeaponDefs.Insert(pszWeaponName, this);
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
