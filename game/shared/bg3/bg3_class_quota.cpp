#include "cbase.h"
#include "bg3_class_quota.h"
#include "bg3_class.h"
#include "hl2mp/hl2mp_gamerules.h"

#ifndef CLIENT_DLL
#include "team.h"
#else
#include "c_team.h"
#endif

extern ConVar mp_autobalanceteams;
extern ConVar mp_autobalancetolerance;
extern ConVar mp_limit_mapsize_low;
extern ConVar mp_limit_mapsize_high;

namespace NClassQuota {
	static CUtlLinkedList<const CPlayerClass*> g_qFutureBotClasses;
	static CUtlLinkedList<const CPlayerClass*> g_qRemoveBotClasses;

	static CUtlLinkedList<const CPlayerClass*> g_aInfiniteAmericanClasses;
	static CUtlLinkedList<const CPlayerClass*> g_aInfiniteBritishClasses;
	inline static CUtlLinkedList<const CPlayerClass*>* InfiniteClassesForTeam(int iTeam) { return iTeam == TEAM_AMERICANS ? &g_aInfiniteAmericanClasses : &g_aInfiniteBritishClasses; }

#ifndef CLIENT_DLL
	void CheckForForcedClassChange(CHL2MP_Player* pPlayer) {
		ConVar * pLimiter;
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
			classCopy = Clamp(classCopy, 0, CLASS_GRENADIER);
			pPlayer->SetNextClass(classCopy);
		}
	}
#endif

	void NotifyPlayerChangedTeamClass(const CHL2MP_Player* pPlayer, const CPlayerClass* pNextClass, uint8 iNextTeam) {
		//check if we need to decrement the population counter of the player's previous class
		if (pPlayer->GetTeamNumber() >= TEAM_AMERICANS) {
			//if true, our previous team was not spectator, so decrement current class
			const CPlayerClass* pClass = pPlayer->GetPlayerClass();
			pClass->m_pPopCounter->RemovePlayer(pPlayer);
		}

		//increment counter for next class
		pNextClass->m_pPopCounter->AddPlayer(pPlayer);
	}

	void NotifyPlayerLeave(const CHL2MP_Player* pPlayer) {
		//update counters
		const CPlayerClass* pClass = pPlayer->GetPlayerClass();
		pClass->m_pPopCounter->RemovePlayer(pPlayer);

		//as far as class limits go, we only need check
		//whether a bot can now take up a new class
		if (pClass->m_pPopCounter->m_iTotalPopulation < GetLimitForClass(pClass)
			&& InfiniteClassesForTeam(pClass->m_iDefaultTeam)->Find(pClass) == -1)
			g_qFutureBotClasses.AddToTail(pClass);
	}

	void NotifyClassLimitChanged(const CPlayerClass* pClass) {
		int lim = GetLimitForClass(pClass);
		CUtlLinkedList<const CPlayerClass*>* pList = InfiniteClassesForTeam(pClass->m_iDefaultTeam);
		//check if we need to remove the class from the list of infinite classes
		if (lim > 0) {
			pList->FindAndRemove(pClass);
		}
		else {
			if (pList->Find(pClass) == pList->InvalidIndex())
				pList->AddToTail(pClass);
		}
	}

	int8 GetLimitTeamClass(uint8 iTeam, uint8 iClass) {
		const CPlayerClass* pClass = CPlayerClass::fromNums(iTeam, iClass);
		return GetLimitForClass(pClass);
	}
	int8 GetLimitForClass(const CPlayerClass* pClass) {
		int numPlayers = HL2MPRules()->NumConnectedClients();


		if (numPlayers <= mp_limit_mapsize_low.GetInt())
			return pClass->GetLimitSml();
		else if (numPlayers <= mp_limit_mapsize_high.GetInt())
			return pClass->GetLimitMed();
		else
			return pClass->GetLimitLrg();
	}

	bool PlayerMayJoinTeam(const CBasePlayer* pPlayer, uint8 iTeam) {
		
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
			if ((iAutoTeamBalanceTeamDiff >= mp_autobalancetolerance.GetInt()) && (iAutoTeamBalanceBiggerTeam == iTeam))
			{
				return false;
			}
		}
		return true;
	}

	EClassAvailability PlayerMayJoinClass(const CHL2MP_Player* pPlayer, const CPlayerClass* pClass) {
		return CLASS_FREE;
	}
}
