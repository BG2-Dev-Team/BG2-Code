/*
	The Battle Grounds 2 - A Source modification
	Copyright (C) 2005, The Battle Grounds 2 Team and Contributors

	The Battle Grounds 2 free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The Battle Grounds 2 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

	Contact information:
		Tomas "Tjoppen" Härdin		mail, in reverse: se . gamedev @ tjoppen

	You may also contact the (future) team via the Battle Grounds website and/or forum at:
		www.bgmod.com

	 Note that because of the sheer volume of files in the Source SDK this
	notice cannot be put in all of them, but merely the ones that have any
	changes from the original SDK.
	 In order to facilitate easy searching, all changes are and must be
	commented on the following form:

	//BG2 - <name of contributer>[ - <small description>]
*/

#include "cbase.h"
#include "weapon_bg2base.h"

const float	LONG_RANGE = 1000 * 3 * 12,		//since damage decreases with range, we can pick a very long range for all weapons, say 1000 yards
			PISTOL_RANGE = LONG_RANGE,		//40 * 3 * 12,		//40 yards
			MUSKET_RANGE = LONG_RANGE,		//175 * 3 * 12,	//175 yards
			RIFLE_RANGE = LONG_RANGE;		//195 * 3 * 12;		//195 yards

#define DAMAGE_ADJUSTED_RANGE() MUSKET_CONSTANT_DAMAGE_RANGE + 2000 * ((m_Attackinfos[0].m_iDamage - 107.f) / 107)

const float PISTOL_CONSTANT_DAMAGE_RANGE = 8 * 36,	//8 yards
			MUSKET_CONSTANT_DAMAGE_RANGE = 30 * 36,	//30 yards
			RIFLE_CONSTANT_DAMAGE_RANGE  = 40 * 36;	//40 yards

const int	PISTOL_STAMINA_DRAIN = 0,
			MUSKET_RIFLE_STAMINA_DRAIN = 0,
			MELEE_STAMINA_DRAIN = 25,
			MELEE_STAMINA_DRAIN_HEAVY = 33;

const float LOCK_TIME_RIFLE = 0.0675f;

//BG2 - Tjoppen - these constants are based on values from weapon_data.h @ BG 1.0F RC14
//they have since been adjusted and normalized to work with a chest damage modifier of 1 instead of 1.85
const int BESS_FIRE_DAMAGE = 111;			//60 * 1.85
const int BESS_BAYONET_DAMAGE = 63;			//43 * 0.8 * 1.85
const int BESS_BAYONET_RANGE = 83;

const int AMER_BESS_FIRE_DAMAGE = 116;			//60 * 1.85
const int AMER_BESS_BAYONET_DAMAGE = 66;			//43 * 0.8 * 1.85
const int AMER_BESS_BAYONET_RANGE = 84;

const int REVOL_FIRE_DAMAGE = 104;			//58 * 1.85 // BG2 - VisualMelon - Formerly 107 (now about 3% less)
const int REVOL_BAYONET_DAMAGE = 61;		//41 * 0.8 * 1.85 + 1
const int REVOL_BAYONET_RANGE = 75;

const int CHARLE_FIRE_DAMAGE = 107; //Formally equivelent to REVOL_FIRE_DAMAGE - Awesome
const int CHARLE_BAYONET_DAMAGE = REVOL_BAYONET_DAMAGE;
const int CHARLE_BAYONET_RANGE = 80;

const int MIQUELET_FIRE_DAMAGE = 104; //Formally equivelent to REVOL_FIRE_DAMAGE - Awesome
const int MIQUELET_BAYONET_DAMAGE = 60;
const int MIQUELET_BAYONET_RANGE = 81;

const int DUTCH_FIRE_DAMAGE = 109;
const int DUTCH_BAYONET_DAMAGE = 62;
const int DUTCH_BAYONET_RANGE = 80;

const int OLDMODEL_CHARLE_FIRE_DAMAGE = 112;
const int OLDMODEL_CHARLE_BAYONET_DAMAGE = 63;
const int OLDMODEL_CHARLE_BAYONET_RANGE = 83;

const int LMODEL_CHARLE_FIRE_DAMAGE = 100; 
const int LMODEL_CHARLE_BAYONET_DAMAGE = 57;
const int LMODEL_CHARLE_BAYONET_RANGE = 81;

const int SEASERVICE_FIRE_DAMAGE = 108;			//60 * 1.85
const int SEASERVICE_BAYONET_DAMAGE = 63;			//43 * 0.8 * 1.85
const int SEASERVICE_BAYONET_RANGE = 78;

const int PENNY_FIRE_DAMAGE = BESS_FIRE_DAMAGE;

const int LONGPATTERN_FIRE_DAMAGE = 114;	//62 * 1.85
const int LONGPATTERN_BAYONET_DAMAGE = 66;	//45 * 0.8 * 1.85
const int LONGPATTERN_BAYONET_RANGE = 86;

const int OLDPATTERN_FIRE_DAMAGE = 124;	//62 * 1.85
const int OLDPATTERN_BAYONET_DAMAGE = 74;	//45 * 0.8 * 1.85
const int OLDPATTERN_BAYONET_RANGE = 87;

const int SPONTOON_DAMAGE = 55;
const int SPONTOON_RANGE = 83;

const int FOWLER_FIRE_DAMAGE = 103;			//damage per ball. 56 * 1.85
const int FOWLER_SHOT_DAMAGE = 27;			//damage per shot. 10 * 1.85 -> 18 -> 8*18 = 144 - VisualMelon - changed to 20 (formerly 18)
const int FOWLER_NUM_SHOT = 7;				// BG2 - VisualMelon - formerly 10

const int CARBINE_FIRE_DAMAGE = 107;		//damage per ball. between fowler and normal brown bess. 58 * 1.85
const int CARBINE_SHOT_DAMAGE = 30;			//damage per shot. 11 * 1.85 -> 20 -> 8*20 = 160 - VisualMelon - changed to 22 (formlery 20)
const int CARBINE_NUM_SHOT = 7;				// BG2 - VisualMelon - formerly 10
const int CARBINE_BAYONET_DAMAGE = 60;		//BG3 - formerly equivalant to bess damage, 63, now 58
const int CARBINE_BAYONET_RANGE = 72;

const int BLUNDERBUSS_SHOT_DAMAGE = 46;

const int PATTERN_FIRE_DAMAGE = 103;

const int JAEGER_FIRE_DAMAGE = 116;			//63 * 1.85

const int PISTOL_FIRE_DAMAGE = 101;			//55 * 1.85

const int KNIFE_DAMAGE = 55;
const int KNIFE_DAMAGE_ALT = 45;
const int SABRE_DAMAGE = 74;				//40 * 1.85
const int TOMAHAWK_DAMAGE = 79;				//43 * 1.85
const int CLUB_DAMAGE		= 82;
const int HIRSCHFAENGER_DAMAGE = 55;		//used to be knife's damage, but then knife was changed from 55 to 45
const int HANGER_DAMAGE = 74;			//this ought to be a good number

//BG2 - Tjoppen - own constants, interpreted from various places in the BG source
const float SABRE_RANGE = 57.0;
const float HIRSCHFAENGER_RANGE = 57.0;
const float SMALLSWORD_RANGE = 57.0f;
const float HANGER_RANGE = 61.0;
const float KNIFE_RANGE = 52.0;
const float TOMAHAWK_RANGE = 55.0;
const float CLUB_RANGE = 57.0f;

const float BAYONET_COS_TOLERANCE = 0.9961946980917;		//+-5 degrees
const float BAYONET_RETRACE_DURATION = 0.1;

ConVar sv_bayonet_angle_tolerance("sv_bayonet_angle_tolerance", "5", FCVAR_REPLICATED | FCVAR_NOTIFY | FCVAR_CHEAT, "How many angles the sights are allowed to move before retracing stops");
ConVar sv_bayonet_retrace_duration("sv_bayonet_retrace_duration", "0.1", FCVAR_REPLICATED | FCVAR_NOTIFY | FCVAR_CHEAT, "How long bayonet melee traces happen for");

const float KNIFE_COS_TOLERANCE				= 0.866f;		//+-30 degrees
const float KNIFE_RETRACE_DURATION			= 0.15f;

const float HANGER_COS_TOLERANCE		= 0.966;		//+-15 degrees
const float HANGER_RETRACE_DURATION		= 0.18f;

const float TOMAHAWK_COS_TOLERANCE			= 0.906f;		//+-25 degrees
const float TOMAHAWK_RETRACE_DURATION		= 0.2f;

const float HIRSCHFAENGER_COS_TOLERANCE		= 0.906f;		//+-25 degrees
const float HIRSCHFAENGER_RETRACE_DURATION	= 0.2f;

const float SABRE_COS_TOLERANCE				= 0.866f;		//+-30 degrees
const float SABRE_RETRACE_DURATION			= 0.25f;

//const float SPONTOON_COS_TOLERANCE			= 0.99813f;		//+- 3.5 degrees
//const float SPONTOON_RETRACE_DURATION		= 0.08f;

const float MUZZLE_VELOCITY_RIFLE = 20400;
const float MUZZLE_VELOCITY_SMOOTHBORE = 15600;
const float MUZZLE_VELOCITY_BUCKSHOT = 12000;	//TODO: implement different muzzle velocity for buckshot
const float MUZZLE_VELOCITY_PISTOL = 10800;

const float ZERO_RANGE_PISTOL = 10 * 36;	//ten yards
const float ZERO_RANGE_MUSKET = 50 * 36;	//fifty yards
const float ZERO_RANGE_RIFLE = 130 * 36;	//two hundred yards

#ifdef CLIENT_DLL
#define CWeaponbrownbess C_Weaponbrownbess
#endif
DECLARE_BG2_WEAPON( brownbess )
{
	m_bCantAbortReload	= true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.6f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset		= -2.5;

	m_bWeaponHasSights = true; 
	//

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage				= BESS_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate			= 1.0;
	m_Attackinfos[0].m_flRecoil				= 0.7;
	m_Attackinfos[0].m_flRange				= MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving		= 12.0f;
	m_Attackinfos[0].m_flCrouchStill		= 3.6f; //2.4
	m_Attackinfos[0].m_flStandMoving		= 13.2f; //12.0f
	m_Attackinfos[0].m_flStandStill			= 3.6f; //2.4
	//Iron Sights. These values will probably be changed.
	m_Attackinfos[0].m_flStandAimStill		= 2.4f;	
	m_Attackinfos[0].m_flStandAimMoving		= 8.8f;
	m_Attackinfos[0].m_flCrouchAimStill		= 2.4f;
	m_Attackinfos[0].m_flCrouchAimMoving	= 8.0f;
	//
	m_Attackinfos[0].m_flConstantDamageRange= DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag		= 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain		= MUSKET_RIFLE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype			= ATTACKTYPE_STAB;
	m_Attackinfos[1].m_iDamage				= BESS_BAYONET_DAMAGE;//60;
	m_Attackinfos[1].m_flAttackrate			= 1.0f;//-0.7f;
	m_Attackinfos[1].m_flRange				= BESS_BAYONET_RANGE;
	//m_Attackinfos[1].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity		= ACT_VM_SECONDARYATTACK;
	m_Attackinfos[1].m_iAttackActivityEmpty	= ACT_VM_SECONDARYATTACK_EMPTY;
	m_Attackinfos[1].m_flCosAngleTolerance  = BAYONET_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration    = BAYONET_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain		= MELEE_STAMINA_DRAIN;

	m_pBayonetDeathNotice = "brownbess_bayonet";

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = 0;
}

#ifdef CLIENT_DLL
#define CWeaponsea_service C_Weaponsea_service
#endif
DECLARE_BG2_WEAPON(sea_service)
{
	m_bCantAbortReload = true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.2f;
	m_flLockTime = 0.09f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset = -2.5;

	m_bWeaponHasSights = true;
	m_bQuickdraw = true;
	//

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage = SEASERVICE_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate = 1.0;
	m_Attackinfos[0].m_flRecoil = 0.6;
	m_Attackinfos[0].m_flRange = MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving = 12.0f;
	m_Attackinfos[0].m_flCrouchStill = 3.7f; //2.4
	m_Attackinfos[0].m_flStandMoving = 13.2f; //12.0f
	m_Attackinfos[0].m_flStandStill = 3.7f; //2.4
	//Iron Sights. These values will probably be changed.
	m_Attackinfos[0].m_flStandAimStill = 2.4f;
	m_Attackinfos[0].m_flStandAimMoving = 8.8f;
	m_Attackinfos[0].m_flCrouchAimStill = 2.4f;
	m_Attackinfos[0].m_flCrouchAimMoving = 8.0f;
	//
	m_Attackinfos[0].m_flConstantDamageRange = DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag = 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain = MUSKET_RIFLE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype = ATTACKTYPE_STAB;
	m_Attackinfos[1].m_iDamage = SEASERVICE_BAYONET_DAMAGE;//60;
	m_Attackinfos[1].m_flAttackrate = 0.95f;//-0.7f;
	m_Attackinfos[1].m_flRange = SEASERVICE_BAYONET_RANGE;
	//m_Attackinfos[1].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity = ACT_VM_SECONDARYATTACK;
	m_Attackinfos[1].m_iAttackActivityEmpty = ACT_VM_SECONDARYATTACK_EMPTY;
	m_Attackinfos[1].m_flCosAngleTolerance = BAYONET_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration = BAYONET_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain = MELEE_STAMINA_DRAIN / 2;

	m_pBayonetDeathNotice = "brownbess_bayonet";

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
MUSKET_ACTTABLE(sea_service)
#endif

#ifndef CLIENT_DLL
MUSKET_ACTTABLE( brownbess )
#endif

#ifdef CLIENT_DLL
#define CWeaponcharleville C_Weaponcharleville
#endif
DECLARE_BG2_WEAPON( charleville )
{
	m_bCantAbortReload	= true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.6f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset		= -2.5;

	m_bWeaponHasSights = true; 
	//

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage				= CHARLE_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate			= 1.0;
	m_Attackinfos[0].m_flRecoil				= 0.6;
	m_Attackinfos[0].m_flRange				= MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving		= 11.5f;
	m_Attackinfos[0].m_flCrouchStill		= 3.4f;
	m_Attackinfos[0].m_flStandMoving		= 12.7f; //11.5f;
	m_Attackinfos[0].m_flStandStill			= 3.4f;
	//Iron Sights. These values will probably be changed.
	m_Attackinfos[0].m_flStandAimStill		= 2.2f;	
	m_Attackinfos[0].m_flStandAimMoving		= 8.5f;
	m_Attackinfos[0].m_flCrouchAimStill		= 2.2f;
	m_Attackinfos[0].m_flCrouchAimMoving	= 7.7f;
	//
	m_Attackinfos[0].m_flConstantDamageRange= DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag		= 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain		= MUSKET_RIFLE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype			= ATTACKTYPE_STAB;
	m_Attackinfos[1].m_iDamage				= CHARLE_BAYONET_DAMAGE;//60;
	m_Attackinfos[1].m_flAttackrate			= 1.0f;//-0.7f;
	m_Attackinfos[1].m_flRange				= CHARLE_BAYONET_RANGE;
	//m_Attackinfos[1].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity		= ACT_VM_SECONDARYATTACK;
	m_Attackinfos[1].m_iAttackActivityEmpty	= ACT_VM_SECONDARYATTACK_EMPTY;
	m_Attackinfos[1].m_flCosAngleTolerance  = BAYONET_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration    = BAYONET_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain		= MELEE_STAMINA_DRAIN;

	m_pBayonetDeathNotice = "charleville_bayonet";

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
MUSKET_ACTTABLE( charleville )
#endif

#ifdef CLIENT_DLL
#define CWeaponlight_model_charleville C_Weaponlight_model_charleville
#endif
DECLARE_BG2_WEAPON(light_model_charleville)
{
	m_bCantAbortReload = true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.6f;
	m_bQuickdraw = true;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset = -2.5;

	m_bWeaponHasSights = true;
	//

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage = LMODEL_CHARLE_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate = 1.0;
	m_Attackinfos[0].m_flRecoil = 0.3;
	m_Attackinfos[0].m_flRange = MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving = 7.5f;
	m_Attackinfos[0].m_flCrouchStill = 3.1f;
	m_Attackinfos[0].m_flStandMoving = 7.7f; //11.5f;
	m_Attackinfos[0].m_flStandStill = 3.1f;
	//Iron Sights. These values will probably be changed.
	m_Attackinfos[0].m_flStandAimStill = 2.1f;
	m_Attackinfos[0].m_flStandAimMoving = 8.5f;
	m_Attackinfos[0].m_flCrouchAimStill = 2.1f;
	m_Attackinfos[0].m_flCrouchAimMoving = 7.7f;
	//
	m_Attackinfos[0].m_flConstantDamageRange = DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag = 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain = MUSKET_RIFLE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype = ATTACKTYPE_STAB;
	m_Attackinfos[1].m_iDamage = LMODEL_CHARLE_BAYONET_DAMAGE;//60;
	m_Attackinfos[1].m_flAttackrate = 0.9f;//-0.7f;
	m_Attackinfos[1].m_flRange = LMODEL_CHARLE_BAYONET_RANGE;
	//m_Attackinfos[1].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity = ACT_VM_SECONDARYATTACK;
	m_Attackinfos[1].m_iAttackActivityEmpty = ACT_VM_SECONDARYATTACK_EMPTY;
	m_Attackinfos[1].m_flCosAngleTolerance = BAYONET_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration = BAYONET_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain = MELEE_STAMINA_DRAIN / 2;

	m_pBayonetDeathNotice = "light_model_charleville_bayonet";

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_flDamageDropoffMultiplier = 1.24f;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
MUSKET_ACTTABLE(light_model_charleville)
#endif


#ifdef CLIENT_DLL
#define CWeaponold_model_charleville C_Weaponold_model_charleville
#endif
DECLARE_BG2_WEAPON(old_model_charleville)
{
	m_bCantAbortReload = true;

	m_flRandomAdditionalLockTimeMax = 0.25f;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.6f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset = -2.5;

	m_bWeaponHasSights = true;
	//

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage = OLDMODEL_CHARLE_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate = 1.0;
	m_Attackinfos[0].m_flRecoil = 0.6;
	m_Attackinfos[0].m_flRange = MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving = 11.1f;
	m_Attackinfos[0].m_flCrouchStill = 3.35f;
	m_Attackinfos[0].m_flStandMoving = 12.3f; //11.5f;
	m_Attackinfos[0].m_flStandStill = 3.35f;
	//Iron Sights. These values will probably be changed.
	m_Attackinfos[0].m_flStandAimStill = 2.15f;
	m_Attackinfos[0].m_flStandAimMoving = 8.2f;
	m_Attackinfos[0].m_flCrouchAimStill = 2.15f;
	m_Attackinfos[0].m_flCrouchAimMoving = 7.0f;
	//
	m_Attackinfos[0].m_flConstantDamageRange = DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag = 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain = MUSKET_RIFLE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype = ATTACKTYPE_STAB;
	m_Attackinfos[1].m_iDamage = OLDMODEL_CHARLE_BAYONET_DAMAGE;//60;
	m_Attackinfos[1].m_flAttackrate = 1.05f;//-0.7f;
	m_Attackinfos[1].m_flRange = OLDMODEL_CHARLE_BAYONET_RANGE;
	//m_Attackinfos[1].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity = ACT_VM_SECONDARYATTACK;
	m_Attackinfos[1].m_iAttackActivityEmpty = ACT_VM_SECONDARYATTACK_EMPTY;
	m_Attackinfos[1].m_flCosAngleTolerance = BAYONET_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration = BAYONET_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain = MELEE_STAMINA_DRAIN;

	m_pBayonetDeathNotice = "old_model_charleville_bayonet";

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
MUSKET_ACTTABLE(old_model_charleville)
#endif

#ifdef CLIENT_DLL
#define CWeapondutch C_Weapondutch
#endif
DECLARE_BG2_WEAPON(dutch)
{
	m_bCantAbortReload = true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.6f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset = -6.5;

	m_bWeaponHasSights = true;

	m_flLockTime = 0.06;
	//

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage = DUTCH_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate = 1.0;
	m_Attackinfos[0].m_flRecoil = 0.7;
	m_Attackinfos[0].m_flRange = MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving = 12.0f;
	m_Attackinfos[0].m_flCrouchStill = 3.5f; //2.4
	m_Attackinfos[0].m_flStandMoving = 13.2f; //12.0f
	m_Attackinfos[0].m_flStandStill = 3.5f; //2.4
	//Iron Sights. These values will probably be changed.
	m_Attackinfos[0].m_flStandAimStill = 2.35f;
	m_Attackinfos[0].m_flStandAimMoving = 8.8f;
	m_Attackinfos[0].m_flCrouchAimStill = 2.35f;
	m_Attackinfos[0].m_flCrouchAimMoving = 8.0f;
	//
	m_Attackinfos[0].m_flConstantDamageRange = DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag = 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain = MUSKET_RIFLE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype = ATTACKTYPE_STAB;
	m_Attackinfos[1].m_iDamage = DUTCH_BAYONET_DAMAGE;//60;
	m_Attackinfos[1].m_flAttackrate = 1.0f;//-0.7f;
	m_Attackinfos[1].m_flRange = DUTCH_BAYONET_RANGE;
	//m_Attackinfos[1].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity = ACT_VM_SECONDARYATTACK;
	m_Attackinfos[1].m_iAttackActivityEmpty = ACT_VM_SECONDARYATTACK_EMPTY;
	m_Attackinfos[1].m_flCosAngleTolerance = BAYONET_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration = BAYONET_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain = MELEE_STAMINA_DRAIN / 2;

	m_pBayonetDeathNotice = "dutch_bayonet";

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
MUSKET_ACTTABLE(dutch)
#endif


#ifdef CLIENT_DLL
#define CWeaponmiquelet C_Weaponmiquelet
#endif
DECLARE_BG2_WEAPON(miquelet)
{
	m_bCantAbortReload = true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.6f;
	m_flLockTime = 0.03f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset = -2.5;

	m_bWeaponHasSights = true;
	//

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage = MIQUELET_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate = 1.0;
	m_Attackinfos[0].m_flRecoil = 0.6;
	m_Attackinfos[0].m_flRange = MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving = 11.5f;
	m_Attackinfos[0].m_flCrouchStill = 3.5f;
	m_Attackinfos[0].m_flStandMoving = 12.7f; //11.5f;
	m_Attackinfos[0].m_flStandStill = 3.5f;
	//Iron Sights. These values will probably be changed.
	m_Attackinfos[0].m_flStandAimStill = 2.3f;
	m_Attackinfos[0].m_flStandAimMoving = 8.5f;
	m_Attackinfos[0].m_flCrouchAimStill = 2.3f;
	m_Attackinfos[0].m_flCrouchAimMoving = 7.7f;
	//
	m_Attackinfos[0].m_flConstantDamageRange = DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag = 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain = MUSKET_RIFLE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype = ATTACKTYPE_STAB;
	m_Attackinfos[1].m_iDamage = MIQUELET_BAYONET_DAMAGE;//60;
	m_Attackinfos[1].m_flAttackrate = 1.0f;//-0.7f;
	m_Attackinfos[1].m_flRange = MIQUELET_BAYONET_RANGE;
	//m_Attackinfos[1].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity = ACT_VM_SECONDARYATTACK;
	m_Attackinfos[1].m_iAttackActivityEmpty = ACT_VM_SECONDARYATTACK_EMPTY;
	m_Attackinfos[1].m_flCosAngleTolerance = BAYONET_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration = BAYONET_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain = MELEE_STAMINA_DRAIN;

	m_pBayonetDeathNotice = "miquelet_bayonet";

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = 0;
}
#ifndef CLIENT_DLL
MUSKET_ACTTABLE(miquelet)
#endif

//jäger rifle, but spelled jaeger to avoid any charset problems
#ifdef CLIENT_DLL
#define CWeaponjaeger C_Weaponjaeger
#endif
DECLARE_BG2_WEAPON( jaeger )
{
	m_bCantAbortReload	= true;
	m_eWeaponType = RIFLE;
	m_flLockTime = LOCK_TIME_RIFLE;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 8.9f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset		= -15;

	m_bWeaponHasSights = true; 
	//

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage				= JAEGER_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate			= 1.0;
	m_Attackinfos[0].m_flRecoil				= 0.9;
	m_Attackinfos[0].m_flRange				= RIFLE_RANGE;
	m_Attackinfos[0].m_flCrouchMoving		= 10.0f;
	m_Attackinfos[0].m_flCrouchStill		= 2.4f; //0.83
	m_Attackinfos[0].m_flStandMoving		= 11.0f; //8.0f
	m_Attackinfos[0].m_flStandStill			= 3.0f; //2.0
	//Iron Sights. These values will probably be changed.
	m_Attackinfos[0].m_flStandAimStill		= 2.0f;	
	m_Attackinfos[0].m_flStandAimMoving		= 7.3f;
	m_Attackinfos[0].m_flCrouchAimStill		= 0.83f;
	m_Attackinfos[0].m_flCrouchAimMoving	= 6.7f;
	//
	m_Attackinfos[0].m_flConstantDamageRange= RIFLE_CONSTANT_DAMAGE_RANGE;
	m_Attackinfos[0].m_flRelativeDrag		= 0.75;			//rifle
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain		= MUSKET_RIFLE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype			= ATTACKTYPE_NONE;

	m_flMuzzleVelocity = MUZZLE_VELOCITY_RIFLE;
	m_flZeroRange = ZERO_RANGE_RIFLE;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
RIFLE_ACTTABLE( jaeger )
#endif


#ifdef CLIENT_DLL
#define CWeaponpennsylvania C_Weaponpennsylvania
#endif
DECLARE_BG2_WEAPON( pennsylvania )
{
	m_bCantAbortReload	= true;
	m_eWeaponType = RIFLE;
	m_flLockTime = LOCK_TIME_RIFLE;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 9.9f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset		= -20;

	m_bWeaponHasSights = true; 
	//

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage				= PENNY_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate			= 1.0;
	m_Attackinfos[0].m_flRecoil				= 1.0;
	m_Attackinfos[0].m_flRange				= RIFLE_RANGE;
	m_Attackinfos[0].m_flCrouchMoving		= 10.0f;
	m_Attackinfos[0].m_flCrouchStill		= 2.3f; //0.75
	m_Attackinfos[0].m_flStandMoving		= 12.0f; //8.0f
	m_Attackinfos[0].m_flStandStill			= 3.0f; //1.75
	//Iron Sights. These values will probably be changed.
	m_Attackinfos[0].m_flStandAimStill		= 1.65f;	
	m_Attackinfos[0].m_flStandAimMoving		= 8.0f;
	m_Attackinfos[0].m_flCrouchAimStill		= 0.65f;
	m_Attackinfos[0].m_flCrouchAimMoving	= 6.7f;
	//
	m_Attackinfos[0].m_flConstantDamageRange= RIFLE_CONSTANT_DAMAGE_RANGE;
	m_Attackinfos[0].m_flRelativeDrag		= 0.75;			//rifle
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain		= MUSKET_RIFLE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype			= ATTACKTYPE_NONE;

	m_flMuzzleVelocity = MUZZLE_VELOCITY_RIFLE;
	m_flZeroRange = ZERO_RANGE_RIFLE;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
RIFLE_ACTTABLE( pennsylvania )
#endif

#ifdef CLIENT_DLL
#define CWeaponpistol_a C_Weaponpistol_a
#endif
DECLARE_BG2_WEAPON( pistol_a )
{
	m_fHolsterTime = 0.50f;
	m_flApproximateReloadTime = 5.0f;
	m_bCantAbortReload = false;

	m_bWeaponHasSights = false; 
	m_eWeaponType = PISTOL;

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage				= PISTOL_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate			= 1.0;
	m_Attackinfos[0].m_flRecoil				= 0.7;
	m_Attackinfos[0].m_flRange				= PISTOL_RANGE;
	m_Attackinfos[0].m_flCrouchMoving		= 7.5f;
	m_Attackinfos[0].m_flCrouchStill		= 6.0f;
	m_Attackinfos[0].m_flStandMoving		= 9.0f;
	m_Attackinfos[0].m_flStandStill			= 6.0f; 
	m_Attackinfos[0].m_flConstantDamageRange= PISTOL_CONSTANT_DAMAGE_RANGE;
	m_Attackinfos[0].m_flRelativeDrag		= 1.25;			//pistol
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain		= PISTOL_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype			= ATTACKTYPE_NONE;

	m_flMuzzleVelocity = MUZZLE_VELOCITY_PISTOL;
	m_flZeroRange = ZERO_RANGE_PISTOL;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
PISTOL_ACTTABLE( pistol_a )
#endif

#ifdef CLIENT_DLL
#define CWeaponpistol_b C_Weaponpistol_b
#endif
DECLARE_BG2_WEAPON( pistol_b )
{
	m_fHolsterTime = 0.50f;
	m_flApproximateReloadTime = 5.0f;
	m_bCantAbortReload = false;

	m_bWeaponHasSights = false; 
	m_eWeaponType = PISTOL;

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage				= PISTOL_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate			= 1.0;
	m_Attackinfos[0].m_flRecoil				= 0.7;
	m_Attackinfos[0].m_flRange				= PISTOL_RANGE;
	m_Attackinfos[0].m_flCrouchMoving		= 7.5f;
	m_Attackinfos[0].m_flCrouchStill		= 6.0f;
	m_Attackinfos[0].m_flStandMoving		= 9.0f;
	m_Attackinfos[0].m_flStandStill			= 6.0f; 
	m_Attackinfos[0].m_flConstantDamageRange= PISTOL_CONSTANT_DAMAGE_RANGE;
	m_Attackinfos[0].m_flRelativeDrag		= 1.25;			//pistol
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain		= PISTOL_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype			= ATTACKTYPE_NONE;

	m_flMuzzleVelocity = MUZZLE_VELOCITY_PISTOL;
	m_flZeroRange = ZERO_RANGE_PISTOL;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
PISTOL_ACTTABLE( pistol_b )
#endif

#ifdef CLIENT_DLL
#define CWeaponsabre C_Weaponsabre
#endif
DECLARE_BG2_WEAPON( sabre )
{
	m_fHolsterTime = 0.50f;

	m_bWeaponHasSights = false; 

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_SLASH;
	m_Attackinfos[0].m_iDamage				= SABRE_DAMAGE;//60;
	m_Attackinfos[0].m_flAttackrate			= 1.4;//-0.7f;
	m_Attackinfos[0].m_flRange				= SABRE_RANGE;
	//m_Attackinfos[0].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_flCosAngleTolerance  = SABRE_COS_TOLERANCE;
	m_Attackinfos[0].m_flRetraceDuration    = SABRE_RETRACE_DURATION;
	m_Attackinfos[0].m_iStaminaDrain		= MELEE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1] = m_Attackinfos[0];
}

#ifndef CLIENT_DLL
MELEE_ACTTABLE( sabre )
#endif


#ifdef CLIENT_DLL
#define CWeaponknife C_Weaponknife
#endif
DECLARE_BG2_WEAPON( knife )
{
	m_fHolsterTime = 0.75f;

	m_bWeaponHasSights = false; 

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_SLASH;
	m_Attackinfos[0].m_iDamage = KNIFE_DAMAGE;//60;
	m_Attackinfos[0].m_flAttackrate = 1.1f;//1.1f;
	m_Attackinfos[0].m_flRange = KNIFE_RANGE;
	//m_Attackinfos[0].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_flCosAngleTolerance = KNIFE_COS_TOLERANCE;
	m_Attackinfos[0].m_flRetraceDuration = KNIFE_RETRACE_DURATION;
	m_Attackinfos[0].m_iStaminaDrain = MELEE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype			= ATTACKTYPE_SLASH;
	m_Attackinfos[1].m_iDamage				= KNIFE_DAMAGE_ALT;//60;
	m_Attackinfos[1].m_flAttackrate			= 0.7f;//1.1f;
	m_Attackinfos[1].m_flRange				= KNIFE_RANGE;
	//m_Attackinfos[0].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[1].m_flCosAngleTolerance  = KNIFE_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration    = KNIFE_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain		= MELEE_STAMINA_DRAIN / 1.4f;

	//secondary
}

#ifndef CLIENT_DLL
MELEE_ACTTABLE( knife )
#endif

#ifdef CLIENT_DLL
#define CWeapondagger C_Weapondagger
#endif
DECLARE_BG2_WEAPON(dagger)
{
	m_fHolsterTime = 0.75f;

	m_bWeaponHasSights = false;

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_SLASH;
	m_Attackinfos[0].m_iDamage = KNIFE_DAMAGE_ALT;//60;
	m_Attackinfos[0].m_flAttackrate = 0.7f;//1.1f;
	m_Attackinfos[0].m_flRange = KNIFE_RANGE;
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_flCosAngleTolerance = KNIFE_COS_TOLERANCE;
	m_Attackinfos[0].m_flRetraceDuration = KNIFE_RETRACE_DURATION;
	m_Attackinfos[0].m_iStaminaDrain = MELEE_STAMINA_DRAIN / 1.4f;

	//secondary
	m_Attackinfos[1] = m_Attackinfos[0];
		
}

#ifndef CLIENT_DLL
MELEE_ACTTABLE(dagger)
#endif

#ifdef CLIENT_DLL
#define CWeaponhirschf C_Weaponhirschf
#endif
DECLARE_BG2_WEAPON( hirschf )
{
	m_fHolsterTime = 0.75f;

	m_bWeaponHasSights = false; 

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_SLASH;
	m_Attackinfos[0].m_iDamage				= HIRSCHFAENGER_DAMAGE;//60;
	m_Attackinfos[0].m_flAttackrate			= 1.4;//-0.7f;
	m_Attackinfos[0].m_flRange				= HIRSCHFAENGER_RANGE;
	//m_Attackinfos[0].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_flCosAngleTolerance  = HIRSCHFAENGER_COS_TOLERANCE;
	m_Attackinfos[0].m_flRetraceDuration    = HIRSCHFAENGER_RETRACE_DURATION;
	m_Attackinfos[0].m_iStaminaDrain		= MELEE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1] = m_Attackinfos[0];
}
#ifndef CLIENT_DLL
MELEE_ACTTABLE( hirschf )
#endif

#ifdef CLIENT_DLL
#define CWeaponhanger C_Weaponhanger
#endif
DECLARE_BG2_WEAPON(hanger)
{
	m_fHolsterTime = 0.75f;

	m_bWeaponHasSights = false;

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_SLASH;
	m_Attackinfos[0].m_iDamage = HANGER_DAMAGE;//60;
	m_Attackinfos[0].m_flAttackrate = 1.2;//-0.7f;
	m_Attackinfos[0].m_flRange = HANGER_RANGE;
	//m_Attackinfos[0].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_flCosAngleTolerance = HANGER_COS_TOLERANCE;
	m_Attackinfos[0].m_flRetraceDuration = HANGER_RETRACE_DURATION;
	m_Attackinfos[0].m_iStaminaDrain = MELEE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype = ATTACKTYPE_SLASH;
	m_Attackinfos[1].m_iDamage = HANGER_DAMAGE / 1.5;//60;
	m_Attackinfos[1].m_flAttackrate = 1.2 / 1.5;//-0.7f;
	m_Attackinfos[1].m_flRange = HANGER_RANGE;
	//m_Attackinfos[0].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[1].m_flCosAngleTolerance = HANGER_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration = HANGER_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain = MELEE_STAMINA_DRAIN / 1.2;
}
#ifndef CLIENT_DLL
MELEE_ACTTABLE(hanger)
#endif

#ifdef CLIENT_DLL
#define CWeaponsmallsword C_Weaponsmallsword
#endif
DECLARE_BG2_WEAPON(smallsword)
{
	m_fHolsterTime = 0.75f;

	m_bWeaponHasSights = false;

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_SLASH;
	m_Attackinfos[0].m_iDamage = HANGER_DAMAGE;//60;
	m_Attackinfos[0].m_flAttackrate = 1.2;//-0.7f;
	m_Attackinfos[0].m_flRange = SMALLSWORD_RANGE;
	//m_Attackinfos[0].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_flCosAngleTolerance = HANGER_COS_TOLERANCE;
	m_Attackinfos[0].m_flRetraceDuration = HANGER_RETRACE_DURATION;
	m_Attackinfos[0].m_iStaminaDrain = MELEE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype = ATTACKTYPE_SLASH;
	m_Attackinfos[1].m_iDamage = HANGER_DAMAGE / 1.5;//60;
	m_Attackinfos[1].m_flAttackrate = 1.2 / 1.5;//-0.7f;
	m_Attackinfos[1].m_flRange = SMALLSWORD_RANGE;
	//m_Attackinfos[0].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[1].m_flCosAngleTolerance = HANGER_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration = HANGER_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain = MELEE_STAMINA_DRAIN / 1.2;
}
#ifndef CLIENT_DLL
MELEE_ACTTABLE(smallsword)
#endif

#ifdef CLIENT_DLL
#define CWeapontomahawk C_Weapontomahawk
#endif
DECLARE_BG2_WEAPON( tomahawk )
{
	m_fHolsterTime = 0.75f;

	m_bWeaponHasSights = false; 

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_SLASH;
	m_Attackinfos[0].m_iDamage				= TOMAHAWK_DAMAGE;//60;
	m_Attackinfos[0].m_flAttackrate			= 1.4;//-0.7f;
	m_Attackinfos[0].m_flRange				= TOMAHAWK_RANGE;
	//m_Attackinfos[0].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_ATTACK;
	m_Attackinfos[0].m_flCosAngleTolerance  = TOMAHAWK_COS_TOLERANCE;
	m_Attackinfos[0].m_flRetraceDuration    = TOMAHAWK_RETRACE_DURATION;
	m_Attackinfos[0].m_iStaminaDrain		= MELEE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1] = m_Attackinfos[0];
}

#ifndef CLIENT_DLL
MELEE_ACTTABLE( tomahawk )
#endif

#ifdef CLIENT_DLL
#define CWeaponclub C_Weaponclub
#endif
DECLARE_BG2_WEAPON(club)
{
	m_fHolsterTime = 0.75f;

	m_bWeaponHasSights = false;

	//m_iOwnerSpeedModOnKill = 3;

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_SLASH;
	m_Attackinfos[0].m_iDamage = CLUB_DAMAGE;//60;
	m_Attackinfos[0].m_flAttackrate = 1.4f;//-0.7f;
	m_Attackinfos[0].m_flRange = CLUB_RANGE;
	//m_Attackinfos[0].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_ATTACK;
	m_Attackinfos[0].m_flCosAngleTolerance = TOMAHAWK_COS_TOLERANCE;
	m_Attackinfos[0].m_flRetraceDuration = TOMAHAWK_RETRACE_DURATION;
	m_Attackinfos[0].m_iStaminaDrain = MELEE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype = ATTACKTYPE_SLASH;
	m_Attackinfos[1].m_iDamage = CLUB_DAMAGE * 0.64f;//60;
	m_Attackinfos[1].m_flAttackrate = 1.4f * 0.64f;//-0.7f;
	m_Attackinfos[1].m_flRange = CLUB_RANGE;
	//m_Attackinfos[0].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity = ACT_VM_ATTACK;
	m_Attackinfos[1].m_flCosAngleTolerance = TOMAHAWK_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration = TOMAHAWK_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain = MELEE_STAMINA_DRAIN * 0.64f;

	//secondary
	//m_Attackinfos[1] = m_Attackinfos[0];
}

#ifndef CLIENT_DLL
MELEE_ACTTABLE(club)
#endif

#ifdef CLIENT_DLL
#define CWeaponrevolutionnaire C_Weaponrevolutionnaire
#endif
DECLARE_BG2_WEAPON( revolutionnaire )
{
	m_bCantAbortReload	= true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.6f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset		= -2.5;

	m_bWeaponHasSights = true; 
	//

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage				= REVOL_FIRE_DAMAGE;//75
	m_Attackinfos[0].m_flAttackrate			= 1.0;
	m_Attackinfos[0].m_flRecoil				= 0.7;
	m_Attackinfos[0].m_flRange				= MUSKET_RANGE;
	// new
	m_Attackinfos[0].m_flCrouchMoving		= 11.12f;
	m_Attackinfos[0].m_flCrouchStill		= 3.06f;  //2.4
	m_Attackinfos[0].m_flStandMoving		= 12.3f; //12.0f
	m_Attackinfos[0].m_flStandStill			= 3.06f; //2.4
	// old
	//m_Attackinfos[0].m_flCrouchMoving		= 11.3f;
	//m_Attackinfos[0].m_flCrouchStill		= 3.2f;  //2.4
	//m_Attackinfos[0].m_flStandMoving		= 12.5f; //12.0f
	//m_Attackinfos[0].m_flStandStill			= 3.2f; //2.4
	//Iron Sights. These values will probably be changed.
	// BG2 - VisualMelon - Accuracy values describe the radius of a circle I believe, we want to reduce area by ~3% (a = pi * r * r)
	m_Attackinfos[0].m_flStandAimStill		= 2.12f;	
	m_Attackinfos[0].m_flStandAimMoving		= 8.17f;
	m_Attackinfos[0].m_flCrouchAimStill		= 2.12f;
	m_Attackinfos[0].m_flCrouchAimMoving	= 7.39f;
	// BG2 - VisualMelon - Old values (new ones are 3% more accurate)
	//m_Attackinfos[0].m_flStandAimStill		= 2.15f;	
	//m_Attackinfos[0].m_flStandAimMoving		= 8.3f;
	//m_Attackinfos[0].m_flCrouchAimStill		= 2.15f;
	//m_Attackinfos[0].m_flCrouchAimMoving	= 7.5f;
	//
	m_Attackinfos[0].m_flConstantDamageRange= DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag		= 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain		= MUSKET_RIFLE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype			= ATTACKTYPE_STAB;
	m_Attackinfos[1].m_iDamage				= REVOL_BAYONET_DAMAGE;
	m_Attackinfos[1].m_flAttackrate			= 1.0f;
	m_Attackinfos[1].m_flRange				= REVOL_BAYONET_RANGE;
	//m_Attackinfos[1].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity		= ACT_VM_SECONDARYATTACK;
	m_Attackinfos[1].m_iAttackActivityEmpty	= ACT_VM_SECONDARYATTACK_EMPTY;
	m_Attackinfos[1].m_flCosAngleTolerance  = BAYONET_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration    = BAYONET_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain		= MELEE_STAMINA_DRAIN;

	m_pBayonetDeathNotice = "revolutionnaire_bayonet";

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
MUSKET_ACTTABLE( revolutionnaire )
#endif

#ifdef CLIENT_DLL
#define CWeaponrevolutionnaire_nobayo C_Weaponrevolutionnaire_nobayo
#endif
DECLARE_BG2_WEAPON(revolutionnaire_nobayo)
{
	m_bCantAbortReload = true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.6f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset = -2.5;

	m_bWeaponHasSights = true;
	//

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage = REVOL_FIRE_DAMAGE;//75
	m_Attackinfos[0].m_flAttackrate = 1.0;
	m_Attackinfos[0].m_flRecoil = 0.7;
	m_Attackinfos[0].m_flRange = MUSKET_RANGE;
	// new
	m_Attackinfos[0].m_flCrouchMoving = 11.12f;
	m_Attackinfos[0].m_flCrouchStill = 3.06f;  //2.4
	m_Attackinfos[0].m_flStandMoving = 12.3f; //12.0f
	m_Attackinfos[0].m_flStandStill = 3.06f; //2.4
	// old
	//m_Attackinfos[0].m_flCrouchMoving		= 11.3f;
	//m_Attackinfos[0].m_flCrouchStill		= 3.2f;  //2.4
	//m_Attackinfos[0].m_flStandMoving		= 12.5f; //12.0f
	//m_Attackinfos[0].m_flStandStill			= 3.2f; //2.4
	//Iron Sights. These values will probably be changed.
	// BG2 - VisualMelon - Accuracy values describe the radius of a circle I believe, we want to reduce area by ~3% (a = pi * r * r)
	m_Attackinfos[0].m_flStandAimStill = 2.12f;
	m_Attackinfos[0].m_flStandAimMoving = 8.17f;
	m_Attackinfos[0].m_flCrouchAimStill = 2.12f;
	m_Attackinfos[0].m_flCrouchAimMoving = 7.39f;
	// BG2 - VisualMelon - Old values (new ones are 3% more accurate)
	//m_Attackinfos[0].m_flStandAimStill		= 2.15f;	
	//m_Attackinfos[0].m_flStandAimMoving		= 8.3f;
	//m_Attackinfos[0].m_flCrouchAimStill		= 2.15f;
	//m_Attackinfos[0].m_flCrouchAimMoving	= 7.5f;
	//
	m_Attackinfos[0].m_flConstantDamageRange = DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag = 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain = MUSKET_RIFLE_STAMINA_DRAIN;

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
MUSKET_ACTTABLE(revolutionnaire_nobayo)
#endif

#ifdef CLIENT_DLL //Basically just a Brownbess without the bayo coded in. 
#define CWeaponbrownbess_nobayo C_Weaponbrownbess_nobayo
#endif
DECLARE_BG2_WEAPON( brownbess_nobayo )
{
	m_bCantAbortReload	= true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.6f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset		= -2.5;

	m_bWeaponHasSights = true; 
	//

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage				= BESS_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate			= 1.0;
	m_Attackinfos[0].m_flRecoil				= 0.7;
	m_Attackinfos[0].m_flRange				= MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving		= 12.0f;
	m_Attackinfos[0].m_flCrouchStill		= 3.6f; //2.4
	m_Attackinfos[0].m_flStandMoving		= 13.2f; //12.0f
	m_Attackinfos[0].m_flStandStill			= 3.6f; //2.4
	//Iron Sights.
	m_Attackinfos[0].m_flStandAimStill		= 2.4f;	
	m_Attackinfos[0].m_flStandAimMoving		= 8.8f;
	m_Attackinfos[0].m_flCrouchAimStill		= 2.4f;
	m_Attackinfos[0].m_flCrouchAimMoving	= 8.0f;
	//
	m_Attackinfos[0].m_flConstantDamageRange= DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag		= 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain		= MUSKET_RIFLE_STAMINA_DRAIN;

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flZeroRange = ZERO_RANGE_MUSKET;

	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
MUSKET_ACTTABLE( brownbess_nobayo )
#endif

#ifdef CLIENT_DLL
#define CWeaponbeltaxe C_Weaponbeltaxe
#endif
DECLARE_BG2_WEAPON( beltaxe )
{
	m_fHolsterTime = 0.75f;

	m_bWeaponHasSights = false; 

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_SLASH;
	m_Attackinfos[0].m_iDamage				= TOMAHAWK_DAMAGE;//60;
	m_Attackinfos[0].m_flAttackrate			= 1.4;//-0.7f;
	m_Attackinfos[0].m_flRange				= TOMAHAWK_RANGE;
	//m_Attackinfos[0].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_flCosAngleTolerance  = TOMAHAWK_COS_TOLERANCE;
	m_Attackinfos[0].m_flRetraceDuration    = TOMAHAWK_RETRACE_DURATION;
	m_Attackinfos[0].m_iStaminaDrain		= MELEE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1] = m_Attackinfos[0];
}
#ifndef CLIENT_DLL
MELEE_ACTTABLE( beltaxe )
#endif

#ifdef CLIENT_DLL
#define CWeaponfowler C_Weaponfowler
#endif
DECLARE_BG2_WEAPON( fowler )
{
	m_bCantAbortReload	= true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.2f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset		= -2.5;

	m_bWeaponHasSights = true; 
	//

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage				= FOWLER_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate			= 1.0;
	m_Attackinfos[0].m_flRecoil				= 0.7;
	m_Attackinfos[0].m_flRange				= MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving		= 12.1f;
	m_Attackinfos[0].m_flCrouchStill		= 3.4f;
	m_Attackinfos[0].m_flStandMoving		= 13.3f;
	m_Attackinfos[0].m_flStandStill			= 3.4f;
	//Iron Sights. These values will probably be changed.
	m_Attackinfos[0].m_flStandAimStill		= 2.05f;	
	m_Attackinfos[0].m_flStandAimMoving		= 8.2f;
	m_Attackinfos[0].m_flCrouchAimStill		= 2.05f;
	m_Attackinfos[0].m_flCrouchAimMoving	= 7.4f;
	//
	m_Attackinfos[0].m_flConstantDamageRange= DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag		= 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain		= MUSKET_RIFLE_STAMINA_DRAIN;

	m_flShotAimModifier = -1.0f;
	m_flShotSpread = 6.95f * 0.75f;		//4 m spread at 33 m -> (4 / 2) / 33 / sin(0.5)
	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flShotMuzzleVelocity = MUZZLE_VELOCITY_BUCKSHOT;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = FOWLER_NUM_SHOT;
	m_iDamagePerShot = FOWLER_SHOT_DAMAGE;
}

#ifndef CLIENT_DLL
MUSKET_ACTTABLE( fowler )
#endif

#ifdef CLIENT_DLL
#define CWeaponlongpattern C_Weaponlongpattern
#endif
DECLARE_BG2_WEAPON( longpattern )
{
	m_bCantAbortReload	= true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 8.2f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset		= -2.5;

	m_bWeaponHasSights = true; 
	//

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage				= LONGPATTERN_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate			= 1.0;
	m_Attackinfos[0].m_flRecoil				= 0.7;
	m_Attackinfos[0].m_flRange				= MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving		= 11.0f;
	m_Attackinfos[0].m_flCrouchStill		= 3.5f; //2.4
	m_Attackinfos[0].m_flStandMoving		= 12.2f; //12.0f
	m_Attackinfos[0].m_flStandStill			= 3.5f; //2.4
	//Iron Sights. These values will probably be changed.
	m_Attackinfos[0].m_flStandAimStill		= 2.3f;	
	m_Attackinfos[0].m_flStandAimMoving		= 8.1f;
	m_Attackinfos[0].m_flCrouchAimStill		= 2.3f;
	m_Attackinfos[0].m_flCrouchAimMoving	= 7.3f;
	//
	m_Attackinfos[0].m_flConstantDamageRange= DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag		= 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain		= MUSKET_RIFLE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype			= ATTACKTYPE_STAB;
	m_Attackinfos[1].m_iDamage				= LONGPATTERN_BAYONET_DAMAGE;//60;
	m_Attackinfos[1].m_flAttackrate			= 1.2f;//-0.7f;
	m_Attackinfos[1].m_flRange				= LONGPATTERN_BAYONET_RANGE;
	//m_Attackinfos[1].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity		= ACT_VM_SECONDARYATTACK;
	m_Attackinfos[1].m_iAttackActivityEmpty	= ACT_VM_SECONDARYATTACK_EMPTY;
	m_Attackinfos[1].m_flCosAngleTolerance  = BAYONET_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration    = BAYONET_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain		= MELEE_STAMINA_DRAIN_HEAVY;

	m_pBayonetDeathNotice = "longpattern_bayonet";

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
MUSKET_ACTTABLE( longpattern )
#endif

#ifdef CLIENT_DLL
#define CWeaponoldpattern C_Weaponoldpattern
#endif
DECLARE_BG2_WEAPON(oldpattern)
{
	m_bCantAbortReload = true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 8.2f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset = -2.5;
	m_bSlowDraw = true;
	m_flLockTime = 0.2f;
	m_flRandomAdditionalLockTimeMax = 0.1f;

	m_bWeaponHasSights = true;
	//

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage = OLDPATTERN_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate = 1.0;
	m_Attackinfos[0].m_flRecoil = 0.9;
	m_Attackinfos[0].m_flRange = MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving = 11.0f;
	m_Attackinfos[0].m_flCrouchStill = 3.5f; //2.4
	m_Attackinfos[0].m_flStandMoving = 12.2f; //12.0f
	m_Attackinfos[0].m_flStandStill = 3.5f; //2.4
	//Iron Sights. These values will probably be changed.
	m_Attackinfos[0].m_flStandAimStill = 2.2f;
	m_Attackinfos[0].m_flStandAimMoving = 8.1f;
	m_Attackinfos[0].m_flCrouchAimStill = 2.2f;
	m_Attackinfos[0].m_flCrouchAimMoving = 7.3f;
	//
	m_Attackinfos[0].m_flConstantDamageRange = DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag = 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain = MUSKET_RIFLE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype = ATTACKTYPE_STAB;
	m_Attackinfos[1].m_iDamage = OLDPATTERN_BAYONET_DAMAGE;//60;
	m_Attackinfos[1].m_flAttackrate = 1.25f;//-0.7f;
	m_Attackinfos[1].m_flRange = OLDPATTERN_BAYONET_RANGE;
	//m_Attackinfos[1].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity = ACT_VM_SECONDARYATTACK;
	m_Attackinfos[1].m_iAttackActivityEmpty = ACT_VM_SECONDARYATTACK_EMPTY;
	m_Attackinfos[1].m_flCosAngleTolerance = BAYONET_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration = BAYONET_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain = MELEE_STAMINA_DRAIN * 1.5f;

	m_pBayonetDeathNotice = "longpattern_bayonet";

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
MUSKET_ACTTABLE(oldpattern)
#endif

#ifdef CLIENT_DLL
#define CWeaponlongpattern_nobayo C_Weaponlongpattern_nobayo
#endif
DECLARE_BG2_WEAPON( longpattern_nobayo )
{
	m_bCantAbortReload	= true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 8.2f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset		= -2.5;

	m_bWeaponHasSights = true; 
	//

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage				= LONGPATTERN_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate			= 1.0;
	m_Attackinfos[0].m_flRecoil				= 0.7;
	m_Attackinfos[0].m_flRange				= MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving		= 11.0f;
	m_Attackinfos[0].m_flCrouchStill		= 3.5f; //2.4
	m_Attackinfos[0].m_flStandMoving		= 12.2f; //12.0f
	m_Attackinfos[0].m_flStandStill			= 3.5f; //2.4
	//Iron Sights. These values will probably be changed.
	m_Attackinfos[0].m_flStandAimStill		= 2.3f;	
	m_Attackinfos[0].m_flStandAimMoving		= 8.1f;
	m_Attackinfos[0].m_flCrouchAimStill		= 2.3f;
	m_Attackinfos[0].m_flCrouchAimMoving	= 7.3f;
	//
	m_Attackinfos[0].m_flConstantDamageRange= DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag		= 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain		= MUSKET_RIFLE_STAMINA_DRAIN;

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
MUSKET_ACTTABLE( longpattern_nobayo )
#endif

#ifdef CLIENT_DLL
#define CWeaponspontoon C_Weaponspontoon
#endif
DECLARE_BG2_WEAPON(spontoon)
{
	m_bWeaponHasSights = false;
	//

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_STAB;
	m_Attackinfos[0].m_iDamage = SPONTOON_DAMAGE;
	m_Attackinfos[0].m_flAttackrate = 0.85f;
	m_Attackinfos[0].m_flRange = SPONTOON_RANGE;
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_SECONDARYATTACK;
	m_Attackinfos[0].m_iAttackActivityEmpty = ACT_VM_SECONDARYATTACK_EMPTY;
	m_Attackinfos[0].m_flCosAngleTolerance = BAYONET_COS_TOLERANCE;
	m_Attackinfos[0].m_flRetraceDuration = BAYONET_RETRACE_DURATION;
	//m_Attackinfos[0].m_flRetraceDelay = 0.1f;
	m_Attackinfos[0].m_iStaminaDrain = MELEE_STAMINA_DRAIN / 2;

	//secondary
	m_Attackinfos[1].m_iAttacktype = ATTACKTYPE_STAB;
	m_Attackinfos[1].m_iDamage = (SPONTOON_DAMAGE) / 1.8f;
	m_Attackinfos[1].m_flAttackrate = 0.85f / 2;
	m_Attackinfos[1].m_flRange = SPONTOON_RANGE;
	m_Attackinfos[1].m_iAttackActivity = ACT_VM_SECONDARYATTACK;
	m_Attackinfos[1].m_iAttackActivityEmpty = ACT_VM_SECONDARYATTACK_EMPTY;
	m_Attackinfos[1].m_flCosAngleTolerance = BAYONET_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration = BAYONET_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain = m_Attackinfos[0].m_iStaminaDrain / 2;

	//Do more damage while in the air - this lets us be fatal
	//m_iAerialDamageMod = 5;
}

#ifndef CLIENT_DLL
MUSKET_ACTTABLE(spontoon)
#endif


#ifdef CLIENT_DLL
#define CWeaponamerican_brownbess C_Weaponamerican_brownbess
#endif
DECLARE_BG2_WEAPON( american_brownbess )
{
	m_bCantAbortReload	= true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.6f;
	m_flLockTime = 0.25f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset		= -2.5;

	m_bWeaponHasSights = true; 
	//

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage				= AMER_BESS_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate			= 1.0;
	m_Attackinfos[0].m_flRecoil				= 0.7;
	m_Attackinfos[0].m_flRange				= MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving		= 12.1f;
	m_Attackinfos[0].m_flCrouchStill		= 3.7f; //2.4
	m_Attackinfos[0].m_flStandMoving		= 13.3f; //12.0f
	m_Attackinfos[0].m_flStandStill			= 3.7f; //2.4
	//Iron Sights. These values will probably be changed.
	m_Attackinfos[0].m_flStandAimStill		= 2.5f;	
	m_Attackinfos[0].m_flStandAimMoving		= 8.9f;
	m_Attackinfos[0].m_flCrouchAimStill		= 2.5f;
	m_Attackinfos[0].m_flCrouchAimMoving	= 8.1f;
	//
	m_Attackinfos[0].m_flConstantDamageRange= DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag		= 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain		= MUSKET_RIFLE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype			= ATTACKTYPE_STAB;
	m_Attackinfos[1].m_iDamage				= AMER_BESS_BAYONET_DAMAGE;//60;
	m_Attackinfos[1].m_flAttackrate			= 1.0f;//-0.7f;
	m_Attackinfos[1].m_flRange				= AMER_BESS_BAYONET_RANGE;
	//m_Attackinfos[1].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity		= ACT_VM_SECONDARYATTACK;
	m_Attackinfos[1].m_iAttackActivityEmpty	= ACT_VM_SECONDARYATTACK_EMPTY;
	m_Attackinfos[1].m_flCosAngleTolerance  = BAYONET_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration    = BAYONET_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain		= MELEE_STAMINA_DRAIN;

	m_pBayonetDeathNotice = "brownbess_bayonet";

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flDamageDropoffMultiplier = 0.8;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
MUSKET_ACTTABLE( american_brownbess )
#endif

#ifdef CLIENT_DLL 
#define CWeaponamerican_brownbess_nobayo C_Weaponamerican_brownbess_nobayo
#endif
DECLARE_BG2_WEAPON( american_brownbess_nobayo )
{
	m_bCantAbortReload	= true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.6f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset		= -2.5;

	m_bWeaponHasSights = true; 
	//

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage				= BESS_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate			= 1.0;
	m_Attackinfos[0].m_flRecoil				= 0.7;
	m_Attackinfos[0].m_flRange				= MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving		= 12.0f;
	m_Attackinfos[0].m_flCrouchStill		= 3.6f; //2.4
	m_Attackinfos[0].m_flStandMoving		= 13.2f; //12.0f
	m_Attackinfos[0].m_flStandStill			= 3.6f; //2.4
	//Iron Sights.
	m_Attackinfos[0].m_flStandAimStill		= 2.4f;	
	m_Attackinfos[0].m_flStandAimMoving		= 8.8f;
	m_Attackinfos[0].m_flCrouchAimStill		= 2.4f;
	m_Attackinfos[0].m_flCrouchAimMoving	= 8.0f;
	//
	m_Attackinfos[0].m_flConstantDamageRange= DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag		= 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain		= MUSKET_RIFLE_STAMINA_DRAIN;

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
MUSKET_ACTTABLE( american_brownbess_nobayo )
#endif

#ifdef CLIENT_DLL 
#define CWeaponbrownbess_carbine C_Weaponbrownbess_carbine
#endif
DECLARE_BG2_WEAPON( brownbess_carbine )
{
	m_bCantAbortReload	= true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.0f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset = -2.5;

	m_bWeaponHasSights = true; 
	m_bQuickdraw = true;
	//

	//primary
	m_Attackinfos[0].m_iAttacktype			= ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage				= CARBINE_FIRE_DAMAGE;
	m_Attackinfos[0].m_flAttackrate			= 1.0;
	m_Attackinfos[0].m_flRecoil				= 0.7;
	m_Attackinfos[0].m_flRange				= MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving		= 12.0f;
	m_Attackinfos[0].m_flCrouchStill		= 3.6f;
	m_Attackinfos[0].m_flStandMoving		= 13.2f;
	m_Attackinfos[0].m_flStandStill			= 3.6f;
	//Iron Sights.
	m_Attackinfos[0].m_flStandAimStill		= 2.6f;	
	m_Attackinfos[0].m_flStandAimMoving		= 8.1f;
	m_Attackinfos[0].m_flCrouchAimStill		= 2.6f;
	m_Attackinfos[0].m_flCrouchAimMoving	= 7.3f;
	//
	m_Attackinfos[0].m_flConstantDamageRange= DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag		= 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity		= ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain		= MUSKET_RIFLE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype			= ATTACKTYPE_STAB;
	m_Attackinfos[1].m_iDamage				= CARBINE_BAYONET_DAMAGE;//60;
	m_Attackinfos[1].m_flAttackrate			= 0.9f;//1.0f;//-0.7f;
	m_Attackinfos[1].m_flRange				= CARBINE_BAYONET_RANGE;
	//m_Attackinfos[1].m_flCosAngleTolerance	= 0.95f;
	m_Attackinfos[1].m_iAttackActivity		= ACT_VM_SECONDARYATTACK;
	m_Attackinfos[1].m_iAttackActivityEmpty	= ACT_VM_SECONDARYATTACK_EMPTY;
	m_Attackinfos[1].m_flCosAngleTolerance  = BAYONET_COS_TOLERANCE;
	m_Attackinfos[1].m_flRetraceDuration    = BAYONET_RETRACE_DURATION;
	m_Attackinfos[1].m_iStaminaDrain		= MELEE_STAMINA_DRAIN / 2;

	m_pBayonetDeathNotice = "brownbess_bayonet";

	m_flShotAimModifier = -1.0f;
	m_flShotSpread = 7.64f * 0.75f;		//4 m spread at 30 m -> (4 / 2) / 30 / sin(0.5)
	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flShotMuzzleVelocity = MUZZLE_VELOCITY_BUCKSHOT;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = CARBINE_NUM_SHOT;
	m_iDamagePerShot = CARBINE_SHOT_DAMAGE;
}

#ifndef CLIENT_DLL
//roob - should be carbibe but someone borked the player anims!
//CARBINE_ACTTABLE( brownbess_carbine )
MUSKET_ACTTABLE( brownbess_carbine )
#endif

#ifdef CLIENT_DLL 
#define CWeaponblunderbuss C_Weaponblunderbuss
#endif
DECLARE_BG2_WEAPON(blunderbuss)
{
	m_bCantAbortReload = true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.0f;
	m_bShotOnly = true;
	m_flLockTime = 0.35f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset = 0;
	m_flReloadMovementSpeedModifier = 1.3f;

	m_bWeaponHasSights = true;
	//

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage = CARBINE_FIRE_DAMAGE;
	m_Attackinfos[0].m_flAttackrate = 1.0;
	m_Attackinfos[0].m_flRecoil = 1.4;
	m_Attackinfos[0].m_flRange = MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving = 25.0f;
	m_Attackinfos[0].m_flCrouchStill = 22.0f;
	m_Attackinfos[0].m_flStandMoving = 25.0f;
	m_Attackinfos[0].m_flStandStill = 22.0f;
	//Iron Sights.
	m_Attackinfos[0].m_flStandAimStill = 4.0f;
	m_Attackinfos[0].m_flStandAimMoving = 8.1f;
	m_Attackinfos[0].m_flCrouchAimStill = 4.0f;
	m_Attackinfos[0].m_flCrouchAimMoving = 7.3f;
	//
	m_Attackinfos[0].m_flConstantDamageRange = DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag = 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain = MUSKET_RIFLE_STAMINA_DRAIN;

	m_flShotAimModifier = 0.0f;
	m_flShotSpread = 4.0f; //tight spread
	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flShotMuzzleVelocity = MUZZLE_VELOCITY_BUCKSHOT;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = CARBINE_NUM_SHOT;
	m_iDamagePerShot = BLUNDERBUSS_SHOT_DAMAGE;
}

#ifndef CLIENT_DLL
//roob - should be carbibe but someone borked the player anims!
//CARBINE_ACTTABLE( brownbess_carbine )
MUSKET_ACTTABLE(blunderbuss)
#endif

#ifdef CLIENT_DLL 
#define CWeaponbrownbess_carbine_nobayo C_Weaponbrownbess_carbine_nobayo
#endif
DECLARE_BG2_WEAPON(brownbess_carbine_nobayo)
{
	m_bCantAbortReload = true;

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.0f;
	m_bQuickdraw = true;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset = -2.5;

	m_bWeaponHasSights = true;
	//

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage = CARBINE_FIRE_DAMAGE;
	m_Attackinfos[0].m_flAttackrate = 1.0;
	m_Attackinfos[0].m_flRecoil = 0.7;
	m_Attackinfos[0].m_flRange = MUSKET_RANGE;
	m_Attackinfos[0].m_flCrouchMoving = 12.0f;
	m_Attackinfos[0].m_flCrouchStill = 3.6f;
	m_Attackinfos[0].m_flStandMoving = 13.2f;
	m_Attackinfos[0].m_flStandStill = 3.6f;
	//Iron Sights.
	m_Attackinfos[0].m_flStandAimStill = 2.6f;
	m_Attackinfos[0].m_flStandAimMoving = 8.1f;
	m_Attackinfos[0].m_flCrouchAimStill = 2.6f;
	m_Attackinfos[0].m_flCrouchAimMoving = 7.3f;
	//
	m_Attackinfos[0].m_flConstantDamageRange = DAMAGE_ADJUSTED_RANGE();
	m_Attackinfos[0].m_flRelativeDrag = 1.0;			//musket
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain = MUSKET_RIFLE_STAMINA_DRAIN;

	m_flShotAimModifier = -1.0f;
	m_flShotSpread = 7.64f * 0.75f;		//4 m spread at 30 m -> (4 / 2) / 30 / sin(0.5)
	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE;
	m_flShotMuzzleVelocity = MUZZLE_VELOCITY_BUCKSHOT;
	m_flZeroRange = ZERO_RANGE_MUSKET;
	m_iNumShot = CARBINE_NUM_SHOT;
	m_iDamagePerShot = CARBINE_SHOT_DAMAGE;
}

#ifndef CLIENT_DLL
//roob - should be carbibe but someone borked the player anims!
//CARBINE_ACTTABLE( brownbess_carbine )
MUSKET_ACTTABLE(brownbess_carbine_nobayo)
#endif

//jäger rifle, but spelled jaeger to avoid any charset problems
#ifdef CLIENT_DLL
#define CWeaponpattern1776 C_Weaponpattern1776
#endif
DECLARE_BG2_WEAPON(pattern1776)
{
	m_bCantAbortReload = true;
	m_eWeaponType = RIFLE;
	//m_flLockTime = LOCK_TIME_RIFLE; //Awesome - avoid most rifle advantages on infantry weapon

	m_fHolsterTime = 0.75f;
	m_flApproximateReloadTime = 7.6f;

	//Iron sights viewmodel settings.
	m_flIronsightFOVOffset = -5;

	m_bWeaponHasSights = true;
	//

	//primary
	m_Attackinfos[0].m_iAttacktype = ATTACKTYPE_FIREARM;
	m_Attackinfos[0].m_iDamage = PATTERN_FIRE_DAMAGE;//75;
	m_Attackinfos[0].m_flAttackrate = 1.0;
	m_Attackinfos[0].m_flRecoil = 0.9;
	m_Attackinfos[0].m_flRange = RIFLE_RANGE;

	m_Attackinfos[0].m_flCrouchMoving = 12.1f;
	m_Attackinfos[0].m_flCrouchStill = 3.0f;
	m_Attackinfos[0].m_flStandMoving = 13.3f;
	m_Attackinfos[0].m_flStandStill = 3.3f;
	//Iron Sights. These values will probably be changed.
	m_Attackinfos[0].m_flStandAimStill = 2.05f;
	m_Attackinfos[0].m_flStandAimMoving = 8.2f;
	m_Attackinfos[0].m_flCrouchAimStill = 1.0f;
	m_Attackinfos[0].m_flCrouchAimMoving = 7.4f;

	m_Attackinfos[0].m_flConstantDamageRange = RIFLE_CONSTANT_DAMAGE_RANGE / 2;
	m_Attackinfos[0].m_flRelativeDrag = 0.75;			//rifle
	m_Attackinfos[0].m_iAttackActivity = ACT_VM_PRIMARYATTACK;
	m_Attackinfos[0].m_iStaminaDrain = MUSKET_RIFLE_STAMINA_DRAIN;

	//secondary
	m_Attackinfos[1].m_iAttacktype = ATTACKTYPE_NONE;

	m_flMuzzleVelocity = MUZZLE_VELOCITY_SMOOTHBORE; //Pattern1776 is shorter, use lower default velocity
	m_flZeroRange = ZERO_RANGE_MUSKET; //this is more of a close range sniper, so use this instead
	m_iNumShot = 0;
}

#ifndef CLIENT_DLL
RIFLE_ACTTABLE(pattern1776)
#endif