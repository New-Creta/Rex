#ifndef REX_DIAGNOSTICS_GLOBAL_H
#define REX_DIAGNOSTICS_GLOBAL_H

#ifdef _WINDOWS
    #if REX_DYNAMIC_LINK
        #ifdef REX_DIAGNOSTICS_LIB
            # define REX_DIAGNOSTICS_EXPORT __declspec(dllexport)
        #else
            # define REX_DIAGNOSTICS_EXPORT __declspec(dllimport)
        #endif
    #else
        #define REX_DIAGNOSTICS_EXPORT
    #endif
#else
    #error We only support windows
#endif

#endif // REX_DIAGNOSTICS_GLOBAL_H
