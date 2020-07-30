#ifndef REX_GRAPHICS_GLOBAL_H
#define REX_GRAPHICS_GLOBAL_H

#ifdef REX_GRAPHICS_LIB
# define REX_GRAPHICS_EXPORT __declspec(dllexport)
#else
# define REX_GRAPHICS_EXPORT __declspec(dllimport)
#endif

#endif // REX_GRAPHICS_GLOBAL_H

