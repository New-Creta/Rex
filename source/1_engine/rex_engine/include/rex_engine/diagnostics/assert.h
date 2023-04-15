#pragma once

#include "rex_std/assert.h"

#define REX_ASSERT(...)         RSL_ASSERT(__VA_ARGS__)
#define REX_ASSERT_X(cond, ...) RSL_ASSERT_X(cond, __VA_ARGS__)