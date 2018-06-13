#ifndef BG3_STATIC_INITIALIZE_H
#define BG3_STATIC_INITIALIZE_H

#define STATIC_INITIALIZE(id) \
	static void __static_initialize_##id(); \
	namespace { \
		class CInit_##id { \
			CInit_##id() { __static_initialize_##id(); } \
		}; \
	} \
	static CInit_##id id; \
	static void __static_initialize_##id()

#endif //BG3_STATIC_INITIALIZE_H