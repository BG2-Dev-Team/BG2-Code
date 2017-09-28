#ifndef BG3_RAND_H
#define BG3_RAND_H

#include "cbase.h"

/*
I'm tired of valve's functions not being random enough, so I'm making my own
*/
//TODO find an entry point to seed this!
void RndSeed(int seed);

int RndInt();

int RndInt(int min, int max);

float RndFloat();

float RndFloat(float min, float max);

inline bool RndBool(float chance) {
	return RndFloat() < chance;
}


#endif //BG3_RAND_H