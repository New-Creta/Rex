#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/memory/pointer_math.h"
#include "rex_engine/engine/types.h"
#include "rex_std/memory.h"

TEST_CASE("Pointer math jumping forward - positive number")
{
  rsl::unique_array<rsl::byte> data = rsl::make_unique<rsl::byte[]>(5);

  rsl::byte* og_ptr = data.get();
  rsl::byte* ptr = og_ptr;
  
  ptr = (rsl::byte*)rex::jump_forward(ptr, 1);
  REX_CHECK(ptr - og_ptr == 1);

  ptr = (rsl::byte*)rex::jump_forward(ptr, 5);
  REX_CHECK(ptr - og_ptr == 6);

  ptr = (rsl::byte*)rex::jump_forward(ptr, 15);
  REX_CHECK(ptr - og_ptr == 21);
}
TEST_CASE("Pointer math jumping forward - negative number")
{
  rsl::unique_array<rsl::byte> data = rsl::make_unique<rsl::byte[]>(5);

  rsl::byte* og_ptr = data.get() + data.byte_size();
  rsl::byte* ptr = og_ptr;

  ptr = (rsl::byte*)rex::jump_forward(ptr, -1);
  REX_CHECK(og_ptr - ptr == 1);

  ptr = (rsl::byte*)rex::jump_forward(ptr, -5);
  REX_CHECK(og_ptr - ptr == 6);

  ptr = (rsl::byte*)rex::jump_forward(ptr, -15);
  REX_CHECK(og_ptr - ptr == 21);
}

TEST_CASE("Pointer math jump backward - positive number")
{
  rsl::unique_array<rsl::byte> data = rsl::make_unique<rsl::byte[]>(5);

  rsl::byte* og_ptr = data.get() + data.byte_size();
  rsl::byte* ptr = og_ptr;

  ptr = (rsl::byte*)rex::jump_backward(ptr, 1);
  REX_CHECK(og_ptr - ptr == 1);

  ptr = (rsl::byte*)rex::jump_backward(ptr, 5);
  REX_CHECK(og_ptr - ptr == 6);

  ptr = (rsl::byte*)rex::jump_backward(ptr, 15);
  REX_CHECK(og_ptr - ptr == 21);
}
TEST_CASE("Pointer math jump backward - negative number")
{
  rsl::unique_array<rsl::byte> data = rsl::make_unique<rsl::byte[]>(5);

  rsl::byte* og_ptr = data.get();
  rsl::byte* ptr = og_ptr;

  ptr = (rsl::byte*)rex::jump_backward(ptr, -1);
  REX_CHECK(ptr - og_ptr == 1);

  ptr = (rsl::byte*)rex::jump_backward(ptr, -5);
  REX_CHECK(ptr - og_ptr == 6);

  ptr = (rsl::byte*)rex::jump_backward(ptr, -15);
  REX_CHECK(ptr - og_ptr == 21);
}

TEST_CASE("Pointer math alignment")
{
  s32 size = 4;

  REX_CHECK(rex::align(size, 256) == 256);
  REX_CHECK(rex::align(size, 1024) == 1024);
  REX_CHECK(rex::align(size, 1) == 4);
  REX_CHECK(rex::align(size, 4) == 4);
}