#include "cbase.h"
#include "bg3_triggers.h"
#include "hl2mp/hl2mp_player.h"

void CSpawnRoom::StartTouch(CBaseEntity* pOther) {
	//Msg("Entity enters trigger\n");

	if (!pOther->IsPlayer())
		return;

	CHL2MP_Player* pPlayer = static_cast<CHL2MP_Player*>(pOther);

	//block players on the wrong team
	if (!m_iForTeam || pPlayer->GetTeamNumber() == m_iForTeam) {
		//Msg("Player entered trigger\n");

		pPlayer->m_bInSpawnRoom = true;
	}
}

void CSpawnRoom::EndTouch(CBaseEntity* pOther) {
	if (!pOther->IsPlayer())
		return;

	CHL2MP_Player* pPlayer = static_cast<CHL2MP_Player*>(pOther);

	//Msg("Player leaves trigger\n");

	//faster to always assign than check against wrong team
	pPlayer->m_bInSpawnRoom = false;
}

LINK_ENTITY_TO_CLASS(trigger_spawn_room, CSpawnRoom);

BEGIN_DATADESC(CSpawnRoom)
DEFINE_KEYFIELD(m_iForTeam, FIELD_INTEGER, "ForTeam"),
END_DATADESC()
