#ifndef REX_DEFINES
#define REX_DEFINES

//-------------------------------------------------------------------------
// Unused parameter.
#if defined __clang__
  #define UNUSED_PARAM(...)                                                                                                                                                                                                                              \
    _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wunused-value\"")                                                                                                                                                              \
    {                                                                                                                                                                                                                                                    \
      __VA_ARGS__;                                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                    \
    _Pragma("clang diagnostic pop")
#elif defined _MSC_VER
  #define UNUSED_PARAM(...)                                                                                                                                                                                                                              \
    {                                                                                                                                                                                                                                                    \
      __VA_ARGS__;                                                                                                                                                                                                                                       \
    }
#elif defined __GNUC__
  #define UNUSED_PARAM(...)                                                                                                                                                                                                                              \
    {                                                                                                                                                                                                                                                    \
      __VA_ARGS__;                                                                                                                                                                                                                                       \
    }
#endif

//-------------------------------------------------------------------------
// Assembly instruction to break execution.
#if defined _MSC_VER
    #define DEBUG_BREAK() __debugbreak()
#elif defined __clang__
    #define DEBUG_BREAK()
#elif defined __GNUC__
    #define DEBUG_BREAK()
#else
    #error DEBUG_BREAK unsupported machine instruction ...
#endif

#endif