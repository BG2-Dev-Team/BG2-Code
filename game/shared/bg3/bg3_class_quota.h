#ifndef BG3_CLASS_QUOTA_H
#define BG3_CLASS_QUOTA_H

#ifndef CLIENT_DLL
#include "hl2mp/hl2mp_player.h"
#else
#include "hl2mp/c_hl2mp_player.h"
#endif

//-------------------------------------------------------------------------
// Controls & manages the limits for different classes.
//		Also manages & switches the classes of bots.
//-------------------------------------------------------------------------
namespace NClassQuota {

	//Each CPlayerClass keeps track of its own population
	struct SPopulationCounter {
		uint8 m_iTotalPopulation;
		uint8 m_iBotPopulation;

		SPopulationCounter() : m_iTotalPopulation(0), m_iBotPopulation(0) {}

		void AddPlayer(const CHL2MP_Player* pPlayer) {
			m_iTotalPopulation++;
			if (pPlayer->IsFakeClient()) m_iBotPopulation++;
		}
		void RemovePlayer(const CHL2MP_Player* pPlayer) {
			m_iTotalPopulation--;
			if (pPlayer->IsFakeClient()) m_iBotPopulation--;
		}
	};

#ifndef CLIENT_DLL

	//Initializer flushes list and creates initial 
	void Init();

	//---------------------------------------------------------------------
	// Notifications/checks of external events
	//---------------------------------------------------------------------

	//Checks if by linebattle rules, the player's whole team is switching to another class
	void CheckForForcedClassChange(CHL2MP_Player*);

	//Switches bot to another class if one is available
	void CheckBotSwitchClass(CHL2MP_Player* pBot);

	void NotifyClassLimitChanged(const CPlayerClass*);

	void CheckBotAddClass(const CPlayerClass* pClass);
	void CheckBotRemoveClass(const CPlayerClass* pClass);
#endif

	void NotifyPlayerChangedTeamClass(const CHL2MP_Player*, const CPlayerClass* pNextClass, uint8 iNextTeam);

	void NotifyPlayerLeave(const CHL2MP_Player*);

	

	//---------------------------------------------------------------------
	// Getters/accessors
	//---------------------------------------------------------------------
	int8 GetLimitTeamClass(uint8 iTeam, uint8 iClass);
	int8 GetLimitForClass(const CPlayerClass*);

	bool PlayerMayJoinTeam(const CBasePlayer* pPlayer, uint8 iTeam);

	EClassAvailability PlayerMayJoinClass(const CHL2MP_Player*, const CPlayerClass*);

}

#endif //BG3_CLASS_QUOTA_H