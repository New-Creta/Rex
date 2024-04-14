#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/engine/numeric.h"

TEST_CASE("merging signed integers together")
{
  s32 high = 0xAA;
  s32 low = 0XBB;

  s64 res = rex::merge_int32_to_int64(high, low);

  REX_CHECK(res == 0x000000AA000000BB);
}

TEST_CASE("merging unsigned integers together")
{
  u32 high = 0xAA;
  u32 low = 0XBB;

  u64 res = rex::merge_int32_to_int64(high, low);

  REX_CHECK(res == 0x000000AA000000BB);
}

TEST_CASE("splitting integer into smaller integers")
{
  u64 input = 0x000000AA000000BB;

  auto res = rex::split_int64_into_int32(input);

  REX_CHECK(res.high == 0xAA);
  REX_CHECK(res.low == 0xBB);
}