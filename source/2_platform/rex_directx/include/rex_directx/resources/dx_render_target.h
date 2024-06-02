//#pragma once
//
//#include "rex_directx/utility/dx_util.h"
//
//#include "rex_renderer_core/resources/render_target.h"
//#include "rex_directx/system/dx_descriptor_heap.h"
//
//namespace rex
//{
//  namespace rhi
//  {
//    class DxRenderTarget : public RenderTarget
//    {
//    public:
//      DxRenderTarget(const wrl::ComPtr<ID3D12Resource>& resource, DescriptorHandle descHandle);
//
//      D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle();
//
//    private:
//      wrl::ComPtr<ID3D12Resource> m_resource;
//      DescriptorHandle m_desc_handle;
//    };
//  }
//}