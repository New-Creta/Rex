#ifndef REX_CORE_GLOBAL_H
#define REX_CORE_GLOBAL_H

#ifdef _WINDOWS
    #if REX_DYNAMIC_LINK
        #ifdef REX_CORE_LIB
            # define REX_CORE_EXPORT __declspec(dllexport)
        #else
            # define REX_CORE_EXPORT __declspec(dllimport)
        #endif
    #else
        #define REX_CORE_EXPORT
    #endif
#else
    #error We only support windows
#endif

#endif // REX_CORE_GLOBAL_H
