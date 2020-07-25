#ifndef REX_UTILITIES_GLOBAL_H
#define REX_UTILITIES_GLOBAL_H

#ifdef REX_UTILITIES_LIB
# define REX_UTILITIES_EXPORT __declspec(dllexport)
#else
# define REX_UTILITIES_EXPORT __declspec(dllimport)
#endif

#endif // REX_UTILITIES_GLOBAL_H
