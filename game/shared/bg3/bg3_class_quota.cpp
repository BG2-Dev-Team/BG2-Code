#include "cbase.h"
#include "bg3_class_quota.h"
#include "bg3_class.h"
#include "hl2mp/hl2mp_gamerules.h"
#include "Math/bg3_rand.h"
#include "../types/CDblLkLst.hpp"

#ifndef CLIENT_DLL
#include "team.h"
#else
#include "c_team.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar mp_autobalanceteams;
extern ConVar mp_autobalancetolerance;
extern ConVar mp_limit_mapsize_low;
extern ConVar mp_limit_mapsize_high;

namespace NClassQuota {
#define MAX_NUM_BOTS_PER_CLASS 2
	
#ifndef CLIENT_DLL
	//BG3 - Awesome - I'm TIRED of Valve's list classes being so non-functional and undocumented...
	// so here we go with my own list type I wrote more than a whole year ago in 2017 - May 2018
	static CDblLkLst<const CPlayerClass*> g_qFutureBotClasses;
	static CDblLkLst<const CPlayerClass*> g_qRemoveBotClasses;

	static CDblLkLst<const CPlayerClass*> g_aInfiniteAmericanClasses;
	static CDblLkLst<const CPlayerClass*> g_aInfiniteBritishClasses;
	inline static CDblLkLst<const CPlayerClass*>* InfiniteClassesForTeam(int iTeam) { return iTeam == TEAM_AMERICANS ? &g_aInfiniteAmericanClasses : &g_aInfiniteBritishClasses; }


	void Init() {
		//flush all lists
		g_qFutureBotClasses.flush();
		g_qRemoveBotClasses.flush();
		g_aInfiniteAmericanClasses.flush();
		g_aInfiniteBritishClasses.flush();

		//re-find all the infinite classes and add them to the list.
		//add available classes at the same time.
		const CPlayerClass* const * pList = CPlayerClass::asList();
		for (uint8 i = 0; i < CPlayerClass::numClasses(); i++) {
			const CPlayerClass* pClass = pList[i];

			//clear population counter
			pClass->m_pPopCounter->m_iBotPopulation = 0;
			pClass->m_pPopCounter->m_iTotalPopulation = 0;

			//ignore grenadier classes - we don't want bots with grenades
			//if (pClass == PlayerClasses::g_pAFrenchGre || pClass == PlayerClasses::g_pBGrenadier)
				//continue;

			//let future bots know the current class settings
			int limit = GetLimitForClass(pClass);

			//Warning("Class limit for %i%s is %i\n", pClass->m_iDefaultTeam, pClass->m_pszAbbreviation, limit);
			if (limit < 0) {
				CDblLkLst<const CPlayerClass*>* pInfiniteList = InfiniteClassesForTeam(pClass->m_iDefaultTeam);
				pInfiniteList->push(pClass);
			}
			//BG3 - Awesome - setting an extra limit of 2, because otherwise there are too MANY other classes
			else for (int j = 0; j < limit && j < MAX_NUM_BOTS_PER_CLASS; j++) {
				//Warning("Adding class %i%s to the queue\n", pClass->m_iDefaultTeam, pClass->m_pszAbbreviation);
				g_qFutureBotClasses.add(RndInt(0, g_qFutureBotClasses.length()), pClass);
			}
		}
	}


	void CheckForForcedClassChange(CHL2MP_Player* pPlayer) {
		/*ConVar * pLimiter;
		if (pPlayer->GetTeamNumber() == TEAM_AMERICANS)
			pLimiter = &lb_enforce_class_amer;
		else
			pLimiter = &lb_enforce_class_brit;
		//do this if we're in linebattle mode AND the cvar tells us to AND we're not an officer
		if (IsLinebattle() && pLimiter && pLimiter->GetBool() && pPlayer->GetNextClass() != CLASS_OFFICER) {
			//the correspondence isn't one-to-one
			pPlayer->SetNextClass(pLimiter->GetInt() - 1);
			//clamp it in case it was too high or low
			//we have to use this copying stuff because we can't pass network vars into templates
			int classCopy = pPlayer->GetNextClass();
			int max = CPlayerClass::numClassesForTeam(pPlayer->GetTeamNumber()) - 1;
			classCopy = Clamp(classCopy, 0, max);
			pPlayer->SetNextClass(classCopy);
		}*/
	}

	void CheckBotSwitchClass(CHL2MP_Player* pBot) {
		//look at g_qFutureBotClasses and g_qRemoveBotClasses
		//to determine if a bot needs to switch classes
		//use g_aInfiniteAmericanClasses or g_aInfiniteBritishClasses
		//to identify a free class, if needed
		const CPlayerClass* pClass = pBot->GetPlayerClass();
		int iTeam = pBot->GetTeamNumber();
		CDblLkLst<const CPlayerClass*>* pList = InfiniteClassesForTeam(iTeam);
		if (pList->length() == 0) {
			Warning("ERROR: Could not find infinite-limit class to switch bot to! Change your class limits!");
			return;
		}

		//check for switching off a class which is full
		int removeIndex = g_qRemoveBotClasses.indexOf(pClass);
		if (g_qRemoveBotClasses.length() > 0 && removeIndex != -1) {
			//find an infinite class to take instead
			int index = RndInt(0, pList->length() - 1);
			index = clamp(index, 0, pList->length());
			const CPlayerClass* pNextClass = pList->get(index);
			pBot->ForceJoin(pNextClass, iTeam, pNextClass->m_iClassNumber);
			g_qRemoveBotClasses.remove(removeIndex);
		}
		else //if (g_qFutureBotClasses.length() > 0) {
			//Msg("Checking if bot %s needs to switch classes. Next team = %i and bot's team = %i", pBot->GetPlayerName(), g_qFutureBotClasses.get(0)->m_iDefaultTeam, iTeam);
			if (g_qFutureBotClasses.length() > 0
				&& g_qFutureBotClasses.get(0)->m_iDefaultTeam == iTeam
				&& pList->contains(pClass)
				) {
				//remove and set at the same TIME!! WOW
				const CPlayerClass* pNextClass = g_qFutureBotClasses.front();
				pBot->ForceJoin(pNextClass, iTeam, pNextClass->m_iClassNumber);
			}
	}

	void NotifyClassLimitChanged(const CPlayerClass* pClass) {
		int lim = GetLimitForClass(pClass);
		CDblLkLst<const CPlayerClass*>* pList = InfiniteClassesForTeam(pClass->m_iDefaultTeam);
		//check if we need to remove the class from the list of infinite classes
		if (lim > 0) {
			pList->findAndRemove(pClass);
			CheckBotRemoveClass(pClass);
		}
		else {
			if (!pList->contains(pClass))
				pList->push(pClass);
			CheckBotAddClass(pClass);
			//CheckBotRemoveClass(pClass);
		}
	}
#endif

	void CheckBotAddClass(const CPlayerClass* pClass) {
#ifndef CLIENT_DLL
		CDblLkLst< const CPlayerClass*>* pList = InfiniteClassesForTeam(pClass->m_iDefaultTeam);
		if (pClass->m_pPopCounter->m_iTotalPopulation < GetLimitForClass(pClass)
			&& pClass->m_pPopCounter->m_iBotPopulation < MAX_NUM_BOTS_PER_CLASS
			&& !pList->contains(pClass)
			//&& (pClass != PlayerClasses::g_pAFrenchGre && pClass != PlayerClasses::g_pBGrenadier)
			) {
			//int index = RndInt(0, g_qFutureBotClasses.Count());
			g_qFutureBotClasses.push(pClass);
		}
#endif
	}

	void CheckBotRemoveClass(const CPlayerClass* pClass) {
#ifndef CLIENT_DLL
		CDblLkLst< const CPlayerClass*>* pList = InfiniteClassesForTeam(pClass->m_iDefaultTeam);
		//Msg("Checking bot removal from class %s... %i > %i\n", pClass->m_pszAbbreviation, pClass->m_pPopCounter->m_iTotalPopulation, GetLimitForClass(pClass));
		if (pClass->m_pPopCounter->m_iTotalPopulation > GetLimitForClass(pClass)
			&& !pList->contains(pClass))
		g_qRemoveBotClasses.push(pClass);
#endif
	}

	const CPlayerClass* FindInfiniteClassForTeam(uint8 iTeam) {
		const CPlayerClass* pClass = CPlayerClass::fromNums(iTeam, CLASS_INFANTRY);
		uint8 numClasses = CPlayerClass::numClassesForTeam(iTeam);
		for (uint8 i = 0; i < numClasses; i++) {
			const CPlayerClass* pPossiblyInfinite = CPlayerClass::fromNums(iTeam, i);
			if (GetLimitForClass(pPossiblyInfinite) < 0) {
				pClass = pPossiblyInfinite;
				break;
			}
		}
		return pClass;
	}

	void NotifyPlayerChangedTeamClass(const CHL2MP_Player* pPlayer, const CPlayerClass* pNextClass, uint8 iNextTeam) {
		if (!pNextClass) {
			Warning("In " __FUNCTION__ ", pNextClass is NULL");
			return;
		}

		//check if we need to decrement the population counter of the player's previous class
		if (pPlayer->GetTeamNumber() >= TEAM_AMERICANS) {
			//if true, our previous team was not spectator, so decrement current class
			const CPlayerClass* pClass = pPlayer->GetNextPlayerClass();
			pClass->m_pPopCounter->RemovePlayer(pPlayer);
			CheckBotAddClass(pClass);
		}

		//increment counter for next class
		if (iNextTeam >= TEAM_AMERICANS) {
			pNextClass->m_pPopCounter->AddPlayer(pPlayer);
			CheckBotRemoveClass(pNextClass);

#ifndef CLIENT_DLL
			//check if we need to remove this class from the queue
			for (int i = 0; i < g_qFutureBotClasses.length(); i++) {
				if (g_qFutureBotClasses.get(i) == pNextClass) {
					g_qFutureBotClasses.remove(i);
					break;
				}
			}
#endif
		}
	}

	void NotifyPlayerLeave(const CHL2MP_Player* pPlayer) {
		//update counters
		//we use GetNextPlayerClass() because the player may have previously chosen to switch class.
		if (pPlayer->GetTeamNumber() >= TEAM_AMERICANS) {
			const CPlayerClass* pClass = pPlayer->GetNextPlayerClass();
			pClass->m_pPopCounter->RemovePlayer(pPlayer);

			//as far as class limits go, we only need check
			//whether a bot can now take up a new class
			CheckBotAddClass(pClass);
		}
	}

	

	int8 GetLimitTeamClass(uint8 iTeam, uint8 iClass) {
		const CPlayerClass* pClass = CPlayerClass::fromNums(iTeam, iClass);
		return GetLimitForClass(pClass);
	}
	int8 GetLimitForClass(const CPlayerClass* pClass) {
		int numPlayers = HL2MPRules()->NumConnectedClients();

		//TODO find a better place to initialize these 
		if (!pClass->GetLimitsAreInitialized())
			pClass->InitLimits();
		if (numPlayers <= mp_limit_mapsize_low.GetInt())
			return pClass->GetLimitSml();
		else if (numPlayers <= mp_limit_mapsize_high.GetInt())
			return pClass->GetLimitMed();
		else
			return pClass->GetLimitLrg();
	}

	bool PlayerMayJoinTeam(const CHL2MP_Player* pPlayer, uint8 iTeam, CHL2MP_Player** ppBotToSwitch) {
		
		//check so we don't ruin the team balance..
		//BG2 - Tjoppen - don't bother with checking balance if we're just changing class, not team.
		//					CHL2MPRules::Think() will make sure the teams are kept balanced.
		//					We want to allow people to change class even if their team is too big.
		if (mp_autobalanceteams.GetInt() == 1 && pPlayer->GetTeamNumber() != iTeam) //So the team we're attempting to join is different from our current team.
		{

			//Initialize just in case.
			int iAutoTeamBalanceTeamDiff = 0,
				iAutoTeamBalanceBiggerTeam = NULL,
				iNumAmericans = g_Teams[TEAM_AMERICANS]->GetNumPlayers(), //
				iNumBritish = g_Teams[TEAM_BRITISH]->GetNumPlayers(); //

			switch (pPlayer->GetTeamNumber()) //Our current team now, but we're changing teams..
			{
			case TEAM_AMERICANS:
				iNumAmericans -= 1; //-1 because we plan on leaving.
				break;
			case TEAM_BRITISH:
				iNumBritish -= 1;
				break;
			}

			//if (pAmericans->GetNumPlayers() > pBritish->GetNumPlayers()) //So there are more Americans than British.
			if (iNumAmericans > iNumBritish)
			{
				//iAutoTeamBalanceTeamDiff = ((pAmericans->GetNumPlayers() - pBritish->GetNumPlayers()) ); //+ 1
				iAutoTeamBalanceTeamDiff = iNumAmericans - iNumBritish;
				iAutoTeamBalanceBiggerTeam = TEAM_AMERICANS;
			}
			else //More british than Americans.
			{
				//iAutoTeamBalanceTeamDiff = ((pBritish->GetNumPlayers() - pAmericans->GetNumPlayers()) ); //+ 1
				iAutoTeamBalanceTeamDiff = iNumBritish - iNumAmericans;
				iAutoTeamBalanceBiggerTeam = TEAM_BRITISH;
			}

			//The +1 is to account for the fact that we're joining the bigger team...
			if ((iAutoTeamBalanceBiggerTeam == iTeam) && (iAutoTeamBalanceTeamDiff + 1 >= mp_autobalancetolerance.GetInt()) )
			{
				//look for a bot we might be able to switch...
				*ppBotToSwitch = NULL;
				for (int i = 0; i <= gpGlobals->maxClients; i++) {
					CHL2MP_Player* pPossibleBot = (CHL2MP_Player*)UTIL_PlayerByIndex(i);
					if (pPossibleBot && pPossibleBot->GetTeamNumber() == iTeam && pPossibleBot->IsFakeClient()) {
						*ppBotToSwitch = pPossibleBot;
						break;
					}
				}

				return (*ppBotToSwitch) != NULL;
			}
		}
		return true;
	}

	EClassAvailability PlayerMayJoinClass(const CHL2MP_Player* pPlayer, const CPlayerClass* pClass) {
		int limit = NClassQuota::GetLimitForClass(pClass);

		if (limit == 0)
			return CLASS_UNAVAILABLE;
		if (limit < 0)
			return CLASS_FREE; //not necessary, but may improve performance

		//how many players are of our class?
		int count;
#ifdef CLIENT_DLL
		count = g_Teams[pClass->m_iDefaultTeam]->GetNumOfNextClassRealPlayers(pClass->m_iClassNumber);
#else
		count = pClass->m_pPopCounter->NumRealPlayers(); //don't include bots in class limits - bots handle those on their own
#endif
		if (pPlayer->GetNextPlayerClass() == pClass) {
			return count >= limit ? CLASS_TAKEN_FULL : CLASS_TAKEN_FREE;
		}
		return count >= limit ? CLASS_FULL : CLASS_FREE;
	}
}

#ifndef CLIENT_DLL
CON_COMMAND(class_report, "Reports populations of all player classes")
{
	const CPlayerClass* const * pList = CPlayerClass::asList();
	for (uint8 i = 0; i < CPlayerClass::numClasses(); i++) {
		const CPlayerClass* pClass = pList[i];
		int limit = NClassQuota::GetLimitForClass(pClass);
		int iTeam = pClass->m_iDefaultTeam;
		
		Msg("%i - %s - (%i/%i)/%i\n",
			iTeam,
			pClass->m_pszAbbreviation,
			pClass->m_pPopCounter->m_iBotPopulation,
			pClass->m_pPopCounter->m_iTotalPopulation,
			limit);
	}
}
#else

CON_COMMAND(class_report_client, "Reports populations of all player classes")
{
	Msg("Local class: %s\n", C_HL2MP_Player::GetLocalHL2MPPlayer()->GetNextPlayerClass()->m_pszAbbreviation);

	const CPlayerClass* const * pList = CPlayerClass::asList();
	for (uint8 i = 0; i < CPlayerClass::numClasses(); i++) {
		const CPlayerClass* pClass = pList[i];
		int limit = NClassQuota::GetLimitForClass(pClass);
		int iTeam = pClass->m_iDefaultTeam;
		C_Team* pTeam = g_Teams[iTeam];
		uint8 botCount;
		uint8 realCount;

		pTeam->GetNumOfNextClass(pClass->m_iClassNumber, &botCount, &realCount);
		uint8 totalCount = botCount + realCount;
		EClassAvailability availability = NClassQuota::PlayerMayJoinClass(C_HL2MP_Player::GetLocalHL2MPPlayer(), pClass);

		Msg("%i - %s - (%i/%i)/%i, Availability: %i\n",
			iTeam,
			pClass->m_pszAbbreviation,
			botCount,
			totalCount,
			limit,
			availability);
	}
}
#endif

#ifndef CLIENT_DLL
CON_COMMAND(class_queue_report, "Lists classes which are scheduled to be switched to by bots") {
	Msg("Classes scheduled to be added:\n");
	for (int i = 0; i < NClassQuota::g_qFutureBotClasses.length(); i++) {
		const CPlayerClass* pClass = NClassQuota::g_qFutureBotClasses.get(i);
		Msg("%i - %s \n",
			pClass->m_iDefaultTeam,
			pClass->m_pszAbbreviation);
	}
	Msg("Classes scheduled to be removed:\n");
	for (int i = 0; i < NClassQuota::g_qRemoveBotClasses.length(); i++) {
		const CPlayerClass* pClass = NClassQuota::g_qRemoveBotClasses.get(i);
		Msg("%i - %s \n",
			pClass->m_iDefaultTeam,
			pClass->m_pszAbbreviation);
	}
}
#endif