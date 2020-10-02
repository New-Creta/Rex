#ifndef REX_GRAPHICS_GLOBAL_H
#define REX_GRAPHICS_GLOBAL_H

#ifdef _WINDOWS
    #if REX_DYNAMIC_LINK
        #ifdef REX_GRAPHICS_LIB
            # define REX_GRAPHICS_EXPORT __declspec(dllexport)
        #else
            # define REX_GRAPHICS_EXPORT __declspec(dllimport)
        #endif
    #else
        #define REX_GRAPHICS_EXPORT
    #endif
#else
    #error We only support windows
#endif

#endif // REX_GRAPHICS_GLOBAL_H

