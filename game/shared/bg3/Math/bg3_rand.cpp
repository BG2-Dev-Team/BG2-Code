#include "cbase.h"
#include "bg3_rand.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef CLIENT_DLL
int a = 9158152, b = 14257153;
#else
extern int a;
extern int b;
#endif

#define abs(a) ((a) < 0 ? -(a) : (a))

void RndSeed(int seed) {
	a = seed;
}

int RndInt()
{
	a += b + (a >> 5);
	b += a + (b >> 11);

	return abs(b);
}

int RndInt(int min, int max) {
	return min + (RndInt() % (max-min+1)) ;
}

//Returns a float between 0 and 1
float RndFloat() {
	float result = (1.0f * RndInt()) / INT_MAX;
	return abs(result);
}

float RndFloat(float min, float max) {
	float result = (1.0f * RndInt()) / INT_MAX;
	if (result < 0)
		result = -result;
	result = min + result*(max - min);
	return result;
}



