#include "rex_directx/resources/shader_resource_view.h"
#include "rex_directx/resources/resource.h"

#include "rex_directx/directx_device.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace renderer
  {
    ShaderResourceView::ShaderResourceView(Device& device, const rsl::shared_ptr<Resource>& resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srv)
        : m_device(device)
        , m_resource(resource)
    {
      REX_ASSERT_X(resource || srv, "Invalid resource was given to ShaderResourceView");

      auto d3d_resource = m_resource ? m_resource->d3d_resource() : nullptr;
      auto d3d_device   = m_device.d3d_device();

      m_descriptor_allocation = m_device.allocate_descriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

      d3d_device->CreateShaderResourceView(d3d_resource.Get(), srv, m_descriptor_allocation.descriptor_handle());
    }

    ShaderResourceView::~ShaderResourceView() = default;

    rsl::shared_ptr<Resource> ShaderResourceView::resource() const
    {
      return m_resource;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE ShaderResourceView::descriptor_handle() const
    {
      return m_descriptor_allocation.descriptor_handle();
    }
  } // namespace renderer
} // namespace rex