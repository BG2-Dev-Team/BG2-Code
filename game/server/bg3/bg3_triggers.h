#ifndef BG3_TRIGGERS_H
#define BG3_TRIGGERS_H

#include "cbase.h"
#include "triggers.h"

//-----------------------------------------------------------------------------
// BG3 - Spawn room allows for players to instantly change class
//			without having to kill themselves
//-----------------------------------------------------------------------------
class CSpawnRoom : public CTriggerMultiple
{
	DECLARE_CLASS(CSpawnRoom, CTriggerMultiple);
	DECLARE_DATADESC();

public:
	//void Spawn(void);
	void StartTouch(CBaseEntity *pOther);
	void EndTouch(CBaseEntity *pOther);

private:
	int m_iForTeam;
};

#endif //BG3_TRIGGERS_H