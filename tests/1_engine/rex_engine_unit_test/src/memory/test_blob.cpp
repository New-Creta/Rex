#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/memory/blob.h"
#include "rex_engine/engine/types.h"

#include "rex_unit_test/test_allocator.h"

TEST_CASE("Blob - Default Creation")
{
  rex::memory::Blob blob;

  REX_CHECK(blob.size() == 0);
  REX_CHECK(blob.data() == nullptr);
}

TEST_CASE("Blob - Blob move construction") 
{
  

  rex::memory::Blob b1(rsl::make_unique<rsl::byte[]>(10));
  rex::memory::Blob b2(std::move(b1));

  REX_CHECK(static_cast<bool>(b1) == false);
  REX_CHECK(b1.size() == 0_bytes);
  REX_CHECK(static_cast<bool>(b2) == true);
  REX_CHECK(b2.size() == 10_bytes);
}

TEST_CASE("Blob - Construction from byte array")
{
  auto arr = rsl::make_unique<rsl::byte[]>(3);
  arr[0] = static_cast<rsl::byte>(1);
  arr[1] = static_cast<rsl::byte>(2);
  arr[2] = static_cast<rsl::byte>(3);

  rex::memory::Blob b(rsl::move(arr));
  REX_CHECK(static_cast<bool>(b) == true);
  REX_CHECK(b.size() == 3_bytes);
  REX_CHECK(static_cast<s32>(b.data()[0]) == 1);
  REX_CHECK(static_cast<s32>(b.data()[1]) == 2);
  REX_CHECK(static_cast<s32>(b.data()[2]) == 3);
}

TEST_CASE("Blob - Construction from typed array") 
{
  auto arr = rsl::make_unique<s32[]>(3);
  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;

  rex::memory::Blob b(rsl::move(arr));
  REX_CHECK(static_cast<bool>(b) == true);
  REX_CHECK(b.size() == 5_bytes);
  REX_CHECK(static_cast<s32>(b.data()[0]) == 1);
  REX_CHECK(static_cast<s32>(b.data()[1]) == 2);
  REX_CHECK(static_cast<s32>(b.data()[2]) == 3);
}

TEST_CASE("Blob - Blob copy static method") 
{
  auto arr = rsl::make_unique<s32[]>(3);
  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;

  rex::memory::Blob src(rsl::move(arr));
  rex::memory::Blob dst;
  rex::memory::Blob::copy(src, dst);

  REX_CHECK(dst.size() == 3_bytes);
  REX_CHECK(static_cast<s32>(dst.data()[0]) == 1);
  REX_CHECK(static_cast<s32>(dst.data()[1]) == 2);
  REX_CHECK(static_cast<s32>(dst.data()[2]) == 3);
}

TEST_CASE("Blob - Blob allocate and release") {
  rex::memory::Blob b;
  b.allocate(10_bytes);
  REX_CHECK(b.size() == 10_bytes);

  b.release();
  REX_CHECK(static_cast<bool>(b) == false);
  REX_CHECK(b.size() == 0_bytes);
}

TEST_CASE("Blob - Blob zero_initialize") {
  rex::memory::Blob b(new rsl::byte[5]{ 1, 2, 3, 4, 5 }, 5_bytes);
  b.zero_initialize();

  REX_CHECK(b.data()[0] == 0);
  REX_CHECK(b.data()[1] == 0);
  REX_CHECK(b.data()[2] == 0);
  REX_CHECK(b.data()[3] == 0);
  REX_CHECK(b.data()[4] == 0);
}

TEST_CASE("Blob - Blob read and write") {
  rex::memory::Blob b;
  b.allocate(4_bytes);
  int num = 12345678;
  b.write(&num, sizeof(num));

  int readNum = b.read<int>();
  REX_CHECK(readNum == 12345678);
}