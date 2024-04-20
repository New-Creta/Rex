#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/containers/ring_buffer.h"

TEST_CASE("Ringbuffer Construction")
{
  rex::RingBuffer<int> buffer(2);

  REX_CHECK(buffer.check() == nullptr);
  REX_CHECK(buffer.get() == nullptr);
}

TEST_CASE("Ringbuffer sequential putting and getting")
{
  rex::RingBuffer<int> buffer(2);

  // we can hold 2 elements, after this line, 1 element is available
  buffer.put(1);
  REX_CHECK(*buffer.check() == 1);
  REX_CHECK(*buffer.get() == 1);

  // this is the second element. we can get this, the next element will wrap
  buffer.put(2);
  REX_CHECK(*buffer.check() == 2);
  REX_CHECK(*buffer.get() == 2);
}

TEST_CASE("Ringbuffer first putting, then getting")
{
  rex::RingBuffer<int> buffer(2);

  // we can hold 2 elements, after this line, 1 element is available
  buffer.put(1);
  // this is the second element. we can get this, the next element will wrap
  buffer.put(2);

  // we get the first element, moving the read idx forward, now pointing to the second element
  REX_CHECK(*buffer.check() == 1);
  REX_CHECK(*buffer.get() == 1);

  // we write over the first element, leaving the second element still untouched and available for reading
  buffer.put(3);
  REX_CHECK(*buffer.check() == 2);
  REX_CHECK(*buffer.get() == 2);

  // checking again will wrap back to the first element
  REX_CHECK(*buffer.check() == 3);
  REX_CHECK(*buffer.get() == 3);

}

TEST_CASE("Ringbuffer using check")
{
  rex::RingBuffer<int> buffer(2);
  buffer.put(1);

  REX_CHECK(*buffer.check() == 1);
  REX_CHECK(*buffer.check() == 1);
}
