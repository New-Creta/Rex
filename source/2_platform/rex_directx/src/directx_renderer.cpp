#include "rex_directx/command_list.h"
#include "rex_directx/command_queue.h"
#include "rex_directx/directx_device.h"
#include "rex_directx/directx_feature_level.h"
#include "rex_directx/directx_feature_shader_model.h"
#include "rex_directx/directx_swapchain.h"
#include "rex_directx/imgui/gui_renderer.h"
#include "rex_directx/resources/clear_state.h"
#include "rex_directx/resources/index_buffer.h"
#include "rex_directx/resources/pipeline_state_object.h"
#include "rex_directx/resources/raster_state.h"
#include "rex_directx/resources/root_signature.h"
#include "rex_directx/resources/shader_library.h"
#include "rex_directx/resources/texture.h"
#include "rex_directx/resources/vertex_buffer.h"
#include "rex_directx/scenegraph/camera_node.h"
#include "rex_directx/scenegraph/camera_visitor.h"
#include "rex_directx/scenegraph/mesh_node.h"
#include "rex_directx/scenegraph/mesh_visitor.h"
#include "rex_dxgi/dxgi_format.h"
#include "rex_engine/scenegraph/scene.h"
#include "rex_renderer_core/clear_bits.h"
#include "rex_renderer_core/gpu_description.h"
#include "rex_renderer_core/primitive_topology.h"
#include "rex_renderer_core/renderer_backend.h"
#include "rex_renderer_core/renderer_info.h"
#include "rex_renderer_core/renderer_output_window_user_data.h"
#include "rex_renderer_core/resource_descriptions/create_clear_state_desc.h"
#include "rex_renderer_core/resource_descriptions/create_index_buffer_desc.h"
#include "rex_renderer_core/resource_descriptions/create_pipeline_state_desc.h"
#include "rex_renderer_core/resource_descriptions/create_raster_state_desc.h"
#include "rex_renderer_core/resource_descriptions/create_texture_desc.h"
#include "rex_renderer_core/resource_descriptions/create_vertex_buffer_desc.h"
#include "rex_renderer_core/resource_pool.h"
#include "rex_renderer_core/scissor_rect.h"
#include "rex_renderer_core/viewport.h"
#include "rex_std/memory.h"

namespace rex
{
  namespace renderer
  {
    namespace conversions
    {
      //-------------------------------------------------------------------------
      D3D12_VIEWPORT to_VIEWPORT(const Viewport& viewport)
      {
        D3D12_VIEWPORT d3d_viewport;

        d3d_viewport.TopLeftX = viewport.top_left_x;
        d3d_viewport.TopLeftY = viewport.top_left_y;
        d3d_viewport.Width    = viewport.width;
        d3d_viewport.Height   = viewport.height;
        d3d_viewport.MinDepth = viewport.min_depth;
        d3d_viewport.MaxDepth = viewport.max_depth;

        return d3d_viewport;
      }

      //-------------------------------------------------------------------------
      D3D12_RECT to_RECT(const ScissorRect& rect)
      {
        D3D12_RECT d3d_rect;

        d3d_rect.left   = (LONG)rect.left;
        d3d_rect.top    = (LONG)rect.top;
        d3d_rect.right  = (LONG)rect.right;
        d3d_rect.bottom = (LONG)rect.bottom;

        return d3d_rect;
      }

      //-------------------------------------------------------------------------
      D3D12_CLEAR_VALUE to_CLEAR_VALUE(const CreateClearStateDesc& desc, renderer::Format format)
      {
        D3D12_CLEAR_VALUE clear_value = {};

        clear_value.Color[0] = desc.rgba.color_data[0];
        clear_value.Color[1] = desc.rgba.color_data[1];
        clear_value.Color[2] = desc.rgba.color_data[2];
        clear_value.Color[3] = desc.rgba.color_data[3];

        clear_value.DepthStencil.Depth   = desc.depth;
        clear_value.DepthStencil.Stencil = desc.stencil;

        clear_value.Format = dxgi::conversions::to_DXGI(format);

        return clear_value;
      }

      //-------------------------------------------------------------------------
      D3D12_RESOURCE_DIMENSION to_RESOURCE_DIMENSION(ResourceDimension dimension)
      {
        switch(dimension)
        {
          case ResourceDimension::DIMENSION_UNKNOWN: return D3D12_RESOURCE_DIMENSION_UNKNOWN;
          case ResourceDimension::DIMENSION_BUFFER: return D3D12_RESOURCE_DIMENSION_BUFFER;
          case ResourceDimension::DIMENSION_TEXTURE1D: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
          case ResourceDimension::DIMENSION_TEXTURE2D: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
          case ResourceDimension::DIMENSION_TEXTURE3D: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        }

        REX_ASSERT("Unsupported resource dimension given");
        return D3D12_RESOURCE_DIMENSION_UNKNOWN;
      }

      //-------------------------------------------------------------------------
      D3D12_TEXTURE_LAYOUT to_TEXTURE_LAYOUT(TextureLayout textureLayout)
      {
        switch(textureLayout)
        {
          case TextureLayout::TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE: return D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE;
          case TextureLayout::TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE: return D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE;
          case TextureLayout::TEXTURE_LAYOUT_ROW_MAJOR: return D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
          case TextureLayout::TEXTURE_LAYOUT_UNKNOWN: return D3D12_TEXTURE_LAYOUT_UNKNOWN;
        }

        REX_ASSERT("Unsupported texture layout given");
        return D3D12_TEXTURE_LAYOUT_UNKNOWN;
      }

      //-------------------------------------------------------------------------
      D3D12_RESOURCE_FLAGS to_RESOURCE_FLAGS(ResourceFlags resourceFlags)
      {
        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

        if(resourceFlags & ResourceFlags::RESOURCE_FLAG_ALLOW_CROSS_ADAPTER)
          flags &= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER;
        if(resourceFlags & ResourceFlags::RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
          flags &= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        if(resourceFlags & ResourceFlags::RESOURCE_FLAG_ALLOW_RENDER_TARGET)
          flags &= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        if(resourceFlags & ResourceFlags::RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS)
          flags &= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
        if(resourceFlags & ResourceFlags::RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
          flags &= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        if(resourceFlags & ResourceFlags::RESOURCE_FLAG_DENY_SHADER_RESOURCE)
          flags &= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
        if(resourceFlags & ResourceFlags::RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY)
          flags &= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY;

        return flags;
      }

      //-------------------------------------------------------------------------
      D3D12_RESOURCE_DESC to_RESOURCE_DESC(const CreateResourceDesc& desc)
      {
        D3D12_RESOURCE_DESC resource_desc;
        resource_desc.Dimension          = to_RESOURCE_DIMENSION(desc.dimension);
        resource_desc.Alignment          = desc.alignment;
        resource_desc.Width              = desc.width;
        resource_desc.Height             = desc.height;
        resource_desc.DepthOrArraySize   = desc.depth_or_array_size;
        resource_desc.MipLevels          = desc.mip_levels;
        resource_desc.Format             = dxgi::conversions::to_DXGI(desc.format);
        resource_desc.SampleDesc.Count   = desc.sample_desc_count;
        resource_desc.SampleDesc.Quality = desc.sample_desc_quality;
        resource_desc.Layout             = to_TEXTURE_LAYOUT(desc.layout);
        resource_desc.Flags              = to_RESOURCE_FLAGS(desc.flags);
        return resource_desc;
      }

      //-------------------------------------------------------------------------
      D3D12_FILL_MODE to_FILL_MODE(FillMode mode)
      {
        switch(mode)
        {
          case FillMode::SOLID: return D3D12_FILL_MODE_SOLID;
          case FillMode::WIREFRAME: return D3D12_FILL_MODE_WIREFRAME;
          default: break;
        }

        REX_ASSERT("Unsupported fill mode given");
        return D3D12_FILL_MODE_SOLID;
      }

      //-------------------------------------------------------------------------
      D3D12_CULL_MODE to_CULL_MODE(CullMode mode)
      {
        switch(mode)
        {
          case CullMode::None: return D3D12_CULL_MODE_NONE;
          case CullMode::FRONT: return D3D12_CULL_MODE_FRONT;
          case CullMode::BACK: return D3D12_CULL_MODE_BACK;
          default: break;
        }

        REX_ASSERT("Unsupported cull mode given");
        return D3D12_CULL_MODE_NONE;
      }

      D3D12_CONSERVATIVE_RASTERIZATION_MODE to_CONSERVATIVE_RASTERIZATION_MODE(ConservativeRasterizationMode mode)
      {
        switch(mode)
        {
          case ConservativeRasterizationMode::MODE_OFF: return D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
          case ConservativeRasterizationMode::MODE_ON: return D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON;
          default: break;
        }

        REX_ASSERT("Unsupported conservative rasterization mode given");
        return D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
      }

      //-------------------------------------------------------------------------
      D3D12_PRIMITIVE_TOPOLOGY to_PRIMITIVE_TOPOLOGY(PrimitiveTopology topology)
      {
        switch(topology)
        {
          case PrimitiveTopology::LINELIST: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
          case PrimitiveTopology::LINESTRIP: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
          case PrimitiveTopology::POINTLIST: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
          case PrimitiveTopology::TRIANGLELIST: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
          case PrimitiveTopology::TRIANGLESTRIP: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
          default: break;
        }
        REX_ASSERT("Unsupported primitive topology given");
        return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
      }

      //-------------------------------------------------------------------------
      D3D12_RASTERIZER_DESC to_RASTERIZER_DESC(const rsl::shared_ptr<RasterState>& rasterState)
      {
        D3D12_RASTERIZER_DESC raster_desc;

        raster_desc.FillMode              = to_FILL_MODE(rasterState->fill_mode());
        raster_desc.CullMode              = to_CULL_MODE(rasterState->cull_mode());
        raster_desc.FrontCounterClockwise = rasterState->front_ccw();
        raster_desc.DepthBias             = rasterState->depth_bias();
        raster_desc.DepthBiasClamp        = rasterState->depth_bias_clamp();
        raster_desc.SlopeScaledDepthBias  = rasterState->sloped_scale_depth_bias();
        raster_desc.DepthClipEnable       = rasterState->depth_clip_enable();
        raster_desc.MultisampleEnable     = rasterState->multisample();
        raster_desc.AntialiasedLineEnable = rasterState->aa_lines();
        raster_desc.ForcedSampleCount     = rasterState->forced_sample_count();
        raster_desc.ConservativeRaster    = to_CONSERVATIVE_RASTERIZATION_MODE(rasterState->conservative_raster());

        return raster_desc;
      }

    } // namespace conversions

    namespace backend
    {
      namespace adaptors
      {
        class MakeSwapchain : public Swapchain
        {
        public:
          MakeSwapchain(Device* device, void* windowHandle, s32 clientWidth, s32 clientHeight, DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R10G10B10A2_UNORM)
              : Swapchain(*device, (HWND)windowHandle, clientWidth, clientHeight, backBufferFormat)
          {
          }

          ~MakeSwapchain() override = default;
        };

        class MakeImGUIRenderer : public ImGUIRenderer
        {
        public:
          MakeImGUIRenderer(renderer::Device* device, void* hwnd, const renderer::RenderTarget& renderTarget)
              : ImGUIRenderer(*device, (HWND)hwnd, renderTarget)
          {
          }

          ~MakeImGUIRenderer() override = default;
        };

        class MakeClearState : public ClearState
        {
        public:
          MakeClearState()
              : ClearState()
          {
          }
          MakeClearState(CreateClearStateDesc&& desc)
              : ClearState(rsl::move(desc))
          {
          }

          ~MakeClearState() override = default;
        };

        class MakeRasterState : public RasterState
        {
        public:
          MakeRasterState(CreateRasterStateDesc&& desc)
              : RasterState(rsl::move(desc))
          {
          }

          ~MakeRasterState() override = default;
        };
      } // namespace adaptors

      class Context
      {
      public:
        //-------------------------------------------------------------------------
        Context(const OutputWindowUserData& userData)
            : m_frame_counter(0)
            , m_current_backbuffer_index(0)
        {
          m_device        = Device::create();
          m_swapchain     = rsl::make_shared<adaptors::MakeSwapchain>(m_device.get(), userData.primary_display_handle, userData.window_width, userData.window_height, DXGI_FORMAT_R8G8B8A8_UNORM);
          m_resource_pool = rsl::make_unique<ResourcePool>();
          m_render_target = rsl::make_unique<RenderTarget>();

          auto& command_queue = m_device->command_queue(D3D12_COMMAND_LIST_TYPE_COPY);

          // Load shader library
          shader_library::load(m_device.get());

          // Create PSO
          struct pipeline_state_stream
          {
            CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE p_root_signature;
            CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT input_layout;
            CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY primitive_topology_type;
            CD3DX12_PIPELINE_STATE_STREAM_VS VS;
            CD3DX12_PIPELINE_STATE_STREAM_PS PS;
            CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSV_format;
            CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTV_formats;
            CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_DESC sample_desc;
          } pipeline_state_stream;

          // Create a color buffer with sRGB for gamma correction.
          DXGI_FORMAT back_buffer_format  = DXGI_FORMAT_R8G8B8A8_UNORM;
          DXGI_FORMAT depth_buffer_format = DXGI_FORMAT_D32_FLOAT;

          // Check the best multisample quality level that can be used for the given back buffer format.
          DXGI_SAMPLE_DESC sample_desc = m_device->multisample_quality_levels(back_buffer_format);

          // Create a color buffer with sRGB for gamma correction.
          D3D12_RT_FORMAT_ARRAY rtv_formats = {};
          rtv_formats.NumRenderTargets      = 1;
          rtv_formats.RTFormats[0]          = back_buffer_format;

          ShaderInfo unlit_info = shader_library::find_shader_info(shader_library::tags::unlit);

          pipeline_state_stream.p_root_signature        = unlit_info.root_signature->d3d_root_signature().Get();
          pipeline_state_stream.input_layout            = unlit_info.input_layout;
          pipeline_state_stream.primitive_topology_type = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
          pipeline_state_stream.VS                      = CD3DX12_SHADER_BYTECODE(unlit_info.vertex_shader.Get());
          pipeline_state_stream.PS                      = CD3DX12_SHADER_BYTECODE(unlit_info.pixel_shader.Get());
          pipeline_state_stream.DSV_format              = depth_buffer_format;
          pipeline_state_stream.RTV_formats             = rtv_formats;
          pipeline_state_stream.sample_desc             = sample_desc;

          m_pipeline_state_object = m_device->create_pipeline_state_object(pipeline_state_stream);

          // Create an off-screen render target with a single color buffer and a depth buffer.
          auto color_desc = CD3DX12_RESOURCE_DESC::Tex2D(back_buffer_format, userData.window_width, userData.window_height, 1, 1, sample_desc.Count, sample_desc.Quality, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

          CreateClearStateDesc clear_state_desc;
          clear_state_desc.rgba.color_data[0] = 0.4f;
          clear_state_desc.rgba.color_data[1] = 0.6f;
          clear_state_desc.rgba.color_data[2] = 0.9f;
          clear_state_desc.rgba.color_data[3] = 1.0f;

          // Create an off-screen render target with a single color buffer and a depth buffer.
          D3D12_CLEAR_VALUE color_clear_value;
          color_clear_value.Format   = color_desc.Format;
          color_clear_value.Color[0] = 0.4f;
          color_clear_value.Color[1] = 0.6f;
          color_clear_value.Color[2] = 0.9f;
          color_clear_value.Color[3] = 1.0f;

          auto color_texture = m_device->create_texture(color_desc, &color_clear_value);
          color_texture->set_resource_name(rsl::wstring(L"Color Render Target"));
          ResourceSlot color_texture_slot             = m_resource_pool->allocate(color_texture);
          ResourceSlot color_texture_clear_state_slot = m_resource_pool->allocate(rsl::make_shared<adaptors::MakeClearState>(rsl::move(clear_state_desc)));

          // Create a depth buffer.
          auto depth_desc = CD3DX12_RESOURCE_DESC::Tex2D(depth_buffer_format, userData.window_width, userData.window_height, 1, 1, sample_desc.Count, sample_desc.Quality, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

          CreateClearStateDesc depth_state_desc;
          depth_state_desc.depth   = 1.0f;
          depth_state_desc.stencil = 0;

          D3D12_CLEAR_VALUE depth_clear_value;
          depth_clear_value.Format       = depth_desc.Format;
          depth_clear_value.DepthStencil = {1.0f, 0};

          auto depth_texture = m_device->create_texture(depth_desc, &depth_clear_value);
          depth_texture->set_resource_name(rsl::wstring(L"Depth Render Target"));
          ResourceSlot depth_texture_slot             = m_resource_pool->allocate(depth_texture);
          ResourceSlot depth_texture_clear_state_slot = m_resource_pool->allocate(rsl::make_shared<adaptors::MakeClearState>(rsl::move(depth_state_desc)));

          // Attach the textures to the render target.
          m_render_target->attach_texture(AttachmentPoint::Color0, color_texture);
          m_render_target->attach_texture(AttachmentPoint::DepthStencil, depth_texture);

          // Creat imgui renderer
          m_gui_renderer = rsl::make_unique<adaptors::MakeImGUIRenderer>(m_device.get(), userData.primary_display_handle, m_swapchain->render_target());

          // Setup sccene visitors
          Viewport viewport        = {0.0f, 0.0f, static_cast<float>(userData.window_width), static_cast<float>(userData.window_height), 0.0f, 1.0f};
          ScissorRect scissor_rect = {0, 0, LONG_MAX, LONG_MAX};

          m_camera_visitor = rsl::make_unique<CameraVisitor>(viewport, scissor_rect);
          m_camera_visitor->set_clear_state(color_texture_slot, color_texture_clear_state_slot);
          m_camera_visitor->set_clear_depth_state(depth_texture_slot, depth_texture_clear_state_slot);
          m_mesh_visitor = rsl::make_unique<MeshVisitor>();

          // Find shader model
          auto adapter = m_device->dxgi_adapter();

          const D3D_FEATURE_LEVEL feature_level = query_feature_level(adapter);
          const D3D_SHADER_MODEL shader_model   = query_shader_model_version(m_device->d3d_device());

          m_renderer_info.shader_version = shader_model_name(shader_model);
          m_renderer_info.api_version    = feature_level_name(feature_level);
          m_renderer_info.adaptor        = m_device->adapter_description().name;
          m_renderer_info.vendor         = m_device->adapter_description().vendor_name;

          command_queue.flush(); // Wait for loading operations to complete before rendering the first frame.

          // Let's make sure we start in a recording state
          begin_frame();
        }

        //-------------------------------------------------------------------------
        ~Context()
        {
          m_camera_visitor.reset();
          m_mesh_visitor.reset();

          shader_library::unload();

          m_gui_renderer.reset();
          m_render_target.reset();

          m_resource_pool->clear();
          m_resource_pool.reset();

          m_pipeline_state_object.reset();
          m_swapchain.reset();
          m_device.reset();
        }

        //-------------------------------------------------------------------------
        void post_initialize()
        {
          end_frame();

          // Let's make sure we start in a recording state
          begin_frame();
          begin_gui_frame();
        }

        //-------------------------------------------------------------------------
        void draw_scene(Scene* scene)
        {
          scene->accept(*m_camera_visitor);

          ShaderInfo unlit_info = shader_library::find_shader_info(shader_library::tags::unlit);

          command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_graphics_root_signature(unlit_info.root_signature);
          command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_pipeline_state(m_pipeline_state_object);

          command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_render_target(*m_render_target);

          scene->accept(*m_mesh_visitor);

          // Resolve the MSAA render target to the swapchain's backbuffer.
          auto& swapchain_RT = m_swapchain->render_target();

          auto swapchain_back_buffer = swapchain_RT.texture(AttachmentPoint::Color0);
          auto msaa_render_target    = m_render_target->texture(AttachmentPoint::Color0);

          command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->resolve_subresource(swapchain_back_buffer, msaa_render_target);
        }

        //-------------------------------------------------------------------------
        void draw_gui()
        {
          m_gui_renderer->draw(command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT), m_swapchain->render_target());
        }

        //-------------------------------------------------------------------------
        void begin_gui_frame()
        {
          m_gui_renderer->new_frame();
        }

        //-------------------------------------------------------------------------
        void end_gui_frame()
        {
          // Nothing to implement
        }

        //-------------------------------------------------------------------------
        void begin_frame()
        {
          ++m_frame_counter;

          activate_command_list(m_current_backbuffer_index);

          m_swapchain->wait_for_swapchain();
        }

        //-------------------------------------------------------------------------
        void end_frame()
        {
          execute_command_list(m_current_backbuffer_index);
        }

        //-------------------------------------------------------------------------
        void present()
        {
          s32 prev_backbuffer_index = m_current_backbuffer_index;

          m_current_backbuffer_index = m_swapchain->present();

          m_compute_active_command_lists[prev_backbuffer_index] = nullptr;
          m_copy_active_command_lists[prev_backbuffer_index]    = nullptr;
          m_direct_active_command_lists[prev_backbuffer_index]  = nullptr;
        }

        //-------------------------------------------------------------------------
        void activate_command_list(u64 index)
        {
          auto& compute_command_queue = m_device->command_queue(D3D12_COMMAND_LIST_TYPE_COMPUTE);
          auto& copy_command_queue    = m_device->command_queue(D3D12_COMMAND_LIST_TYPE_COPY);
          auto& direct_command_queue  = m_device->command_queue(D3D12_COMMAND_LIST_TYPE_DIRECT);

          m_compute_active_command_lists[index] = compute_command_queue.command_list();
          m_copy_active_command_lists[index]    = copy_command_queue.command_list();
          m_direct_active_command_lists[index]  = direct_command_queue.command_list();
        }

        //-------------------------------------------------------------------------
        void execute_command_list(u64 index)
        {
          auto& compute_command_queue = m_device->command_queue(D3D12_COMMAND_LIST_TYPE_COMPUTE);
          auto& copy_command_queue    = m_device->command_queue(D3D12_COMMAND_LIST_TYPE_COPY);
          auto& direct_command_queue  = m_device->command_queue(D3D12_COMMAND_LIST_TYPE_DIRECT);

          compute_command_queue.execute_command_list(m_compute_active_command_lists[index]);
          copy_command_queue.execute_command_list(m_copy_active_command_lists[index]);
          direct_command_queue.execute_command_list(m_direct_active_command_lists[index]);
        }

        //-------------------------------------------------------------------------
        const Info& renderer_info() const
        {
          return m_renderer_info;
        }

        //-------------------------------------------------------------------------
        rsl::shared_ptr<CommandList> command_list_for_backbuffer_index(D3D12_COMMAND_LIST_TYPE type, s32 index)
        {
          switch(type)
          {
            case D3D12_COMMAND_LIST_TYPE_COMPUTE: return m_compute_active_command_lists[index];
            case D3D12_COMMAND_LIST_TYPE_COPY: return m_copy_active_command_lists[index];
            case D3D12_COMMAND_LIST_TYPE_DIRECT: return m_direct_active_command_lists[index];
          }

          REX_ASSERT("Invalid CommandListType given");
          return nullptr;
        }

        //-------------------------------------------------------------------------
        rsl::shared_ptr<CommandList> command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE type)
        {
          return command_list_for_backbuffer_index(type, m_current_backbuffer_index);
        }

        //-------------------------------------------------------------------------
        ResourcePool* resource_pool()
        {
          return m_resource_pool.get();
        }

        //-------------------------------------------------------------------------
        Device* device()
        {
          return m_device.get();
        }

        //-------------------------------------------------------------------------
        Swapchain* swapchain()
        {
          return m_swapchain.get();
        }

      private:
        rsl::unique_ptr<CameraVisitor> m_camera_visitor;
        rsl::unique_ptr<MeshVisitor> m_mesh_visitor;

        rsl::unique_ptr<ImGUIRenderer> m_gui_renderer;

        rsl::unique_ptr<RenderTarget> m_render_target;
        rsl::unique_ptr<ResourcePool> m_resource_pool;
        rsl::shared_ptr<PipelineStateObject> m_pipeline_state_object;
        rsl::shared_ptr<Device> m_device;
        rsl::shared_ptr<Swapchain> m_swapchain;

        rsl::unordered_map<s32, rsl::shared_ptr<CommandList>> m_direct_active_command_lists;
        rsl::unordered_map<s32, rsl::shared_ptr<CommandList>> m_copy_active_command_lists;
        rsl::unordered_map<s32, rsl::shared_ptr<CommandList>> m_compute_active_command_lists;

        Info m_renderer_info;

        u64 m_frame_counter;
        s32 m_current_backbuffer_index;
      };

      rsl::unique_ptr<Context> g_ctx; // NOLINT (fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

      //-------------------------------------------------------------------------
      bool initialize(const OutputWindowUserData& userData)
      {
        g_ctx = rsl::make_unique<Context>(userData);

        return true;
      }

      //-------------------------------------------------------------------------
      bool post_initialize()
      {
        g_ctx->post_initialize();

        return true;
      }

      //-------------------------------------------------------------------------
      void shutdown()
      {
        g_ctx.reset();
      }

      //-------------------------------------------------------------------------
      const Info& info()
      {
        return g_ctx->renderer_info();
      }

      //-------------------------------------------------------------------------
      bool is_y_up()
      {
        return false;
      }

      //-------------------------------------------------------------------------
      bool is_depth_0_to_1()
      {
        return true;
      }

      //-------------------------------------------------------------------------
      void present()
      {
        g_ctx->end_gui_frame();
        g_ctx->end_frame();

        g_ctx->present();

        // Always make sure the renderer is in a recording state!
        g_ctx->begin_frame();
        g_ctx->begin_gui_frame();
      }

      //-------------------------------------------------------------------------
      void draw_scene(Scene* scene)
      {
        g_ctx->draw_scene(scene);
      }

      //-------------------------------------------------------------------------
      void draw_gui()
      {
        g_ctx->draw_gui();
      }

      //-------------------------------------------------------------------------
      void flush()
      {
        g_ctx->device()->flush();
      }

      //-------------------------------------------------------------------------
      ResourceSlot copy_vertex_buffer(size_t numVertices, size_t vertexStride, const void* vertexBufferData)
      {
        rsl::shared_ptr<VertexBuffer> vertex_buffer = g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_COPY)->copy_vertex_buffer(numVertices, vertexStride, vertexBufferData);

        return g_ctx->resource_pool()->allocate(vertex_buffer);
      }

      //-------------------------------------------------------------------------
      ResourceSlot copy_index_buffer(size_t numIndices, Format indexFormat, const void* indexBufferData)
      {
        rsl::shared_ptr<IndexBuffer> index_buffer = g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_COPY)->copy_index_buffer(numIndices, dxgi::conversions::to_DXGI(indexFormat), indexBufferData);

        return g_ctx->resource_pool()->allocate(index_buffer);
      }

      //-------------------------------------------------------------------------
      ResourceSlot create_clear_state(CreateClearStateDesc&& desc)
      {
        return g_ctx->resource_pool()->allocate(rsl::make_shared<adaptors::MakeClearState>(rsl::move(desc)));
      }

      //-------------------------------------------------------------------------
      ResourceSlot create_raster_state(CreateRasterStateDesc&& desc)
      {
        return g_ctx->resource_pool()->allocate(rsl::make_shared<adaptors::MakeRasterState>(rsl::move(desc)));
      }

      //-------------------------------------------------------------------------
      ResourceSlot create_texture(CreateTextureDesc&& desc)
      {
        rsl::shared_ptr<Texture> texture = nullptr;

        D3D12_RESOURCE_DESC resource_desc = conversions::to_RESOURCE_DESC(desc.resource_desc);
        if(desc.clear_state_desc)
        {
          D3D12_CLEAR_VALUE clear_value_desc = conversions::to_CLEAR_VALUE(*desc.clear_state_desc, desc.clear_state_format);
          texture                            = g_ctx->device()->create_texture(resource_desc, &clear_value_desc);
        }
        else
        {
          texture = g_ctx->device()->create_texture(resource_desc, nullptr);
        }

        return g_ctx->resource_pool()->allocate(texture);
      }

      //-------------------------------------------------------------------------
      ResourceSlot create_vertex_buffer(CreateVertexBufferDesc&& desc)
      {
        rsl::shared_ptr<VertexBuffer> vertex_buffer = g_ctx->device()->create_vertex_buffer(desc.num_vertices, desc.vertex_stride);

        return g_ctx->resource_pool()->allocate(vertex_buffer);
      }

      //-------------------------------------------------------------------------
      ResourceSlot create_index_buffer(CreateIndexBufferDesc&& desc)
      {
        DXGI_FORMAT dxgi_format = dxgi::conversions::to_DXGI(desc.index_format);

        rsl::shared_ptr<IndexBuffer> index_buffer = g_ctx->device()->create_index_buffer(desc.num_indices, dxgi_format);

        return g_ctx->resource_pool()->allocate(index_buffer);
      }

      //-------------------------------------------------------------------------
      ResourceSlot create_pipeline_state_object(CreatePipelineStateDesc&& desc)
      {
        struct pipeline_state_stream
        {
          CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE p_root_signature;
          CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT input_layout;
          CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY primitive_topology_type;
          CD3DX12_PIPELINE_STATE_STREAM_VS VS;
          CD3DX12_PIPELINE_STATE_STREAM_PS PS;
          CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSV_format;
          CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTV_formats;
          CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_DESC sample_desc;
          CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER raster_desc;
          CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC blend_desc;
        } pipeline_state_stream;

        // Create a color buffer with sRGB for gamma correction.
        DXGI_FORMAT back_buffer_format  = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        DXGI_FORMAT depth_buffer_format = DXGI_FORMAT_D32_FLOAT;

        // Check the best multisample quality level that can be used for the given back buffer format.
        DXGI_SAMPLE_DESC sample_desc = g_ctx->device()->multisample_quality_levels(back_buffer_format);

        D3D12_RASTERIZER_DESC raster_desc = conversions::to_RASTERIZER_DESC(g_ctx->resource_pool()->as<RasterState>(desc.rasterizer_state));

        D3D12_RT_FORMAT_ARRAY rtv_formats = {};
        rtv_formats.NumRenderTargets      = 1;
        rtv_formats.RTFormats[0]          = back_buffer_format;

        ShaderInfo unlit_info = shader_library::find_shader_info(shader_library::tags::unlit);

        pipeline_state_stream.p_root_signature        = unlit_info.root_signature->d3d_root_signature().Get();
        pipeline_state_stream.input_layout            = unlit_info.input_layout;
        pipeline_state_stream.primitive_topology_type = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pipeline_state_stream.VS                      = CD3DX12_SHADER_BYTECODE(unlit_info.vertex_shader.Get());
        pipeline_state_stream.PS                      = CD3DX12_SHADER_BYTECODE(unlit_info.pixel_shader.Get());
        pipeline_state_stream.DSV_format              = depth_buffer_format;
        pipeline_state_stream.RTV_formats             = rtv_formats;
        pipeline_state_stream.sample_desc             = sample_desc;
        pipeline_state_stream.raster_desc             = CD3DX12_RASTERIZER_DESC(raster_desc);
        pipeline_state_stream.blend_desc              = CD3DX12_BLEND_DESC();

        rsl::shared_ptr<PipelineStateObject> pso = g_ctx->device()->create_pipeline_state_object(pipeline_state_stream);

        return g_ctx->resource_pool()->allocate(pso);
      }

      void release_resource(const ResourceSlot& slot)
      {
        g_ctx->resource_pool()->remove(slot);
      }

      //-------------------------------------------------------------------------
      void clear_texture(const ResourceSlot& clearStateTarget, const ResourceSlot& clearState)
      {
        rsl::shared_ptr<Texture> texture        = g_ctx->resource_pool()->as<Texture>(clearStateTarget);
        rsl::shared_ptr<ClearState> clear_state = g_ctx->resource_pool()->as<ClearState>(clearState);

        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->clear_texture(texture, clear_state->rgba().color_data.data());
      }

      //-------------------------------------------------------------------------
      void clear_depth_stencil_texture(const ResourceSlot& clearStateTarget, const ResourceSlot& clearState)
      {
        rsl::shared_ptr<Texture> texture        = g_ctx->resource_pool()->as<Texture>(clearStateTarget);
        rsl::shared_ptr<ClearState> clear_state = g_ctx->resource_pool()->as<ClearState>(clearState);

        s32 depth_stencil_clear_flags = 0;
        depth_stencil_clear_flags |= (clear_state->flags() & ClearBits::ClearDepthBuffer) ? D3D12_CLEAR_FLAG_DEPTH : 0;
        depth_stencil_clear_flags |= (clear_state->flags() & ClearBits::ClearStencilBuffer) ? D3D12_CLEAR_FLAG_STENCIL : 0;

        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->clear_depth_stencil_texture(texture, (D3D12_CLEAR_FLAGS)depth_stencil_clear_flags, clear_state->depth(), clear_state->stencil());
      }

      //-------------------------------------------------------------------------
      void draw_geometry(u32 vertexCount, u32 instanceCount, u32 startVertex, u32 startInstance)
      {
        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->draw(vertexCount, instanceCount, startVertex, startInstance);
      }

      //-------------------------------------------------------------------------
      void draw_geometry_indexed(u32 indexCount, u32 instanceCount, u32 startIndex, s32 baseVertex, u32 startInstance)
      {
        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->draw_indexed(indexCount, instanceCount, startIndex, baseVertex, startInstance);
      }

      //-------------------------------------------------------------------------
      void set_render_target(const ResourceSlot& renderTarget)
      {
        rsl::shared_ptr<RenderTarget> render_target = g_ctx->resource_pool()->as<RenderTarget>(renderTarget);

        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_render_target(*render_target);
      }

      //-------------------------------------------------------------------------
      void set_pipeline_state(const ResourceSlot& pipelineState)
      {
        rsl::shared_ptr<PipelineStateObject> pso = g_ctx->resource_pool()->as<PipelineStateObject>(pipelineState);

        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_pipeline_state(pso);
      }

      //-------------------------------------------------------------------------
      void set_graphics_dynamic_constant_buffer(u32 rootParameterIndex, rsl::memory_size sizeInBytes, const void* bufferData)
      {
        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_graphics_dynamic_constant_buffer(rootParameterIndex, sizeInBytes, bufferData);
      }

      //-------------------------------------------------------------------------
      void set_graphics_32_bit_constants(u32 rootParameterIndex, u32 numConstants, const void* constants)
      {
        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_graphics_32_bit_constants(rootParameterIndex, numConstants, constants);
      }

      //-------------------------------------------------------------------------
      void set_vertex_buffer(u32 slot, const ResourceSlot& vertexBuffer)
      {
        rsl::shared_ptr<VertexBuffer> vertex_buffer = g_ctx->resource_pool()->as<VertexBuffer>(vertexBuffer);

        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_vertex_buffer(slot, vertex_buffer);
      }

      //-------------------------------------------------------------------------
      void set_vertex_buffers(u32 slot, const rsl::vector<ResourceSlot>& vertexBuffers)
      {
        rsl::vector<rsl::shared_ptr<VertexBuffer>> vertex_buffers;
        vertex_buffers.reserve(vertexBuffers.size());

        for(const ResourceSlot& vertex_buffer_slot: vertexBuffers)
        {
          vertex_buffers.push_back(g_ctx->resource_pool()->as<VertexBuffer>(vertex_buffer_slot));
        }

        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_vertex_buffers(slot, vertex_buffers);
      }

      //-------------------------------------------------------------------------
      void set_dynamic_vertex_buffer(u32 slot, size_t numVertices, rsl::memory_size vertexSize, const void* vertexBufferData)
      {
        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_dynamic_vertex_buffer(slot, numVertices, vertexSize, vertexBufferData);
      }

      //-------------------------------------------------------------------------
      void set_index_buffer(const ResourceSlot& indexBuffer)
      {
        rsl::shared_ptr<IndexBuffer> index_buffer = g_ctx->resource_pool()->as<IndexBuffer>(indexBuffer);

        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_index_buffer(index_buffer);
      }

      //-------------------------------------------------------------------------
      void set_dynamic_index_buffer(size_t numIndicies, Format indexFormat, const void* indexBufferData)
      {
        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_dynamic_index_buffer(numIndicies, dxgi::conversions::to_DXGI(indexFormat), indexBufferData);
      }

      //-------------------------------------------------------------------------
      void set_primitive_topology(PrimitiveTopology primitiveTopology)
      {
        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_primitive_topology(conversions::to_PRIMITIVE_TOPOLOGY(primitiveTopology));
      }

      //-------------------------------------------------------------------------
      void set_viewport(const Viewport& vp)
      {
        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_viewport(conversions::to_VIEWPORT(vp));
      }

      //-------------------------------------------------------------------------
      void set_viewports(const rsl::vector<Viewport>& vps)
      {
        rsl::vector<D3D12_VIEWPORT> viewports;
        viewports.reserve(vps.size());
        for(const Viewport& vp: vps)
        {
          viewports.push_back(conversions::to_VIEWPORT(vp));
        }
        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_viewports(viewports);
      }

      //-------------------------------------------------------------------------
      void set_scissor_rect(const ScissorRect& sr)
      {
        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_scissor_rect(conversions::to_RECT(sr));
      }

      //-------------------------------------------------------------------------
      void set_scissor_rects(const rsl::vector<ScissorRect>& srs)
      {
        rsl::vector<D3D12_RECT> rects;
        rects.reserve(srs.size());
        for(const ScissorRect& sr: srs)
        {
          rects.push_back(conversions::to_RECT(sr));
        }
        g_ctx->command_list_for_current_backbuffer(D3D12_COMMAND_LIST_TYPE_DIRECT)->set_scissor_rects(rects);
      }
    } // namespace backend
  }   // namespace renderer
} // namespace rex