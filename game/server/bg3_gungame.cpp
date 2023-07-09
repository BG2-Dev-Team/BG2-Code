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

#include "cbase.h";
#include "../shared/bg3/bg3_class.h"
#include "../shared/bg3/Math/bg3_rand.h"
#include "bg3_gungame.h"
#include "hl2mp/hl2mp_player.h"

namespace NGunGame {
	GLOBAL_BOOL(g_bGunGameActive, mp_gungame, 0, false, FCVAR_GAMEDLL | FCVAR_NOTIFY, 0, 1);
	GLOBAL_BOOL(g_bGunGamePersistent, mp_gungame_persistent, 0, false, FCVAR_GAMEDLL | FCVAR_NOTIFY | FCVAR_ARCHIVE, 0, 1);

	CUtlVector<SGunGameKit> g_allGunKits;

	bool g_bHasInitGunKits = false;

	int g_iNumGunKits = 0;

	//no class has the bottle in any of their kits, so to get a bottle-only kit in gungame
	//we'll make a fake one here
	static CGunKit g_BottleKit;

	void InitGunKits() {
		//init the bottle kit stats
		CGunKit& bk = g_BottleKit;
		bk.m_pszWeaponPrimaryName = "weapon_bottle";

		auto ppClasses = CPlayerClass::asList();
		for (int cIndex = 0; cIndex < TOTAL_NUM_CLASSES; cIndex++) {
			auto pClass = ppClasses[cIndex];
			for (int gIndex = 0; gIndex < pClass->numChooseableWeapons(); gIndex++) {
				auto kit = &pClass->m_aWeapons[gIndex];

				if (!kit->m_pszWeaponPrimaryName) {
					continue;
				}

				//first weapon
				if (!GunAlreadyInKitList(kit, 0)) {
					g_allGunKits.AddToTail(SGunGameKit{ kit, 0, kit->m_pszWeaponPrimaryName });
				}

				//second weapon
				if (kit->m_pszWeaponSecondaryName && !GunAlreadyInKitList(kit, 1)) {
					g_allGunKits.AddToTail(SGunGameKit{ kit, 1, kit->m_pszWeaponSecondaryName });
				}

				//third weapon
				if (kit->m_pszWeaponTertiaryName && !GunAlreadyInKitList(kit, 2)) {
					g_allGunKits.AddToTail(SGunGameKit{ kit, 2, kit->m_pszWeaponTertiaryName });
				}
			}
		}
		g_allGunKits.AddToTail(SGunGameKit{ &g_BottleKit, 0, g_BottleKit.m_pszWeaponPrimaryName });
		g_iNumGunKits = g_allGunKits.Count();
	}

	const int GetNumGunKits() {
		return g_iNumGunKits;
	}

	void ScrambleGunKits() {
		if (!g_bHasInitGunKits) {
			InitGunKits();
			g_bHasInitGunKits = true;
		}

		auto swap = [](int x, int y) {
			SGunGameKit tmp = g_allGunKits[x];
			g_allGunKits[x] = g_allGunKits[y];
			g_allGunKits[y] = tmp;
		};
		for (int i = 0; i < g_allGunKits.Count() * 2; i++) {
			int r1 = RndInt(0, g_allGunKits.Count() - 1);
			int r2 = RndInt(0, g_allGunKits.Count() - 1);
			swap(r1, r2);
		}
	}

	void ResetGunKitsOnPlayers() {
		for (int i = 0; i < gpGlobals->maxClients; i++) {
			CBasePlayer* pPlayer = UTIL_PlayerByIndex(i);
			if (pPlayer) {
				CHL2MP_Player* pBG3Player = ToHL2MPPlayer(pPlayer);
				pBG3Player->m_iGunGameKit = 0;
				pBG3Player->SetFragCount(1);
			}
		}
	}

	SGunGameKit* GetGunKitFromIndex(int iGunGameKit) {
		return &g_allGunKits[iGunGameKit];
	}

	const char* GetWeaponNameFromGunKit(const CGunKit* pKit, int iSlot) {
		if (iSlot == 0) {
			return pKit->m_pszWeaponPrimaryName;
		}
		if (iSlot == 1 && pKit->m_pszWeaponSecondaryName) {
			return pKit->m_pszWeaponSecondaryName;
		}
		if (iSlot == 2 && pKit->m_pszWeaponTertiaryName) {
			return pKit->m_pszWeaponTertiaryName;
		}

		return NULL;
	}

	bool GunAlreadyInKitList(const CGunKit* pKit, int iSlot) {
		const char* psz = GetWeaponNameFromGunKit(pKit, iSlot);
		for (int i = 0; i < g_allGunKits.Count(); i++) {
			if (g_allGunKits[i].m_pszWeaponName == psz) {
				return true;
			}
		}
		return false;
	}
}

CON_COMMAND_F(gungame, "Command for turning on gun game mode", FCVAR_GAMEDLL) {
	if (!verifyMapModePermissions(__FUNCTION__))
		return;

	if (NGunGame::g_bGunGameActive) {
		NGunGame::mp_gungame.SetValue(0);
	}
	else {
		NGunGame::ScrambleGunKits();
		NGunGame::mp_gungame.SetValue(1);
		//NGunGame::ResetGunKitsOnPlayers();
		extern ConVar mp_respawnstyle, sv_restartmap, mp_flagmode;
		mp_respawnstyle.SetValue(1);
		sv_restartmap.SetValue(1);
		mp_flagmode.SetValue(-2); //hide flags this way
	}
}

CON_COMMAND_F(gungame_kits, "Reports list of gungame kits", FCVAR_GAMEDLL) {
	for (int i = 0; i < NGunGame::g_allGunKits.Count(); i++) {
		const NGunGame::SGunGameKit& ggk = NGunGame::g_allGunKits[i];
		const CGunKit* k = (ggk.m_pOrigKit);
		if (k) {
			if (ggk.m_iWeapon == 0) {
				Msg("%i. %i %s\n", i, 0, k->m_pszWeaponPrimaryName);
			}
			if (ggk.m_iWeapon == 1) {
				Msg("%i. %i %s\n", i, 1, k->m_pszWeaponSecondaryName);
			}
			if (ggk.m_iWeapon == 2) {
				Msg("%i. %i %s\n", i, 2, k->m_pszWeaponTertiaryName);
			}
		}
		else {
			Msg("Missing gun kit!\n");
		}
		
	}
}