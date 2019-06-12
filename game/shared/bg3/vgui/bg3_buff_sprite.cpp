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
#if 1

#include "bg3_buff_sprite.h"

#define MODEL_NAME "models/effects/buff_sprite.mdl"
#define BUFF_SOUND "Buff.Activate"
#define DEFAULT_SCALE 5

void CBuffSprite::Spawn() {
	Precache();
	BaseClass::Spawn();
	SetModel(MODEL_NAME);
	SetModelScale(DEFAULT_SCALE);
	SetThink(&CBuffSprite::Think);
	SetNextThink(gpGlobals->curtime);
	EmitSound(BUFF_SOUND);
	
#ifndef CLIENT_DLL
	m_flCreationTime = gpGlobals->curtime;
#endif
	
}

void CBuffSprite::Precache() {
	BaseClass::Precache();
	PrecacheModel(MODEL_NAME);
#ifdef CLIENT_DLL
	PrecacheScriptSound(BUFF_SOUND);
#endif
}

#ifndef CLIENT_DLL
void CBuffSprite::Think() {
	//Msg(__FUNCTION__ "\n");
	//Msg("Remaining time: %f\n", m_flCreationTime + BUFF_SPRITE_LIFETIME - gpGlobals->curtime);
	if (gpGlobals->curtime > m_flCreationTime + BUFF_SPRITE_LIFETIME) {
		Remove();
	}
	
	float lifetime = gpGlobals->curtime - m_flCreationTime;
	lifetime = fmodf(lifetime, 0.5f);
	float scale = DEFAULT_SCALE + (DEFAULT_SCALE * lifetime);
	//Msg("%f\n", scale);
	SetModelScale(scale);
	//BaseClass::Think();
	SetNextThink(gpGlobals->curtime);
}


LINK_ENTITY_TO_CLASS(buff_sprite, CBuffSprite);
BEGIN_PREDICTION_DATA(CBuffSprite)
END_PREDICTION_DATA()
//PRECACHE_REGISTER(CBuffSprite);

IMPLEMENT_NETWORKCLASS_ALIASED(BuffSprite, DT_BuffSprite)

BEGIN_NETWORK_TABLE(CBuffSprite, DT_BuffSprite)
	SendPropInt(SENDINFO(m_iForTeam), Q_log2(NUM_TEAMS), SPROP_UNSIGNED),
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CBuffSprite)
END_PREDICTION_DATA()

BEGIN_DATADESC(CBuffSprite)
	DEFINE_THINKFUNC(Think),
	DEFINE_KEYFIELD(m_iForTeam, FIELD_INTEGER, "ForTeam"),
END_DATADESC()

#else //client

#undef CBuffSprite
IMPLEMENT_CLIENTCLASS_DT(C_BuffSprite, DT_BuffSprite, CBuffSprite)
	RecvPropInt(RECVINFO(m_iForTeam))
END_RECV_TABLE()
#define CBuffSprite C_BuffSprite

#endif
#endif
