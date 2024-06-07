#pragma once

#include "rex_std/bonus/compiler.h"
#include "rex_std/bonus/defines.h"

//-------------------------------------------------------------------------
// Unused parameter.
#define REX_UNUSED_PARAM(...) RSL_UNUSED(__VA_ARGS__)

//-------------------------------------------------------------------------
// Assembly instruction to break execution.
#define REX_DEBUG_BREAK RSL_DEBUG_BREAK

//-------------------------------------------------------------------------
// BIT TWIDDLING
#define BIT(x) (1 << x)

//-------------------------------------------------------------------------
// Func signature
#define REX_FUNC_SIG = RSL_FUNC_SIGNATURE

//-------------------------------------------------------------------------
// Static messaging
#define REX_STATIC_WARNING(expr) RSL_STATIC_WARNING(expr)
#define REX_STATIC_INFO(expr)    RSL_STATIC_INFO(expr)

//-------------------------------------------------------------------------
// Anonymous Variable
#define REX_ANONYMOUS_VARIABLE(str) RSL_ANONYMOUS_VARIABLE(str)

//-------------------------------------------------------------------------
// Attributes
#define REX_NO_DISCARD   RSL_NO_DISCARD
#define REX_FORCE_INLINE RSL_FORCE_INLINE
#define REX_MAYBE_UNUSED RSL_MAYBE_UNUSED

//-------------------------------------------------------------------------
// Execute a bit of code only once
#define REX_EXECUTE_ONCE(expression) static auto RSL_MERGE(execute_once_var_,__LINE__) = [&]() {expression; return 1;}();
