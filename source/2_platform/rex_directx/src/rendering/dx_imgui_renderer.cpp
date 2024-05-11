//#include "imgui/imgui.h"
//#ifndef IMGUI_DISABLE
//#include "rex_directx/rendering/dx_imgui_renderer.h"
//
//#include "imgui/platform/win/imgui_impl_win32.h"
//
//// DirectX
//#include <d3d12.h>
//#include <dxgi1_4.h>
//#include <d3dcompiler.h>
//
//#include "rex_directx/system/dx_commandlist.h"
//#include "rex_directx/system/dx_commandlist.h"
//#include "rex_directx/system/dx_command_queue.h"
//#include "rex_directx/system/dx_constant_buffer.h"
//#include "rex_directx/system/dx_debug_interface.h"
//#include "rex_directx/system/dx_descriptor_heap.h"
//#include "rex_directx/system/dx_device.h"
//#include "rex_directx/system/dx_feature_level.h"
//#include "rex_directx/system/dx_feature_shader_model.h"
//#include "rex_directx/system/dx_fence.h"
//#include "rex_directx/system/dx_index_buffer.h"
//#include "rex_directx/system/dx_pipeline_library.h"
//#include "rex_directx/system/dx_pipeline_state.h"
//#include "rex_directx/system/dx_resource.h"
//#include "rex_directx/system/dx_resource_heap.h"
//#include "rex_directx/system/dx_rhi.h"
//#include "rex_directx/system/dx_shader_compiler.h"
//#include "rex_directx/system/dx_swapchain.h"
//#include "rex_directx/system/dx_texture_2d.h"
//#include "rex_directx/system/dx_vertex_buffer.h"
//#include "rex_directx/resources/dx_shader_program_resource.h"
//
//#include "rex_directx/diagnostics/log.h"
//#include "rex_directx/diagnostics/dx_call.h"
//
//#include "rex_directx/rendering/dx_imgui_viewport.h"
//
//#include "rex_engine/memory/global_allocator.h"
//
//DEFINE_LOG_CATEGORY(LogImgui);
//
//// First render the main window widgets
//// Next render all the child windows
//
//namespace rex
//{
//  namespace renderer
//  {
//    ImGuiRenderer* g_imgui_renderer = nullptr;
//
//    // global functions used for callbacks.
//    // Imgui uses functions pointers, so can't use lambdas
//    void create_window_callback(ImGuiViewport* viewport)
//    {
//      g_imgui_renderer->create_window(viewport);
//    }
//    void destroy_window_callback(ImGuiViewport* viewport)
//    {
//      g_imgui_renderer->destroy_window(viewport);
//    }
//    void set_window_size_callback(ImGuiViewport* viewport, ImVec2 size)
//    {
//      g_imgui_renderer->set_window_size(viewport, size);
//    }
//    void render_window_callback(ImGuiViewport* viewport, void* /*renderArg*/)
//    {
//      g_imgui_renderer->render_window(viewport);
//    }
//    void swap_buffers_callback(ImGuiViewport* viewport, void* /*renderArg*/)
//    {
//      g_imgui_renderer->swap_buffers(viewport);
//    }
//
//    ImGuiRenderer::ImGuiRenderer(ID3D12Device1* device, s32 numFramesInFlight, DXGI_FORMAT rtvFormat, HWND hwnd)
//      : m_device(device)
//      , m_rtv_format(rtvFormat)
//      , m_max_num_frames_in_flight(numFramesInFlight)
//    {
//      REX_ASSERT_X(g_imgui_renderer == nullptr, "You can only have 1 imgui renderer");
//      g_imgui_renderer = this;
//
//      init_imgui(hwnd);
//
//      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//      {
//        init_platform_interface();
//      }
//
//      if (init_gpu_resources())
//      {
//        REX_ERROR(LogImgui, "Failed to create imgui device objects");
//        return;
//      }
//
//      // Create a dummy ImGui_ImplDX12_ViewportData holder for the main viewport,
//      // Since this is created and managed by the application, we will only use the ->Resources[] fields.
//      ImGuiViewport* main_viewport = ImGui::GetMainViewport();
//      main_viewport->RendererUserData = IM_NEW(RexImGuiViewport)(main_viewport, m_device, m_max_num_frames_in_flight, m_rtv_format, m_shader_program, m_pipeline_state, m_constant_buffer);
//
//    }
//    ImGuiRenderer::~ImGuiRenderer()
//    {
//      ImGuiIO& io = ImGui::GetIO();
//
//      // Manually delete main viewport render resources in-case we haven't initialized for viewports
//      ImGuiViewport* main_viewport = ImGui::GetMainViewport();
//      if (RexImGuiViewport* imgui_window = (RexImGuiViewport*)main_viewport->RendererUserData)
//      {
//        IM_DELETE(imgui_window);
//        main_viewport->RendererUserData = nullptr;
//      }
//
//      // Clean up windows and device objects
//      ImGui::DestroyPlatformWindows();
//
//      io.BackendRendererName = nullptr;
//      io.BackendRendererUserData = nullptr;
//      io.BackendFlags &= ~(ImGuiBackendFlags_RendererHasVtxOffset | ImGuiBackendFlags_RendererHasViewports);
//    }
//
//    void ImGuiRenderer::new_frame()
//    {
//      ImGui_ImplWin32_NewFrame();
//
//      ImGui::NewFrame();
//    }
//    void ImGuiRenderer::render()
//    {
//      ImGui::Render();
//
//      ImGuiViewport* main_viewport = ImGui::GetMainViewport();
//      if (RexImGuiViewport* imgui_window = (RexImGuiViewport*)main_viewport->RendererUserData)
//      {
//        rhi::CommandList* cmd_list = rhi::cmd_list();
//        ID3D12DescriptorHeap* desc_heap = m_srv_desc_heap->get();
//        cmd_list->get()->SetDescriptorHeaps(1, &desc_heap);
//
//        imgui_window->draw(cmd_list);
//      }
//
//      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//      {
//        ImGui::UpdatePlatformWindows();
//        ImGui::RenderPlatformWindowsDefault(nullptr, (void*)rhi::cmd_list());
//      }
//    }
//
//    void ImGuiRenderer::init_imgui(HWND hwnd)
//    {
//      IMGUI_CHECKVERSION();
//      ImGui::CreateContext();
//      ImGuiIO& io = ImGui::GetIO();
//      io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls
//      io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable gamepad controls
//      io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
//      io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable multi-viewport / Platform windows
//
//      // Enable dark mode
//      ImGui::StyleColorsDark();
//
//      // We support multiple viewport, having imgui widget in their own windows when dragged outside of the main window
//      ImGuiStyle& style = ImGui::GetStyle();
//      if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//      {
//        style.WindowRounding = 0.0f;
//        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//      }
//
//      io.BackendRendererName = "DirectX 12 ImGui Renderer";
//      io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
//
//      // Win32 boiletplate code
//      ImGui_ImplWin32_Init(hwnd);
//
//      IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");
//
//      // Setup backend capabilities flags
//      io.BackendRendererUserData = (void*)this;
//      io.BackendRendererName = "ImGui DirectX12 Renderer";
//      io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
//      io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)
//    }
//
//    Error ImGuiRenderer::init_gpu_resources()
//    {
//      Error err = Error::no_error();
//
//      err = init_src_desc_heap();
//      if (err)
//      {
//        return Error::create_with_log(LogImgui, "Failed to create desc heap for imgui texture");
//      }
//
//      err = init_font_texture();
//      if (err)
//      {
//        return Error::create_with_log(LogImgui, "Failed to create font texture");
//      }
//
//      err = init_shader();
//      if (err)
//      {
//        return Error::create_with_log(LogImgui, "Failed to create imgui shader");
//      }
//
//      err = init_input_layout();
//      if (err)
//      {
//        return Error::create_with_log(LogImgui, "Failed to create imgui input layout");
//      }
//
//      err = init_constant_buffer();
//      if (err)
//      {
//        return Error::create_with_log(LogImgui, "Failed to create imgui resource buffers");
//      }
//
//      err = init_pso();
//      if (err)
//      {
//        return Error::create_with_log(LogImgui, "Failed to create imgui pso");
//      }
//
//      return err;
//    }
//    Error ImGuiRenderer::init_input_layout()
//    {
//      rex::rhi::InputLayoutDesc input_layout_desc;
//      input_layout_desc.input_layout =
//      {
//        rex::rhi::InputLayoutElementDesc { "POSITION",  rex::renderer::VertexBufferFormat::Float2, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 0, 0 },
//        rex::rhi::InputLayoutElementDesc { "TEXCOORD",  rex::renderer::VertexBufferFormat::Float2, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 8, 0 },
//        rex::rhi::InputLayoutElementDesc { "COLOR", rex::renderer::VertexBufferFormat::UNorm4, rex::renderer::InputLayoutClassification::PerVertexData, 0, 0, 16, 0 }
//      };
//      m_input_layout = rex::rhi::create_input_layout(input_layout_desc);
//
//      return Error::no_error();
//    }
//    Error ImGuiRenderer::init_shader()
//    {
//      // Compile the shaders
//      static rsl::string_view vertex_shader =
//        "cbuffer vertexBuffer : register(b0) \
//      {\
//        float4x4 ProjectionMatrix; \
//      };\
//      struct VS_INPUT\
//      {\
//        float2 pos : POSITION;\
//        float4 col : COLOR0;\
//        float2 uv  : TEXCOORD0;\
//      };\
//      \
//      struct PS_INPUT\
//      {\
//        float4 pos : SV_POSITION;\
//        float4 col : COLOR0;\
//        float2 uv  : TEXCOORD0;\
//      };\
//      \
//      PS_INPUT main(VS_INPUT input)\
//      {\
//        PS_INPUT output;\
//        output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
//        output.col = input.col;\
//        output.uv  = input.uv;\
//        return output;\
//      }";
//
//      rex::rhi::CompileShaderDesc compile_vs_desc{};
//      compile_vs_desc.shader_code = rex::memory::Blob(rsl::make_unique<char[]>(vertex_shader.length()));
//      compile_vs_desc.shader_code.write(vertex_shader.data(), vertex_shader.length());
//      compile_vs_desc.shader_entry_point = "main";
//      compile_vs_desc.shader_feature_target = "vs_5_0";
//      compile_vs_desc.shader_name = "imgui_vertex_shader";
//      compile_vs_desc.shader_type = rex::rhi::ShaderType::Vertex;
//      m_vertex_shader = rex::rhi::compile_shader(compile_vs_desc);
//
//      static rsl::string_view pixel_shader =
//        "struct PS_INPUT\
//       {\
//         float4 pos : SV_POSITION;\
//         float4 col : COLOR0;\
//         float2 uv  : TEXCOORD0;\
//       };\
//       SamplerState sampler0 : register(s0);\
//       Texture2D texture0 : register(t0);\
//       \
//       float4 main(PS_INPUT input) : SV_Target\
//       {\
//         float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
//         return out_col; \
//       }";
//
//      rex::rhi::CompileShaderDesc compile_ps_desc{};
//      compile_ps_desc.shader_code = rex::memory::Blob(rsl::make_unique<char[]>(pixel_shader.length()));
//      compile_ps_desc.shader_code.write(pixel_shader.data(), pixel_shader.length());
//      compile_ps_desc.shader_entry_point = "main";
//      compile_ps_desc.shader_feature_target = "ps_5_0";
//      compile_ps_desc.shader_name = "imgui_pixel_shader";
//      compile_ps_desc.shader_type = rex::rhi::ShaderType::Pixel;
//      m_pixel_shader = rex::rhi::compile_shader(compile_ps_desc);
//
//      // Link shaders
//      rex::rhi::LinkShaderDesc link_shader_desc{};
//      link_shader_desc.vertex_shader = m_vertex_shader;
//      link_shader_desc.pixel_shader = m_pixel_shader;
//
//      // We have 2 constants for the shader, 1 in the vertex shader and 1 in the pixel shader
//      link_shader_desc.constants = rsl::make_unique<rex::rhi::ShaderParameterLayoutDescription[]>(1);
//      link_shader_desc.constants[0] = { rex::rhi::ShaderParameterType::CBuffer, "vertexBuffer", 0, rex::renderer::ShaderVisibility::Vertex }; // We have 1 constant buffer in the vertex shader
//
//      link_shader_desc.desc_tables = rsl::make_unique<rex::rhi::DescriptorTableDescription[]>(1);
//      link_shader_desc.desc_tables[0].ranges = rsl::make_unique<rex::rhi::DescriptorRange[]>(1);
//      link_shader_desc.desc_tables[0].ranges[0] = { rex::rhi::DescriptorRangeType::ShaderResourceView, 1 }; // We have 1 src which points to our font texture
//      link_shader_desc.desc_tables[0].visibility = rex::renderer::ShaderVisibility::Pixel;
//
//      // We have 1 sampler, used for sampling the font texture
//      link_shader_desc.samplers = rsl::make_unique<rex::rhi::ShaderSamplerDescription[]>(1);
//      link_shader_desc.samplers[0].filtering = rex::renderer::SamplerFiltering::MinMagMipLinear;
//      link_shader_desc.samplers[0].address_mode_u = rex::renderer::TextureAddressMode::Wrap;
//      link_shader_desc.samplers[0].address_mode_v = rex::renderer::TextureAddressMode::Wrap;
//      link_shader_desc.samplers[0].address_mode_w = rex::renderer::TextureAddressMode::Wrap;
//      link_shader_desc.samplers[0].mip_lod_bias = 0.0f;
//      link_shader_desc.samplers[0].max_anisotropy = 0;
//      link_shader_desc.samplers[0].comparison_func = rex::renderer::ComparisonFunc::Always;
//      link_shader_desc.samplers[0].border_color = rex::renderer::BorderColor::TransparentBlack;
//      link_shader_desc.samplers[0].min_lod = 0.0f;
//      link_shader_desc.samplers[0].max_lod = 0.0f;
//      link_shader_desc.samplers[0].shader_register = 0;
//      link_shader_desc.samplers[0].register_space = 0;
//      link_shader_desc.samplers[0].shader_visibility = rex::renderer::ShaderVisibility::Pixel;
//
//      m_shader_program = rex::rhi::link_shader(link_shader_desc);
//
//      return Error::no_error();
//    }
//    Error ImGuiRenderer::init_src_desc_heap()
//    {
//      D3D12_DESCRIPTOR_HEAP_DESC desc{};
//
//      desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//      desc.NumDescriptors = 1;
//      desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//      desc.NodeMask = 0; // For single-adapter operation, set this to zero. ( https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_descriptor_heap_desc )
//
//      wrl::ComPtr<ID3D12DescriptorHeap> desc_heap;
//      if (DX_FAILED(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&desc_heap))))
//      {
//        return Error::create_with_log(LogImgui, "Failed to create imgui descriptor heap resource");
//      }
//
//      rhi::set_debug_name_for(desc_heap.Get(), "Imgui's texture descriptor heap");
//
//      m_srv_desc_heap = rsl::make_unique<rhi::DescriptorHeap>(desc_heap, m_device);
//
//      return Error::no_error();
//    }
//    Error ImGuiRenderer::init_font_texture()
//    {
//      // Build texture atlas
//      ImGuiIO& io = ImGui::GetIO();
//      unsigned char* pixels;
//      int width, height;
//      io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
//
//      // Upload texture to graphics system
//      m_texture = rex::rhi::create_texture2d((const char*)pixels, DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
//      m_texture_handle = rex::rhi::create_texture2d_srv(m_srv_desc_heap.get(), m_texture);
//
//      // Store our identifier
//      // READ THIS IF THE STATIC_ASSERT() TRIGGERS:
//      // - Important: to compile on 32-bit systems, this backend requires code to be compiled with '#define ImTextureID ImU64'.
//      // - This is because we need ImTextureID to carry a 64-bit value and by default ImTextureID is defined as void*.
//      // [Solution 1] IDE/msbuild: in "Properties/C++/Preprocessor Definitions" add 'ImTextureID=ImU64' (this is what we do in the 'example_win32_direct12/example_win32_direct12.vcxproj' project file)
//      // [Solution 2] IDE/msbuild: in "Properties/C++/Preprocessor Definitions" add 'IMGUI_USER_CONFIG="my_imgui_config.h"' and inside 'my_imgui_config.h' add '#define ImTextureID ImU64' and as many other options as you like.
//      // [Solution 3] IDE/msbuild: edit imconfig.h and add '#define ImTextureID ImU64' (prefer solution 2 to create your own config file!)
//      // [Solution 4] command-line: add '/D ImTextureID=ImU64' to your cl.exe command-line (this is what we do in the example_win32_direct12/build_win32.bat file)
//      static_assert(sizeof(ImTextureID) >= sizeof(m_texture_handle.get_gpu().ptr), "Can't pack descriptor handle into TexID, 32-bit not supported yet.");
//      io.Fonts->SetTexID((ImTextureID)m_texture_handle.get_gpu().ptr);
//
//      return Error::no_error();
//    }
//    Error ImGuiRenderer::init_constant_buffer()
//    {
//      m_constant_buffer = rex::rhi::create_constant_buffer(sizeof(ImGuiVertexConstantBuffer));
//
//      return Error::no_error();
//    }
//    Error ImGuiRenderer::init_pso()
//    {
//      // Raster State
//      rex::rhi::RasterStateDesc rasterizer_desc{};
//      rasterizer_desc.fill_mode = rex::renderer::FillMode::Solid;
//      rasterizer_desc.cull_mode = rex::renderer::CullMode::None;
//      rasterizer_desc.front_ccw = false;
//      rasterizer_desc.depth_bias = D3D12_DEFAULT_DEPTH_BIAS;
//      rasterizer_desc.depth_bias_clamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
//      rasterizer_desc.sloped_scale_depth_bias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
//      rasterizer_desc.depth_clip_enable = true;
//      rasterizer_desc.multisample = false;
//      rasterizer_desc.aa_lines = false;
//      rasterizer_desc.forced_sample_count = 0;
//
//      rex::rhi::PipelineStateDesc pso_desc{};
//      pso_desc.input_layout = m_input_layout;
//      pso_desc.raster_state = rex::rhi::create_raster_state(rasterizer_desc);
//      pso_desc.shader = m_shader_program;
//
//      // Blend State
//      pso_desc.blend_state = rex::rhi::BlendDesc();
//      rex::rhi::BlendDesc& blend_state = pso_desc.blend_state.value();
//      blend_state.enable_alpha_to_coverage = false;
//      blend_state.render_target[0].blend_enable = true;
//      blend_state.render_target[0].src_blend = rex::rhi::Blend::SrcAlpha;
//      blend_state.render_target[0].dst_blend = rex::rhi::Blend::InvSrcAlpha;
//      blend_state.render_target[0].blend_op = rex::rhi::BlendOp::Add;
//      blend_state.render_target[0].src_blend_alpha = rex::rhi::Blend::One;
//      blend_state.render_target[0].dst_blend_alpha = rex::rhi::Blend::InvSrcAlpha;
//      blend_state.render_target[0].blend_op_alpha = rex::rhi::BlendOp::Add;
//      blend_state.render_target[0].render_target_write_mask = rex::rhi::RenderTargetWriteMask::All;
//
//      // depth stencil state
//      pso_desc.depth_stencil_state = rex::rhi::DepthStencilDesc();
//      rex::rhi::DepthStencilDesc& depth_stencil_desc = pso_desc.depth_stencil_state.value();
//      depth_stencil_desc.depth_enable = false;
//      depth_stencil_desc.depth_write_mask = rex::rhi::DepthWriteMask::DepthWriteMaskAll;
//      depth_stencil_desc.depth_func = rex::renderer::ComparisonFunc::Always;
//      depth_stencil_desc.stencil_enable = false;
//      depth_stencil_desc.front_face.stencil_fail_op = rex::rhi::StencilOp::Keep;
//      depth_stencil_desc.front_face.stencil_depth_fail_op = rex::rhi::StencilOp::Keep;
//      depth_stencil_desc.front_face.stencil_pass_op = rex::rhi::StencilOp::Keep;
//      depth_stencil_desc.front_face.stencil_func = rex::renderer::ComparisonFunc::Always;
//      depth_stencil_desc.back_face = depth_stencil_desc.front_face;
//
//      m_pipeline_state = rex::rhi::create_pso(pso_desc);
//
//      return Error::no_error();
//    }
//
//    void ImGuiRenderer::init_platform_interface()
//    {
//      ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
//      platform_io.Renderer_CreateWindow = create_window_callback;
//      platform_io.Renderer_DestroyWindow = destroy_window_callback;
//      platform_io.Renderer_SetWindowSize = set_window_size_callback;
//      platform_io.Renderer_RenderWindow = render_window_callback;
//      platform_io.Renderer_SwapBuffers = swap_buffers_callback;
//    }
//    void ImGuiRenderer::create_window(ImGuiViewport* viewport)
//    {
//      void* mem = rex::global_debug_allocator().allocate<ImGuiWindow>();
//      ImGuiWindow* imgui_window = new (mem)(ImGuiWindow)(viewport, m_device, m_max_num_frames_in_flight, m_rtv_format, m_shader_program, m_pipeline_state, m_constant_buffer);
//      viewport->RendererUserData = imgui_window;
//    }
//    void ImGuiRenderer::destroy_window(ImGuiViewport* viewport)
//    {
//      if (ImGuiWindow* imgui_window = (ImGuiWindow*)viewport->RendererUserData)
//      {
//        imgui_window->wait_for_pending_operations();
//        rex::global_debug_allocator().destroy(imgui_window);
//        rex::global_debug_allocator().deallocate(imgui_window);
//      }
//      viewport->RendererUserData = nullptr;
//    }
//    void ImGuiRenderer::render_window(ImGuiViewport* viewport)
//    {
//      ImGuiWindow* imgui_window = (ImGuiWindow*)viewport->RendererUserData;
//
//      imgui_window->begin_draw(m_srv_desc_heap.get());
//
//      if (!(viewport->Flags & ImGuiViewportFlags_NoRendererClear))
//      {
//        const ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
//        imgui_window->clear_render_target(clear_color);
//      }
//
//      imgui_window->draw();
//      imgui_window->end_draw();
//    }
//    void ImGuiRenderer::set_window_size(ImGuiViewport* viewport, ImVec2 size)
//    {
//      ImGuiWindow* imgui_window = (ImGuiWindow*)viewport->RendererUserData;
//      imgui_window->wait_for_pending_operations();
//      imgui_window->resize_buffers(size.x, size.y);
//    }
//    void ImGuiRenderer::swap_buffers(ImGuiViewport* viewport)
//    {
//      ImGuiWindow* imgui_window = (ImGuiWindow*)viewport->RendererUserData;
//      
//      imgui_window->present();
//      imgui_window->yield_thread_until_in_sync_with_gpu();
//    }
//  }
//}
//
//#endif // #ifndef IMGUI_DISABLE
