#ifndef REX_DEFINES
#define REX_DEFINES

#include "rex_std/bonus/compiler.h"

//-------------------------------------------------------------------------
// Unused parameter.
#if defined REX_COMPILER_CLANG
  #define UNUSED_PARAM(...)                                                                                                                                                                                                                              \
    _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wunused-value\"")                                                                                                                                                              \
    {                                                                                                                                                                                                                                                    \
      __VA_ARGS__;                                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                    \
    _Pragma("clang diagnostic pop")
#elif defined REX_COMPILER_MSVC
  #define UNUSED_PARAM(...)                                                                                                                                                                                                                              \
    {                                                                                                                                                                                                                                                    \
      __VA_ARGS__;                                                                                                                                                                                                                                       \
    }
#endif

//-------------------------------------------------------------------------
// Assembly instruction to break execution.
#if defined REX_COMPILER_CLANG
    #define DEBUG_BREAK() __builtin_trap()
#elif defined REX_COMPILER_MSVC
    #define DEBUG_BREAK() __debugbreak()
#else
    #error DEBUG_BREAK unsupported machine instruction ...
#endif

#endif

//-------------------------------------------------------------------------
// BIT TWIDDLING
#define BIT(x)       (1 << x)