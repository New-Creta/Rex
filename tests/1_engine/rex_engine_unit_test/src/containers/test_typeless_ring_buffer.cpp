#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/containers/typeless_ring_buffer.h"

TEST_CASE("TEST - TypelessRingBuffer - basic operations")
{
  rex::TypelessRingBuffer ring_buffer(10_bytes);
  REX_CHECK(ring_buffer.size() == 10);
  REX_CHECK(ring_buffer.max_count() == 10);
  REX_CHECK(ring_buffer.count() == 0);

  s32 x = 0xFFEEDDCC;
  ring_buffer.write(&x, sizeof(x));
  REX_CHECK(ring_buffer.count() == sizeof(x));

  s32 y = 0;
  ring_buffer.peek(&y, sizeof(y));
  REX_CHECK(y == x);
  REX_CHECK(ring_buffer.count() == sizeof(x));

  s32 z = 0;
  ring_buffer.read(&z, sizeof(z));
  REX_CHECK(z == x);
  REX_CHECK(ring_buffer.count() == 0);
}

TEST_CASE("TEST - TypelessRingBuffer - wrapping")
{
  rex::TypelessRingBuffer ring_buffer(12_bytes);

  s32 a = 0x00112233;
  s32 b = 0x44556677;
  s32 c = 0x8899AABB;
  ring_buffer.write(&a, sizeof(a));
  ring_buffer.write(&b, sizeof(b));
  ring_buffer.write(&c, sizeof(c));
  REX_CHECK(ring_buffer.count() == sizeof(a) + sizeof(b) + sizeof(c));

  s32 z, y, x;
  ring_buffer.read(&z, sizeof(z));
  ring_buffer.read(&y, sizeof(y));
  ring_buffer.read(&x, sizeof(x));
  REX_CHECK(z == a);
  REX_CHECK(y == b);
  REX_CHECK(x == c);
  REX_CHECK(ring_buffer.count() == 0);

  ring_buffer.write(&c, sizeof(c));
  ring_buffer.write(&b, sizeof(b));
  ring_buffer.write(&a, sizeof(a));
  REX_CHECK(ring_buffer.count() == sizeof(a) + sizeof(b) + sizeof(c));

  ring_buffer.read(&z, sizeof(z));
  ring_buffer.read(&y, sizeof(y));
  ring_buffer.read(&x, sizeof(x));
  REX_CHECK(ring_buffer.count() == 0);

  REX_CHECK(z == c);
  REX_CHECK(y == b);
  REX_CHECK(x == a);
}

TEST_CASE("TEST - TypelessRingBuffer - wrapping by splitting data")
{
  rex::TypelessRingBuffer ring_buffer(10_bytes);

  s32 a = 0x00112233;
  s32 b = 0x44556677;
  s32 c = 0x8899AABB;
  ring_buffer.write(&a, sizeof(a));
  ring_buffer.write(&b, sizeof(b));
  s32 z, y, x;
  ring_buffer.read(&z, sizeof(z));
  ring_buffer.read(&y, sizeof(y));
  REX_CHECK(z == a);
  REX_CHECK(y == b);

  ring_buffer.write(&c, sizeof(c));
  ring_buffer.read(&x, sizeof(x));
  REX_CHECK(x == c);

  ring_buffer.write(&c, sizeof(c));
  ring_buffer.write(&b, sizeof(b));
  ring_buffer.read(&z, sizeof(z));
  ring_buffer.read(&y, sizeof(y));
  REX_CHECK(z == c);
  REX_CHECK(y == b);

  ring_buffer.write(&a, sizeof(a));
  ring_buffer.read(&x, sizeof(x));
  REX_CHECK(x == a);
}

TEST_CASE("TEST - TypelessRingBuffer - resetting")
{
  rex::TypelessRingBuffer ring_buffer(24_bytes);

  s32 a = 0x00112233;
  s32 b = 0x44556677;
  s32 c = 0x8899AABB;
  ring_buffer.write(&a, sizeof(a));
  ring_buffer.write(&b, sizeof(b));
  ring_buffer.write(&c, sizeof(c));
  REX_CHECK(ring_buffer.count() == sizeof(a) + sizeof(b) + sizeof(c));

  s32 z, y, x;
  ring_buffer.read(&z, sizeof(z));
  ring_buffer.read(&y, sizeof(y));
  ring_buffer.read(&x, sizeof(x));
  REX_CHECK(ring_buffer.count() == 0);
  REX_CHECK(x == c);
  REX_CHECK(y == b);
  REX_CHECK(z == a);

  ring_buffer.reset();
  REX_CHECK(ring_buffer.count() == 0);
  REX_CHECK(z == a);
  REX_CHECK(y == b);
  REX_CHECK(x == c);

  ring_buffer.write(&c, sizeof(c));
  ring_buffer.write(&b, sizeof(b));
  ring_buffer.write(&a, sizeof(a));
  REX_CHECK(ring_buffer.count() == sizeof(a) + sizeof(b) + sizeof(c));

  ring_buffer.reset();
  REX_CHECK(ring_buffer.count() == 0);

  ring_buffer.write(&c, sizeof(c));
  ring_buffer.write(&b, sizeof(b));
  ring_buffer.write(&a, sizeof(a));
  REX_CHECK(ring_buffer.count() == sizeof(a) + sizeof(b) + sizeof(c));

  ring_buffer.read(&z, sizeof(z));
  ring_buffer.read(&y, sizeof(y));
  ring_buffer.read(&x, sizeof(x));
  REX_CHECK(ring_buffer.count() == 0);

  REX_CHECK(z == c);
  REX_CHECK(y == b);
  REX_CHECK(x == a);
}

TEST_CASE("TEST - TypelessRingBuffer - skipping")
{
  rex::TypelessRingBuffer ring_buffer(24_bytes);

  s32 a = 0x00112233;
  s32 b = 0x44556677;
  s32 c = 0x8899AABB;
  ring_buffer.write(&a, sizeof(a));
  ring_buffer.write(&b, sizeof(b));
  ring_buffer.write(&c, sizeof(c));
  REX_CHECK(ring_buffer.count() == sizeof(a) + sizeof(b) + sizeof(c));

  s32 z, y, x;
  ring_buffer.read(&z, sizeof(z));
  ring_buffer.skip(sizeof(y));
  ring_buffer.read(&x, sizeof(x));

  REX_CHECK(z == a);
  REX_CHECK(x == c);
  REX_CHECK(ring_buffer.count() == 0);

  ring_buffer.write(&a, sizeof(a));
  ring_buffer.write(&b, sizeof(b));
  ring_buffer.write(&c, sizeof(c));
}