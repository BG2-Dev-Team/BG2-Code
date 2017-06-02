#ifndef BG3_MATH
#define BG3_MATH

#ifndef CLIENT_DLL
#include "../../server/cbase.h"

#else
#include "../../client/cbase.h"

#endif 

//Length of vector between two entities
vec_t EntDist(const CBaseEntity& ent1, const CBaseEntity& ent2);

#endif //BG3_MATH