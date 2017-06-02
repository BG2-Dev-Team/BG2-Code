//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#ifdef WIN32
#pragma once
#endif
#include "cbase.h"
#include "npcevent.h"
#include "in_buttons.h"

#ifdef CLIENT_DLL
#include "c_hl2mp_player.h"
#include "c_te_effect_dispatch.h"
#else
#include "hl2mp_player.h"
#include "te_effect_dispatch.h"
#include "grenade_frag.h"
#endif

#include "../shared/bg2/bg3_weapon_shared.h"
#include "weapon_ar2.h"
#include "effect_dispatch_data.h"
#include "weapon_hl2mpbasehlmpcombatweapon.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//BG2 - no grenades - yet ! - Awesome


#define GRENADE_TIMER	2.5f //Seconds

#define GRENADE_PAUSED_NO			0
#define GRENADE_PAUSED_PRIMARY		1
#define GRENADE_PAUSED_SECONDARY	2

#define GRENADE_RADIUS	4.0f // inches

#define GRENADE_DAMAGE_RADIUS 250.0f

#ifdef CLIENT_DLL
#define CWeaponFrag C_WeaponFrag
#endif

//-----------------------------------------------------------------------------
// Fragmentation grenades
//-----------------------------------------------------------------------------
class CWeaponFrag : public CBaseHL2MPCombatWeapon
{
	DECLARE_CLASS(CWeaponFrag, CBaseHL2MPCombatWeapon);
public:

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CWeaponFrag();

	void	Precache(void);
	void	PrimaryAttack(void);
	void	SecondaryAttack(void);
	void	DecrementAmmo(CBaseCombatCharacter *pOwner);
	void	ItemPostFrame(void);

	bool	Deploy(void);
	bool	Holster(CBaseCombatWeapon *pSwitchingTo = NULL);
	void	FuseSound() { 
		EmitSound(GRENADE_FUSE_SOUND);
		Msg("Played grenade fuse sound!\n");
	}

	bool	Reload(void);

#ifndef CLIENT_DLL
	void Operator_HandleAnimEvent(animevent_t *pEvent, CBaseCombatCharacter *pOperator);
	void CheckForFuseEnd(); //BG3 - Awesome - BOOM! The grenade explodes right in your face
#endif

	void	ThrowGrenade(CBasePlayer *pPlayer);
	bool	IsPrimed() const { return (m_AttackPaused != 0); }

	void	RollGrenade(CBasePlayer *pPlayer);
	void	LobGrenade(CBasePlayer *pPlayer, float speedOverride = 350);

private:
	// check a throw from vecSrc.  If not valid, move the position back along the line to vecEye
	void	CheckThrowPosition(CBasePlayer *pPlayer, const Vector &vecEye, Vector &vecSrc);

	CNetworkVar(bool, m_bRedraw);	//Draw the weapon again after throwing a grenade

	CNetworkVar(int, m_AttackPaused);
	CNetworkVar(bool, m_fDrawbackFinished);
	CNetworkVar(float, m_flFuseEndTime);

	CWeaponFrag(const CWeaponFrag &);

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif
};