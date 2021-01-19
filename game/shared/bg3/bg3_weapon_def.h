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

/*
Weapon stats will now be stored in this comprehensive struct for two reason:
1. To improve performance, so that two brownbesses won't contain two copies of the same stat.
	When a 20 player game restarts, there would otherwise by 2k of data to unload & reload
2. To expose weapon stats to the client even when an instance of the weapon does not exist.
*/

#ifndef BG3_WEAPON_DEF_H
#define BG3_WEAPON_DEF_H

#include "cbase.h"
#include "bg3_player_shared.h"

//This was ported from BG2
//-----------------------------------------------------------------------------
// Purpose: Outlines stats for a left or right mouse button attack.
//-----------------------------------------------------------------------------
struct attackinfo
{
	int			m_iAttacktype;
	Activity	m_iAttackActivity,
				m_iAttackActivityEmpty;

	float	m_flRange;
	int		m_iDamage;
	float	m_flRecoil,				//factor of 357 standard recoil - firearm only

			
			// - melee only
			m_flCosAngleTolerance,	//tolerance of melee hits(sqrt(0.5) for crowbar, or 45 degrees)
			m_flRetraceDuration,
			m_flRetraceDelay,
			m_flAttackrate;
	int		m_iStaminaDrain;			//stamina drained by attack

	//cone values.. firearms only
	float	m_flStandMoving,
			m_flStandStill,
			m_flCrouchMoving,
			m_flCrouchStill,
			// For iron sights. -HairyPotter
			m_flStandAimStill,
			m_flStandAimMoving,
			m_flCrouchAimStill,
			m_flCrouchAimMoving,
			//
			m_flConstantDamageRange,	//how long until we start losing damage?
			m_flRelativeDrag;			//how does the drag on this bullet compare to a musket's?

	inline bool HasMelee() const;
};

bool attackinfo::HasMelee() const {
	return m_iAttacktype == ATTACKTYPE_SLASH || m_iAttacktype == ATTACKTYPE_STAB;
}

//-----------------------------------------------------------------------------
// Purpose: Not used very much, see comments below for more
//-----------------------------------------------------------------------------
enum WeaponType {
	PISTOL,
	CARBINE, //used for slower officer speed checking, as client can't access m_iGunKit
	MUSKET,
	RIFLE, //BG3 - Awesome - This is only used to determine flintlock delay and dynamic FOV adjust
	GRENADE, //
	GENERIC,
};

//-----------------------------------------------------------------------------
// Purpose: Stores all weapon stats into a single definition, rather than including
//		copies in every instance of a weapon.
//-----------------------------------------------------------------------------
class CWeaponDef {
public:
	//Two attackinfos for the two modes of attack
	attackinfo	m_Attackinfos[2];
	const char	*m_pBayonetDeathNotice;

	//values related to internal ballistics
	float	m_flShotAimModifier;//applies to aim cone. afterwards m_flShotSpread applies. should be negative
	float	m_flShotSpread;		//used when firing shot, if the current gun is capable of that
	float	m_flMuzzleVelocity;
	float	m_flShotMuzzleVelocity;	//muzzle velocity when firing buckshot
	float	m_flDamageDropoffMultiplier;
	float	m_flVerticalAccuracyScale;
	float	m_flZeroRange;		//range to zero the gun in at
	uint8	m_iNumShot;
	float	m_iDamagePerShot;
	bool	m_bShotOnly;

	//uint8	m_iOwnerSpeedModOnKill; //BG3 - was for Native war club, removed for now
	//uint8	m_iAerialDamageMod; //damage modifier for hits in the air

	bool	m_bDontAutoreload;
	bool	m_bCantAbortReload;
	float	m_flReloadMovementSpeedModifier;

	WeaponType m_eWeaponType;

	float	m_flIronsightFOVOffset;
	bool	m_bWeaponHasSights;
	//bool	m_bQuickdraw;
	//bool	m_bSlowDraw;
	float	m_flIronsightsTime;

	bool	m_bFiresUnderwater,
			m_bAltFiresUnderwater;

	float	m_fHolsterTime;
	float	m_flLockTime;
	float	m_flRandomAdditionalLockTimeMax;
	float	m_flApproximateReloadTime; //approximate because the actual time used is based on the animation. These values need to be kept up-to-date with animation times

	const char* m_pszWeaponDefName;
	CWeaponDef(const char* pszWeaponName);

	static const CWeaponDef* GetDefForWeapon(const char* pszWeaponName);
	static const CWeaponDef* GetDefault(); //for non-standard weapons, to avoid crashing
	inline bool HasMelee() const { return m_Attackinfos[0].HasMelee() || m_Attackinfos[1].HasMelee(); }
};

//Don't use this macro by itself - combine it with DEC_BG3_WEAPON_ENT, unless you know what you're doing
//This forward declares a weapon definition of the format g_Def_##weaponName
#define DEC_BG3_WEAPON_DEF(weaponName) \
	class CWeaponDef_##weaponName : public CWeaponDef { public: CWeaponDef_##weaponName(); }; \
	const CWeaponDef_##weaponName g_Def_##weaponName;

//Use this to define constructor for weapon def
#define DEF_BG3_WEAPON_DEF(weaponName) \
	CWeaponDef_##weaponName::CWeaponDef_##weaponName() : CWeaponDef(#weaponName)

#endif //BG3_WEAPON_DEF_H

