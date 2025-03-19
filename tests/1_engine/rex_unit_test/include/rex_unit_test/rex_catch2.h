#pragma once

#include "catch2/catch.hpp"

#include "rex_unit_test/string_convertors.h"

#ifndef REX_DISABLE_CATCH
#define REX_CHECK(...) CHECK(__VA_ARGS__)
#define REX_CHECK_NOTHROW(...) __VA_ARGS__ // Because rex doesn't use exceptions, we cannot use the nothrow // CHECK_NOTHROW(__VA_ARGS__)
#else
#define REX_CHECK(...)
#define REX_CHECK_NOTHROW(...) 
#endif
