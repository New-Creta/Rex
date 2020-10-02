#ifndef REX_PLATFORM_GLOBAL_H
#define REX_PLATFORM_GLOBAL_H

#ifdef _WINDOWS
    #if REX_DYNAMIC_LINK
        #ifdef REX_PLATFORM_LIB
            # define REX_PLATFORM_EXPORT __declspec(dllexport)
        #else
            # define REX_PLATFORM_EXPORT __declspec(dllimport)
        #endif
    #else
        #define REX_PLATFORM_EXPORT
    #endif
#else
    #error We only support windows
#endif

#endif // REX_PLATFORM_GLOBAL_H
