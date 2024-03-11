#include "rex_directx/directx_renderer.h"
#include "rex_directx/imgui/imgui_impl_dx12.h"
#include "rex_renderer_core/renderer.h"
#include "rex_renderer_core/renderer_imgui.h"

namespace rex
{
  namespace renderer
  {
    namespace imgui
    {
      bool initialize()
      {
        bool result = ImGui_ImplDX12_Init(renderer::backend::device(), renderer::max_frames_in_flight(), renderer::backend::rtv_format(), renderer::backend::cbv_srv_heap(), renderer::backend::cbv_srv_uav_cpu_desc_handle_for_heap_start(),
                                          renderer::backend::cbv_srv_uav_gpu_desc_handle_for_heap_start());

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // io.Fonts->AddFontDefault();
        // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
        // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
        // IM_ASSERT(font != nullptr);

        return result;
      }
      void shutdown()
      {
        // Has the GPU finished processing the commands of the current frame resource?
        // If not, wait until the GPU has completed commands up to this fence point.
        //
        // This call is not queued but directly executed!
        rex::renderer::wait_for_active_frame();

        ImGui_ImplDX12_Shutdown();
      }

      void begin() 
      {
        ImGui_ImplDX12_NewFrame();
      }

      void render() 
      {
        ImGui::Render();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource   = renderer::backend::back_buffer();
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
        renderer::backend::command_list()->Reset(renderer::backend::frame_command_allocator(), nullptr);
        renderer::backend::command_list()->ResourceBarrier(1, &barrier);

        // Render Dear ImGui graphics
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        const float clear_color_with_alpha[4] = {clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w};
        renderer::backend::command_list()->ClearRenderTargetView(renderer::backend::back_buffer_cpu_desc_handle_for_heap_start(), clear_color_with_alpha, 0, nullptr);
        renderer::backend::command_list()->OMSetRenderTargets(1, &renderer::backend::back_buffer_cpu_desc_handle_for_heap_start(), FALSE, nullptr);

        ID3D12DescriptorHeap* desc_heap = renderer::backend::cbv_srv_heap();
        ID3D12DescriptorHeap* desc_heaps[] = {desc_heap};

        renderer::backend::command_list()->SetDescriptorHeaps(1, desc_heaps);

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), renderer::backend::command_list());
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
        renderer::backend::command_list()->ResourceBarrier(1, &barrier);
      }

      void end() 
      {
        renderer::backend::command_list()->Close();

        rsl::array<ID3D12CommandList*, 1> command_lists = {renderer::backend::command_list()};
        renderer::backend::command_queue()->ExecuteCommandLists(command_lists.size(), command_lists.data());

        // Update and Render additional Platform Windows
        ImGuiIO& io = ImGui::GetIO();
        if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
          ImGui::UpdatePlatformWindows();
          ImGui::RenderPlatformWindowsDefault(nullptr, (void*)renderer::backend::command_list());
        }
      }
    } // namespace imgui
  }   // namespace renderer
} // namespace rex