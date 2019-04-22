//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//===========================================================================//
#include "cbase.h"
#include "c_basetempentity.h"
#include "c_te_legacytempents.h"
#include "tempent.h"
#include "tier1/KeyValues.h"
#include "toolframework_client.h"
#include "tier0/vprof.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Sprite TE
//-----------------------------------------------------------------------------
class C_TEBuffSprite : public C_BaseTempEntity
{
public:
	DECLARE_CLASS(C_TEBuffSprite, C_BaseTempEntity);
	DECLARE_CLIENTCLASS();

	C_TEBuffSprite(void);
	virtual			~C_TEBuffSprite(void);

	virtual void	PostDataUpdate(DataUpdateType_t updateType);

public:
	int				m_iPlayerIndex; //which player activated the buff?
	int				m_nModelIndex;
	float			m_fScale;
};


//-----------------------------------------------------------------------------
// Networking
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_EVENT_DT(C_TEBuffSprite, DT_TEBuffSprite, CTEBuffSprite)
RecvPropInt(RECVINFO(m_iPlayerIndex)),
RecvPropInt(RECVINFO(m_nModelIndex)),
RecvPropFloat(RECVINFO(m_fScale)),
END_RECV_TABLE()


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TEBuffSprite::C_TEBuffSprite(void)
{
	m_iPlayerIndex = 0; //the most significant bit also stores whether or not we were effected
	m_nModelIndex = 0;
	m_fScale = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TEBuffSprite::~C_TEBuffSprite(void)
{
}


//-----------------------------------------------------------------------------
// Recording 
//-----------------------------------------------------------------------------
static inline void RecordSprite(int nModelIndex,
	float flScale, int officerIndex)
{
	if (!ToolsEnabled())
		return;

	if (clienttools->IsInRecordingMode())
	{
		const model_t* pModel = (nModelIndex != 0) ? modelinfo->GetModel(nModelIndex) : NULL;
		const char *pModelName = pModel ? modelinfo->GetModelName(pModel) : "";

		KeyValues *msg = new KeyValues("TempEntity");

		msg->SetInt("te", TE_SPRITE_SINGLE);
		msg->SetString("name", "TE_Sprite");
		msg->SetFloat("time", gpGlobals->curtime);
		msg->SetString("model", pModelName);
		msg->SetFloat("scale", flScale);
		msg->SetInt("pindex", officerIndex);

		ToolFramework_PostToolMessage(HTOOLHANDLE_INVALID, msg);
		msg->deleteThis();
	}
}

static void BuffSpriteCreate(int nModelIndex, float scale, int officerIndex) {
	C_BasePlayer* pPlayer = UTIL_PlayerByIndex(officerIndex & ~(1 << 31));
	if (!pPlayer)
		return;

	Vector origin;
	Vector dir; {
		if (officerIndex & (1 << 31)) {
			//we're being buff, sprite faces upward
			dir.x = dir.y = 0;
			dir.z = 1;

			//move origin down to be visible
			origin = pPlayer->GetAbsOrigin();
			origin.z -= 10.f;
		}
		else {
			//sprite faces toward player
			C_BasePlayer* pLocal = C_BasePlayer::GetLocalPlayer();
			if (!pLocal)
				return;
			dir = pLocal->Weapon_ShootPosition() - pPlayer->GetAbsOrigin();

			origin = pPlayer->Weapon_ShootPosition();
		}
	}

	//tempents->TempSprite(m_vecOrigin, dir, m_fScale, m_nModelIndex, kRenderTransAdd, 0, a, 0, FTENT_SPRANIMATE);
	//RecordSprite(m_vecOrigin, m_nModelIndex, m_fScale, m_nBrightness, m_iPlayerIndex);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TEBuffSprite::PostDataUpdate(DataUpdateType_t updateType)
{
	VPROF("C_TEBuffSprite::PostDataUpdate");

	

	
}

void TE_BuffSprite(
	const Vector* pos, int modelindex, float size, int brightness)
{
	float a = (1.0 / 255.0) * brightness;
	tempents->TempSprite(*pos, vec3_origin, size, modelindex, kRenderTransAdd, 0, a, 0, FTENT_SPRANIMATE);
	RecordSprite(modelindex, size, brightness);
}

void TE_BuffSprite(KeyValues *pKeyValues)
{
	/*Vector vecOrigin, vecDirection;
	vecOrigin.x = pKeyValues->GetFloat("originx");
	vecOrigin.y = pKeyValues->GetFloat("originy");
	vecOrigin.z = pKeyValues->GetFloat("originz");
	const char *pModelName = pKeyValues->GetString("model");
	int nModelIndex = pModelName[0] ? modelinfo->GetModelIndex(pModelName) : 0;
	float flScale = pKeyValues->GetFloat("scale");
	int nBrightness = pKeyValues->GetInt("brightness");*/

	//TE_BuffSprite(filter, delay, &vecOrigin, nModelIndex, flScale, nBrightness);
}

/*CON_COMMAND(createsprite, "") {
	int model = CBaseEntity::PrecacheModel("effects/buff_sprite.mdl");
	CBasePlayer* pPlayer = CBasePlayer::GetLocalPlayer();
	C_LocalTempEntity* pEnt = tempents->TempSprite(pPlayer->GetAbsOrigin(), Vector(0, 0, 1), 30, model, RenderMode_t::kRenderTransAdd, 0, 1.0f, 5, 0);
	if (!pEnt)
		Warning("failed\n");
	//pEnt->SetAbsAngles(QAngle(0, 0, 1));
}*/
