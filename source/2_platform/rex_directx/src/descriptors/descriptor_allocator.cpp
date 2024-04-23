#include "rex_directx/descriptors/descriptor_allocator.h"
#include "rex_directx/descriptors/descriptor_allocator_page.h"
#include "rex_directx/directx_device.h"

namespace rex
{
  namespace renderer
  {
    namespace internal
    {
      struct MakeAllocatorPage : public DescriptorAllocatorPage
      {
      public:
        MakeAllocatorPage(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 numDescriptors)
            : DescriptorAllocatorPage(device, type, numDescriptors)
        {
        }

        ~MakeAllocatorPage() override = default;
      };
    } // namespace internal

    DescriptorAllocator::DescriptorAllocator(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 numDescriptorsPerHeap)
        : m_device(device)
        , m_heap_type(type)
        , m_num_descriptors_per_heap(numDescriptorsPerHeap)
    {
    }

    DescriptorAllocator::~DescriptorAllocator() {}

    DescriptorAllocation DescriptorAllocator::allocate(u32 numDescriptors)
    {
      rsl::unique_lock<rsl::mutex> lock(m_allocation_mutex);

      DescriptorAllocation allocation;

      for(auto iter = m_available_heaps.begin(); iter != m_available_heaps.end(); ++iter)
      {
        auto allocator_page = m_heap_pool[*iter];

        allocation = allocator_page->allocate(numDescriptors);

        if(allocator_page->num_free_handles() == 0)
        {
          iter = m_available_heaps.erase(iter);
        }

        // A valid allocation has been found.
        if(!allocation.is_null())
        {
          break;
        }
      }

      // No available heap could satisfy the requested number of descriptors.
      if(allocation.is_null())
      {
        m_num_descriptors_per_heap = (rsl::max)(m_num_descriptors_per_heap, numDescriptors);
        auto new_page              = create_allocator_page();

        allocation = new_page->allocate(numDescriptors);
      }

      return allocation;
    }

    void DescriptorAllocator::release_stale_descriptors()
    {
      rsl::unique_lock<rsl::mutex> lock(m_allocation_mutex);

      for(size_t i = 0; i < m_heap_pool.size(); ++i)
      {
        auto page = m_heap_pool[i];

        page->release_stale_descriptors();

        if(page->num_free_handles() > 0)
        {
          m_available_heaps.insert(i);
        }
      }
    }

    rsl::shared_ptr<DescriptorAllocatorPage> DescriptorAllocator::create_allocator_page()
    {
      auto new_page = rsl::make_shared<internal::MakeAllocatorPage>(m_device, m_heap_type, m_num_descriptors_per_heap);

      m_heap_pool.emplace_back(new_page);
      m_available_heaps.insert(m_heap_pool.size() - 1);

      return new_page;
    }
  } // namespace renderer
} // namespace rex