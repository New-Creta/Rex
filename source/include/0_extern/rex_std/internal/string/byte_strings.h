// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: byte_strings.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/string/string_utils.h"
#include "rex_std/bonus/types.h"
#include "rex_std/optional.h"

// Character array functions (cstring.h)
#include "rex_std/internal/memory/memcpy.h"
#include "rex_std/internal/memory/memcpy_backward.h" // Not in C++ Standard
#include "rex_std/internal/memory/memmove.h"
#include "rex_std/internal/memory/memset.h"
#include "rex_std/internal/string/memchr.h"
