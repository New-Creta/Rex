#include "rex_directx/resources/unordered_access_view.h"
#include "rex_directx/resources/resource.h"

#include "rex_directx/directx_device.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace renderer
  {
    UnorderedAccessView::UnorderedAccessView(Device& device, const rsl::shared_ptr<Resource>& inResource, const rsl::shared_ptr<Resource>& inCounterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav)
        : m_device(device)
        , m_resource(inResource)
        , m_counter_resource(inCounterResource)
    {
      REX_ASSERT_X(m_resource || uav, "Invalid resource was given to the UnorderedAccessView");

      auto d3d_device           = m_device.d3d_device();
      auto d3d_resource         = m_resource ? m_resource->d3d_resource() : nullptr;
      auto d3d_counter_resource = m_counter_resource ? m_counter_resource->d3d_resource() : nullptr;

      if(m_resource)
      {
        auto d3d_resource_desc = m_resource->d3d_resource_desc();

        REX_ASSERT_X((d3d_resource_desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) != 0, "Resource must be created with the D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS flag");
      }

      m_descriptor_allocation = m_device.allocate_descriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

      d3d_device->CreateUnorderedAccessView(d3d_resource.Get(), d3d_counter_resource.Get(), uav, m_descriptor_allocation.descriptor_handle());
    }

    UnorderedAccessView::~UnorderedAccessView() = default;

    rsl::shared_ptr<Resource> UnorderedAccessView::resource() const
    {
      return m_resource;
    }

    rsl::shared_ptr<Resource> UnorderedAccessView::counter_resource() const
    {
      return m_counter_resource;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccessView::descriptor_handle() const
    {
      return m_descriptor_allocation.descriptor_handle();
    }
  } // namespace renderer
} // namespace rex