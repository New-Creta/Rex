#ifndef REX_CORE_GLOBAL_H
#define REX_CORE_GLOBAL_H
    
#ifdef REX_CORE_LIB
# define REX_CORE_EXPORT __declspec(dllexport)
#else
# define REX_CORE_EXPORT __declspec(dllimport)
#endif

#endif // REX_CORE_GLOBAL_H
