//#pragma once
//
//#include "rex_renderer_core/resource_management/resource_slot.h"
//
//#include "rex_directx/system/dx_command_allocator.h"
//#include "rex_directx/utility/dx_util.h"
//
//#include "rex_engine/engine/types.h"
//
//#include "rex_std/memory.h"
//
//namespace rex
//{
//  namespace renderer
//  {
//    // We support multiple frames in flight at the same time.
//    // Therefore we need some resources that are tied to each frame that's currently in flight
//    class ImGuiFrameContext
//    {
//    public:
//      ImGuiFrameContext(ID3D12Device1* device);
//
//      rex::rhi::CommandAllocator* allocator();
//
//    private:
//      rsl::unique_ptr<rex::rhi::CommandAllocator> m_command_allocator;
//    };
//
//    struct ImGuiVertexConstantBuffer
//    {
//      f32   mvp[4][4];
//    };
//
//    // Buffers used during the rendering of a frame
//    struct ImGuiRenderBuffer
//    {
//      s32                 IndexBufferSize;    // the number of indices the index buffer supports
//      s32                 VertexBufferSize;   // the number of vertices the vertex buffer supports
//
//      rex::rhi::ResourceSlot vertex_buffer;   // resource slot of the vertex buffer
//      rex::rhi::ResourceSlot index_buffer;    // resource slot of the index buffer
//    };
//  }
//}