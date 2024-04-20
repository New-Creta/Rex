#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/memory/blob_reader.h"
#include "rex_engine/memory/blob.h"
#include "rex_engine/engine/types.h"

TEST_CASE("BlobReader - Read from blob")
{
  auto data = rsl::make_unique<s32[]>(5);
  data[0] = 1;
  data[1] = 2;
  data[2] = 3;
  data[3] = 4;
  data[4] = 5;

  rex::memory::Blob blob(rsl::move(data));
  rex::memory::BlobReader reader(blob);

  REX_CHECK(reader.read_offset() == sizeof(s32) * 0);
  REX_CHECK(reader.read<s32>() == 1);
  REX_CHECK(reader.read_offset() == sizeof(s32) * 1);
  REX_CHECK(reader.read<s32>() == 2);
  REX_CHECK(reader.read_offset() == sizeof(s32) * 2);
  REX_CHECK(reader.read<s32>() == 3);
  REX_CHECK(reader.read_offset() == sizeof(s32) * 3);
  REX_CHECK(reader.read<s32>() == 4);
  REX_CHECK(reader.read_offset() == sizeof(s32) * 4);
  REX_CHECK(reader.read<s32>() == 5);
  REX_CHECK(reader.read_offset() == sizeof(s32) * 5);
}

TEST_CASE("BlobReader - Read from blob from offset")
{
  auto data = rsl::make_unique<s32[]>(5);
  data[0] = 1;
  data[1] = 2;
  data[2] = 3;
  data[3] = 4;
  data[4] = 5;

  rex::memory::Blob blob(rsl::move(data));
  rex::memory::BlobReader reader(blob, sizeof(s32));

  REX_CHECK(reader.read_offset() == sizeof(s32) * 1);
  REX_CHECK(reader.read<s32>() == 2);
  REX_CHECK(reader.read_offset() == sizeof(s32) * 2);
  REX_CHECK(reader.read<s32>() == 3);
  REX_CHECK(reader.read_offset() == sizeof(s32) * 3);
  REX_CHECK(reader.read<s32>() == 4);
  REX_CHECK(reader.read_offset() == sizeof(s32) * 4);
  REX_CHECK(reader.read<s32>() == 5);
  REX_CHECK(reader.read_offset() == sizeof(s32) * 5);
}

TEST_CASE("BlobReader - Read from blob with skipping")
{
  auto data = rsl::make_unique<s32[]>(5);
  data[0] = 1;
  data[1] = 2;
  data[2] = 3;
  data[3] = 4;
  data[4] = 5;

  rex::memory::Blob blob(rsl::move(data));
  rex::memory::BlobReader reader(blob);

  REX_CHECK(reader.read_offset() == sizeof(s32) * 0);
  REX_CHECK(reader.read<s32>() == 1);
  REX_CHECK(reader.read_offset() == sizeof(s32) * 1);
  reader.skip(sizeof(s32));
  REX_CHECK(reader.read_offset() == sizeof(s32) * 2);
  REX_CHECK(reader.read<s32>() == 3);
  REX_CHECK(reader.read_offset() == sizeof(s32) * 3);
  reader.skip(sizeof(s32));
  REX_CHECK(reader.read_offset() == sizeof(s32) * 4);
  REX_CHECK(reader.read<s32>() == 5);
  REX_CHECK(reader.read_offset() == sizeof(s32) * 5);
}

TEST_CASE("BlobReader - Read from blob from offset with skipping")
{
  auto data = rsl::make_unique<s32[]>(5);
  data[0] = 1;
  data[1] = 2;
  data[2] = 3;
  data[3] = 4;
  data[4] = 5;

  rex::memory::Blob blob(rsl::move(data));
  rex::memory::BlobReader reader(blob, sizeof(s32));

  REX_CHECK(reader.read_offset() == sizeof(s32) * 1);
  REX_CHECK(reader.read<s32>() == 2);
  REX_CHECK(reader.read_offset() == sizeof(s32) * 2);
  reader.skip(sizeof(s32));
  REX_CHECK(reader.read_offset() == sizeof(s32) * 3);
  reader.skip(sizeof(s32));
  REX_CHECK(reader.read_offset() == sizeof(s32) * 4);
  REX_CHECK(reader.read<s32>() == 5);
  REX_CHECK(reader.read_offset() == sizeof(s32) * 5);
}