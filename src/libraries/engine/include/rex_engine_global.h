#ifndef REX_ENGINE_GLOBAL_H
#define REX_ENGINE_GLOBAL_H

#ifdef REX_ENGINE_LIB
# define REX_ENGINE_EXPORT __declspec(dllexport)
#else
# define REX_ENGINE_EXPORT __declspec(dllimport)
#endif

#endif // REX_ENGINE_GLOBAL_H
