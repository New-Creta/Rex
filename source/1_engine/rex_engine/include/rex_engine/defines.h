#ifndef REX_DEFINES
#define REX_DEFINES

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

#endif