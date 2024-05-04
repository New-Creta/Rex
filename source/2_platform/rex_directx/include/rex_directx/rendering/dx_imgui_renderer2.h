#pragma once
#include "imgui/imgui.h"      // IMGUI_IMPL_API
#ifndef IMGUI_DISABLE
#include <dxgiformat.h> // DXGI_FORMAT
#include <d3d12.h>

#include "rex_directx/system/dx_descriptor_heap.h"

#include "rex_engine/diagnostics/error.h"

struct ID3D12Device;
struct ID3D12DescriptorHeap;
struct ID3D12GraphicsCommandList;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct D3D12_GPU_DESCRIPTOR_HANDLE;

// cmd_list is the command list that the implementation will use to render imgui draw lists.
// Before calling the render function, caller must prepare cmd_list by resetting it and setting the appropriate
// render target and descriptor heap that contains font_srv_cpu_desc_handle/font_srv_gpu_desc_handle.
// font_srv_cpu_desc_handle and font_srv_gpu_desc_handle are handles to a single SRV descriptor to use for the internal font texture.
IMGUI_IMPL_API void     ImGui_ImplDX12_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplDX12_RenderDrawData(ImDrawData* draw_data, ID3D12GraphicsCommandList* graphics_command_list);

// Use if you want to reset your rendering device without losing Dear ImGui state.
IMGUI_IMPL_API void     ImGui_ImplDX12_InvalidateDeviceObjects();
IMGUI_IMPL_API bool     ImGui_ImplDX12_CreateDeviceObjects();



namespace rex
{
  namespace renderer
  {
    class ImGuiRenderer
    {
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
      Error init_input_layout();
      Error init_shader();
      Error init_font_texture();
      Error init_buffers();

    private:
      ID3D12Device1* m_device;
    };
  }
}


#endif // #ifndef IMGUI_DISABLE
