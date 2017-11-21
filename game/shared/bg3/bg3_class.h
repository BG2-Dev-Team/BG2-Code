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

#ifndef BG3_PLAYER_CLASS
#define BG3_PLAYER_CLASS

#include "../../shared/bg3/bg3_player_shared.h"

/*
PLAYER-MODELS LIST
This list is used to precache and validate the player models.
A forward declaration here exposes it to rest of program
*/
extern const char* g_ppszBritishPlayerModels[];
extern const char* g_ppszAmericanPlayerModels[];

class CGunKit {
public:
	char*	m_pszWeaponName = nullptr;

	bool	m_bAllowBuckshot = false;
	bool	m_bAlwaysGive = false;
};

/*
A "class" in BG3 is unique with its own model, skins, weapons, and stats. 
I'm tired of these stats and information being spread out into unwieldly functions, 
so I'm putting them all into these separate files
*/
class CPlayerClass {
public:
	const char* m_pszAbbreviation; //for example, "off" or "gre", used for player class limits
protected:
	CPlayerClass(const char* abrv) { m_pszAbbreviation = abrv; }
	CPlayerClass() { }

public:
	int			m_iDefaultTeam;
	int			m_iClassNumber = 0; //for backwards-compatibility

	float		m_flBaseSpeed = 190.f;
	float		m_flFlagWeightMultiplier = 1.0f;
		
	const char* m_pszPlayerModel;

	const char* m_pszPrimaryAmmo = AMMO_DEFAULT_NAME;
	int			m_iDefaultPrimaryAmmoCount;
	const char* m_pszSecondaryAmmo = nullptr;
	int			m_iDefaultSecondaryAmmoCount = 0;

#define NUM_POSSIBLE_WEAPON_KITS 4
	CGunKit		m_aWeapons[NUM_POSSIBLE_WEAPON_KITS];

	int			m_iSkinDepth = 1; //how many skin variations per uniform
	int			m_iNumUniforms = 1; //how many uniforms?
	int			m_iSleeveBase = 0; //chosen sleeve skin is m_iSleeveBase + pOwner->m_iClassSkin - 1
	const char* m_pszDroppedHat = 0;
	//bool		m_bAllowUniformSelection = false; //allow uniform selection in the menu?

	bool		m_bCanDoVcommBuffs = false; //this will be true for officer

//Functions
	inline bool isAmerican() const { return m_iDefaultTeam == TEAM_AMERICANS; }
	inline bool isBritish() const { return m_iDefaultTeam == TEAM_BRITISH; }

	int			numChooseableWeapons() const;
#ifdef CLIENT_DLL
	bool shouldHaveWeaponSelectionMenu() const;
#endif
	static const CPlayerClass* fromNums(int iTeam, int iClass); //for backwards-compatability with old numbering system
private:
	mutable ConVar* m_pcvLimit_lrg = nullptr;
	mutable ConVar* m_pcvLimit_med = nullptr;
	mutable ConVar* m_pcvLimit_sml = nullptr;
public:
	static int getClassLimit(const CPlayerClass* pClass); //maximum number of players on the team who can use this class
	static int numClasses(); //teams count individually, so American Infantry and British Infantry are separate classes
	static int numModelsForTeam(int iTeam);
	inline static int numClassesForTeam(int iTeam) { return numModelsForTeam(iTeam); }
	static const CPlayerClass** asList(); //retrieves a list of pointers to the classes

};

/*
EXPOSE INDIVIDUAL CLASSES TO REST OF PROGRAM
*/
namespace PlayerClasses {
#define dec(name) extern const CPlayerClass* g_p##name;
	dec(BInfantry)
	dec(BOfficer)
	dec(BJaeger)
	dec(BNative)
	dec(BLinf)
	dec(BGrenadier)

	dec(AInfantry)
	dec(AOfficer)
	dec(AFrontiersman)
	dec(AMilitia)
	dec(AStateMilitia)
	dec(AFrenchGre)

#undef dec

}

#endif //BG3_PLAYER_CLASS