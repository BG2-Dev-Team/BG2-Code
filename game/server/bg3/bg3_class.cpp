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
#include "bg3_class.h"
#include "hl2mp_gamerules.h"

//BG2 - Tjoppen - beautiful defines. you will see another one further down
#ifdef CLIENT_DLL
#define CVAR_FLAGS	(FCVAR_REPLICATED | FCVAR_NOTIFY)
#define CVAR_FLAGS_HIDDEN (FCVAR_REPLICATED)
#else
#define CVAR_FLAGS	(FCVAR_GAMEDLL | FCVAR_REPLICATED | FCVAR_NOTIFY)
#define CVAR_FLAGS_HIDDEN (FCVAR_GAMEDLL | FCVAR_REPLICATED)
#endif

int CPlayerClass::numChooseableWeapons() const {
	int numChooseableWeapons = 0;
	for (int i = 0; i < ARRAYSIZE(m_aWeapons); i++) {
		if (m_aWeapons[i].m_pszWeaponName && !m_aWeapons[i].m_bAlwaysGive)
			numChooseableWeapons++;
	}
	return numChooseableWeapons;
}

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

#define TOTAL_NUM_CLASSES 12 //used to build the list of classes and model lists
#define TOTAL_BRIT_CLASSES 6 //same as above
#define TOTAL_AMER_CLASSES 6 //^

const CPlayerClass* g_ppClasses[TOTAL_NUM_CLASSES];
const char* g_ppszBritishPlayerModels[TOTAL_BRIT_CLASSES];
const char* g_ppszAmericanPlayerModels[TOTAL_AMER_CLASSES];

void addSelfToGlobalLists(const CPlayerClass* pClass) {
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
}

const CPlayerClass* CPlayerClass::fromNums(int iTeam, int iClass) {
	return g_ppClasses[iTeam == TEAM_AMERICANS ? iClass + TOTAL_BRIT_CLASSES : iClass];
}

ConVar mp_limit_mapsize_low("mp_limit_mapsize_low", "16", CVAR_FLAGS, "Servers with player counts <= this number are small, above it are medium or large");
ConVar mp_limit_mapsize_high("mp_limit_mapsize_high", "32", CVAR_FLAGS, "Servers with player counts <= this number are small or medium, above it are large");


/*
Given a player class determines the maximum number of players who can take that class.
*/
#define CVAR_SEARCH_BUFFER_SIZE 30
char cvarSearchBuffer[CVAR_SEARCH_BUFFER_SIZE];
int CPlayerClass::getClassLimit(const CPlayerClass* pClass) {
	//just use the cvars, and find them if we don't have have them already
	if (!pClass->m_pcvLimit_sml) {
		//find the cvars
		char cTeam = charForTeam(pClass->m_iDefaultTeam);
		#define find(size) \
			V_snprintf(cvarSearchBuffer, CVAR_SEARCH_BUFFER_SIZE, "mp_limit_%s_%c_"#size"\0", pClass->m_pszAbbreviation, cTeam); \
			{ \
				ConVarRef cvarRef(cvarSearchBuffer); \
				pClass->m_pcvLimit_##size = static_cast<ConVar*>(cvarRef.GetLinkedConVar()); \
			}

		find(sml)
		find(med)
		find(lrg)
		#undef find
	}
	int numPlayers = HL2MPRules()->NumConnectedClients();

	if (numPlayers <= mp_limit_mapsize_low.GetInt())
		return pClass->m_pcvLimit_sml->GetInt();
	else if (numPlayers <= mp_limit_mapsize_high.GetInt())
		return pClass->m_pcvLimit_med->GetInt();
	else
		return pClass->m_pcvLimit_lrg->GetInt();
}

int CPlayerClass::numClasses() {
	return TOTAL_NUM_CLASSES;
}

int CPlayerClass::numModelsForTeam(int iTeam) {
	return iTeam == TEAM_AMERICANS ? TOTAL_AMER_CLASSES : TOTAL_BRIT_CLASSES;
}

const CPlayerClass** CPlayerClass::asList() {
	return g_ppClasses;
}

/*
PLAYER MODEL PATHS AND NAMES - these are used repeatedly for precacheing the models and assigning them to players
*/
#define MODEL_BINFANTRY		"models/player/british_new/INF/br_infantry.mdl"
#define MODEL_BOFFICER		"models/player/british/light_b/light_b.mdl"
#define MODEL_BJAEGER		"models/player/british/jager/jager.mdl"
#define MODEL_BNATIVE		"models/player/british/mohawk/mohawk.mdl"
#define MODEL_BLINF			"models/player/british/loyalist/loyalist.mdl"
#define MODEL_BGRENADIER	"models/player/british_new/INF/br_infantry.mdl" //TODO change model!

#define MODEL_AINFANTRY		"models/player/american_new/INF/am_infantry.mdl"
#define MODEL_AOFFICER		"models/player/american/light_a/light_a.mdl"
#define MODEL_AFRONTIERSMAN "models/player/american_new/FRONT/am_frontiersman.mdl"
#define MODEL_AMILITIA		"models/player/american/militia/militia.mdl"
#define MODEL_ASTATEMILITIA "models/player/american_new/INF/am_infantry.mdl" //TODO change model!
#define MODEL_AFRENCH		"models/player/american/french/french_infantry.mdl"





//Use this for declaring a class. Note that "name" must match those used
//in the macros which declared the global pointers to these classes
#define DEC_BG3_PLAYER_CLASS(name, abbrev, teamChar) \
	class name : public CPlayerClass { public: name(); }; \
	name g_##name; \
	ConVar mp_limit_##abbrev##_##teamChar##_sml("mp_limit_"#abbrev"_"#teamChar"_sml", "-1", CVAR_FLAGS, "Maximum number of this class on small maps"); \
	ConVar mp_limit_##abbrev##_##teamChar##_med("mp_limit_"#abbrev"_"#teamChar"_med", "-1", CVAR_FLAGS, "Maximum number of this class on medium-sized maps"); \
	ConVar mp_limit_##abbrev##_##teamChar##_lrg("mp_limit_"#abbrev"_"#teamChar"_lrg", "-1", CVAR_FLAGS, "Maximum number of this class on large maps"); \
	namespace PlayerClasses { const CPlayerClass* g_p##name = &g_##name; } \
	name::name() : CPlayerClass(#abbrev)

DEC_BG3_PLAYER_CLASS(BInfantry, inf, b) {
	m_iDefaultTeam = TEAM_BRITISH;
	m_iClassNumber = CLASS_INFANTRY;

	m_flBaseSpeed = SPEED_INFANTRY;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_INFANTRY;

	m_pszPlayerModel = MODEL_BINFANTRY;

	m_iDefaultPrimaryAmmoCount = AMMO_INFANTRY;

	m_iSkinDepth = 8;
	m_iNumUniforms = 3;
	//m_bAllowUniformSelection = true;

	m_aWeapons[0].m_pszWeaponName = "weapon_brownbess";
	m_aWeapons[1].m_pszWeaponName = "weapon_longpattern";

	addSelfToGlobalLists(this);
}

DEC_BG3_PLAYER_CLASS(BOfficer, off, b) {
	m_iDefaultTeam = TEAM_BRITISH;
	m_iClassNumber = CLASS_OFFICER;

	m_flBaseSpeed = SPEED_OFFICER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_OFFICER;

	m_pszPlayerModel = MODEL_BOFFICER;

	m_iDefaultPrimaryAmmoCount = AMMO_OFFICER;

	m_iSkinDepth = 1;
	m_iNumUniforms = 3;
	//m_bAllowUniformSelection = true;
	m_bCanDoVcommBuffs = true;

	m_aWeapons[0].m_pszWeaponName = "weapon_pistol_b";
	m_aWeapons[1].m_pszWeaponName = "weapon_brownbess_carbine_nobayo";

	m_aWeapons[2].m_pszWeaponName = "weapon_sabre_b";
	m_aWeapons[2].m_bAlwaysGive = true;

	addSelfToGlobalLists(this);
}

DEC_BG3_PLAYER_CLASS(BJaeger, rif, b) {
	m_iDefaultTeam = TEAM_BRITISH;
	m_iClassNumber = CLASS_SNIPER;

	m_flBaseSpeed = SPEED_SNIPER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_SNIPER;

	m_pszPlayerModel = MODEL_BJAEGER;

	m_iDefaultPrimaryAmmoCount = AMMO_SNIPER;

	m_iSkinDepth = 2;
	m_iNumUniforms = 1;

	m_aWeapons[0].m_pszWeaponName = "weapon_jaeger";
	m_aWeapons[0].m_bAlwaysGive = true;
	m_aWeapons[1].m_pszWeaponName = "weapon_hirschf";
	m_aWeapons[1].m_bAlwaysGive = true;

	addSelfToGlobalLists(this);
}

DEC_BG3_PLAYER_CLASS(BNative, ski, b) {
	m_iDefaultTeam = TEAM_BRITISH;
	m_iClassNumber = CLASS_SKIRMISHER;

	m_flBaseSpeed = SPEED_SKIRMISHER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_SKIRMISHER;
	
	m_pszPlayerModel = MODEL_BNATIVE;

	m_iDefaultPrimaryAmmoCount = AMMO_SKIRMISHER;

	m_iSkinDepth = 2;
	m_iNumUniforms = 1;

	m_aWeapons[0].m_pszWeaponName = "weapon_brownbess_nobayo";
	m_aWeapons[1].m_pszWeaponName = "weapon_longpattern_nobayo";

	m_aWeapons[2].m_pszWeaponName = "weapon_tomahawk";
	m_aWeapons[2].m_bAlwaysGive = true;

	addSelfToGlobalLists(this);
}

DEC_BG3_PLAYER_CLASS(BLinf, linf, b) {
	m_iDefaultTeam = TEAM_BRITISH;
	m_iClassNumber = CLASS_LIGHT_INFANTRY;

	m_flBaseSpeed = SPEED_LIGHT_INF;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_LIGHT_INFANTRY;

	m_pszPlayerModel = MODEL_BLINF;

	m_iDefaultPrimaryAmmoCount = AMMO_LIGHT_INFANTRY;

	m_iSkinDepth = 1;
	m_iNumUniforms = 1;

	m_aWeapons[0].m_pszWeaponName = "weapon_brownbess_carbine";

	addSelfToGlobalLists(this);
}

DEC_BG3_PLAYER_CLASS(BGrenadier, gre, b) {
	m_iDefaultTeam = TEAM_BRITISH;
	m_iClassNumber = CLASS_GRENADIER;

	m_flBaseSpeed = SPEED_GRENADIER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_GRENADIER;

	m_pszPlayerModel = MODEL_BGRENADIER;

	m_iDefaultPrimaryAmmoCount = AMMO_GRENADIER;

	m_pszSecondaryAmmo = "Grenade";
	m_iDefaultSecondaryAmmoCount = 1;

	m_iSkinDepth = 1;
	m_iNumUniforms = 1;

	m_aWeapons[0].m_pszWeaponName = "weapon_brownbess_nobayo";
	m_aWeapons[0].m_bAlwaysGive = true;
	m_aWeapons[1].m_pszWeaponName = "weapon_frag";
	m_aWeapons[1].m_bAlwaysGive = true;
	m_aWeapons[2].m_pszWeaponName = "weapon_shortsword";
	m_aWeapons[2].m_bAlwaysGive = true;

	addSelfToGlobalLists(this);
}

DEC_BG3_PLAYER_CLASS(AInfantry, inf, a) {
	m_iDefaultTeam = TEAM_AMERICANS;
	m_iClassNumber = CLASS_INFANTRY;

	m_flBaseSpeed = SPEED_INFANTRY;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_INFANTRY;

	m_pszPlayerModel = MODEL_AINFANTRY;

	m_iDefaultPrimaryAmmoCount = AMMO_INFANTRY;

	m_iSkinDepth = 8;
	m_iNumUniforms = 3;
	//m_bAllowUniformSelection = true;

	m_aWeapons[0].m_pszWeaponName = "weapon_longpattern";
	m_aWeapons[1].m_pszWeaponName = "weapon_charleville";

	addSelfToGlobalLists(this);
}

DEC_BG3_PLAYER_CLASS(AOfficer, off, a) {
	m_iDefaultTeam = TEAM_AMERICANS;
	m_iClassNumber = CLASS_OFFICER;

	m_flBaseSpeed = SPEED_OFFICER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_OFFICER;

	m_pszPlayerModel = MODEL_AOFFICER;

	m_iDefaultPrimaryAmmoCount = AMMO_OFFICER;

	m_iSkinDepth = 1;
	m_iNumUniforms = 3;
	//m_bAllowUniformSelection = true;
	m_bCanDoVcommBuffs = true;

	m_aWeapons[0].m_pszWeaponName = "weapon_pistol_a";
	m_aWeapons[1].m_pszWeaponName = "weapon_brownbess_carbine_nobayo";

	m_aWeapons[2].m_pszWeaponName = "weapon_sabre_a";
	m_aWeapons[2].m_bAlwaysGive = true;

	addSelfToGlobalLists(this);
}

DEC_BG3_PLAYER_CLASS(AFrontiersman, rif, a) {
	m_iDefaultTeam = TEAM_AMERICANS;
	m_iClassNumber = CLASS_SNIPER;

	m_flBaseSpeed = SPEED_SNIPER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_SNIPER;

	m_pszPlayerModel = MODEL_AFRONTIERSMAN;

	m_iDefaultPrimaryAmmoCount = AMMO_SNIPER;

	m_iSkinDepth = 2;
	m_iNumUniforms = 1;

	m_aWeapons[0].m_pszWeaponName = "weapon_pennsylvania";
	m_aWeapons[0].m_bAlwaysGive = true;
	m_aWeapons[1].m_pszWeaponName = "weapon_knife";
	m_aWeapons[1].m_bAlwaysGive = true;

	addSelfToGlobalLists(this);
}

DEC_BG3_PLAYER_CLASS(AMilitia, ski, a) {
	m_iDefaultTeam = TEAM_AMERICANS;
	m_iClassNumber = CLASS_SKIRMISHER;

	m_flBaseSpeed = SPEED_SKIRMISHER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_SKIRMISHER;

	m_pszPlayerModel = MODEL_AMILITIA;

	m_iDefaultPrimaryAmmoCount = AMMO_SKIRMISHER;

	m_iSkinDepth = 2;
	m_iNumUniforms = 1;

	m_aWeapons[0].m_pszWeaponName = "weapon_fowler";
	m_aWeapons[1].m_pszWeaponName = "weapon_american_brownbess_nobayo";

	m_aWeapons[2].m_pszWeaponName = "weapon_beltaxe";
	m_aWeapons[2].m_bAlwaysGive = true;

	addSelfToGlobalLists(this);
}

DEC_BG3_PLAYER_CLASS(AStateMilitia, linf, a) {
	m_iDefaultTeam = TEAM_AMERICANS;
	m_iClassNumber = CLASS_LIGHT_INFANTRY;

	m_flBaseSpeed = SPEED_LIGHT_INF;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_LIGHT_INFANTRY;

	m_pszPlayerModel = MODEL_ASTATEMILITIA;

	m_iDefaultPrimaryAmmoCount = AMMO_LIGHT_INFANTRY;

	m_iSkinDepth = 1;
	m_iNumUniforms = 2;

	m_aWeapons[0].m_pszWeaponName = "weapon_longpattern_nobayo";
	m_aWeapons[0].m_bAlwaysGive = true;
	m_aWeapons[1].m_pszWeaponName = "weapon_smallsword";
	m_aWeapons[1].m_bAlwaysGive = true;

	addSelfToGlobalLists(this);
}

DEC_BG3_PLAYER_CLASS(AFrenchGre, gre, a) {
	m_iDefaultTeam = TEAM_AMERICANS;
	m_iClassNumber = CLASS_GRENADIER;

	m_flBaseSpeed = SPEED_GRENADIER;
	m_flFlagWeightMultiplier = SPEED_MOD_CARRY_GRENADIER;

	m_pszPlayerModel = MODEL_AFRENCH;

	m_iDefaultPrimaryAmmoCount = AMMO_GRENADIER;

	m_pszSecondaryAmmo = "Grenade";
	m_iDefaultSecondaryAmmoCount = 1;

	m_iSkinDepth = 1;
	m_iNumUniforms = 1;

	m_aWeapons[0].m_pszWeaponName = "weapon_revolutionnaire_nobayo";
	m_aWeapons[0].m_bAlwaysGive = true;
	m_aWeapons[1].m_pszWeaponName = "weapon_frag";
	m_aWeapons[1].m_bAlwaysGive = true;
	m_aWeapons[2].m_pszWeaponName = "weapon_shortsword";
	m_aWeapons[2].m_bAlwaysGive = true;

	addSelfToGlobalLists(this);
}