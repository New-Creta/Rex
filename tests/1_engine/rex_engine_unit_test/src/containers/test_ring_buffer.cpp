#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/containers/ring_buffer.h"

TEST_CASE("Ringbuffer Construction")
{
  rex::RingBuffer<int> buffer(2);

  REX_CHECK(buffer.count() == 0);
  REX_CHECK(buffer.max_count() == 2);
  REX_CHECK(buffer.size() == buffer.max_count() * sizeof(int));
}

TEST_CASE("Ringbuffer sequential writing and reading")
{
  rex::RingBuffer<int> buffer(2);

  // we can hold 2 elements, after this line, 1 element is available
  buffer.write(1);
  REX_CHECK(buffer.peek() == 1);
  REX_CHECK(buffer.count() == 1);
  REX_CHECK(buffer.read() == 1);
  REX_CHECK(buffer.count() == 0);

  // this is the second element. we can get this, the next element will wrap
  buffer.write(2);
  REX_CHECK(buffer.peek() == 2);
  REX_CHECK(buffer.count() == 1);
  REX_CHECK(buffer.read() == 2);
  REX_CHECK(buffer.count() == 0);
}

TEST_CASE("Ringbuffer first putting, then getting")
{
  rex::RingBuffer<int> buffer(2);

  // we can hold 2 elements, after this line, 1 element is available
  buffer.write(1);
  // this is the second element. we can get this, the next element will wrap
  buffer.write(2);

  // we get the first element, moving the read idx forward, now pointing to the second element
  REX_CHECK(buffer.count() == 2);
  REX_CHECK(buffer.peek() == 1);
  REX_CHECK(buffer.read() == 1);
  REX_CHECK(buffer.count() == 1);

  // we write over the first element, leaving the second element still untouched and available for reading
  buffer.write(3);
  REX_CHECK(buffer.count() == 2);
  REX_CHECK(buffer.peek() == 2);
  REX_CHECK(buffer.read() == 2);
  REX_CHECK(buffer.count() == 1);

  // checking again will wrap back to the first element
  REX_CHECK(buffer.peek() == 3);
  REX_CHECK(buffer.read() == 3);
  REX_CHECK(buffer.count() == 0);

}

TEST_CASE("Ringbuffer using peek")
{
  rex::RingBuffer<int> buffer(2);
  buffer.write(1);

  REX_CHECK(buffer.peek() == 1);
  REX_CHECK(buffer.peek() == 1);
  REX_CHECK(buffer.count() == 1);

  buffer.write(1);
  REX_CHECK(buffer.peek() == 1);
  REX_CHECK(buffer.count() == 2);
}

TEST_CASE("Ringbuffer Resetting")
{
  rex::RingBuffer<int> buffer(20);

  buffer.write(1);
  buffer.write(2);

  buffer.reset();

  REX_CHECK(buffer.count() == 0);

  buffer.write(3);
  buffer.write(4);

  REX_CHECK(buffer.count() == 2);

  REX_CHECK(buffer.peek() == 3);
  REX_CHECK(buffer.read() == 3);

  REX_CHECK(buffer.peek() == 4);
  REX_CHECK(buffer.read() == 4);
  REX_CHECK(buffer.count() == 0);
}
