// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: logging.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/iostream.h"

namespace rsl
{
  inline namespace v1
  {
    namespace internal
    {
      template <typename ... Args>
      void log(rsl::ostream& os, Args&& ... args)
      {
        (os << ... << args);
      }
    }

#ifdef REX_ENABLE_LOGGING
#define REX_ERROR_X(cond, ...) if (!cond) { rsl::internal::log(rsl::cerr, "[Error] ", __VA_ARGS__, '\n'); }
#define REX_WARN_X(cond, ...) if (!cond) { rsl::internal::log(rsl::cout, "[Warning] ", __VA_ARGS__, '\n'); }
#define REX_INFO_X(cond, ...) if (!cond) { rsl::internal::log(rsl::cout, "[Info] ", __VA_ARGS__, '\n'); }
#define REX_TODO_X(cond, ...) if (!cond) { rsl::internal::log(rsl::cout, "[Todo] ", __VA_ARGS__, '\n'); }
#define REX_ERROR(...) rsl::internal::log(rsl::cerr, "[Error] ", __VA_ARGS__, '\n');
#define REX_WARN(...) rsl::internal::log(rsl::cout, "[Warning] ", __VA_ARGS__, '\n');
#define REX_INFO(...) rsl::internal::log(rsl::cout, "[Info] ", __VA_ARGS__, '\n');
#define REX_TODO(...) rsl::internal::log(rsl::cout, "[Todo] ", __VA_ARGS__, '\n');
#else
#define REX_ERROR_X(cond, ...) 
#define REX_WARN_X(cond, ...) 
#define REX_INFO_X(cond, ...) 
#define REX_TODO_X(cond, ...) 
#define REX_ERROR(...) 
#define REX_WARN(...) 
#define REX_INFO(...) 
#define REX_TODO(...) 
#endif
  }
}
