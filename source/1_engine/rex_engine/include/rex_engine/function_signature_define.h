#pragma once

#include "rex_engine/defines.h"

#if defined REX_COMPILER_CLANG
  #define REX_FUNC_SIG __PRETTY_FUNCTION__
#elif defined REX_COMPILER_MSVC
  #define REX_FUNC_SIG __FUNCSIG__
#else
  #define REX_FUNC_SIG "REX_FUNC_SIG unknown!"
#endif