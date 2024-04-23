#include "rex_directx/resources/upload_buffer.h"
#include "rex_directx/directx_call.h"
#include "rex_directx/directx_device.h"

#include "rex_engine/memory/pointer_math.h"
#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace renderer
  {
    UploadBuffer::UploadBuffer(Device& device, rsl::memory_size pageSize)
        : m_device(device)
        , m_page_size(pageSize)
    {
    }

    UploadBuffer::~UploadBuffer() = default;

    s64 UploadBuffer::page_size() const
    {
      return m_page_size;
    }

    UploadBuffer::Allocation UploadBuffer::allocate(rsl::memory_size sizeInBytes, s64 alignment)
    {
      REX_ASSERT_X(sizeInBytes <= m_page_size, "bad allocation");

      // If there is no current Page, or the requested allocation exceeds the
      // remaining space in the current Page, request a new Page.
      if(!m_current_page || !m_current_page->has_space(sizeInBytes, alignment))
      {
        m_current_page = request_page();
      }

      return m_current_page->allocate(sizeInBytes, alignment);
    }

    void UploadBuffer::reset()
    {
      m_current_page = nullptr;

      // Reset all available pages.
      m_available_pages = m_page_pool;

      for(auto a_page: m_available_pages)
      {
        // Reset the Page for new allocations.
        a_page->reset();
      }
    }

    rsl::shared_ptr<UploadBuffer::Page> UploadBuffer::request_page()
    {
      rsl::shared_ptr<Page> new_page;

      if(!m_available_pages.empty())
      {
        new_page = m_available_pages.front();
        m_available_pages.pop_front();
      }
      else
      {
        new_page = rsl::make_shared<Page>(m_device, m_page_size);
        m_page_pool.push_back(new_page);
      }

      return new_page;
    }

    UploadBuffer::Page::Page(Device& device, rsl::memory_size sizeInBytes)
        : m_device(device)
        , m_page_size(sizeInBytes)
        , m_offset(0)
        , m_CPU_ptr(nullptr)
        , m_GPU_ptr(D3D12_GPU_VIRTUAL_ADDRESS(0))
    {
      auto d3d_device = m_device.d3d_device();

      auto default_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
      auto default_resource_desc_buffer = CD3DX12_RESOURCE_DESC::Buffer(m_page_size);

      if(DX_FAILED(d3d_device->CreateCommittedResource(&default_heap_properties, D3D12_HEAP_FLAG_NONE, &default_resource_desc_buffer, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_d3d12_resource))))
      {
        REX_ASSERT("Failed to create UploadBuffer Page");
      }

      m_d3d12_resource->SetName(L"Upload Buffer (Page)");

      m_GPU_ptr = m_d3d12_resource->GetGPUVirtualAddress();
      m_d3d12_resource->Map(0, nullptr, &m_CPU_ptr);
    }

    UploadBuffer::Page::~Page()
    {
      m_d3d12_resource->Unmap(0, nullptr);
      m_CPU_ptr = nullptr;
      m_GPU_ptr = D3D12_GPU_VIRTUAL_ADDRESS(0);
    }

    bool UploadBuffer::Page::has_space(rsl::memory_size sizeInBytes, s64 alignment) const
    {
      auto aligned_size   = (s64)align_up((s64)sizeInBytes.size_in_bytes(), alignment);
      auto aligned_offset = (s64)align_up((s64)m_offset, alignment);

      return aligned_offset + aligned_size <= m_page_size.size_in_bytes();
    }

    UploadBuffer::Allocation UploadBuffer::Page::allocate(rsl::memory_size sizeInBytes, s64 alignment)
    {
      // function is not thread safe! insert a rsl::unique_lock if thread safety is a requirement.
      //
      // since UploadBuffer class instances are not used across multiple threads this is unnecessay
      // overhead to incorperate this functionality

      REX_ASSERT_X(has_space(sizeInBytes, alignment), "Can't allocator space from Page");

      auto aligned_size = align_up((s64)sizeInBytes.size_in_bytes(), alignment);

      m_offset = align_up((s64)m_offset, alignment);

      UploadBuffer::Allocation allocation;
      allocation.CPU = static_cast<u8*>(m_CPU_ptr) + m_offset;
      allocation.GPU = m_GPU_ptr + m_offset;

      m_offset += aligned_size;

      return allocation;
    }

    void UploadBuffer::Page::reset()
    {
      m_offset = 0;
    }
  } // namespace renderer
} // namespace rex