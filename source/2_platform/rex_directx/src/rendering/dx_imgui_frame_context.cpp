//#include "rex_directx/rendering/dx_imgui_frame_context.h"
//
//#include "rex_directx/diagnostics/dx_call.h"
//
//namespace rex
//{
//  namespace renderer
//  {
//    ImGuiFrameContext::ImGuiFrameContext(ID3D12Device1* device)
//    {
//      rex::wrl::ComPtr<ID3D12CommandAllocator> cmd_alloc;
//      DX_CALL(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmd_alloc.GetAddressOf())));
//      m_command_allocator = rsl::make_unique<rex::rhi::CommandAllocator>(cmd_alloc);
//    }
//
//    rex::rhi::CommandAllocator* ImGuiFrameContext::allocator()
//    {
//      return m_command_allocator.get();
//    }
//  }
//}