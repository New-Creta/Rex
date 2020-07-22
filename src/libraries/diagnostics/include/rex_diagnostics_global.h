#ifndef REX_DIAGNOSTICS_GLOBAL_H
#define REX_DIAGNOSTICS_GLOBAL_H

#ifdef REX_DIAGNOSTICS_LIB
# define REX_DIAGNOSTICS_EXPORT __declspec(dllexport)
#else
# define REX_DIAGNOSTICS_EXPORT __declspec(dllimport)
#endif

#endif // REX_DIAGNOSTICS_GLOBAL_H
