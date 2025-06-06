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

#ifndef BG3_PLAYER_CLASS
#define BG3_PLAYER_CLASS

#include "../../shared/bg3/bg3_player_shared.h"
#include "../shared/bg3/bg3_unlockable.h"

/*
PLAYER-MODELS LIST
This list is used to precache and validate the player models.
A forward declaration here exposes it to rest of program
*/
extern const char* g_ppszBritishPlayerModels[];
extern const char* g_ppszAmericanPlayerModels[];

extern ConVar mp_year_accuracy;
extern ConVar mp_year;

//forward declaration
namespace NClassQuota {
	struct SPopulationCounter;
}

class CGunKit {
	friend class CPlayerClass;
	friend class CKitButton;
	friend class CKitSelectionButton;
public:
	char*	m_pszWeaponPrimaryName		= nullptr;
	char*	m_pszWeaponSecondaryName	= nullptr;
	char*	m_pszWeaponTertiaryName		= nullptr;


	bool	m_bAllowBuckshot = false;
	uint8	m_iAmmoOverrideCount = 0;
	int8	m_iMovementSpeedModifier = 0;
	int8	m_iSleeveSkinOverride = -1;
	//int8	m_iArmModelOverride = -1;
	//int8	m_iSleeveInnerModelOverride = -1;
	//bool	m_bAlwaysGive = false;
	char*	m_pszAmmoOverrideName			= nullptr;
	char*	m_pszPlayerModelOverrideName	= nullptr;

	uint16	m_iMinYear = 0;
	uint16	m_iMaxYear = 1860;
	bool	m_bDisabledOnHistoricityDisabled = false;
	UnlockableBit m_iControllingBit = (UnlockableBit)0;

	inline bool ProgressionLocked() const { return m_iControllingBit != 0; }

#ifdef CLIENT_DLL
	wchar*	GetLocalizedName() const { return m_pLocalizedName; }
	wchar*	GetLocalizedDesc() const { return m_pLocalizedDesc; }

	bool	AvailableForCurrentYear() const;

public:
	void	SetLocalizedName(const char* pszToken) { m_pszLocalizedNameOverride = pszToken; }
	void	SetLocalizedDesc(const char* pszToken) { m_pszLocalizedDescOverride = pszToken; }

private:
	mutable const char* m_pszLocalizedNameOverride = nullptr;
	mutable const char* m_pszLocalizedDescOverride = nullptr;
	mutable wchar*	m_pLocalizedName;
	mutable wchar*	m_pLocalizedDesc;
#endif
};

#define TOTAL_BRIT_CLASSES 6 //same as above
#define TOTAL_AMER_CLASSES 5 // merged state militia and minuteman classes
#define TOTAL_NUM_CLASSES (TOTAL_BRIT_CLASSES + TOTAL_AMER_CLASSES) //used to build the list of classes and model lists

/*
A "class" in BG3 is unique with its own model, skins, weapons, and stats. 
I'm tired of these stats and information being spread out into unwieldly functions, 
so I'm putting them all into these separate files
*/
class CPlayerClass {

public:
	const char* m_pszAbbreviation; //for example, "off" or "gre", used for player class limits
protected:
	CPlayerClass(const char* abrv);
	CPlayerClass() { }
	static void postClassConstruct(CPlayerClass*);

public:
	uint8		m_iDefaultTeam;
	uint8		m_iClassNumber = 0; //for backwards-compatibility

	uint8		m_iDefaultPrimaryAmmoCount;
	uint8		m_iDefaultSecondaryAmmoCount = 0;
	const char* m_pszPrimaryAmmo = AMMO_DEFAULT_NAME;
	const char* m_pszSecondaryAmmo = "Grenade";


	mutable float		m_flBaseSpeedCalculated = 190.f;
	float		m_flBaseSpeedOriginal = 190.f;
	float		m_flFlagWeightMultiplier = 1.0f;

	const char* m_pszPlayerModel;
	const char* m_pszJoinName = nullptr;

#define NUM_POSSIBLE_WEAPON_KITS 9
	CGunKit		m_aWeapons[NUM_POSSIBLE_WEAPON_KITS];
private:
	mutable uint8		m_iChooseableKits;
public:

	uint8			m_iSkinDepth = 1; //how many skin variations per uniform
protected:
	uint8			m_iNumUniforms = 1; //how many uniforms?
public:
	uint8			m_iSleeveBase = 0; //chosen sleeve skin is m_iSleeveBase + pOwner->m_iClassSkin - 1
	uint8			m_iArmModel = 0;
	int8			m_iSleeveInnerModel = 0;
	
	bool			m_bLastUniformRestricted = false; //beta tester only uniforms
	const char*		m_pszDroppedHat = 0;
#define NUM_POSSIBLE_UNIFORMS 4
	const char*		m_pszUniformModelOverrides[NUM_POSSIBLE_UNIFORMS]; //per-uniform model overrides
	int8			m_aUniformSleeveOverrides[NUM_POSSIBLE_UNIFORMS]; //per-uniform sleeve base overrides
	int8			m_aUniformArmModelOverrides[NUM_POSSIBLE_UNIFORMS]; //per-uniform sleeve model overrides

	//progression system uniform stuff
	bool			m_bDefaultRandomUniform = false;
	UnlockableBit	m_iDerandomizerControllingBit = (UnlockableBit)0; //if unlockable profile has this bit, let us choose
	UnlockableBit	m_aUniformControllingBits[NUM_POSSIBLE_UNIFORMS]; //tells whether or not each uniform is locked
	bool			m_aUniformControllingBitsActive[NUM_POSSIBLE_UNIFORMS]; //whether or not use the locking uniform bits

	bool			m_bCanDoVcommBuffs = false; //this will be true for officer
	bool			m_bHasImplicitDamageResistance = false;
	bool			m_bHasImplicitDamageWeakness = false;
	bool			m_bNerfResistance = false;

//Functions
	inline bool isAmerican() const { return m_iDefaultTeam == TEAM_AMERICANS; }
	inline bool isBritish() const { return m_iDefaultTeam == TEAM_BRITISH; }

	inline uint8		numChooseableWeapons() const { return m_iChooseableKits; }
	uint8				numChooseableUniformsForPlayer(CBasePlayer* pPlayer) const; // { return m_bForceRandomUniform ? 1 : m_iNumUniforms; }
	inline uint8		numUniforms() const { return m_iNumUniforms; }
	const CWeaponDef*	getWeaponDef(byte iKit) const;
	void				getWeaponDef(byte iKit, const CWeaponDef** ppPrimary, const CWeaponDef** ppSecondary, const CWeaponDef** ppTertiary) const;
	const CGunKit*		getWeaponKitChooseable(byte iWeapon) const; //indexes choosable weapons, skipping over non-choosable ones.

	

#ifdef CLIENT_DLL
	bool		shouldHaveWeaponSelectionMenu() const;

	//Might as well store the menu-related items here instead of making separate data structures.
	//With this, each class will remember its last selected gun, ammo, and uniform kits.
private:
	mutable ConVar* m_pcvWeapon = nullptr;
	mutable ConVar* m_pcvAmmo = nullptr;
	mutable ConVar* m_pcvUniform = nullptr;
public:
	//It's a bit wierd to see these as const functions, but it's okay because they aren't actually changing the stats of the class.
	void SetDefaultWeapon	(byte iWeapon)	const; //0-based
	void SetDefaultAmmo	(byte iAmmo)		const; //0-based
	void SetDefaultUniform	(byte iUniform) const; //0-based
	byte GetDefaultWeapon	(void)			const { return m_pcvWeapon->GetInt(); }
	byte GetDefaultAmmo	(void)				const { return m_pcvAmmo->GetInt(); }
	byte GetDefaultUniform (void)			const { return m_pcvUniform->GetInt(); }

	//This function links all classes to external data stored in ConVars and localizer
	//multi-call safe; the function will do nothing after the first call
	static void InitClientRunTimeData(void);
private:
	static void InitPrevKitData();
	static void Localize();

public:
	mutable const wchar* m_pLocalizedName;
	mutable const wchar* m_pLocalizedDesc;
#endif
	static const CPlayerClass* fromNums(int iTeam, int iClass); //for backwards-compatability with old numbering system
	static const CPlayerClass* fromAbbreviation(int iTeam, const char* pszAbbreviation);
	bool GetLimitsAreInitialized() const { return m_pcvLimit_sml != NULL; }
	void InitLimits() const;
public:
	mutable ConVar* m_pcvLimit_lrg = nullptr;
	mutable ConVar* m_pcvLimit_med = nullptr;
	mutable ConVar* m_pcvLimit_sml = nullptr;
public:
	int		GetLimitLrg() const { return m_pcvLimit_lrg->GetInt(); }
	int		GetLimitMed() const { return m_pcvLimit_med->GetInt(); }
	int		GetLimitSml() const { return m_pcvLimit_sml->GetInt(); }

	NClassQuota::SPopulationCounter* m_pPopCounter;
	//static int getClassLimit(const CPlayerClass* pClass); //maximum number of players on the team who can use this class
	static int numClasses(); //teams count individually, so American Infantry and British Infantry are separate classes
	static int numModelsForTeam(int iTeam);
	inline static int numClassesForTeam(int iTeam) { return iTeam == TEAM_AMERICANS ? TOTAL_AMER_CLASSES : TOTAL_BRIT_CLASSES; }
	static const CPlayerClass* const * asList(); //retrieves a list of pointers to the classes

	static void RemoveClassLimits();
	static void UpdateClassSpeeds(); //updates class base speeds based on server variables
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
	//dec(AStateMilitia)
	dec(AFrenchGre)

#undef dec
	
}

/*
PLAYER MODEL PATHS AND NAMES - these are used repeatedly for precacheing the models and assigning them to players
*/

#define MODEL_BINFANTRY			"models/player/british/infantry/br_infantry.mdl"
#define MODEL_BOFFICER			"models/player/british/officer/br_officer.mdl"
//#define MODEL_BOFFICER			"models/player/british/light_b/light_b.mdl"
#define MODEL_BJAEGER			"models/player/british/jager/jager.mdl"
#define MODEL_BNATIVE			"models/player/british/mohawk/mohawk.mdl"
#define MODEL_BLINF				"models/player/british/loyalist/loyalist.mdl"
#define MODEL_BNEWFOUNDLAND_INFANTRY				"models/player/british/newfoundland_infantry/newfoundland_infantry.mdl"
#define MODEL_BGRENADIER		"models/player/british/grenadier/br_grenadier.mdl"
#define MODEL_BGRENADIER_ALT		"models/player/british/grenadier/br_grenadier_alt.mdl"

#define MODEL_AINFANTRY			"models/player/american/infantry/american_infantry.mdl"
#define MODEL_AOFFICER			"models/player/american/light_a/light_a.mdl"
#define MODEL_AFRONTIERSMAN		"models/player/american/frontiersman/frontiersman.mdl"
#define MODEL_AMILITIA			"models/player/american/minuteman/minuteman.mdl"
#define MODEL_ASTATEMILITIA		"models/player/american/state/state_militia.mdl"
#define MODEL_AFRENCH			"models/player/american/french_gre/french_gre.mdl"
#define MODEL_AFRENCH_ALT		"models/player/american/french_gre/french_gre_alt.mdl"
#define MODEL_AFRENCHOFFICER	"models/player/american/french_officer/french_officer.mdl"

#define ARMS_REGULAR 0
#define ARMS_NATIVE 1
#define ARMS_BRIT_GRE 2
#define ARMS_FRE 3

#define SLEEVE_INNER_BLANK 0
#define SLEEVE_INNER_PLAIN 1
#define SLEEVE_INNER_LACE 2

#endif //BG3_PLAYER_CLASS