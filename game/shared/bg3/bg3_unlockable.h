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

#ifndef BG3_UNLOCKABLE_H
#define BG3_UNLOCKABLE_H
#include "cbase.h"


#ifdef CLIENT_DLL
#define CHL2MP_Player C_HL2MP_Player
#endif
class CHL2MP_Player;

enum class EUnlockableType {
	Weapon,
	Uniform, //ex. new light infantry uniform
	Bodygroup, //ex. feathered cap
	UniformChoice, //ex. Militia uniform no longer random, now choosable
	SpawnFunction, //arbitrary code to run on player spawn
	NUM_TYPES
};


//Flags for unlockable bitfield -- turning these on/off in the bitfield is what actually controls the buffs
//one for each unlockable
enum UnlockableBit : unsigned long long {

	//Unlockable tier 1
	ULK_1_UNI_BRIT_LINF_1,
	ULK_1_UNI_BRIT_LINF_2,
	ULK_1_UNI_AMER_GRE,
	ULK_1_UNI_BRIT_GRE,
	ULK_1_UNI_AMER_OFF,

	//Unlockable tier 2
	ULK_2_BDY_FEATHER,
	ULK_2_UNC_AMER_SKI,
	ULK_2_UNC_AMER_RIF,
	ULK_2_FUN_BUTTER, //spawn with butterknife
	ULK_2_FUN_BUFF_INC, //buff increment

	//Unlockable tier 3
	ULK_3_WEP_LMC,
	ULK_3_WEP_FRE_CARB,
	ULK_3_WEP_OLD_PATTERN,
	ULK_3_WEP_BLUNDER,
	ULK_3_WEP_TRADE,

	NUM_UNLOCKABLES
};

enum class EUnlockableTier {
	TIER_1,
	TIER_2,
	TIER_3,
	NUM_TIERS
};


//Unlockable object stores all unlockable information 
class Unlockable {
public:
	uint64 m_iBit; //our bit index in the bitfield
	EUnlockableType m_eType; //what type of unlockable are we?
	EUnlockableTier m_eTier;

	const char* m_pszImagePath; //Path to image for unlockable menu
	bool m_bToggleable; //Is player able to turn this on/off after unlocking?

#ifndef CLIENT_DLL
	//Depending on type, each unlockable will have its own data
	union {
		//Weapons handled by existing kit code with bitfield checks

		//Uniforms handled by existing uniform code with bitfield checks

		//Bodygroup data
		union {
			const char* m_pszBodygroupName;
			uint8 m_iBodygroupIndex;
		};

		//Uniform choice handled by existing uniform code with bitfield checks

		//Arbitrary code on player spawn
		void(*m_pfSpawnFunc)(CHL2MP_Player*);
	};
#endif

	uint8 getCost(); //how many points does this unlockable cost?

	//Contructor defines unlockable data and adds self to global list
	Unlockable();
	Unlockable(uint64 bit, EUnlockableType type, EUnlockableTier tier, const char* imagePath, bool toggleable);

	static Unlockable* fromBitIndex(UnlockableBit bit);
	static CUtlVector<Unlockable*>* unlockablesOfType(EUnlockableType type);
	static CUtlVector<Unlockable*>* unlockablesOfTier(EUnlockableTier tier);

private:
	static Unlockable* s_aUnlockables[NUM_UNLOCKABLES]; //global list of unlockables
	static CUtlVector<Unlockable*> s_aUnlockablesByType[(uint32)EUnlockableType::NUM_TYPES];
	static CUtlVector<Unlockable*> s_aUnlockablesByTier[(uint32)EUnlockableTier::NUM_TIERS];
};


enum class EExperienceEventType {
	//might as well map each enum to its experience amount here
	//instead of mapping it later
	FLAG_CAPTURE = 15,
	CTF_FLAG_CAPTURE = 16,
	WEAPON_KILL = 10, //from handheld weapon
	WEAPON_KILL_LONG_RANGE = 21,
	GRENADE_KILL = 17,
	SWIVEL_KILL = 20,
	TEAM_ROUND_WIN = 50,
	MATCH_WIN = 100,
};

//Each player has their own profile, stored on their local system
#define MAX_UNLOCKABLE_LEVEL 45
class UnlockableProfile {
	uint8 m_iVersionNumber = 1; //version number in case format changes later

public:
	uint64 m_iUnlockedBits; //What the user has unlocked, but not necessarily activated
	uint64 m_iActivatedBits; //Which unlockables are actually enabled, as user may turn some off

	uint32 m_iExperience; //resets to 0 on each level gain
	uint32 m_iLevel; //current level
	uint32 m_iPointsSpent; //points available = level - pointsSpent

	inline bool isUnlockableUnlocked(Unlockable* pUnlockable) { return !!(m_iUnlockedBits & (1ULL >> pUnlockable->m_iBit)); }
	inline bool isUnlockableActivated(Unlockable* pUnlockable) { return isUnlockableUnlocked(pUnlockable) && !!(m_iActivatedBits & (1ULL << pUnlockable->m_iBit)); }

#ifdef CLIENT_DLL
	inline int	 getPointsAvailable() { return m_iPointsSpent - m_iLevel; }

	void unlockItem(Unlockable* pUnlockable);
	bool toggleItemActivation(Unlockable* pUnlockable);

	void addExperience(EExperienceEventType experience);

	static UnlockableProfile* get();
#else
	void createExperienceEvent(CHL2MP_Player* pRecipient, EExperienceEventType type, float pointScale = 1.0f);

#endif
private:

#ifdef CLIENT_DLL
	uint32 getExperienceForNextLevel();
	void saveToFile();
	void readFromFile();
	void encryptForServer(char* buffer, uint32 bufferlen);
#else
	void decryptFromClient(char* buffer, uint32 bufferlen, CHL2MP_Player* pPlayer);
#endif
};

//Global function for deciding whether or not experience should be added/counted
bool shouldUnlockableExperienceBeCounted(CHL2MP_Player* pPlayer = NULL, bool bForceUpdate = false);

#endif //BG3_UNLOCKABLE_H