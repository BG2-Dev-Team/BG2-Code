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
#include "../shared/bg3/bg3_class.h"
#include "../shared/bg3/Math/bg3_rand.h"

namespace NGunGame {

	extern bool g_bGunGameActive;
	DECLARE_GLOBAL_BOOL(g_bGunGameActive, mp_gungame);
	DECLARE_GLOBAL_BOOL(g_bGunGamePersistent, mp_gungame_persistent);

	struct SGunGameKit {
		const CGunKit* m_pOrigKit; //corresponds with kit that player would normally select in the class menu
		uint8 m_iWeapon; //which weapon within the kit we're using, ex. Grenade, Brown Bess, or Hanger Sword.
		const char* m_pszWeaponName;
	};

	extern CUtlVector<SGunGameKit> g_allGunKits;

	void InitGunKits();

	const int GetNumGunKits();

	void ScrambleGunKits();

	void ResetGunKitsOnPlayers(); //sets them all back to the starting kit

	SGunGameKit* GetGunKitFromIndex(int iGunGameKit);

	const char* GetWeaponNameFromGunKit(const CGunKit* pKit, int iSlot);

	//Used so that weapons aren't added twice to the list, i.e. some weapons are shared by multiple classes
	bool GunAlreadyInKitList(const CGunKit* pKit, int iSlot);
}