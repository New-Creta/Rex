// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: defines.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/attributes.h"

#define REX_UNUSED(...) __VA_ARGS__

#define STRINGIZE(x)  STRINGIZE2(x)
#define STRINGIZE2(x) #x

#define LINE_TO_STRING(L) #L
#define CALL_MACRO(M, L)  M(L)

#ifdef __clang__
  #define STATIC_WARNING(expr)
#else
  #define THIS_LINE             CALL_MACRO(STRINGIZE2, __LINE__)
  #define STATIC_WARNING_PREFIX __FILE__ "(" THIS_LINE "): [WARNING] "
  #define STATIC_WARNING(expr)  __pragma(message(STATIC_WARNING_PREFIX##expr))
#endif

#ifdef __clang__
  #define STATIC_INFO(expr)
#else
  #define STATIC_INFO_PREFIX __FILE__ "(" THIS_LINE "): [INFO] "
  #define STATIC_INFO(expr)  __pragma(message(STATIC_INFO_PREFIX##expr))
#endif

#define MERGE2(A, B) A##B
#define MERGE(A, B)  MERGE(A, B)

#define FUNC_SIGNATURE __FUNCSIG__

#ifdef __COUNTER__
  #define ANONYMOUS_VARIABLE(str) MERGE(str, __COUNTER__)
#else
MERGE(str, __LINE__)
#endif

#define EXECUTE_ONCE(expression)                                                                                                                                                                                                                         \
  static auto ANONYMOUS_VARIABLE(execute_once_var) = [&]()                                                                                                                                                                                               \
  {                                                                                                                                                                                                                                                      \
    expression;                                                                                                                                                                                                                                          \
    return 1;                                                                                                                                                                                                                                            \
  }();

#define TO_WIDE_STRING(str) L##str

#ifdef REX_ENABLE_TODO_CHECKING
  #define REX_STATIC_TODO(msg) static_assert(false, "TODO: ", msg);
#else
  #define REX_STATIC_TODO(msg)
#endif

#ifdef _MSC_VER
  #define REX_ALIGN(x) __declspec(align(n))
#else
  #error No REX_ALIGN specified for the current compiler
#endif
