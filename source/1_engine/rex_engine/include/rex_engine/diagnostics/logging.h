// ============================================
//
// REX GAME ENGINE
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: logging.h
// Copyright (c) Nick De Breuck 2023
//
// ============================================

#pragma once

#include "rex_std/format.h"
#include "rex_std/iostream.h"

#define REX_INFO(msg, ...)  rsl::cout << "INFO: " << rsl::format(msg, __VA_ARGS__) << "\n"
#define REX_WARN(msg, ...)  rsl::cout << "WARN: " << rsl::format(msg, __VA_ARGS__) << "\n"
#define REX_ERROR(msg, ...) rsl::cout << "ERR: " << rsl::format(msg, __VA_ARGS__) << "\n"

#define REX_TODO(msg, ...) rsl::cout << "TODO: " << rsl::format(msg, __VA_ARGS__) << "\n"

#define REX_INFO_X(cond, msg, ...) \
[&](){\
  if (!(cond)) \
  {\
    REX_INFO(msg, __VA_ARGS__); \
    return true;\
  } \
  return false;\
}()
#define REX_WARN_X(cond, msg, ...) \
[&](){\
  if (!(cond)) \
  {\
    REX_WARN(msg, __VA_ARGS__); \
    return true;\
  } \
  return false;\
}()
#define REX_ERROR_X(cond, msg, ...) \
[&](){\
  if (!(cond)) \
  {\
    REX_ERROR(msg, __VA_ARGS__); \
    return true;\
  } \
  return false;\
}()

#define REX_TODO_X(cond, msg, ...) \
[&](){\
  if (!(cond)) \
  {\
    REX_TODO(msg, __VA_ARGS__); \
    return true;\
  } \
  return false;\
}()
