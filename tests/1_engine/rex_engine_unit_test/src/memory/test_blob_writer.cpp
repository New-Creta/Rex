#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/memory/blob_writer.h"
#include "rex_engine/memory/blob.h"
#include "rex_engine/engine/types.h"

TEST_CASE("BlobWriter - Write to blob")
{
  auto data = rsl::make_unique<s32[]>(5);

  rex::memory::Blob blob(rsl::move(data));
  rex::memory::BlobWriter writer(blob);

  REX_CHECK(writer.write_offset() == sizeof(s32) * 0);
  writer.write(1);
  REX_CHECK(writer.write_offset() == sizeof(s32) * 1);
  writer.write(2);
  REX_CHECK(writer.write_offset() == sizeof(s32) * 2);
  writer.write(3);
  REX_CHECK(writer.write_offset() == sizeof(s32) * 3);
  writer.write(4);
  REX_CHECK(writer.write_offset() == sizeof(s32) * 4);
  writer.write(5);
  REX_CHECK(writer.write_offset() == sizeof(s32) * 5);

  REX_CHECK(blob.read<s32>(0) == 1);
  REX_CHECK(blob.read<s32>(4) == 2);
  REX_CHECK(blob.read<s32>(8) == 3);
  REX_CHECK(blob.read<s32>(12) == 4);
  REX_CHECK(blob.read<s32>(16) == 5);
}

TEST_CASE("BlobWriter - Write to blob from offset")
{
  auto data = rsl::make_unique<s32[]>(5);

  rex::memory::Blob blob(rsl::move(data));
  blob.zero_initialize();
  rex::memory::BlobWriter writer(blob, sizeof(s32));

  REX_CHECK(writer.write_offset() == sizeof(s32) * 1);
  writer.write(2);
  REX_CHECK(writer.write_offset() == sizeof(s32) * 2);
  writer.write(3);
  REX_CHECK(writer.write_offset() == sizeof(s32) * 3);
  writer.write(4);
  REX_CHECK(writer.write_offset() == sizeof(s32) * 4);
  writer.write(5);
  REX_CHECK(writer.write_offset() == sizeof(s32) * 5);

  REX_CHECK(blob.read<s32>(0) == 0);
  REX_CHECK(blob.read<s32>(4) == 2);
  REX_CHECK(blob.read<s32>(8) == 3);
  REX_CHECK(blob.read<s32>(12) == 4);
  REX_CHECK(blob.read<s32>(16) == 5);
}

TEST_CASE("BlobWriter - Write to blob using ptr and size")
{
  auto data = rsl::make_unique<s32[]>(5);

  rex::memory::Blob blob(rsl::move(data));
  blob.zero_initialize();
  rex::memory::BlobWriter writer(blob);

  s32 x = 10;

  REX_CHECK(writer.write_offset() == sizeof(s32) * 0);
  writer.write(&x, sizeof(x));
  REX_CHECK(writer.write_offset() == sizeof(s32) * 1);
  x = 20;
  writer.write(&x, sizeof(x));
  REX_CHECK(writer.write_offset() == sizeof(s32) * 2);
  x = 30;
  writer.write(&x, sizeof(x));
  REX_CHECK(writer.write_offset() == sizeof(s32) * 3);
  x = 40;
  writer.write(&x, sizeof(x));
  REX_CHECK(writer.write_offset() == sizeof(s32) * 4);
  x = 50;
  writer.write(&x, sizeof(x));
  REX_CHECK(writer.write_offset() == sizeof(s32) * 5);

  REX_CHECK(blob.read<s32>(0) == 10);
  REX_CHECK(blob.read<s32>(4) == 20);
  REX_CHECK(blob.read<s32>(8) == 30);
  REX_CHECK(blob.read<s32>(12) == 40);
  REX_CHECK(blob.read<s32>(16) == 50);
}

TEST_CASE("BlobWriter - Write to blob using ptr and size from offset")
{
  auto data = rsl::make_unique<s32[]>(5);

  rex::memory::Blob blob(rsl::move(data));
  blob.zero_initialize();
  rex::memory::BlobWriter writer(blob, sizeof(s32));

  s32 x = 20;

  REX_CHECK(writer.write_offset() == sizeof(s32) * 1);
  writer.write(&x, sizeof(x));
  REX_CHECK(writer.write_offset() == sizeof(s32) * 2);
  x = 30;
  writer.write(&x, sizeof(x));
  REX_CHECK(writer.write_offset() == sizeof(s32) * 3);
  x = 40;
  writer.write(&x, sizeof(x));
  REX_CHECK(writer.write_offset() == sizeof(s32) * 4);
  x = 50;
  writer.write(&x, sizeof(x));
  REX_CHECK(writer.write_offset() == sizeof(s32) * 5);

  REX_CHECK(blob.read<s32>(0) == 0);
  REX_CHECK(blob.read<s32>(4) == 20);
  REX_CHECK(blob.read<s32>(8) == 30);
  REX_CHECK(blob.read<s32>(12) == 40);
  REX_CHECK(blob.read<s32>(16) == 50);
}