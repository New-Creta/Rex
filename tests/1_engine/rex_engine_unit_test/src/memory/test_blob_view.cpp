#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/memory/blob.h"
#include "rex_engine/memory/blob_view.h"
#include "rex_engine/engine/types.h"

TEST_CASE("Blob View - default construction")
{
  rex::memory::BlobView view;

  REX_CHECK(view.data() == nullptr);
  REX_CHECK(view.size() == 0);
}

TEST_CASE("Blob view - construct from blob")
{
  auto data = rsl::make_unique<s32[]>(1);
  data[0] = 1;
  rex::memory::Blob b1(rsl::move(data));
  auto ptr = b1.data();
  auto size = b1.size();
  rex::memory::BlobView view(b1);

  REX_CHECK(view.data() == ptr);
  REX_CHECK(view.size() == size);
  REX_CHECK(*view.data_as<s32>() == 1);
  REX_CHECK(view.read<s32>() == 1);
}

TEST_CASE("Blob view - construct from ptr and size")
{
  auto data = rsl::make_unique<s32[]>(1);
  data[0] = 1;
  auto ptr = (rsl::byte*)data.get();
  auto size = data.byte_size();
  rex::memory::BlobView view(data.get(), data.byte_size());

  REX_CHECK(view.data() == ptr);
  REX_CHECK(view.size() == size);
  REX_CHECK(*view.data_as<s32>() == 1);
  REX_CHECK(view.read<s32>() == 1);
}

TEST_CASE("Blob view - copy construction")
{
  auto data = rsl::make_unique<s32[]>(1);
  data[0] = 1;
  rex::memory::Blob b1(rsl::move(data));
  auto ptr = b1.data();
  auto size = b1.size();
  rex::memory::BlobView view(b1);
  rex::memory::BlobView view2(view);

  REX_CHECK(view.data() == ptr);
  REX_CHECK(view.size() == size);
  REX_CHECK(*view.data_as<s32>() == 1);
  REX_CHECK(view.read<s32>() == 1);

  REX_CHECK(view2.data() == ptr);
  REX_CHECK(view2.size() == size);
  REX_CHECK(*view2.data_as<s32>() == 1);
  REX_CHECK(view2.read<s32>() == 1);
}

TEST_CASE("Blow view - move construction")
{
  auto data = rsl::make_unique<s32[]>(1);
  data[0] = 1;
  rex::memory::Blob b1(rsl::move(data));
  auto ptr = b1.data();
  auto size = b1.size();
  rex::memory::BlobView view(b1);
  rex::memory::BlobView view2(rsl::move(view));

  REX_CHECK(view.data() == ptr);
  REX_CHECK(view.size() == size);
  REX_CHECK(*view.data_as<s32>() == 1);
  REX_CHECK(view.read<s32>() == 1);

  REX_CHECK(view2.data() == ptr);
  REX_CHECK(view2.size() == size);
  REX_CHECK(*view2.data_as<s32>() == 1);
  REX_CHECK(view2.read<s32>() == 1);
}

TEST_CASE("BlobView - Copy assignment")
{
  auto data = rsl::make_unique<s32[]>(1);
  data[0] = 1;
  rex::memory::Blob b1(rsl::move(data));
  auto ptr = b1.data();
  auto size = b1.size();
  rex::memory::BlobView view(b1);
  rex::memory::BlobView view2;
  view2 = view;

  REX_CHECK(view.data() == ptr);
  REX_CHECK(view.size() == size);
  REX_CHECK(*view.data_as<s32>() == 1);
  REX_CHECK(view.read<s32>() == 1);

  REX_CHECK(view2.data() == ptr);
  REX_CHECK(view2.size() == size);
  REX_CHECK(*view2.data_as<s32>() == 1);
  REX_CHECK(view2.read<s32>() == 1);

  auto data_2 = rsl::make_unique<s32[]>(1);
  data_2[0] = 1;
  rex::memory::Blob b2(rsl::move(data_2));
  auto ptr_2 = b2.data();
  auto size_2 = b2.size();
  rex::memory::BlobView view3(b2);
  view2 = view3;

  REX_CHECK(view.data() == ptr);
  REX_CHECK(view.size() == size);
  REX_CHECK(*view.data_as<s32>() == 1);
  REX_CHECK(view.read<s32>() == 1);

  REX_CHECK(view2.data() == ptr_2);
  REX_CHECK(view2.size() == size_2);
  REX_CHECK(*view2.data_as<s32>() == 2);
  REX_CHECK(view2.read<s32>() == 2);

  REX_CHECK(view3.data() == ptr_2);
  REX_CHECK(view3.size() == size_2);
  REX_CHECK(*view3.data_as<s32>() == 2);
  REX_CHECK(view3.read<s32>() == 2);
}

TEST_CASE("BlobView - Move assignment")
{
  auto data = rsl::make_unique<s32[]>(1);
  data[0] = 1;
  rex::memory::Blob b1(rsl::move(data));
  auto ptr = b1.data();
  auto size = b1.size();
  rex::memory::BlobView view(b1);
  rex::memory::BlobView view2;
  view2 = rsl::move(view);

  REX_CHECK(view.data() == ptr);
  REX_CHECK(view.size() == size);
  REX_CHECK(*view.data_as<s32>() == 1);
  REX_CHECK(view.read<s32>() == 1);

  REX_CHECK(view2.data() == ptr);
  REX_CHECK(view2.size() == size);
  REX_CHECK(*view2.data_as<s32>() == 1);
  REX_CHECK(view2.read<s32>() == 1);

  auto data_2 = rsl::make_unique<s32[]>(1);
  data_2[0] = 1;
  rex::memory::Blob b2(rsl::move(data_2));
  auto ptr_2 = b2.data();
  auto size_2 = b2.size();
  rex::memory::BlobView view3(b2);
  view2 = rsl::move(view3);

  REX_CHECK(view.data() == ptr);
  REX_CHECK(view.size() == size);
  REX_CHECK(*view.data_as<s32>() == 1);
  REX_CHECK(view.read<s32>() == 1);

  REX_CHECK(view2.data() == ptr_2);
  REX_CHECK(view2.size() == size_2);
  REX_CHECK(*view2.data_as<s32>() == 2);
  REX_CHECK(view2.read<s32>() == 2);

  REX_CHECK(view3.data() == ptr_2);
  REX_CHECK(view3.size() == size_2);
  REX_CHECK(*view3.data_as<s32>() == 2);
  REX_CHECK(view3.read<s32>() == 2);
}

TEST_CASE("BlobView - reading")
{
  auto arr = rsl::make_unique<s32[]>(3);
  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;

  rex::memory::Blob b(rsl::move(arr));
  rex::memory::BlobView view(b);

  REX_CHECK(view.read<s32>(0) == 1);
  REX_CHECK(view.read<s32>(4) == 2);
  REX_CHECK(view.read<s32>(8) == 3);

  s32 x = 0;
  view.read_bytes(&x, sizeof(x), 0);
  REX_CHECK(x == 1);
  view.read_bytes(&x, sizeof(x), 4);
  REX_CHECK(x == 2);
  view.read_bytes(&x, sizeof(x), 8);
  REX_CHECK(x == 3);
}
