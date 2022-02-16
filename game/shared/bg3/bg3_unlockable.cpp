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
#include "bg3_unlockable.h"
#include "hl2mp/hl2mp_gamerules.h"

#ifdef CLIENT_DLL
	#include "hl2mp/c_hl2mp_player.h"
	#include "c_team.h"
	#include "c_playerresource.h"
	#include "bg2/bg2_hud_main.h"
#else
	#include "hl2mp/hl2mp_player.h"
	#include "team.h"
	#include "player_resource.h"
#endif

uint64 getDeviceIdChecksum();

uint8 Unlockable::getCost() {
	uint8 result;
	switch (m_eTier) {
	case EUnlockableTier::TIER_1:
		result = 1;
		break;
	case EUnlockableTier::TIER_2:
		result = 3;
		break;
	default:
		result = 5;
		break;
	}
	return result;
}

Unlockable::Unlockable() {}

Unlockable::Unlockable(uint64 bit, EUnlockableType type, EUnlockableTier tier, const char* imagePath, bool toggleable) {
	s_aUnlockables[bit] = this;
	s_aUnlockablesByType[(uint64)type].AddToTail(this);
	s_aUnlockablesByTier[(uint64)tier].AddToTail(this);

	m_iBit = (uint64)bit;
	m_eType = type;
	m_eTier = tier;
	m_pszImagePath = imagePath;
	m_bToggleable = toggleable;
}

Unlockable* Unlockable::fromBitIndex(UnlockableBit bit) {
	return s_aUnlockables[(uint32) bit];
}

CUtlVector<Unlockable*>* Unlockable::unlockablesOfType(EUnlockableType type) {
	return &s_aUnlockablesByType[(uint32) type];
}

CUtlVector<Unlockable*>* Unlockable::unlockablesOfTier(EUnlockableTier tier) {
	return &s_aUnlockablesByTier[(uint32)tier];
}


Unlockable* Unlockable::s_aUnlockables[NUM_UNLOCKABLES];
CUtlVector<Unlockable*> Unlockable::s_aUnlockablesByType[(uint32)EUnlockableType::NUM_TYPES];
CUtlVector<Unlockable*> Unlockable::s_aUnlockablesByTier[(uint32)EUnlockableTier::NUM_TIERS];

#ifdef CLIENT_DLL
const char* MapExpEventToText(EExperienceEventType type) {
	const char* s = "";
	switch (type) {
	case EExperienceEventType::FLAG_CAPTURE:
		s = "#BG3_ExpEvent_Flag";
		break;
	case EExperienceEventType::CTF_FLAG_CAPTURE:
		s = "#BG3_ExpEvent_CtfFlag";
		break;
	case EExperienceEventType::WEAPON_KILL_LONG_RANGE:
		s = "#BG3_ExpEvent_KillLongRange";
		break;
	case EExperienceEventType::GRENADE_KILL:
		s = "#BG3_ExpEvent_KillGrenade";
		break;
	case EExperienceEventType::SWIVEL_KILL:
		s = "#BG3_ExpEvent_KillSwivel";
		break;
	case EExperienceEventType::TEAM_ROUND_WIN:
		s = "#BG3_ExpEvent_RoundWin";
		break;
	case EExperienceEventType::MATCH_WIN:
		s = "#BG3_ExpEvent_MatchWin";
		break;
	default:
		s = "#BG3_ExpEvent_Kill";
		break;
	}
	return s;
}
#endif


//---------------------------------------------------------------------------------
// UNLOCKABLE DEFINITION SETUP
//---------------------------------------------------------------------------------
#define UNLOCKABLE_IMAGE_PATH ""
#define DECLARE_UNLOCKABLE(bit, type, tier, path, toggle) \
	class _Unlockable_##bit : public Unlockable { \
		public: \
		_Unlockable_##bit(uint64 _bt, EUnlockableType _tpe, EUnlockableTier _tir, const char* _path, bool _toggle); \
	}; \
	_Unlockable_##bit _unlockable_##bit##_instance = _Unlockable_##bit(bit, EUnlockableType::type, EUnlockableTier::tier, UNLOCKABLE_IMAGE_PATH path, toggle); \
	_Unlockable_##bit::_Unlockable_##bit(uint64 _bt, EUnlockableType _tpe, EUnlockableTier _tir, const char* _path, bool _toggle) : Unlockable(_bt, _tpe, _tir, _path, _toggle)



//---------------------------------------------------------------------------------
// UNLOCKABLE DEFINITIONS
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// Tier 1 Unlock definitions
//---------------------------------------------------------------------------------
DECLARE_UNLOCKABLE(ULK_1_UNI_BRIT_LINF_1,	Uniform, TIER_1, "1_blinf_1", false) {}
DECLARE_UNLOCKABLE(ULK_1_UNI_BRIT_LINF_2,	Uniform, TIER_1, "1_blinf_2", false) {}
DECLARE_UNLOCKABLE(ULK_1_UNI_AMER_GRE,		Uniform, TIER_1, "1_agre", false) {}
DECLARE_UNLOCKABLE(ULK_1_UNI_BRIT_GRE,		Uniform, TIER_1, "1_bgre", false) {}
DECLARE_UNLOCKABLE(ULK_1_UNI_AMER_OFF,		Uniform, TIER_1, "1_aoff", false) {}

//---------------------------------------------------------------------------------
// Tier 2 Unlock definitions
//---------------------------------------------------------------------------------
DECLARE_UNLOCKABLE(ULK_2_BDY_FEATHER, Bodygroup, TIER_2, "2_feather", true) {
#ifndef CLIENT_DLL
	m_bodyGroupData.m_pszBodygroupName = "macaroni";
	m_bodyGroupData.m_iBodygroupIndex = 1;
#endif
}
DECLARE_UNLOCKABLE(ULK_2_UNC_AMER_SKI, UniformChoice, TIER_2, "2_aski_choice", true) {}
DECLARE_UNLOCKABLE(ULK_2_UNC_AMER_RIF, UniformChoice, TIER_2, "2_arif_choice", true) {}
DECLARE_UNLOCKABLE(ULK_2_FUN_BOTTLE, SpawnFunction, TIER_2, "2_bottle", true) {
#ifndef CLIENT_DLL
	m_pfSpawnFunc = [](CHL2MP_Player* pPlayer) {
		pPlayer->GiveNamedItem(	"weapon_bottle");
	};
#endif
}
DECLARE_UNLOCKABLE(ULK_2_FUN_EASTER_EGG, Bodygroup, TIER_2, "2_easter_egg", true) {
#ifndef CLIENT_DLL
	m_bodyGroupData.m_pszBodygroupName = "easter_egg";
	m_bodyGroupData.m_iBodygroupIndex = 1;
#endif
}

//---------------------------------------------------------------------------------
// Tier 3 Unlock definitions
//---------------------------------------------------------------------------------
DECLARE_UNLOCKABLE(ULK_3_WEP_LMC,			Weapon, TIER_3, "3_lmc", false) {}
DECLARE_UNLOCKABLE(ULK_3_WEP_FRE_CARB,		Weapon, TIER_3, "3_fc", false) {}
DECLARE_UNLOCKABLE(ULK_3_WEP_OLD_PATTERN,	Weapon, TIER_3, "3_op", false) {}
DECLARE_UNLOCKABLE(ULK_3_WEP_BLUNDER,		Weapon, TIER_3, "3_blunder", false) {}
DECLARE_UNLOCKABLE(ULK_3_WEP_TRADE,			Weapon, TIER_3, "3_trade", false) {}

#ifdef CLIENT_DLL
void UnlockableProfile::unlockItem(Unlockable* pUnlockable) {
	if (!(m_iUnlockedBits & (1ULL << pUnlockable->m_iBit))) {
		m_iUnlockedBits |= (1ULL << pUnlockable->m_iBit);
		m_iPointsSpent += pUnlockable->getCost();
		m_iActivatedBits |= (1ULL << pUnlockable->m_iBit); //activated by default

		saveToFile();
	}
}

bool UnlockableProfile::toggleItemActivation(Unlockable* pUnlockable) {
	if ((m_iUnlockedBits & (1ULL << pUnlockable->m_iBit)) && pUnlockable->m_bToggleable) {
		m_iActivatedBits ^= (1ULL << pUnlockable->m_iBit);
	}

	return !!(m_iActivatedBits & (1ULL << pUnlockable->m_iBit));
}

void UnlockableProfile::addExperience(EExperienceEventType experience) {
	//if someone is trying to hack us with illegal experience, block that
	/*if (!shouldUnlockableExperienceBeCounted())
		return;*/
	int exp = (uint32)experience;
	Msg("Received %i!\n", exp);
	m_iExperience += exp;
	uint32 expNextLevel = getExperienceForNextLevel();
	Msg("Next level: %llu/%i\n", m_iExperience, expNextLevel);
	if (m_iExperience > expNextLevel) {

		//level up local variables
		m_iLevel++;
		m_iExperience -= expNextLevel;
		calcExperienceForNextLevel();

		Msg("Level-up! :D\n");

		//check if we need to level-up again somehow
		//int exp = (uint32)experience;

		//notify hud of level-up
		CHudBG2::GetInstance()->PlayLevelUpSound();
	}
}

void UnlockableProfile::calcExperienceForNextLevel() {
	m_iExperienceForNextLevel = 400 + m_iLevel * 50 + (m_iLevel * m_iLevel / 2);
	Msg("Calculating next level... %llu\n", m_iExperienceForNextLevel);
}

//Singleton pattern for local
static UnlockableProfile g_unlockableProfile;
static bool g_bUnlockableProfileInitialized = false;
UnlockableProfile* UnlockableProfile::get() {
	if (!g_bUnlockableProfileInitialized) {
		g_unlockableProfile.readFromFile();
		g_bUnlockableProfileInitialized = true;
		g_unlockableProfile.calcExperienceForNextLevel();
	}
	return &g_unlockableProfile;
}

void UnlockableProfile::reset() {
	m_iPointsSpent = m_iExperience = m_iActivatedBits = m_iUnlockedBits = 0ULL;
	m_iVersionNumber = UNLOCKABLE_PROFILE_VERSION_CURRENT;
	m_iLevel = 1;
	calcExperienceForNextLevel();
	m_iUniqueIdChecksum = getDeviceIdChecksum();
}





bool ChecksumCheck();
void UnlockableProfile::sendSettingsToServer() {
	char buffer[256];

	//encrypt bits with encrypt key
	uint64 key = getEncryptKey();
	uint64 unlock = m_iUnlockedBits ^ key;
	uint64 activated = m_iActivatedBits ^ key;

	Q_snprintf(buffer, sizeof(buffer), "ps_settings %llu %llu %llu", m_iUniqueIdChecksum, unlock, activated);
	engine->ClientCmd(buffer);

	/*extern bool g_bGoodDeviceId;
	if (g_bGoodDeviceId)
		engine->ClientCmd(DMG_BUFF);
	if (ChecksumCheck())
		engine->ClientCmd(DMG_BUFF2);*/
}

void UnlockableProfile::sendDeviceIdCheckSumToServer() {
	char buffer[128];
	Q_snprintf(buffer, sizeof(buffer), "ps_checksum %llu", m_iUniqueIdChecksum);
	//Msg("Sending checksum %llu\n", m_iUniqueIdChecksum);
	engine->ClientCmd(buffer);
}

#else
void UnlockableProfile::createExperienceEvent(CHL2MP_Player* pRecipient, EExperienceEventType type) {
	//if not allowed, ignore the event
	if (!NIntegrity::shouldUnlockableExperienceBeCounted(pRecipient) || pRecipient->IsFakeClient())
		return;

	//float points = (int)type * pointScale;
	CSingleUserRecipientFilter recpfilter(pRecipient);
	UserMessageBegin(recpfilter, "ExpEvent");
	//no WRITE_UNSIGNED_LONG_LONG, so let's just truncate
	WRITE_LONG((int)pRecipient->m_unlockableProfile.m_iUniqueIdChecksum);
	WRITE_LONG((int)type);
	MessageEnd();
}

#endif
extern ConVar* sv_cheats;
extern ConVar bot_pause;
extern int g_iNumFlagsOnMap;
extern bool g_bBotExists;

namespace NIntegrity {

	//Somewhat expensive calculation, only call periodically
	float g_flNextExperienceEnabledCheck = -FLT_MAX;
	static bool g_bLastExperienceEnabledCheck = false;
	bool shouldUnlockableExperienceBeCounted(CHL2MP_Player* pPlayer, bool bForceUpdate) {
		//skip all other checks if the map is bad
		if (!progressionAllowedOnCurrentMap())
			return false;

		if ((gpGlobals->curtime < g_flNextExperienceEnabledCheck) && !bForceUpdate) {
			return g_bLastExperienceEnabledCheck;
		}
		g_flNextExperienceEnabledCheck = gpGlobals->curtime + 10;
		
#ifndef CLIENT_DLL
		
		if (sv_cheats->GetBool())
			return (g_bLastExperienceEnabledCheck = false);

		//if (g_bBotExists)
			//return (g_bLastExperienceEnabledCheck = false);
		
		if (g_iNumFlagsOnMap > 6)
			return (g_bLastExperienceEnabledCheck = false);
#endif

		//Calculate roundtime
		float roundTime;
		if (mp_rounds.GetBool())
			roundTime = (float)mp_rounds.GetInt() * mp_roundtime.GetFloat();
		else
			roundTime = mp_timelimit.GetInt() * 60.f;

		//Minimum 15 minutes time
		if (roundTime < 900)
			return (g_bLastExperienceEnabledCheck = false);

#ifdef CLIENT_DLL
		int numAmericans = g_PR->GetNumAmericans();
		int numBritish = g_PR->GetNumBritish();
#else
		int numAmericans = g_pPlayerResource->GetNumAmericans();
		int numBritish = g_pPlayerResource->GetNumBritish();
#endif

		//at least 3 players on each team
		if ((numAmericans < 3 || numBritish < 3) && HL2MPRules()->m_flGameStartTime + 60 < gpGlobals->curtime)
			return (g_bLastExperienceEnabledCheck = false);

		
#ifndef CLIENT_DLL
		//not bot pause
		if (bot_pause.GetBool()) 
			return (g_bLastExperienceEnabledCheck = false);

		//server-side only -- no buddha
		if (pPlayer && (pPlayer->m_debugOverlays & OVERLAY_BUDDHA_MODE))
			return (g_bLastExperienceEnabledCheck = false);
#endif
			
		//if all else passes, return true
		return (g_bLastExperienceEnabledCheck = true);
	}
}