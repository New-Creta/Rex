//#include "rex_directx/resources/dx_render_target.h"
//
//namespace rex
//{
//  namespace rhi
//  {
//    DxRenderTarget::DxRenderTarget(const wrl::ComPtr<ID3D12Resource>& resource, DescriptorHandle descHandle)
//      : RenderTarget(resource->GetDesc().Width, resource->GetDesc().Height)
//      , m_resource(resource)
//      , m_desc_handle(descHandle)
//    {
//
//    }
//
//    D3D12_CPU_DESCRIPTOR_HANDLE DxRenderTarget::cpu_handle()
//    {
//      return m_desc_handle.get();
//    }
//
//  }
//}