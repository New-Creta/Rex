#include "rex_renderer_resources/resource_pool.h"
#include "rex_unit_test/rex_catch2.h"

namespace rex
{
  namespace renderer
  {
    bool release_resource(const ResourceSlot& slot)
    {
      return true;
    }
  } // namespace renderer
} // namespace rex

TEST_CASE("ResourceSlot default constructor initializes correctly", "[ResourceSlot]")
{
  rex::renderer::ResourceSlot slot;
  REQUIRE_FALSE(slot.is_valid());
  REQUIRE_FALSE(slot.is_about_to_be_removed());
  REQUIRE(slot.slot_id() == rex::globals::g_invalid_slot_id);
}

TEST_CASE("ResourceSlot constructor with slotId initializes correctly", "[ResourceSlot]")
{
  rsl::function<bool(const rex::renderer::ResourceSlot&)> release_fn = [](const rex::renderer::ResourceSlot& slot) { return rex::renderer::release_resource(slot); };

  const int slotId = 123;
  rex::renderer::ResourceSlot slot(slotId, &release_fn);
  REQUIRE(slot.is_valid());
  REQUIRE_FALSE(slot.is_about_to_be_removed());
  REQUIRE(slot.slot_id() == slotId);
}

TEST_CASE("ResourceSlot copy constructor creates a new valid instance", "[ResourceSlot]")
{
  rsl::function<bool(const rex::renderer::ResourceSlot&)> release_fn = [](const rex::renderer::ResourceSlot& slot) { return rex::renderer::release_resource(slot); };

  const int slotId = 123;
  rex::renderer::ResourceSlot original(slotId, &release_fn);
  rex::renderer::ResourceSlot copy(original);
  REQUIRE(copy.is_valid());
  REQUIRE_FALSE(copy.is_about_to_be_removed());
  REQUIRE(copy.slot_id() == original.slot_id());
}

TEST_CASE("ResourceSlot move constructor leaves source invalid", "[ResourceSlot]")
{
  rsl::function<bool(const rex::renderer::ResourceSlot&)> release_fn = [](const rex::renderer::ResourceSlot& slot) { return rex::renderer::release_resource(slot); };

  const int slotId = 123;
  rex::renderer::ResourceSlot original(slotId, &release_fn);
  rex::renderer::ResourceSlot moved(std::move(original));
  REQUIRE(moved.is_valid());
  REQUIRE_FALSE(moved.is_about_to_be_removed());
  REQUIRE(moved.slot_id() == slotId);
  REQUIRE_FALSE(original.is_valid());
}

TEST_CASE("ResourceSlot copy assignment creates a new valid instance", "[ResourceSlot]")
{
  rsl::function<bool(const rex::renderer::ResourceSlot&)> release_fn = [](const rex::renderer::ResourceSlot& slot) { return rex::renderer::release_resource(slot); };

  const int slotId1 = 123;
  const int slotId2 = 456;
  rex::renderer::ResourceSlot slot1(slotId1, &release_fn);
  rex::renderer::ResourceSlot slot2(slotId2, &release_fn);
  slot2 = slot1;
  REQUIRE(slot2.is_valid());
  REQUIRE_FALSE(slot2.is_about_to_be_removed());
  REQUIRE(slot2.slot_id() == slot1.slot_id());
}

TEST_CASE("ResourceSlot move assignment leaves source invalid", "[ResourceSlot]")
{
  rsl::function<bool(const rex::renderer::ResourceSlot&)> release_fn = [](const rex::renderer::ResourceSlot& slot) { return rex::renderer::release_resource(slot); };

  const int slotId1 = 123;
  const int slotId2 = 456;
  rex::renderer::ResourceSlot slot1(slotId1, &release_fn);
  rex::renderer::ResourceSlot slot2(slotId2, &release_fn);
  slot2 = std::move(slot1);
  REQUIRE(slot2.is_valid());
  REQUIRE_FALSE(slot2.is_about_to_be_removed());
  REQUIRE(slot2.slot_id() == slotId1);
  REQUIRE_FALSE(slot1.is_valid());
}

TEST_CASE("ResourceSlot equality comparison", "[ResourceSlot]")
{
  rsl::function<bool(const rex::renderer::ResourceSlot&)> release_fn = [](const rex::renderer::ResourceSlot& slot) { return rex::renderer::release_resource(slot); };

  const int slotId1 = 123;
  const int slotId2 = 456;
  rex::renderer::ResourceSlot slot1(slotId1, &release_fn);
  rex::renderer::ResourceSlot slot2(slotId2, &release_fn);
  rex::renderer::ResourceSlot slot3(slotId1, &release_fn);
  REQUIRE(slot1 == slot3);
  REQUIRE_FALSE(slot1 == slot2);
  REQUIRE_FALSE(slot1 != slot3);
  REQUIRE(slot1 != slot2);
}

TEST_CASE("ResourceSlot release function", "[ResourceSlot]")
{
  rsl::function<bool(const rex::renderer::ResourceSlot&)> release_fn = [](const rex::renderer::ResourceSlot& slot) { return rex::renderer::release_resource(slot); };

  const int slotId = 123;
  rex::renderer::ResourceSlot slot(slotId, &release_fn);
  REQUIRE(slot.release() == 1);
  REQUIRE_FALSE(slot.is_valid());
  REQUIRE(slot.release() == 0);
  REQUIRE_FALSE(slot.is_valid());
}

TEST_CASE("ResourceSlot is_about_to_be_removed", "[ResourceSlot]")
{
  rsl::function<bool(const rex::renderer::ResourceSlot&)> release_fn = [](const rex::renderer::ResourceSlot& slot) { return rex::renderer::release_resource(slot); };

  const int slotId = 123;
  rex::renderer::ResourceSlot slot(slotId, &release_fn);
  REQUIRE_FALSE(slot.is_about_to_be_removed());
  slot.release();
  REQUIRE_FALSE(slot.is_valid());
  REQUIRE_FALSE(slot.is_about_to_be_removed());
  REQUIRE(slot.slot_id() == rex::globals::g_invalid_slot_id);
}
