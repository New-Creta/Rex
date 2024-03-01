#include "rex_renderer_resources/resource_pool.h"
#include "rex_unit_test/rex_catch2.h"

namespace rex
{
  namespace renderer
  {
    // Mock Resource class for testing
    class MockResource : public IResource
    {
    public:
      MockResource(int id)
          : id_(id)
      {
      }
      int id() const
      {
        return id_;
      }

      size_t type() const override
      {
        return 0;
      }

    private:
      int id_;
    };

  } // namespace renderer
} // namespace rex

// Test fixture for ResourcePool tests
class ResourcePoolTest
{
protected:
  void setup()
  {
    // Initialize ResourcePool with a reserved capacity
    resource_pool.initialize(10);
  }

  bool release(const rex::renderer::ResourceSlot& slot)
  {
    if(slot.is_about_to_be_removed() == false && resource_pool.has_slot(slot))
    {
      resource_pool.remove(slot);
    }

    return true;
  }

  void teardown()
  {
    // Clean up the ResourcePool after each test
    resource_pool.clear();
  }

  rex::renderer::ResourcePool resource_pool;
};

// Test ResourcePool initialization
TEST_CASE_METHOD(ResourcePoolTest, "ResourcePool Initialization")
{
  setup();

  // Check if ResourcePool is initialized with the correct reserved capacity
  REQUIRE(resource_pool.size() == 0);

  teardown();
}

// Test insertion of resources into ResourcePool
TEST_CASE_METHOD(ResourcePoolTest, "ResourcePool Insertion")
{
  setup();

  // Insert a resource
  rsl::function<bool(const rex::renderer::ResourceSlot&)> release_fn = [&](const rex::renderer::ResourceSlot& slot) { return release(slot); };

  rex::renderer::ResourceSlot slot(1, &release_fn); // Assuming slot ID is 1
  auto resource = rsl::make_unique<rex::renderer::MockResource>(1);
  resource_pool.insert(slot, rsl::move(resource));

  // Check if insertion is successful
  REQUIRE(resource_pool.has_slot(slot) == true);
  REQUIRE(resource_pool.size() == 1);

  teardown();
}

// Test removal of resources from ResourcePool
TEST_CASE_METHOD(ResourcePoolTest, "ResourcePool Removal")
{
  setup();

  // Insert a resource
  rsl::function<bool(const rex::renderer::ResourceSlot&)> release_fn = [&](const rex::renderer::ResourceSlot& slot) { return release(slot); };

  rex::renderer::ResourceSlot slot(1, &release_fn); // Assuming slot ID is 1
  auto resource = rsl::make_unique<rex::renderer::MockResource>(1);
  resource_pool.insert(slot, rsl::move(resource));

  // Remove the resource
  resource_pool.remove(slot);

  // Check if removal is successful
  REQUIRE(resource_pool.has_slot(slot) == false);
  REQUIRE(resource_pool.size() == 0);

  teardown();
}

// Test accessing resources from ResourcePool
TEST_CASE_METHOD(ResourcePoolTest, "ResourcePool Access")
{
  setup();

  // Insert a resource
  rsl::function<bool(const rex::renderer::ResourceSlot&)> release_fn = [&](const rex::renderer::ResourceSlot& slot) { return release(slot); };

  rex::renderer::ResourceSlot slot(1, &release_fn); // Assuming slot ID is 1
  auto resource = rsl::make_unique<rex::renderer::MockResource>(1);
  resource_pool.insert(slot, rsl::move(resource));

  // Access the resource
  auto& accessed_resource = resource_pool.at(slot);

  // Convert to mock resource
  rex::renderer::MockResource* accessed_resource_ptr = static_cast<rex::renderer::MockResource*>(accessed_resource.get());

  // Check if accessed resource matches the inserted resource
  REQUIRE(accessed_resource_ptr->id() == 1);

  teardown();
}

// Test accessing resources from ResourcePool with const reference
TEST_CASE_METHOD(ResourcePoolTest, "ResourcePool Const Access")
{
  setup();

  // Insert a resource
  rsl::function<bool(const rex::renderer::ResourceSlot&)> release_fn = [&](const rex::renderer::ResourceSlot& slot) { return release(slot); };

  rex::renderer::ResourceSlot slot(1, &release_fn); // Assuming slot ID is 1
  auto resource = rsl::make_unique<rex::renderer::MockResource>(1);
  resource_pool.insert(slot, rsl::move(resource));

  // Access the resource with const reference
  const auto& accessed_resource = resource_pool.at(slot);

  // Convert to mock resource
  const rex::renderer::MockResource* accessed_resource_ptr = static_cast<const rex::renderer::MockResource*>(accessed_resource.get());

  // Check if accessed resource matches the inserted resource
  REQUIRE(accessed_resource_ptr->id() == 1);

  teardown();
}
