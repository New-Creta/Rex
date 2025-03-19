#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/memory/blob.h"
#include "rex_engine/engine/types.h"

#include "rex_unit_test/test_allocator.h"

TEST_CASE("TEST - Blob - Default Creation")
{
  rex::memory::Blob blob;

  REX_CHECK(blob.size() == 0);
  REX_CHECK(blob.data() == nullptr);
}

TEST_CASE("TEST - Blob - Blob move construction") 
{
  rex::memory::Blob b1(rsl::make_unique<rsl::byte[]>(10));
  rex::memory::Blob b2(std::move(b1));

  REX_CHECK(static_cast<bool>(b1) == false);
  REX_CHECK(b1.size() == 0_bytes);
  REX_CHECK(b1.data() == nullptr);
  REX_CHECK(static_cast<bool>(b2) == true);
  REX_CHECK(b2.size() == 10_bytes);
  REX_CHECK(b2.data() != nullptr);
}

TEST_CASE("TEST - Blob - Construction from byte array")
{
  auto arr = rsl::make_unique<rsl::byte[]>(3);
  arr[0] = static_cast<rsl::byte>(1);
  arr[1] = static_cast<rsl::byte>(2);
  arr[2] = static_cast<rsl::byte>(3);

  rsl::byte* byte_array = arr.get();
  rex::memory::Blob b(rsl::move(arr));
  REX_CHECK(static_cast<bool>(b) == true);
  REX_CHECK(b.size() == 3_bytes);
  REX_CHECK(b.data() == byte_array);
  REX_CHECK(static_cast<s32>(b[0]) == 1);
  REX_CHECK(static_cast<s32>(b[1]) == 2);
  REX_CHECK(static_cast<s32>(b[2]) == 3);
}

TEST_CASE("TEST - Blob - Construction from typed array") 
{
  auto arr = rsl::make_unique<s32[]>(3);
  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;

  s32* int_array = arr.get();
  rex::memory::Blob b(rsl::move(arr));
  REX_CHECK(static_cast<bool>(b) == true);
  REX_CHECK(b.size() == 3 * sizeof(s32));
  REX_CHECK(b.data() == (rsl::byte*)int_array);
  REX_CHECK(*reinterpret_cast<const s32*>(b.data() + sizeof(s32) * 0) == 1);
  REX_CHECK(*reinterpret_cast<const s32*>(b.data() + sizeof(s32) * 1) == 2);
  REX_CHECK(*reinterpret_cast<const s32*>(b.data() + sizeof(s32) * 2) == 3);
}

TEST_CASE("TEST - Blob - Move assignment")
{
  rex::memory::Blob b1(rsl::make_unique<rsl::byte[]>(10));
  rex::memory::Blob b2;

  b2 = rsl::move(b1);

  REX_CHECK(static_cast<bool>(b1) == false);
  REX_CHECK(b1.size() == 0_bytes);
  REX_CHECK(b1.data() == nullptr);
  REX_CHECK(static_cast<bool>(b2) == true);
  REX_CHECK(b2.size() == 10_bytes);
  REX_CHECK(b2.data() != nullptr);
}

TEST_CASE("TEST - Blob - Blob zero_initialize") 
{
  auto arr = rsl::make_unique<s32[]>(3);
  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;

  rex::memory::Blob b(rsl::move(arr));
  b.zero_initialize();

  REX_CHECK(static_cast<s32>(b[0]) == 0);
  REX_CHECK(static_cast<s32>(b[1]) == 0);
  REX_CHECK(static_cast<s32>(b[2]) == 0);
}

TEST_CASE("TEST - Blob - Blob data_as")
{
  {
    auto arr = rsl::make_unique<s32[]>(1);
    arr[0] = 1;

    rex::memory::Blob b(rsl::move(arr));

    REX_CHECK(static_cast<s32>(*b.data_as<s32>()) == 1);
  }

  {
    struct SomeVariables
    {
      s32 x;
      s32 y;
      s32 z;
    };

    SomeVariables vars{};
    vars.x = 1;
    vars.y = 2;
    vars.z = 3;

    rsl::unique_array<rsl::byte> byte_arr = rsl::make_unique<rsl::byte[]>(sizeof(vars));
    rsl::memcpy(byte_arr.get(), &vars, sizeof(vars));
    rex::memory::Blob b(rsl::move(byte_arr));

    const SomeVariables* vars2 = b.data_as<SomeVariables>();
    REX_CHECK(vars.x == vars2->x);
    REX_CHECK(vars.y == vars2->y);
    REX_CHECK(vars.z == vars2->z);
  }
}

TEST_CASE("TEST - Blob - reading")
{
  auto arr = rsl::make_unique<s32[]>(3);
  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;

  rex::memory::Blob b(rsl::move(arr));

  REX_CHECK(b.read<s32>(0) == 1);
  REX_CHECK(b.read<s32>(4) == 2);
  REX_CHECK(b.read<s32>(8) == 3);

  s32 x = 0;
  b.read_bytes(&x, sizeof(x), 0);
  REX_CHECK(x == 1);
  b.read_bytes(&x, sizeof(x), 4);
  REX_CHECK(x == 2);
  b.read_bytes(&x, sizeof(x), 8);
  REX_CHECK(x == 3);
}

TEST_CASE("TEST - Blob - writing")
{
  rex::memory::Blob b(rsl::make_unique<s32[]>(3));
  
  s32 x = 1;
  b.write(&x, sizeof(x), 0);
  REX_CHECK(b.read<s32>(0) == x);
  x = 2;
  b.write(&x, sizeof(x), 4);
  REX_CHECK(b.read<s32>(4) == x);
  x = 3;
  b.write(&x, sizeof(x), 8);
  REX_CHECK(b.read<s32>(8) == x);
}

TEST_CASE("Test - Blob - release as array")
{
  auto arr = rsl::make_unique<rsl::byte[]>(3);
  arr[0] = static_cast<rsl::byte>(1);
  arr[1] = static_cast<rsl::byte>(2);
  arr[2] = static_cast<rsl::byte>(3);

  rex::memory::Blob b(rsl::move(arr));

  rsl::unique_array<rsl::byte> bytes = b.release_as_array<rsl::byte>();
  REX_CHECK(bytes[0] == rsl::byte(1));
  REX_CHECK(bytes[1] == rsl::byte(2));
  REX_CHECK(bytes[2] == rsl::byte(3));
}

TEST_CASE("Test - Blob - release")
{
  auto arr = rsl::make_unique<rsl::byte[]>(4);
  arr[0] = static_cast<rsl::byte>(1);
  arr[1] = static_cast<rsl::byte>(2);
  arr[2] = static_cast<rsl::byte>(3);
  arr[3] = static_cast<rsl::byte>(4);

  rex::memory::Blob b(rsl::move(arr));

  s32 value = b.release_as<s32>();
  REX_CHECK(value == 0x04030201);
}
