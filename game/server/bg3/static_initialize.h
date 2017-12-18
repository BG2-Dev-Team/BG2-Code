#ifndef BG3_STATIC_INITIALIZE_H
#define BG3_STATIC_INITIALIZE_H

#define STATIC_INITIALIZE(id) \
	namespace NStaticInitialize { \
		class CInit_##id { \
			CInit_##id(); \
		}; \
	} \
	NStaticInitialize::CInit_##id id; \
	NStaticInitialize::CInit_##id::CInit_##id()

#endif //BG3_STATIC_INITIALIZE_H