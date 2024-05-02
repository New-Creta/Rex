//#pragma once
//
//#include "rex_directx/utility/d3dx12.h"
//#include "rex_directx/utility/dx_util.h"
//#include "imgui/imgui.h"
//
//#include "rex_renderer_core/resource_management/resource_slot.h"
//#include "rex_directx/system/dx_descriptor_heap.h"
//
//#include "rex_engine/diagnostics/error.h"
//
//namespace rex
//{
//  namespace renderer
//  {
//    namespace imgui
//    {
//      struct ImGuiFrameResource
//      {
//        rhi::ResourceSlot vertex_buffer_slot;
//        rhi::ResourceSlot index_buffer_slot;
//
//        s32                 IndexBufferSize;
//        s32                 VertexBufferSize;
//      };
//
//      class ImGuiRenderer
//      {
//      public:
//        ImGuiRenderer(HWND hwnd, DXGI_FORMAT rtvFormat, s32 maxFramesInFlight);
//
//        void new_frame();
//        void render_draw_data(ImDrawData* drawData, ID3D12GraphicsCommandList* ctx);
//
//      private:
//        void setup_render_state(ImDrawData* drawData, ID3D12GraphicsCommandList* ctx, ImGuiFrameResource& fr);
//        Error create_vertex_buffer(ImGuiFrameResource& fr, s32 size);
//        Error create_index_buffer(ImGuiFrameResource& fr, s32 size);
//        Error create_gfx_resources();
//        Error create_constant_buffer();
//        Error create_shader();
//        Error create_pso();
//        Error create_fonts_texture();
//
//      private:
//        wrl::ComPtr<ID3DBlob> m_vs_blob;
//        wrl::ComPtr<ID3DBlob> m_ps_blob;
//
//        rhi::ResourceSlot m_input_layout;
//        rhi::ResourceSlot m_pipeline_state;
//        rhi::ResourceSlot m_vertex_shader;
//        rhi::ResourceSlot m_pixel_shader;
//        rhi::ResourceSlot m_shader_program;
//        rhi::ResourceSlot m_constant_buffer;
//
//        rhi::ResourceSlot m_font_texture_slot;
//        rhi::DescriptorHandle m_font_texture_srv;
//
//        rsl::unique_array<ImGuiFrameResource> m_frame_resources;
//        DXGI_FORMAT m_rtv_format;
//        s32 m_frame_idx;
//      };
//    }
//  }
//}