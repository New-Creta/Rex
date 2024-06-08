#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_directx/system/dx_descriptor_heap.h"

#include "rex_renderer_core/resources/render_target.h"

namespace rex
{
  namespace rhi
  {
    class DxRenderTarget : public RenderTarget
    {
    public:
      DxRenderTarget(const wrl::ComPtr<ID3D12Resource>& resource, DescriptorHandle descHandle)
        : RenderTarget(resource->GetDesc().Width, resource->GetDesc().Height)
        , m_resource(resource)
        , m_desc_handle(descHandle)
      {}

      ID3D12Resource* dx_object()
      {
        return m_resource.Get();
      }

      DescriptorHandle handle() const
      {
        return m_desc_handle;
      }

    private:
      wrl::ComPtr<ID3D12Resource> m_resource;
      DescriptorHandle m_desc_handle;
    };
  }
}