#pragma once
#include "imgui/imgui.h"      // IMGUI_IMPL_API
#ifndef IMGUI_DISABLE
#include <dxgiformat.h> // DXGI_FORMAT
#include <d3d12.h>

#include "rex_directx/system/dx_commandlist.h"
#include "rex_directx/system/dx_descriptor_heap.h"
#include "rex_directx/system/dx_rhi.h"

#include "rex_engine/diagnostics/error.h"

#include "rex_directx/rendering/dx_imgui_window.h"

struct ID3D12Device;
struct ID3D12DescriptorHeap;
struct ID3D12GraphicsCommandList;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct D3D12_GPU_DESCRIPTOR_HANDLE;

class ImGui_ImplDX12_RenderBuffers;

namespace rex
{
  namespace renderer
  {
    class ImGuiRenderer
    {
    public:
      // global functions used for callbacks.
      // Imgui uses functions pointers, so can't use lambdas
      friend void create_window_callback(ImGuiViewport* viewport);
      friend void destroy_window_callback(ImGuiViewport* viewport);
      friend void set_window_size_callback(ImGuiViewport* viewport, ImVec2 size);
      friend void render_window_callback(ImGuiViewport* viewport, void* renderArg);
      friend void swap_buffers_callback(ImGuiViewport* viewport, void* renderArg);

    public:
      ImGuiRenderer(ID3D12Device1* device, s32 numFramesInFlight, DXGI_FORMAT rtvFormat, HWND hwnd);
      ~ImGuiRenderer();

      ImGuiRenderer(const ImGuiRenderer&) = delete;
      ImGuiRenderer(ImGuiRenderer&&) = delete;

      ImGuiRenderer& operator=(const ImGuiRenderer&) = delete;
      ImGuiRenderer& operator=(ImGuiRenderer&&) = delete;

      void new_frame();
      void render();

    private:
      void init_imgui(HWND hwnd);

      Error init_gpu_resources();
      Error init_src_desc_heap();
      Error init_input_layout();
      Error init_shader();
      Error init_font_texture();
      Error init_constant_buffer();
      Error init_pso();

      void init_platform_interface();
      void create_window(ImGuiViewport* viewport);
      void destroy_window(ImGuiViewport* viewport);
      void render_window(ImGuiViewport* viewport);
      void set_window_size(ImGuiViewport* viewport, ImVec2 size);
      void swap_buffers(ImGuiViewport* viewport);

    private:
      ID3D12Device1* m_device;                                  // Needed to initialize child windows with their own directx objects
      DXGI_FORMAT                 m_rtv_format;                 // Comes in from the main rendered, to match child windows rtv format with that of the main window
      rex::rhi::DescriptorHandle m_texture_handle;              // the handle to the shader resource view of the font texture
      rsl::unique_ptr<rex::rhi::DescriptorHeap> m_srv_desc_heap;                // Probably not needed, rex uses single descriptor heaps throughout the process.
      UINT                        m_max_num_frames_in_flight;   // used to store the number of frames we can handle at once.

      rsl::unique_ptr<rhi::CommandQueue> m_cmd_queue;
      rsl::unique_ptr<rhi::CommandList> m_cmd_list;

      rex::rhi::ResourceSlot m_shader_program;                 // resource slot for the compiled imgui shader as well as its root signature
      rex::rhi::ResourceSlot m_pipeline_state;                 // resource slot for the pipeline state of imgui
      rex::rhi::ResourceSlot m_constant_buffer;                // resource slot for the constant buffer holding the mvp matrix. This single object is shared between all viewports
      rex::rhi::ResourceSlot m_input_layout;                   // resource slot for the input layout of imgui
      rex::rhi::ResourceSlot m_vertex_shader;                  // resource slot for the vertex shader of imgui
      rex::rhi::ResourceSlot m_pixel_shader;                   // resource slot for the pixel shader of imgui
      rex::rhi::ResourceSlot m_texture;                        // resources lot for the fonts texture, used by imgui

      rsl::vector<ImGuiWindow> m_imgui_child_windows;          // Child windows of ImGui, spawned by draging a viewport outside of the main window
    };
  }
}


#endif // #ifndef IMGUI_DISABLE
