#include "cbase.h"
#include "bg3_math_shared.h"

vec_t EntDist(const CBaseEntity& ent1, const CBaseEntity& ent2) {
	return (ent1.GetAbsOrigin() - ent2.GetAbsOrigin()).Length();
}