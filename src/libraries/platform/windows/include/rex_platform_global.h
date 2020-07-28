#ifndef REX_PLATFORM_GLOBAL_H
#define REX_PLATFORM_GLOBAL_H

#ifdef REX_PLATFORM_LIB
# define REX_PLATFORM_EXPORT __declspec(dllexport)
#else
# define REX_PLATFORM_EXPORT __declspec(dllimport)
#endif

#endif // REX_PLATFORM_GLOBAL_H
