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
#include "bg3_class.h"
#include "hl2mp_gamerules.h"
#include <game/client/iviewport.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Menu.h>
#include "IGameUIFuncs.h" // for key bindings
#include <imapoverview.h>
#include <shareddefs.h>
#include <igameresources.h>
#include "engine/IEngineSound.h"
#include <vgui/ILocalize.h>
#include "bg3_class_quota.h"
#include "../shared/bg3/bg3_unlockable.h"

#ifdef CLIENT_DLL
#include "c_team.h"
#include "bg3/persistent/versioning.h"
#else
#include "team.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//BG2 - Tjoppen - beautiful defines. you will see another one further down
#ifdef CLIENT_DLL
#define CVAR_FLAGS	(FCVAR_REPLICATED | FCVAR_NOTIFY)
#define CVAR_FLAGS_HIDDEN (FCVAR_REPLICATED)
#else
#define CVAR_FLAGS	(FCVAR_GAMEDLL | FCVAR_REPLICATED | FCVAR_NOTIFY)
#define CVAR_FLAGS_HIDDEN (FCVAR_GAMEDLL | FCVAR_REPLICATED)
#endif

//in case the year setting changes while we're in the classmenu, we'll have to force
//it to update whenever it changes
#ifndef CLIENT_DLL
ConVar mp_year("mp_year", "1779", CVAR_FLAGS, "Year setting controls which weapons are available, for historical accuracy. Used only if mp_year_accuracy = 1", true, 0.f, true, 1850.f);
ConVar mp_year_accuracy("mp_year_accuracy", "0", CVAR_FLAGS, "Controls whether or not to use year-by-year historical weapon accuracy.");
#else
void OnYearSettingChanged(IConVar* pVar, const char* pszOldValue, float flOldValue);
ConVar mp_year("mp_year", "1779", CVAR_FLAGS, "Year setting controls which weapons are available, for historical accuracy. Used only if mp_year_accuracy = 1", true, 0.f, true, 1850.f, OnYearSettingChanged);
ConVar mp_year_accuracy("mp_year_accuracy", "0", CVAR_FLAGS, "Controls whether or not to use year-by-year historical weapon accuracy.", OnYearSettingChanged);
#endif

#ifdef CLIENT_DLL
bool CGunKit::AvailableForCurrentYear() const {
	//return true;
	if (mp_year_accuracy.GetBool() && mp_year.GetBool()) {
		int year = mp_year.GetInt();
		return (year >= m_iMinYear && year <= m_iMaxYear);
	}
	else {
		return !m_bDisabledOnHistoricityDisabled || mp_year.GetInt() == 0;
	}
}
#endif

CPlayerClass::CPlayerClass(const char* abrv) {
		m_pszAbbreviation = abrv;
		m_pPopCounter = new NClassQuota::SPopulationCounter;
		memset(m_aUniformControllingBits, 0, sizeof(m_aUniformControllingBits));
		memset(m_aUniformControllingBitsActive, 0, sizeof(m_aUniformControllingBitsActive));
		memset(m_pszUniformModelOverrides, 0, sizeof(m_pszUniformModelOverrides));
		for (int i = 0; i < NUM_POSSIBLE_UNIFORMS; i++) {
			m_aUniformSleeveOverrides[i] = m_aUniformArmModelOverrides[i] = -1;
		}
}

uint8 CPlayerClass::numChooseableUniformsForPlayer(CBasePlayer* pPlayer) const {
	uint8 result;
#ifdef CLIENT_DLL
	if (m_bLastUniformRestricted && !NVersioning::HadDoneBetaTestParticipation())
		result = m_iNumUniforms - 1;
	else
#endif
		result = m_iNumUniforms;
	return result;
}

const CWeaponDef* CPlayerClass::getWeaponDef(byte iWeapon) const {
	const char* name = m_aWeapons[iWeapon].m_pszWeaponPrimaryName;
	return CWeaponDef::GetDefForWeapon(name);
}

void CPlayerClass::getWeaponDef(byte iKit, const CWeaponDef** ppPrimary, const CWeaponDef** ppSecondary, const CWeaponDef** ppTertiary) const {
	//we always have a primary
	*ppPrimary = CWeaponDef::GetDefForWeapon(m_aWeapons[iKit].m_pszWeaponPrimaryName);
	const char* pszSecond = m_aWeapons[iKit].m_pszWeaponSecondaryName;
	const char* pszThird = m_aWeapons[iKit].m_pszWeaponTertiaryName;
	if (pszSecond) {
		*ppSecondary = CWeaponDef::GetDefForWeapon(pszSecond);
		if (pszThird) {
			*ppTertiary = CWeaponDef::GetDefForWeapon(pszThird);
		}
		else {
			*ppTertiary = nullptr;
		}
	}
	else {
		*ppSecondary = nullptr;
		*ppTertiary = nullptr;
	}
}

const CGunKit* CPlayerClass::getWeaponKitChooseable(byte iWeapon) const {
	//this function used to decipher between always-give and chooseable weapons,
	//but we've gotten rid of that in favor of the unrolled list of 3 weapons per kit
	//more flexible
	const CGunKit* pResult = &m_aWeapons[iWeapon];
	return pResult;
}

//BG3 - moved this to NClassQuota
/*EClassAvailability CPlayerClass::availabilityForPlayer(const CBasePlayer* pPlayer) const {
	int limit = NClassQuota::GetLimitForClass(this);

	if (limit == 0)
		return CLASS_UNAVAILABLE;
	if (limit == -1)
		return CLASS_FREE; //not necessary, but may improve performance
	
	//how many players are of our class?
	//int count = g_Teams[m_iDefaultTeam]->GetNumOfClass(m_iClassNumber);
	int count = m_pPopCounter->m_iTotalPopulation - m_pPopCounter->m_iBotPopulation; //don't include bots in class limits - bots handle those on their own

	//eh
	if (pPlayer) {
		const CHL2MP_Player* pPlayer2 = static_cast<const CHL2MP_Player*>(pPlayer);

		if (pPlayer2->GetPlayerClass() == this) {
			return count >= limit ? CLASS_TAKEN_FULL : CLASS_TAKEN_FREE;
		}
	}

	return count >= limit ? CLASS_FULL : CLASS_FREE;
}*/

#ifdef CLIENT_DLL
bool CPlayerClass::shouldHaveWeaponSelectionMenu() const {
	bool result = false;

	if (m_iNumUniforms > 1) {
		result = true;
	} else if (numChooseableWeapons() >= 2) {
			result = true;
	}
	return result;
}
#endif

const CPlayerClass* g_ppClasses[TOTAL_NUM_CLASSES];

#define NUM_BRIT_PLAYERMODELS 6
#define NUM_AMER_PLAYERMODELS 6

const char* g_ppszBritishPlayerModels[NUM_BRIT_PLAYERMODELS];
const char* g_ppszAmericanPlayerModels[NUM_AMER_PLAYERMODELS];

void CPlayerClass::postClassConstruct(CPlayerClass* pClass) {

	pClass->m_flBaseSpeedCalculated = pClass->m_flBaseSpeedOriginal;

	//count number of chooseable weapons
	const CGunKit* pKit = pClass->m_aWeapons;
	int chooseable = 0;
	for (int i = 0; i < NUM_POSSIBLE_WEAPON_KITS; i++) {
		if (pKit[i].m_pszWeaponPrimaryName)
			chooseable++;
	}
	pClass->m_iChooseableKits = chooseable;

	int index = pClass->m_iClassNumber;

	//this will make it so that british classes come first in the list
	if (pClass->m_iDefaultTeam == TEAM_AMERICANS)
		index += TOTAL_BRIT_CLASSES;

	g_ppClasses[index] = pClass;

	//why not make it one-to-one?
	const char** pModelList;
	if (pClass->m_iDefaultTeam == TEAM_AMERICANS)
		pModelList = g_ppszAmericanPlayerModels;
	else
		pModelList = g_ppszBritishPlayerModels;
	pModelList[pClass->m_iClassNumber] = pClass->m_pszPlayerModel;

	//once all classes are constructed
	/*if (index == numClasses() - 1) {
		
	}*/
}

#ifdef CLIENT_DLL
/**********************************************************
* Purpose: These client-only functions help handle data 
* used by class menu, nothing more.
**********************************************************/
void CPlayerClass::SetDefaultWeapon(byte iWeapon) const {
	int weapon = Clamp((int)iWeapon, 0, numChooseableWeapons() - 1);

	if (weapon < 0) weapon = 0; //the clamp sometimes makes it -1

	//clamp weapon kit value
	m_pcvWeapon->SetValue(weapon);

	//ensure ammo is valid
	if (!m_aWeapons[weapon].m_bAllowBuckshot)
		m_pcvAmmo->SetValue(0);
}

void CPlayerClass::SetDefaultAmmo(byte iAmmo) const {
	int ammo = Clamp((int)iAmmo, AMMO_KIT_BALL, AMMO_KIT_BUCKSHOT);
	m_pcvAmmo->SetValue(ammo);
}

void CPlayerClass::SetDefaultUniform(byte iUniform) const {
	int uniform = Clamp((int)iUniform, 0, m_iNumUniforms - 1);
	m_pcvUniform->SetValue(uniform);
}

namespace { bool g_bHasInit = false; }

void CPlayerClass::InitClientRunTimeData() {
	if (!g_bHasInit) {
		g_bHasInit = true;
		InitPrevKitData();
		Localize();
	}
}

//Finds this class' ConVars used for last-used-kit tracking and remembers them for later
void CPlayerClass::InitPrevKitData() {
	const int BUFFER_SIZE = 32;
	char buff[BUFFER_SIZE];
	const CPlayerClass* pClass;
	for (int i = 0; i < TOTAL_NUM_CLASSES; i++) {
		pClass = g_ppClasses[i];
		char cTeam = charForTeam(pClass->m_iDefaultTeam);

		//get weapon
		V_snprintf(buff, BUFFER_SIZE, "cl_%c%s_weapon\0", cTeam, pClass->m_pszAbbreviation);
		{
			ConVarRef cvarRef(buff);
			pClass->m_pcvWeapon = static_cast<ConVar*>(cvarRef.GetLinkedConVar());
		}
		//get ammo
		V_snprintf(buff, BUFFER_SIZE, "cl_%c%s_ammo\0", cTeam, pClass->m_pszAbbreviation);
		{
			ConVarRef cvarRef(buff);
			pClass->m_pcvAmmo = static_cast<ConVar*>(cvarRef.GetLinkedConVar());
		}

		//get uniform
		V_snprintf(buff, BUFFER_SIZE, "cl_%c%s_uniform\0", cTeam, pClass->m_pszAbbreviation);
		{
			ConVarRef cvarRef(buff);
			pClass->m_pcvUniform = static_cast<ConVar*>(cvarRef.GetLinkedConVar());
		}
	}
}

void CPlayerClass::Localize() {
	char buffer[80];
	for (int i = 0; i < TOTAL_NUM_CLASSES; i++) {
		const CPlayerClass& pc = *(g_ppClasses[i]);
		char team = charForTeam(pc.m_iDefaultTeam);

		Q_snprintf(buffer, sizeof(buffer), "#BG3_%c%s_desc", team, pc.m_pszAbbreviation);
		pc.m_pLocalizedDesc = g_pVGuiLocalize->Find(buffer);

		Q_snprintf(buffer, sizeof(buffer), "#BG3_%c%s_name", team, pc.m_pszAbbreviation);
		pc.m_pLocalizedName = g_pVGuiLocalize->Find(buffer);

		//localize weapon kit infos
		for (int k = 0; k < NUM_POSSIBLE_WEAPON_KITS; k++) {
			const CGunKit& gk = pc.m_aWeapons[k];

			//we're only doing this for chooseable weapons
			if (!gk.m_pszWeaponPrimaryName)
				break;

			const char* name = gk.m_pszWeaponPrimaryName;
			if (gk.m_pszLocalizedDescOverride)
				name = gk.m_pszLocalizedNameOverride;

			const char* desc = gk.m_pszWeaponPrimaryName;
			if (gk.m_pszLocalizedNameOverride)
				desc = gk.m_pszLocalizedNameOverride;

			Q_snprintf(buffer, sizeof(buffer), "#%s", name);
			gk.m_pLocalizedName = g_pVGuiLocalize->Find(buffer);

			Q_snprintf(buffer, sizeof(buffer), "#i_%s", desc);
			gk.m_pLocalizedDesc = g_pVGuiLocalize->Find(buffer);
		}
	}
}
#endif

/**********************************************************
* Purpose: Translates team number and class number into
*	a reference to the class data.
**********************************************************/
const CPlayerClass* CPlayerClass::fromNums(int iTeam, int iClass) {
	return g_ppClasses[iTeam == TEAM_AMERICANS ? iClass + TOTAL_BRIT_CLASSES : iClass];
}

const CPlayerClass* CPlayerClass::fromAbbreviation(int iTeam, const char* pszAbbreviation) {
	const CPlayerClass* pResult = NULL;
	int numClasses = numClassesForTeam(iTeam);
	for (int i = 0; i < numClasses; i++) {
		if (Q_strcmp(pszAbbreviation, fromNums(iTeam, i)->m_pszAbbreviation) == 0) {
			pResult = fromNums(iTeam, i);
			break;
		}
	}
	return pResult;
}

ConVar mp_limit_mapsize_low("mp_limit_mapsize_low", "16", CVAR_FLAGS, "Servers with player counts <= this number are small, above it are medium or large");
ConVar mp_limit_mapsize_high("mp_limit_mapsize_high", "32", CVAR_FLAGS, "Servers with player counts <= this number are small or medium, above it are large");


/*
Given a player class determines the maximum number of players who can take that class.
*/
#define CVAR_SEARCH_BUFFER_SIZE 30
void CPlayerClass::InitLimits() const {
	//find the cvars
	char cvarSearchBuffer[CVAR_SEARCH_BUFFER_SIZE];
	char cTeam = charForTeam(m_iDefaultTeam);
	#define find(size) \
		V_snprintf(cvarSearchBuffer, CVAR_SEARCH_BUFFER_SIZE, "mp_limit_%s_%c_"#size"\0", m_pszAbbreviation, cTeam); \
		{ \
			ConVarRef cvarRef(cvarSearchBuffer); \
			m_pcvLimit_##size = static_cast<ConVar*>(cvarRef.GetLinkedConVar()); \
		}

	find(sml)
	find(med)
	find(lrg)
	#undef find
}


int CPlayerClass::numClasses() {
	return TOTAL_NUM_CLASSES;
}

int CPlayerClass::numModelsForTeam(int iTeam) {
	return iTeam == TEAM_AMERICANS ? NUM_AMER_PLAYERMODELS : NUM_BRIT_PLAYERMODELS;
}

const CPlayerClass* const * CPlayerClass::asList() {
	return g_ppClasses;
}

void CPlayerClass::RemoveClassLimits() {
	for (int i = 0; i < numClasses(); i++) {
		g_ppClasses[i]->m_pcvLimit_lrg->SetValue(-1);
		g_ppClasses[i]->m_pcvLimit_med->SetValue(-1);
		g_ppClasses[i]->m_pcvLimit_sml->SetValue(-1);
	}
}

#ifdef CLIENT_DLL
CON_COMMAND_F(update_playerclass_speeds, "", FCVAR_HIDDEN) {
	CPlayerClass::UpdateClassSpeeds();
}
#endif
void CPlayerClass::UpdateClassSpeeds() {

	extern ConVar mp_bg2_speed;
	for (int i = 0; i < numClasses(); i++) {
		if (mp_bg2_speed.GetBool()) {
			g_ppClasses[i]->m_flBaseSpeedCalculated = g_ppClasses[i]->m_flBaseSpeedOriginal;
			//Warning("%f\n", g_ppClasses[i]->m_flBaseSpeedCalculated);
		}
		else {
			g_ppClasses[i]->m_flBaseSpeedCalculated = g_ppClasses[i]->m_flBaseSpeedOriginal * 1.1f;
			//Warning("%f\n", g_ppClasses[i]->m_flBaseSpeedCalculated);
		}
	}
}



#define SLEEVE_BINFANTRY		0
#define SLEEVE_BOFFICER			0
#define SLEEVE_BJAEGER			4
#define SLEEVE_BNATIVE			0
#define SLEEVE_BLINF			6		
#define SLEEVE_BGRENADIER		9

#define SLEEVE_AINFANTRY		16
#define SLEEVE_AOFFICER			17
#define SLEEVE_AFRONTIERSMAN	20
#define SLEEVE_AMILITIA			23
#define SLEEVE_ASTATEMILITIA	26
#define SLEEVE_AFRENCH			27



//Use this for declaring a class. Note that "name" must match those used
//in the macros which declared the global pointers to these classes
//Client's defines are identical except they have additional ConVars used by class menu
#ifndef CLIENT_DLL
#define DEC_BG3_PLAYER_CLASS(name, abbrev, teamChar) \
	class name : public CPlayerClass { public: name(); }; \
	name g_##name; \
	ConVar mp_limit_##abbrev##_##teamChar##_sml("mp_limit_"#abbrev"_"#teamChar"_sml", "-1", CVAR_FLAGS, "Maximum number of this class on small maps"); \
	ConVar mp_limit_##abbrev##_##teamChar##_med("mp_limit_"#abbrev"_"#teamChar"_med", "-1", CVAR_FLAGS, "Maximum number of this class on medium-sized maps"); \
	ConVar mp_limit_##abbrev##_##teamChar##_lrg("mp_limit_"#abbrev"_"#teamChar"_lrg", "-1", CVAR_FLAGS, "Maximum number of this class on large maps"); \
	namespace PlayerClasses { const CPlayerClass* g_p##name = &g_##name; } \
	name::name() : CPlayerClass(#abbrev)
#else
#define DEC_BG3_PLAYER_CLASS(name, abbrev, teamChar) \
	class name : public CPlayerClass { public: name(); }; \
	name g_##name; \
	ConVar mp_limit_##abbrev##_##teamChar##_sml("mp_limit_"#abbrev"_"#teamChar"_sml", "-1", CVAR_FLAGS, "Maximum number of this class on small maps"); \
	ConVar mp_limit_##abbrev##_##teamChar##_med("mp_limit_"#abbrev"_"#teamChar"_med", "-1", CVAR_FLAGS, "Maximum number of this class on medium-sized maps"); \
	ConVar mp_limit_##abbrev##_##teamChar##_lrg("mp_limit_"#abbrev"_"#teamChar"_lrg", "-1", CVAR_FLAGS, "Maximum number of this class on large maps"); \
	ConVar cl_##teamChar##abbrev##_weapon("cl_"#teamChar #abbrev"_weapon", "1", FCVAR_ARCHIVE, "Default weapon for "#name" class"); \
	ConVar cl_##teamChar##abbrev##_ammo("cl_"#teamChar #abbrev"_ammo", "1", FCVAR_ARCHIVE, "Default ammo for "#name" class"); \
	ConVar cl_##teamChar##abbrev##_uniform("cl_"#teamChar #abbrev"_uniform", "1", FCVAR_ARCHIVE, "Default uniform for "#name" class"); \
	namespace PlayerClasses { const CPlayerClass* g_p##name = &g_##name; } \
	name::name() : CPlayerClass(#abbrev)
#endif

DEC_BG3_PLAYER_CLASS(BInfantry, inf, b) {
	m_iDefaultTeam = TEAM_BRITISH;
	m_iClassNumber = CLASS_INFANTRY;

	m_flBaseSpeedOriginal = SPEED_INFANTRY;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_INFANTRY;

	m_pszPlayerModel = MODEL_BINFANTRY;
	m_pszJoinName = "Royal Infantry";

	m_iDefaultPrimaryAmmoCount = AMMO_INFANTRY;

	m_iSkinDepth = 8;
	m_iSleeveBase = SLEEVE_BINFANTRY;
	m_iNumUniforms = 3;
	//m_bLastUniformRestricted = true;

	m_pszDroppedHat = "models/player/british/infantry/british_hat.mdl";

	//m_bAllowUniformSelection = true;

	m_aWeapons[0].m_pszWeaponPrimaryName = "weapon_brownbess";
	m_aWeapons[1].m_pszWeaponPrimaryName = "weapon_longpattern";
	m_aWeapons[2].m_pszWeaponPrimaryName = "weapon_sea_service";
	m_aWeapons[2].m_iMovementSpeedModifier = 3;
	m_aWeapons[3].m_pszWeaponPrimaryName = "weapon_serjeant_fusil";
	m_aWeapons[3].m_pszWeaponSecondaryName = "weapon_smallsword";
	//m_aWeapons[3].m_iMovementSpeedModifier = -5;
	m_aWeapons[4].m_pszWeaponPrimaryName = "weapon_oldpattern";
	m_aWeapons[4].m_iControllingBit = ULK_3_WEP_OLD_PATTERN;
	m_aWeapons[5].m_pszWeaponPrimaryName = "weapon_artillery_common_carbine";
	m_aWeapons[5].m_iMovementSpeedModifier = 3;
	

	postClassConstruct(this);
}

DEC_BG3_PLAYER_CLASS(BOfficer, off, b) {
	m_iDefaultTeam = TEAM_BRITISH;
	m_iClassNumber = CLASS_OFFICER;

	m_flBaseSpeedOriginal = SPEED_OFFICER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_OFFICER;

	m_pszPlayerModel = MODEL_BOFFICER;
	m_pszJoinName = "a Royal Commander";

	m_iDefaultPrimaryAmmoCount = AMMO_OFFICER;

	m_iSkinDepth = 1;
	m_iSleeveBase = SLEEVE_BOFFICER;
	m_iNumUniforms = 3;
	m_iSleeveInnerModel = SLEEVE_INNER_LACE;

	//m_bAllowUniformSelection = true;
	m_bCanDoVcommBuffs = true;

	m_aWeapons[0].m_pszWeaponPrimaryName = "weapon_pistol_b";
	m_aWeapons[0].m_pszWeaponSecondaryName = "weapon_sabre";
	m_aWeapons[1].m_pszWeaponPrimaryName = "weapon_brownbess_carbine_nobayo";
	m_aWeapons[1].m_pszWeaponSecondaryName = "weapon_sabre";
	m_aWeapons[2].m_pszWeaponPrimaryName = "weapon_spontoon";
	m_aWeapons[2].m_iMovementSpeedModifier = -5;
	m_aWeapons[3].m_pszWeaponPrimaryName = "weapon_serjeant_fusil";
	m_aWeapons[3].m_pszWeaponSecondaryName = "weapon_smallsword";
	m_aWeapons[3].m_iMovementSpeedModifier = -10;

	m_aWeapons[4].m_pszWeaponPrimaryName = "weapon_serjeant_fusil";
	m_aWeapons[4].m_pszWeaponSecondaryName = "weapon_rapier";
	m_aWeapons[4].m_iControllingBit = ULK_3_WEP_FRE_CARB;
	m_aWeapons[4].m_iMinYear = 1778;

#ifdef CLIENT_DLL
	m_aWeapons[4].SetLocalizedName("weapon_rapier_brit");
	m_aWeapons[4].SetLocalizedDesc("i_weapon_rapier_brit");
#endif

	postClassConstruct(this);
}

DEC_BG3_PLAYER_CLASS(BJaeger, rif, b) {
	m_iDefaultTeam = TEAM_BRITISH;
	m_iClassNumber = CLASS_SNIPER;

	m_flBaseSpeedOriginal = SPEED_SNIPER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_SNIPER;

	m_pszPlayerModel = MODEL_BJAEGER;
	m_pszJoinName = "a Jaeger";

	m_iDefaultPrimaryAmmoCount = AMMO_SNIPER;

	m_iSkinDepth = 2;
	m_iSleeveBase = SLEEVE_BJAEGER;
	m_iNumUniforms = 1;

	m_pszUniformModelOverrides[0] = NULL;
	m_pszUniformModelOverrides[1] = NULL;
	m_pszUniformModelOverrides[2] = NULL;
	m_pszUniformModelOverrides[3] = NULL;

	m_aWeapons[0].m_pszWeaponPrimaryName = "weapon_jaeger";
	m_aWeapons[0].m_pszWeaponSecondaryName = "weapon_hirschf";

	postClassConstruct(this);
}

DEC_BG3_PLAYER_CLASS(BNative, ski, b) {
	m_iDefaultTeam = TEAM_BRITISH;
	m_iClassNumber = CLASS_SKIRMISHER;
	m_bHasImplicitDamageWeakness = true;

	m_flBaseSpeedOriginal = SPEED_SKIRMISHER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_SKIRMISHER;
	
	m_pszPlayerModel = MODEL_BNATIVE;
	m_pszJoinName = "a Native";

	m_iDefaultPrimaryAmmoCount = AMMO_SKIRMISHER;

	m_iSkinDepth = 2;
	m_iSleeveBase = SLEEVE_BNATIVE;
	m_iNumUniforms = 1;
	m_iArmModel = ARMS_NATIVE;

	m_aWeapons[0].m_pszWeaponPrimaryName = "weapon_brownbess_nobayo";
	m_aWeapons[0].m_pszWeaponSecondaryName = "weapon_tomahawk";
	m_aWeapons[0].m_iMinYear = 1776;

	m_aWeapons[1].m_pszWeaponPrimaryName = "weapon_longpattern_nobayo";
	m_aWeapons[1].m_pszWeaponSecondaryName = "weapon_tomahawk";

	m_aWeapons[2].m_pszWeaponPrimaryName = "weapon_club";
	m_aWeapons[2].m_iMovementSpeedModifier = 5;

	m_aWeapons[3].m_pszWeaponPrimaryName = "weapon_trade_musket";
	m_aWeapons[3].m_pszWeaponSecondaryName = "weapon_dagger";
	//m_aWeapons[3].m_iMovementSpeedModifier = -5;
	m_aWeapons[3].m_iControllingBit = ULK_3_WEP_TRADE;

	m_aWeapons[4].m_pszWeaponPrimaryName = "weapon_gunstock";
	m_aWeapons[4].m_iMovementSpeedModifier = -20;
	//m_aWeapons[4].m_iMinYear = 2000;


	postClassConstruct(this);
}

DEC_BG3_PLAYER_CLASS(BLinf, linf, b) {
	m_iDefaultTeam = TEAM_BRITISH;
	m_iClassNumber = CLASS_LIGHT_INFANTRY;

	m_flBaseSpeedOriginal = SPEED_LIGHT_INF;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_LIGHT_INFANTRY;
	m_bNerfResistance = true;

	m_pszPlayerModel = MODEL_BLINF;
	m_pszJoinName = "Light Infantry";

	m_iDefaultPrimaryAmmoCount = AMMO_LIGHT_INFANTRY;

	m_iSkinDepth = 1;
	m_iSleeveBase = SLEEVE_BLINF;
	m_iNumUniforms = 3;
	m_iSleeveInnerModel = SLEEVE_INNER_PLAIN;

	m_aUniformControllingBits[1] = ULK_1_UNI_BRIT_LINF_1;
	m_aUniformControllingBits[2] = ULK_1_UNI_BRIT_LINF_2; 
	//HACK HACK for all other unlockable bits, we can just use the bits themselves,
	//but bit offset for the first light infantry is 0 , so we have to use something
	//else for whether or not the uniform should be locked
	m_aUniformControllingBitsActive[1] = true;
	m_pszUniformModelOverrides[2] = MODEL_BNEWFOUNDLAND_INFANTRY;

	m_aWeapons[0].m_pszWeaponPrimaryName = "weapon_brownbess_carbine";
	m_aWeapons[0].m_bAllowBuckshot = true;

	m_aWeapons[1].m_pszWeaponPrimaryName = "weapon_pattern1776";
	m_aWeapons[1].m_pszWeaponSecondaryName = "weapon_smallsword";
	m_aWeapons[1].m_iMovementSpeedModifier = 5;
	m_aWeapons[1].m_iMinYear = 1776;

	m_aWeapons[2].m_pszWeaponPrimaryName = "weapon_artillery_common_carbine";
	m_aWeapons[2].m_iMovementSpeedModifier = -3;

	postClassConstruct(this);
}

DEC_BG3_PLAYER_CLASS(BGrenadier, gre, b) {
	m_iDefaultTeam = TEAM_BRITISH;
	m_iClassNumber = CLASS_GRENADIER;
	m_bHasImplicitDamageResistance = true;

	m_flBaseSpeedOriginal = SPEED_GRENADIER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_GRENADIER;

	m_pszPlayerModel = MODEL_BGRENADIER;
	m_pszJoinName = "a Grenadier";

	m_iDefaultPrimaryAmmoCount = AMMO_GRENADIER;

	m_pszSecondaryAmmo = "Grenade";
	m_iDefaultSecondaryAmmoCount = AMMO_GRENADE;

	m_iSkinDepth = 8;
	m_iSleeveBase = SLEEVE_BGRENADIER;
	m_iNumUniforms = 2;
	m_iArmModel = ARMS_BRIT_GRE;
	m_iSleeveInnerModel = SLEEVE_INNER_PLAIN;

	m_pszUniformModelOverrides[1] = MODEL_BGRENADIER_ALT;
	m_aUniformControllingBits[1] = ULK_1_UNI_BRIT_GRE;

	m_aWeapons[0].m_pszWeaponPrimaryName = "weapon_brownbess_nobayo";
	m_aWeapons[0].m_pszWeaponSecondaryName = "weapon_frag";
	m_aWeapons[0].m_pszWeaponTertiaryName = "weapon_hanger";

	m_aWeapons[1].m_pszWeaponPrimaryName = "weapon_longpattern_nobayo";
	m_aWeapons[1].m_pszWeaponSecondaryName = "weapon_frag";
	m_aWeapons[1].m_pszWeaponTertiaryName = "weapon_hanger";

#ifdef CLIENT_DLL
	m_aWeapons[0].SetLocalizedName("weapon_brownbess_gre");
	m_aWeapons[0].SetLocalizedDesc("i_weapon_brownbess_gre");

	m_aWeapons[1].SetLocalizedName("weapon_longpattern_gre");
	m_aWeapons[1].SetLocalizedDesc("i_weapon_longpattern_gre");
#endif


	/*m_aWeapons[1].m_pszWeaponPrimaryName = "weapon_frag";
	m_aWeapons[1].m_pszWeaponSecondaryName = "weapon_hanger";
	m_aWeapons[1].m_pszAmmoOverrideName = "Grenade";
	m_aWeapons[1].m_iAmmoOverrideCount = 3;*/

	postClassConstruct(this);
}

DEC_BG3_PLAYER_CLASS(AInfantry, inf, a) {
	m_iDefaultTeam = TEAM_AMERICANS;
	m_iClassNumber = CLASS_INFANTRY;

	m_flBaseSpeedOriginal = SPEED_INFANTRY;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_INFANTRY;

	m_pszPlayerModel = MODEL_AINFANTRY;
	m_pszJoinName = "a Continental Soldier";

	m_iDefaultPrimaryAmmoCount = AMMO_INFANTRY;

	m_iSkinDepth = 8;
	m_iSleeveBase = SLEEVE_AINFANTRY;
	m_iNumUniforms = 4;
	m_pszDroppedHat = "models/player/american/infantry/american_hat.mdl";
	m_bLastUniformRestricted = true;

	//m_bAllowUniformSelection = true;

	m_aWeapons[0].m_pszWeaponPrimaryName = "weapon_miquelet";
	m_aWeapons[0].m_iMinYear = 1777;
	m_aWeapons[1].m_pszWeaponPrimaryName = "weapon_longpattern";
	m_aWeapons[2].m_pszWeaponPrimaryName = "weapon_charleville";
	m_aWeapons[2].m_iMinYear = 1777;
	m_aWeapons[3].m_pszWeaponPrimaryName = "weapon_old_model_charleville";
	m_aWeapons[3].m_iMinYear = 1775;
	m_aWeapons[4].m_pszWeaponPrimaryName = "weapon_dutch";
	m_aWeapons[4].m_iMinYear = 1778;
	m_aWeapons[5].m_pszWeaponPrimaryName = "weapon_american_brownbess";
	m_aWeapons[6].m_pszWeaponPrimaryName = "weapon_light_model_charleville";
	m_aWeapons[6].m_iMinYear = 1779;
	m_aWeapons[6].m_iControllingBit = ULK_3_WEP_LMC;
	m_aWeapons[7].m_pszWeaponPrimaryName = "weapon_spontoon";
	m_aWeapons[7].m_iMaxYear = 1775;
	m_aWeapons[7].m_iMovementSpeedModifier = 5;
	m_aWeapons[8].m_pszWeaponPrimaryName = "weapon_oldpattern";
	m_aWeapons[8].m_iMaxYear = 1777;
	m_aWeapons[8].m_iControllingBit = ULK_3_WEP_OLD_PATTERN;
	m_aWeapons[7].m_bDisabledOnHistoricityDisabled = m_aWeapons[8].m_bDisabledOnHistoricityDisabled = true;

	postClassConstruct(this);
}

DEC_BG3_PLAYER_CLASS(AOfficer, off, a) {
	m_iDefaultTeam = TEAM_AMERICANS;
	m_iClassNumber = CLASS_OFFICER;

	m_flBaseSpeedOriginal = SPEED_OFFICER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_OFFICER;

	m_pszPlayerModel = MODEL_AOFFICER;
	m_pszJoinName = "a Continental Officer";

	m_iDefaultPrimaryAmmoCount = AMMO_OFFICER;

	m_iSkinDepth = 1;
	m_iSleeveBase = SLEEVE_AOFFICER;
	m_iSleeveInnerModel = SLEEVE_INNER_LACE;
	m_iNumUniforms = 4;
	m_bCanDoVcommBuffs = true;

	//french officer variation
	m_aUniformSleeveOverrides[3] = SLEEVE_AFRENCH + 2;
	m_aUniformArmModelOverrides[3] = ARMS_FRE;
	m_pszUniformModelOverrides[3] = MODEL_AFRENCHOFFICER;
	m_aUniformControllingBits[3] = ULK_1_UNI_AMER_OFF;


	m_aWeapons[0].m_pszWeaponPrimaryName = "weapon_pistol_a";
	m_aWeapons[0].m_pszWeaponSecondaryName = "weapon_sabre";
	
	m_aWeapons[1].m_pszWeaponPrimaryName = "weapon_french_carbine";
	m_aWeapons[1].m_pszWeaponSecondaryName = "weapon_rapier";
	m_aWeapons[1].m_iControllingBit = ULK_3_WEP_FRE_CARB;
	m_aWeapons[1].m_iMinYear = 1778;

	m_aWeapons[2].m_pszWeaponPrimaryName = "weapon_brownbess_carbine_nobayo";
	m_aWeapons[2].m_pszWeaponSecondaryName = "weapon_sabre";

	m_aWeapons[3].m_pszWeaponPrimaryName = "weapon_spontoon";
	m_aWeapons[3].m_iMovementSpeedModifier = -5;

	postClassConstruct(this);
}

DEC_BG3_PLAYER_CLASS(AFrontiersman, rif, a) {
	m_iDefaultTeam = TEAM_AMERICANS;
	m_iClassNumber = CLASS_SNIPER;
	m_bHasImplicitDamageResistance = true;

	m_flBaseSpeedOriginal = SPEED_SNIPER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_SNIPER;

	m_pszPlayerModel = MODEL_AFRONTIERSMAN;
	m_pszJoinName = "a Frontiersman";

	m_iDefaultPrimaryAmmoCount = AMMO_SNIPER;

	m_iSkinDepth = 8;
	m_iSleeveBase = SLEEVE_AFRONTIERSMAN;
	m_iNumUniforms = 3;
	m_bDefaultRandomUniform = true;
	m_iDerandomizerControllingBit = ULK_2_UNC_AMER_RIF;

	m_aWeapons[0].m_pszWeaponPrimaryName = "weapon_pennsylvania";
	m_aWeapons[0].m_pszWeaponSecondaryName = "weapon_knife";

	postClassConstruct(this);
}

DEC_BG3_PLAYER_CLASS(AMilitia, ski, a) {
	m_iDefaultTeam = TEAM_AMERICANS;
	m_iClassNumber = CLASS_SKIRMISHER;
	m_bHasImplicitDamageWeakness = true;
	m_bNerfResistance = true;

	m_flBaseSpeedOriginal = SPEED_SKIRMISHER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_SKIRMISHER;

	m_pszPlayerModel = MODEL_AMILITIA;
	m_pszJoinName = "Militia";

	m_iDefaultPrimaryAmmoCount = AMMO_SKIRMISHER;

	m_iSkinDepth = 8;
	m_iSleeveBase = SLEEVE_AMILITIA;
	m_iNumUniforms = 3;
	m_bDefaultRandomUniform = true;
	m_iDerandomizerControllingBit = ULK_2_UNC_AMER_SKI;

	m_aWeapons[0].m_pszWeaponPrimaryName = "weapon_fowler";
	m_aWeapons[0].m_pszWeaponSecondaryName = "weapon_beltaxe";
	m_aWeapons[0].m_bAllowBuckshot = true;

	m_aWeapons[1].m_pszWeaponPrimaryName = "weapon_american_brownbess_nobayo";
	m_aWeapons[1].m_pszWeaponSecondaryName = "weapon_beltaxe";

	m_aWeapons[2].m_pszWeaponPrimaryName = "weapon_longpattern_nobayo";
	m_aWeapons[2].m_pszWeaponSecondaryName = "weapon_smallsword";
	m_aWeapons[2].m_pszPlayerModelOverrideName = MODEL_ASTATEMILITIA; //BG3 - minuteman/state class merge
	m_aWeapons[2].m_iSleeveSkinOverride = SLEEVE_ASTATEMILITIA;

	m_aWeapons[3].m_pszWeaponPrimaryName = "weapon_blunderbuss";
	m_aWeapons[3].m_pszWeaponSecondaryName = "weapon_dagger";
	m_aWeapons[3].m_iControllingBit = ULK_3_WEP_BLUNDER;

#ifdef CLIENT_DLL
	m_aWeapons[2].SetLocalizedName("weapon_longpattern_state");
	m_aWeapons[2].SetLocalizedDesc("i_weapon_longpattern_state");
#endif 

	postClassConstruct(this);
}

/*DEC_BG3_PLAYER_CLASS(AStateMilitia, linf, a) {
	m_iDefaultTeam = TEAM_AMERICANS;
	m_iClassNumber = CLASS_LIGHT_INFANTRY;

	m_flBaseSpeedOriginal = SPEED_LIGHT_INF;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_LIGHT_INFANTRY;

	m_pszPlayerModel = MODEL_ASTATEMILITIA;
	m_pszJoinName = "Light Infantry";

	m_iDefaultPrimaryAmmoCount = AMMO_LIGHT_INFANTRY;

	m_iSkinDepth = 8;
	m_iSleeveBase = SLEEVE_ASTATEMILITIA;
	m_iNumUniforms = 1;

	m_aWeapons[0].m_pszWeaponPrimaryName = "weapon_longpattern_nobayo";
	m_aWeapons[0].m_pszWeaponSecondaryName = "weapon_smallsword";
	m_aWeapons[1].m_pszWeaponPrimaryName = "weapon_brownbess_carbine";
	m_aWeapons[1].m_bAllowBuckshot = true;

#ifdef CLIENT_DLL
	m_aWeapons[0].SetLocalizedName("weapon_longpattern_state");
	m_aWeapons[0].SetLocalizedDesc("i_weapon_longpattern_state");
#endif 

	postClassConstruct(this);
}*/

DEC_BG3_PLAYER_CLASS(AFrenchGre, gre, a) {
	m_iDefaultTeam = TEAM_AMERICANS;
	m_iClassNumber = CLASS_LIGHT_INFANTRY;
	m_bHasImplicitDamageResistance = true;

	m_flBaseSpeedOriginal = SPEED_GRENADIER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_GRENADIER;

	m_pszPlayerModel = MODEL_AFRENCH;
	m_pszJoinName = "a Grenadier";

	m_iDefaultPrimaryAmmoCount = AMMO_GRENADIER;

	m_pszSecondaryAmmo = "Grenade";
	m_iDefaultSecondaryAmmoCount = AMMO_GRENADE;

	m_iSkinDepth = 4;
	m_iSleeveBase = SLEEVE_AFRENCH;
	m_iNumUniforms = 2;
	m_iArmModel = ARMS_FRE;

	m_pszUniformModelOverrides[1] = MODEL_AFRENCH_ALT;
	m_aUniformControllingBits[1] = ULK_1_UNI_AMER_GRE;

	m_aWeapons[0].m_pszWeaponPrimaryName = "weapon_revolutionnaire_nobayo";
	m_aWeapons[0].m_pszWeaponSecondaryName = "weapon_frag";
	m_aWeapons[0].m_pszWeaponTertiaryName = "weapon_hanger";

	/*m_aWeapons[1].m_pszWeaponPrimaryName = "weapon_frag";
	m_aWeapons[1].m_pszWeaponSecondaryName = "weapon_hanger";
	m_aWeapons[1].m_pszAmmoOverrideName = "Grenade";
	m_aWeapons[1].m_iAmmoOverrideCount = 3;*/

	postClassConstruct(this);
}