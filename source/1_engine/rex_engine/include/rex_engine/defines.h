
#ifndef REX_DEFINES
  #define REX_DEFINES

  #include "rex_std/bonus/compiler.h"
  #include "rex_std/string_view.h"

  //-------------------------------------------------------------------------
  // Unused parameter.
  #if defined REX_COMPILER_CLANG
    #define UNUSED_PARAM(...)                                                                                                                                                                                                                            \
      _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wunused-value\"")                                                                                                                                                            \
      {                                                                                                                                                                                                                                                  \
        __VA_ARGS__;                                                                                                                                                                                                                                     \
      }                                                                                                                                                                                                                                                  \
      _Pragma("clang diagnostic pop")
  #elif defined REX_COMPILER_MSVC
    #define UNUSED_PARAM(...)                                                                                                                                                                                                                            \
      {                                                                                                                                                                                                                                                  \
        __VA_ARGS__;                                                                                                                                                                                                                                     \
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

//-------------------------------------------------------------------------
// End of line
namespace rex
{
  #ifndef REX_EOL
    #ifdef REX_PLATFORM_WINDOWS
  inline constexpr rsl::string_view g_default_eol = "\r\n";
    #else
  inline constexpr rsl::string_view g_default_eol = "\n";
    #endif
  #endif
} // namespace rex

#endif

//-------------------------------------------------------------------------
// BIT TWIDDLING
#define BIT(x) (1 << x)