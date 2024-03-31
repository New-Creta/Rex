#include "rex_directx/system/dx_rhi.h"

#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/adapter_manager.h"
#include "rex_directx/dxgi/adapter.h"

#include "rex_directx/utility/dx_util.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/diagnostics/log.h"
#include "rex_directx/system/dx_device.h"
#include "rex_directx/system/dx_feature_level.h"
#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/system/dx_commandlist.h"
#include "rex_directx/system/dx_descriptor_heap.h"
#include "rex_directx/system/dx_resource_heap.h"
#include "rex_directx/system/dx_resource.h"
#include "rex_directx/system/dx_swapchain.h"
#include "rex_directx/system/dx_shader_compiler.h"
#include "rex_directx/system/dx_vertex_buffer.h"
#include "rex_directx/system/dx_index_buffer.h"
#include "rex_directx/system/dx_constant_buffer.h"
#include "rex_directx/system/dx_feature_shader_model.h"

#include "rex_directx/resources/dx_vertex_shader_resource.h"
#include "rex_directx/resources/dx_pixel_shader_resource.h"
#include "rex_directx/resources/dx_shader_program_resource.h"
#include "rex_directx/resources/dx_clear_state_resource.h"
#include "rex_directx/resources/dx_raster_state_resource.h"
#include "rex_directx/resources/dx_input_layout_resource.h"

#include "rex_directx/utility/d3dx12.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/pointer_math.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"
#include "rex_renderer_core/rendering/viewport.h"
#include "rex_renderer_core/rendering/scissor_rect.h"
#include "rex_renderer_core/system/resource_hasher.h"

#include "rex_std/bonus/utility.h"

#include <D3Dcompiler.h>

namespace rex
{
  namespace rhi
  {
    DEFINE_LOG_CATEGORY(LogRhi, rex::LogVerbosity::Log);

    namespace internal
    {
      // The RHI class. 
      // This manages rhi initializes and shutdown
      // It owns the lifetime of GPU objects.
      struct RenderHardwareInfrastructure
      {
      public:
        RenderHardwareInfrastructure(const renderer::OutputWindowUserData& userData);
        ~RenderHardwareInfrastructure();

      private:
        // DXGI Factory
        s32 init_debug_interface();
        bool init_dxgi_factory();
        void init_debug_controller();

        // D3D Device
        count_t highest_scoring_gpu(const rsl::vector<GpuDescription>& gpus);
        void init_debug_layer();

        bool init_d3d_device();

        // D3D Command Objects
        bool init_command_queue();

        bool init_command_list();

        // DXGI Swapchain
        bool init_swapchain(const renderer::OutputWindowUserData& userData);

        // Resource Heaps
        bool init_resource_heaps();

        // Descriptor Heaps
        bool init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE type, s32 numDescriptors);
        bool init_descriptor_heaps();

        // Swapchain Buffer Views
        bool init_swapchain_buffer_views(const renderer::OutputWindowUserData& userData);
        bool init_swapchain_rtvs(const renderer::OutputWindowUserData& userData);
        bool init_swapchain_dsvs(const renderer::OutputWindowUserData& userData);

        // Pipeline Library
        bool init_pipeline_library();

        // Upload buffers
        bool init_upload_buffers();

      private:
        constexpr static s32 s_swapchain_buffer_count = 2;
        constexpr static s32 s_num_rtv_descs = 8;
        constexpr static s32 s_num_dsv_descs = 1;
        constexpr static s32 s_num_cbv_descs = 128;
        constexpr static rsl::array m_expected_feature_levels = { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1 };

      public:
        bool init_successful;
        // Keep the debug interface as the last resource to be destroyed
        // it automatically reports if any resources are still active on destruction
        DebugInterface debug_interface;
        rsl::unique_ptr<dxgi::Factory> factory;
        rsl::unique_ptr<DirectXDevice> device;
        rsl::unique_ptr<Swapchain> swapchain;
        rsl::unique_ptr<CommandQueue> command_queue;
        rsl::unique_ptr<CommandList> command_list;
        rsl::unique_ptr<ResourceHeap> heap;
        rsl::unique_ptr<Resource> depth_stencil_buffer;
        rsl::unique_ptr<UploadBuffer> upload_buffer;
        rsl::unique_ptr<PipelineLibrary> pso_lib;
        ResourcePool resource_pool;

        wrl::ComPtr<IDXGIInfoQueue> debug_info_queue;
        rsl::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, DescriptorHeap> descriptor_heap_pool;

        STATIC_WARNING("Move these fields into the swapchain")
        rsl::vector<Resource> swapchain_buffers;   // swapchain render target buffer indices
        rsl::array<D3D12_CPU_DESCRIPTOR_HANDLE, s_swapchain_buffer_count> swapchain_rtvs;
        D3D12_CPU_DESCRIPTOR_HANDLE dsv;
        s32 current_swapchain_buffer_idx;
      };

      rsl::unique_ptr<RenderHardwareInfrastructure> g_rhi;
      RenderHardwareInfrastructure* get()
      {
        return g_rhi.get();
      }
    }

    const Info& info()
    {
      return internal::get()->device->info();
    }

    bool init(const renderer::OutputWindowUserData& userData)
    {
      internal::g_rhi = rsl::make_unique<internal::RenderHardwareInfrastructure>(userData);

      if (!internal::get()->init_successful)
      {
        REX_ERROR(LogRhi, "Failed to initialize RHI");
        shutdown();
        return false;
      }

      // Flush the command queue, so we can be sure the entire rhi is setup
      // and the gpu has all the resources it needs by the time we exit
      // the initialization phase.
      internal::get()->command_queue->flush();

      return internal::get()->init_successful;
    }
    void shutdown()
    {
      internal::g_rhi.reset();
    }

    void prepare_user_initialization()
    {
      reset_command_list(ResourceSlot::make_invalid());
    }
    void finish_user_initialization()
    {
      exec_command_list();
      flush_command_queue();
    }

    // Command line interface
    void reset_command_list(const ResourceSlot& psoSlot)
    {
      ID3D12PipelineState* pso = nullptr;
      if (psoSlot.is_valid())
      {
        pso = internal::get()->resource_pool.as<PipelineState>(psoSlot)->get();
      }
      internal::get()->command_list->reset(pso);
    }
    void exec_command_list()
    {
      internal::get()->command_list->exec(internal::get()->command_queue.get());
    }
    void flush_command_queue()
    {
      internal::get()->command_queue->flush();
    }

    // shader API
    ResourceSlot compile_shader(const CompileShaderDesc& desc)
    {
      // Check if we don't have the compiled shader already
      ResourceHash hash = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(hash))
      {
        return internal::get()->resource_pool.at(hash);
      }

      // If we don't have it already, compile the shader
      ShaderCompiler compiler;
      wrl::ComPtr<ID3DBlob> byte_code = compiler.compile_shader(desc);

      if (!byte_code)
      {
        REX_ERROR(LogRhi, "Failed to compile shader");
        return ResourceSlot::make_invalid();
      }

      // store the new compiled shader with its hash in the resource pool
      switch (desc.shader_type)
      {
      case ShaderType::VERTEX:
        return internal::get()->resource_pool.insert(rsl::make_unique<VertexShaderResource>(hash, byte_code));
        break;
      case ShaderType::PIXEL:
        return internal::get()->resource_pool.insert(rsl::make_unique<PixelShaderResource>(hash, byte_code));
        break;

      default:
        REX_ERROR(LogDirectX, "Unsupported Shader Type was given");
      }

      return ResourceSlot::make_invalid();

    }
    ResourceSlot link_shader(const LinkShaderDesc& desc)
    {
      // If there's already a linked shader, return it.
      ResourceHash hash = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(hash))
      {
        return internal::get()->resource_pool.at(hash);
      }

      // If not link the shader with the params provided
      auto root_sig = d3d::create_shader_root_signature(desc.constants);

      if (!root_sig)
      {
        REX_ERROR(LogRhi, "Failed to create root signature");
        return ResourceSlot::make_invalid();
      }

      auto* vertex_shader = internal::get()->resource_pool.as<VertexShaderResource>(desc.vertex_shader);
      auto* pixel_shader = internal::get()->resource_pool.as<PixelShaderResource>(desc.pixel_shader);

      rsl::unique_array<ConstantLayoutDescription> constants = rsl::make_unique<ConstantLayoutDescription[]>(desc.constants.count());
      rsl::memcpy(constants.get(), desc.constants.get(), desc.constants.byte_size());

      // create a combined shader object with the root sig, the vertex shader and the pixel shader
      return internal::get()->resource_pool.insert(rsl::make_unique<ShaderProgramResource>(hash, root_sig, vertex_shader, pixel_shader, rsl::move(constants)));
    }
    ResourceSlot create_shader(const ShaderDesc& desc)
    {
      // Check if the shader isn't already loaded
      ResourceHash hash = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(hash))
      {
        return internal::get()->resource_pool.at(hash);
      }

      // If the shader hasn't been loaded before
      wrl::ComPtr<ID3DBlob> byte_code = rex::d3d::create_blob(desc.shader_byte_code);

      switch (desc.shader_type)
      {
      case ShaderType::VERTEX: return internal::get()->resource_pool.insert(rsl::make_unique<VertexShaderResource>(hash, byte_code));
      case ShaderType::PIXEL: return internal::get()->resource_pool.insert(rsl::make_unique<PixelShaderResource>(hash, byte_code));

      default:
        REX_ERROR(LogDirectX, "Unsupported Shader Type was given");
      }

      return ResourceSlot::make_invalid();
    }

    // A clear state is just a struct holding different values to clear a buffer
    // Flags control which part of the buffer (color, depths or stencil) should be cleared
    ResourceSlot create_clear_state(const ClearStateDesc& desc)
    {
      ResourceHash hash = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(hash))
      {
        return internal::get()->resource_pool.at(hash);
      }

      return internal::get()->resource_pool.insert(rsl::make_unique<ClearStateResource>(hash, desc));
    }
    // A raster state holds rasterization settings
    // settings like cull mode, fill mode, depth bias, normal orientation, ..
    // are all included in the raster state
    ResourceSlot create_raster_state(const RasterStateDesc& desc)
    {
      ResourceHash hash = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(hash))
      {
        return internal::get()->resource_pool.at(hash);
      }

      D3D12_RASTERIZER_DESC d3d_rs;

      d3d_rs.FillMode = rex::d3d::to_d3d12_fill_mode(desc.fill_mode);
      d3d_rs.CullMode = rex::d3d::to_d3d12_cull_mode(desc.cull_mode);
      d3d_rs.FrontCounterClockwise = desc.front_ccw;
      d3d_rs.DepthBias = desc.depth_bias;
      d3d_rs.DepthBiasClamp = desc.depth_bias_clamp;
      d3d_rs.SlopeScaledDepthBias = desc.sloped_scale_depth_bias;
      d3d_rs.DepthClipEnable = desc.depth_clip_enable;
      d3d_rs.ForcedSampleCount = desc.forced_sample_count;

      /**
       * Conservative rasterization means that all pixels that are at least partially covered by a rendered primitive are rasterized, which means that the pixel shader is invoked.
       * Normal behavior is sampling, which is not used if conservative rasterization is enabled.
       *
       * Conservative rasterization is useful in a number of situations outside of rendering (collision detection, occlusion culling, and visibility detection).
       *
       * https://learn.microsoft.com/en-us/windows/win32/direct3d11/conservative-rasterization
       */
      d3d_rs.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
      d3d_rs.MultisampleEnable = desc.multisample;
      d3d_rs.AntialiasedLineEnable = desc.aa_lines;

      return internal::get()->resource_pool.insert(rsl::make_unique<RasterStateResource>(hash, d3d_rs));
    }
    // An input layout determines the format of vertices
    // It determines where a shader can find the position, normal, color
    // of a vertex.
    ResourceSlot create_input_layout(const InputLayoutDesc& desc)
    {
      ResourceHash hash = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(hash))
      {
        return internal::get()->resource_pool.at(hash);
      }
      rsl::vector<D3D12_INPUT_ELEMENT_DESC> input_element_descriptions(rsl::Size(desc.input_layout.size()));
      REX_ASSERT_X(!input_element_descriptions.empty(), "No input elements provided for input layout");

      for (s32 i = 0; i < desc.input_layout.size(); ++i)
      {
        input_element_descriptions[i].SemanticName = desc.input_layout[i].semantic_name.data();
        input_element_descriptions[i].SemanticIndex = desc.input_layout[i].semantic_index;
        input_element_descriptions[i].Format = rex::d3d::to_d3d12_vertex_format(desc.input_layout[i].format);
        input_element_descriptions[i].InputSlot = desc.input_layout[i].input_slot;
        input_element_descriptions[i].AlignedByteOffset = desc.input_layout[i].aligned_byte_offset;
        input_element_descriptions[i].InputSlotClass = rex::d3d::to_d3d12_input_layout_classification(desc.input_layout[i].input_slot_class);
        input_element_descriptions[i].InstanceDataStepRate = desc.input_layout[i].instance_data_step_rate;
      }

      return internal::get()->resource_pool.insert(rsl::make_unique<InputLayoutResource>(hash, input_element_descriptions));
    }
    // A vertex buffer is a buffer holding vertices of 1 or more objects
    ResourceSlot create_vertex_buffer(const VertexBufferDesc& desc)
    {
      ResourceHash hash = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(hash))
      {
        return internal::get()->resource_pool.at(hash);
      }

      // 1) Create the resource on the gpu that'll hold the data of the vertex buffer
      s32 size = desc.blob_view.size();
      wrl::ComPtr<ID3D12Resource> buffer = internal::get()->heap->create_buffer(size);
      set_debug_name_for(buffer.Get(), "Vertex Buffer");
      rsl::unique_ptr<VertexBuffer> vb = rsl::make_unique<VertexBuffer>(buffer, desc.blob_view.size(), desc.vertex_size);

      // 2) Copy the data into the upload buffer
      internal::get()->upload_buffer->write(internal::get()->command_list.get(), vb.get(), desc.blob_view.data(), size);

      //// 3) Upload the data from the upload buffer onto the gpu
      //internal::get()->upload_buffer->upload(internal::get()->command_list.get());

      // 4) Cache that the data is available on the gpu (or will be soon)
      // so if another request comes in for the same data, we don't upload it again

      // 5) Add the resource to the resource pool
      return internal::get()->resource_pool.insert(rsl::move(vb));
    }
    // An index buffer is a buffer holding indices of 1 or more objects
    ResourceSlot create_index_buffer(const IndexBufferDesc& desc)
    {
      ResourceHash hash = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(hash))
      {
        return internal::get()->resource_pool.at(hash);
      }

      // 1) Create the resource on the gpu that'll hold the data of the vertex buffer
      s32 size = desc.blob_view.size();
      DXGI_FORMAT d3d_format = rex::d3d::to_d3d12_index_format(desc.format);
      wrl::ComPtr<ID3D12Resource> buffer = internal::get()->heap->create_buffer(size, 0);
      set_debug_name_for(buffer.Get(), "Index Buffer");
      rsl::unique_ptr<IndexBuffer> ib = rsl::make_unique<IndexBuffer>(buffer, D3D12_RESOURCE_STATE_INDEX_BUFFER, desc.blob_view.size(), d3d_format);

      // 2) Copy the data into the upload buffer
      internal::get()->upload_buffer->write(internal::get()->command_list.get(), ib.get(), desc.blob_view.data(), size);

      // 3) Upload the data from the upload buffer onto the gpu
      //internal::get()->upload_buffer->upload(internal::get()->command_list.get());

      // 4) add the resource to the resource pool
      return internal::get()->resource_pool.insert(rsl::move(ib));
    }
    // A constant buffer is a buffer holding data that's accessible to a shader
    // This can hold data like ints, floats, vectors and matrices
    ResourceSlot create_constant_buffer(const ConstantBufferDesc& desc)
    {
      ResourceHash hash = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(hash))
      {
        return internal::get()->resource_pool.at(hash);
      }

      // 1) Create the resource on the gpu that'll hold the data of the vertex buffer
      s32 size = desc.blob_view.size();
      s32 aligned_size = rex::align(size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
      wrl::ComPtr<ID3D12Resource> buffer = internal::get()->heap->create_buffer(aligned_size);
      set_debug_name_for(buffer.Get(), "Constant Buffer");
      rsl::unique_ptr<ConstantBuffer> cb = rsl::make_unique<ConstantBuffer>(buffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, desc.blob_view.size());

      // 2) Copy the data into the upload buffer
      internal::get()->upload_buffer->write(internal::get()->command_list.get(), cb.get(), desc.blob_view.data(), aligned_size);

      // 3) Create a view to this constant buffer
      internal::get()->descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).create_cbv(buffer.Get(), aligned_size);

      // 4) add the buffer to the resource pool
      return internal::get()->resource_pool.insert(rsl::move(cb));
    }
    // A pipeline state object defines a state for the graphics pipeline.
    // It holds the input layout, root signature, shaders, raster state, blend state ..
    // needed for a draw call.
    ResourceSlot create_pso(const PipelineStateDesc& desc)
    {
      // 1) Load the resources from the resource pool
      InputLayoutResource* input_layout = internal::get()->resource_pool.as<InputLayoutResource>(desc.input_layout);
      ShaderProgramResource* shader = internal::get()->resource_pool.as<ShaderProgramResource>(desc.shader);

      D3D12_RASTERIZER_DESC d3d_raster_state = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
      if (desc.raster_state.is_valid())
      {
        RasterStateResource* raster_state = internal::get()->resource_pool.as<RasterStateResource>(desc.raster_state);
        d3d_raster_state = *raster_state->get();
      }

      D3D12_BLEND_DESC d3d_blend_state = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
      if (desc.blend_state.is_valid())
      {
        STATIC_WARNING("Implement blend state");
        //BlendStateResource* blend_state = internal::get()->resource_pool.as<BlendStateResource>(desc.blend_state);
        //d3d_blend_state = blend_state->get();
      }

      D3D12_DEPTH_STENCIL_DESC d3d_depth_stencil_state = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
      if (desc.depth_stencil_state.is_valid())
      {
        STATIC_WARNING("Implement depth stencil state");
        //DepthStencilStateResource* depth_stencil_state = internal::get()->resource_pool.as<DepthStencilStateResource>(desc.depth_stencil_state);
        //d3d_depth_stencil_state = depth_stencil_state->get();
      }

      // 2) Fill in the PSO desc
      D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc{};
      pso_desc.InputLayout = *input_layout->get();
      pso_desc.pRootSignature = shader->root_signature();
      pso_desc.VS = shader->vs();
      pso_desc.PS = shader->ps();
      pso_desc.RasterizerState = d3d_raster_state;
      pso_desc.BlendState = d3d_blend_state;
      pso_desc.DepthStencilState = d3d_depth_stencil_state;
      pso_desc.SampleMask = UINT_MAX;
      pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
      pso_desc.NumRenderTargets = 1;
      pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
      pso_desc.SampleDesc.Count = 1;
      pso_desc.SampleDesc.Quality = 0;
      pso_desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

      // 3) Create a resource slot for a pso if it already exists in the lib
      rsl::unique_ptr<PipelineState> pso = internal::get()->pso_lib->load_pso(pso_desc);
      if (pso)
      {
        return internal::get()->resource_pool.insert(rsl::move(pso));
      }

      // 4) Ohterwise store the new PSO in the pipeline library
      pso = internal::get()->pso_lib->store_pso(pso_desc);

      return internal::get()->resource_pool.insert(rsl::move(pso));
    }

    void set_viewport(const Viewport& viewport)
    {
      D3D12_VIEWPORT d3d_viewport;
      d3d_viewport.TopLeftX = viewport.top_left_x;
      d3d_viewport.TopLeftY = viewport.top_left_y;
      d3d_viewport.Width    = viewport.width;
      d3d_viewport.Height   = viewport.height;
      d3d_viewport.MinDepth = viewport.min_depth;
      d3d_viewport.MaxDepth = viewport.max_depth;

      internal::get()->command_list->get()->RSSetViewports(1, &d3d_viewport);    
    }
    void set_scissor_rect(const ScissorRect& rect)
    {
      RECT scissor_rect{};
      scissor_rect.top    = (LONG)rect.top;
      scissor_rect.left   = (LONG)rect.left;
      scissor_rect.bottom = (LONG)rect.bottom;
      scissor_rect.right  = (LONG)rect.right;

      internal::get()->command_list->get()->RSSetScissorRects(1, &scissor_rect);
    }

    void transition_backbuffer(D3D12_RESOURCE_STATES state)
    {
      Resource& backbuffer = internal::get()->swapchain_buffers[internal::get()->current_swapchain_buffer_idx];
      backbuffer.transition(internal::get()->command_list->get(), state);
    }
    void clear_backbuffer(const ResourceSlot& clearState)
    {
      ClearStateResource* clear_state = internal::get()->resource_pool.as<ClearStateResource>(clearState);
      auto& clear_flags = clear_state->get()->flags;
      if (clear_flags.has_state(renderer::ClearBits::ClearColorBuffer))
      {
        D3D12_CPU_DESCRIPTOR_HANDLE rtv = internal::get()->swapchain_rtvs[internal::get()->current_swapchain_buffer_idx];
        internal::get()->command_list->get()->ClearRenderTargetView(rtv, clear_state->get()->rgba.data(), 0, nullptr);
      }

      if (clear_flags.has_state(renderer::ClearBits::ClearDepthBuffer) || clear_flags.has_state(renderer::ClearBits::ClearStencilBuffer))
      {
        s32 d3d_clear_flags = 0;
        d3d_clear_flags |= clear_flags.has_state(renderer::ClearBits::ClearDepthBuffer) ? D3D12_CLEAR_FLAG_DEPTH : 0;
        d3d_clear_flags |= clear_flags.has_state(renderer::ClearBits::ClearStencilBuffer) ? D3D12_CLEAR_FLAG_STENCIL : 0;

        D3D12_CPU_DESCRIPTOR_HANDLE dsv = internal::get()->dsv;
        internal::get()->command_list->get()->ClearDepthStencilView(dsv, (D3D12_CLEAR_FLAGS)d3d_clear_flags, clear_state->get()->depth, clear_state->get()->stencil, 0, nullptr);
      }
    }
    void present()
    {
      internal::get()->swapchain->present();
      internal::get()->current_swapchain_buffer_idx++;

      if (internal::get()->current_swapchain_buffer_idx == internal::get()->swapchain_buffers.size())
      {
        internal::get()->current_swapchain_buffer_idx = 0;
      }
    }

    void set_vertex_buffer(const rhi::ResourceSlot& vb)
    {
      D3D12_VERTEX_BUFFER_VIEW view{};
      VertexBuffer* vertex_buffer = internal::get()->resource_pool.as<VertexBuffer>(vb);
      view.BufferLocation = vertex_buffer->get()->GetGPUVirtualAddress();
      view.SizeInBytes = vertex_buffer->size();
      view.StrideInBytes = vertex_buffer->stride();
      
      internal::get()->command_list->get()->IASetVertexBuffers(0, 1, &view);
    }
    void set_index_buffer(const rhi::ResourceSlot& ib)
    {
      D3D12_INDEX_BUFFER_VIEW view{};
      IndexBuffer* index_buffer = internal::get()->resource_pool.as<IndexBuffer>(ib);
      view.BufferLocation = index_buffer->get()->GetGPUVirtualAddress();
      view.Format = index_buffer->format();
      view.SizeInBytes = index_buffer->size();

      internal::get()->command_list->get()->IASetIndexBuffer(&view);
    }
    void set_constant_buffer(s32 idx, const rhi::ResourceSlot& cb)
    {
      ConstantBuffer* constant_buffer = internal::get()->resource_pool.as<ConstantBuffer>(cb);

      static auto cbv = internal::get()->descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).gpu_heap_start();

      internal::get()->command_list->get()->SetGraphicsRootConstantBufferView(idx, constant_buffer->get()->GetGPUVirtualAddress());
    }
    void set_primitive_topology(renderer::PrimitiveTopology topology)
    {
      auto d3d_topology = rex::d3d::to_d3d12_topology(topology);
      internal::get()->command_list->get()->IASetPrimitiveTopology(d3d_topology);
    }

    void draw_indexed(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertexLoc)
    {
      internal::get()->command_list->get()->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertexLoc, startInstance);
    }

    void set_shader(const rhi::ResourceSlot& slot)
    {
      auto* shader_program = internal::get()->resource_pool.as<ShaderProgramResource>(slot);

      internal::get()->command_list->get()->SetGraphicsRootSignature(shader_program->get()->root_signature.Get());
    }
    void set_pso(const rhi::ResourceSlot& slot)
    {
      auto* pso = internal::get()->resource_pool.as<PipelineState>(slot);
      internal::get()->command_list->get()->SetPipelineState(pso->get());
    }

    void swap_rendertargets()
    {
      auto rtv = internal::get()->swapchain_rtvs[internal::get()->current_swapchain_buffer_idx];
      internal::get()->command_list->get()->OMSetRenderTargets(1, &rtv, true, &internal::get()->dsv);
    }

    void reset_upload_buffer()
    {
      internal::get()->upload_buffer->reset();
    }

    void update_buffer(const ResourceSlot& slot, const void* data, s32 size)
    {
      Resource* resource = internal::get()->resource_pool.as<Resource>(slot);
      internal::get()->upload_buffer->write(internal::get()->command_list.get(), resource, data, size);
    }

    ScopedCommandList create_scoped_commandlist()
    {
      // Command Allocator
      wrl::ComPtr<ID3D12CommandAllocator> allocator;
      if (DX_FAILED(internal::get()->device->get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocator.GetAddressOf()))))
      {
        REX_ERROR(LogRhi, "Failed to create command allocator");
      }

      rhi::set_debug_name_for(allocator.Get(), "Global Command Allocator");

      // Command List
      wrl::ComPtr<ID3D12GraphicsCommandList> cmd_list;
      if (DX_FAILED(internal::get()->device->get()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.Get(), nullptr, IID_PPV_ARGS(cmd_list.GetAddressOf()))))
      {
        REX_ERROR(LogRhi, "Failed to create command list");
      }

      rhi::set_debug_name_for(cmd_list.Get(), "Global Command List");
      rsl::unique_ptr<CommandList> command_list = rsl::make_unique<CommandList>(cmd_list, allocator);

      return ScopedCommandList(rsl::move(command_list), internal::get()->command_queue.get());
    }

    namespace d3d
    {
      wrl::ComPtr<ID3D12RootSignature> create_shader_root_signature(const rsl::unique_array<ConstantLayoutDescription>& constants)
      {
        // Root parameter can be a table, root descriptor or root constants.
        auto root_parameters = rsl::vector<CD3DX12_ROOT_PARAMETER>(rsl::Capacity(constants.count()));

        // This bit is hardcoded for now, we need to fix this later
        // We want to sort the constants and then iterate over them with ranges
        // If we find a constant, we initialize a root param as a constant
        // If we only find 1 type of param, we init it directly
        // If we find multiple of the same type, we init it as a range
        root_parameters.emplace_back().InitAsConstantBufferView(0);
        root_parameters.emplace_back().InitAsConstantBufferView(1);

        // A root signature is an array of root parameters.
        CD3DX12_ROOT_SIGNATURE_DESC root_sig_desc(
          root_parameters.size(),
          root_parameters.data(),
          0,
          nullptr,
          D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        // Create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
        wrl::ComPtr<ID3DBlob> serialized_root_sig = nullptr;
        wrl::ComPtr<ID3DBlob> error_blob = nullptr;

        HRESULT hr = D3D12SerializeRootSignature(&root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1, serialized_root_sig.GetAddressOf(), error_blob.GetAddressOf());
        if (error_blob != nullptr)
        {
          REX_ERROR(LogDirectX, "{}", (char*)error_blob->GetBufferPointer());
          return nullptr;
        }

        if (DX_FAILED(hr))
        {
          REX_ERROR(LogDirectX, "Failed to serialize root signature");
          return nullptr;
        }

        wrl::ComPtr<ID3D12RootSignature> root_signature;
        if (DX_FAILED(internal::get()->device->get()->CreateRootSignature(0, serialized_root_sig->GetBufferPointer(), serialized_root_sig->GetBufferSize(), IID_PPV_ARGS(&root_signature))))
        {
          REX_ERROR(LogDirectX, "Failed to create root signature");
          return nullptr;
        }

        rhi::set_debug_name_for(root_signature.Get(), "Root Signature");

        return root_signature;
      }
    }

#pragma region RHI Class
    internal::RenderHardwareInfrastructure::RenderHardwareInfrastructure(const renderer::OutputWindowUserData& userData)
      : init_successful(true)
      , current_swapchain_buffer_idx(0)
    {
      // Create a scopeguard that automatically marks initialization as failed
      // This is to make it easy to extend initialization where needed with only
      // needing 1 place where we mark initialization as successful.
      rsl::scopeguard mark_init_failed = [this]() { init_successful = false; };

      // 1) we need to init the dxgi factory.
      // This is the system we use to create most other systems.
      if (!init_dxgi_factory())
      {
        REX_ERROR(LogRhi, "Failed to create DXGI Factory");
        return;
      }

#ifdef REX_ENABLE_DX12_DEBUG_LAYER
      // 1.1) Create the debug controller before the device gets created
      // This way we can have some additional debugging information if something goes wrong
      init_debug_controller();
#endif

      // 2) we need to init the device.
      // A DirectX Device is used for creation of other resources
      // You can think of it as an abstraction of the GPU,
      // but just an abstraction over resource creation,
      // not an abstraction of the gpu itself, that's what an IDXGIAdapter is for.
      if (!init_d3d_device())
      {
        REX_ERROR(LogRhi, "Failed to create D3D Device");
        return;
      }

#ifdef REX_ENABLE_DX12_DEBUG_LAYER
      // 2.1) Create the debug layer aftet device gets created
      // The debug layer is linked to the device and can therefore
      // only get created when the device is created
      init_debug_layer();
#endif

      // 3) we create the command queue.
      // The command queue is used to send commands to the gpu
      // It also holds the fence for syncronysation between cpu and gpu
      if (!init_command_queue())
      {
        REX_ERROR(LogRhi, "Failed to create command queue");
        return;
      }

      // 4) Create the global command list
      // A gpu works with commands, that get added to a command list.
      // command lists themselves are not executable, but need to be added
      // to a command queue, which in turn executes the commands.
      if (!init_command_list())
      {
        REX_ERROR(LogRhi, "Failed to create command list");
        return;
      }

      // 5) we need to create a swapchain which is responsible of presenting.
      // There's no benefit in creating all the above systems if we don't have anything
      // to actually present something on screen, that's what the swapchain is for.
      if (!init_swapchain(userData))
      {
        REX_ERROR(LogRhi, "Failed to create swapchain");
        return;
      }

      // 6) We need to create initial heaps that store the data that'll be used for rendering
      // These heaps only hold the data, but letting the graphics pipeline know which resource to use
      // is never done with the heaps directly, but instead are only used to store the data
      // To let graphics pipeline know which resources to use, we use descriptors, which are stored
      // in their own heap
      if (!init_resource_heaps())
      {
        REX_ERROR(LogRhi, "Failed to create resource heaps");
        return;
      }

      // 7) We need a few heaps to store descriptors which point to the actual resources in other heaps
      // A descriptor is just some metadata about the resource, holding all the information
      // the gpu needs to use a resource.
      // A heap on the gpu is just the same as on the cpu except it's more limited to what it can store
      // You don't get random access into it, a heap can only store specific types and you access into the heap by offset
      // Where each offset is the multiplied by the size of the type the heap stores.
      // The heap doesn't store the content of a resource but stores the view to the resource.
      if (!init_descriptor_heaps())
      {
        REX_ERROR(LogRhi, "Failed to create descriptor heaps");
        return;
      }

      // 8) Now create the views for the swapchain's back buffers
      // These views are needed for basic presenting as the GPU
      // doesn't interact with resources directly but uses views instead
      if (!init_swapchain_buffer_views(userData))
      {
        REX_ERROR(LogRhi, "Failed to create swapchain buffer views");
        return;
      }

      // 9) Create the pipeline library
      // This holds all the pso objects we need.
      // Ideally this should get prefilled on boot.
      // However on the very first boot time, all pso need to get compiled first
      if (!init_pipeline_library())
      {
        REX_ERROR(LogRhi, "Failed to create pipeline library");
        return;
      }

      // 10) Create the upload buffers
      // To make sure we can upload data to the gpu
      // we need to make sure the upload buffers 
      // are create which perform this upload for us
      if (!init_upload_buffers())
      {
        REX_ERROR(LogRhi, "Failed to create upload buffers");
        return;
      }

      // 11) Execute the commandlist to finish initialization
      command_list->exec(command_queue.get());

      // release scopeguard so that init gets marked successful
      mark_init_failed.release();
    }
    internal::RenderHardwareInfrastructure::~RenderHardwareInfrastructure()
    {
      if (device)
      {
        // A command queue needs to be flushed to shutdown properly
        command_queue->flush();

        // Now that all gpu commands are cleared, we can release all resources
      }
    }

    // DXGI Factory
    s32 internal::RenderHardwareInfrastructure::init_debug_interface()
    {
      /*
      * Bug in the DXGI Debug Layer interaction with the DX12 Debug Layer w/ Windows 11.
      * There's a simple workaround which is to suppress D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE.
      * The bug itself will be fixed in a future Windows update.
      *
      * The Debug Layer has always had quirks when it comes to dealing with 'hybrid graphics' systems
      * (i.e. laptops with both Intel Integrated and discrete GPUs)
      *
      * https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
      * https://github.com/walbourn/directx-vs-templates/commit/18e3eaa444e98ba75d37d506ab18df8db0b82441
      */
      s32 dxgi_factory_flags = 0;

      if (DX_SUCCESS(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug_info_queue.GetAddressOf()))))
      {
        dxgi_factory_flags = DXGI_CREATE_FACTORY_DEBUG;

        debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE, globals::g_enable_dxgi_severity_message);
        debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO, globals::g_enable_dxgi_severity_info);
        debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, globals::g_enable_dxgi_severity_warning);
        debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, globals::g_enable_dxgi_severity_error);
        debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, globals::g_enable_dxgi_severity_corruption);

        rsl::array<DXGI_INFO_QUEUE_MESSAGE_ID, 1> dxgi_hide = {
            80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
        };
        DXGI_INFO_QUEUE_FILTER dxgi_filter = {};
        dxgi_filter.DenyList.NumIDs = rsl::safe_numeric_cast<u32>(dxgi_hide.size());
        dxgi_filter.DenyList.pIDList = dxgi_hide.data();

        debug_info_queue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &dxgi_filter);
      }
      else
      {
        REX_WARN(LogRhi, "Unable to get GXGI Debug Interface");
      }

      return dxgi_factory_flags;
    }
    bool internal::RenderHardwareInfrastructure::init_dxgi_factory()
    {
      s32 dxgi_factory_flags = 0;

#ifdef REX_ENABLE_DXGI_DEBUG_LAYER
      dxgi_factory_flags = init_debug_interface();
#endif

      factory = dxgi::Factory::create(dxgi_factory_flags);
      return true;
    }
    void internal::RenderHardwareInfrastructure::init_debug_controller()
    {
      // Enable extra debugging and send debug messages to the VC++ output window
      rex::wrl::ComPtr<ID3D12Debug> debug_controller;
      if (DX_SUCCESS(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
      {
        debug_controller->EnableDebugLayer();
      }
      else
      {
        REX_WARN(LogRhi, "Failed to create DX debug controller");
      }
    }

    // D3D Device
    count_t internal::RenderHardwareInfrastructure::highest_scoring_gpu(const rsl::vector<GpuDescription>& gpus)
    {
      auto it = rsl::max_element(gpus.cbegin(), gpus.cend(),
        [](const GpuDescription& lhs, const GpuDescription& rhs)
        {
          const size_t lhs_vram = lhs.dedicated_video_memory.size_in_bytes();
          const size_t rhs_vram = rhs.dedicated_video_memory.size_in_bytes();

          return rhs_vram > lhs_vram;
        });

      return it != gpus.cend() ? rsl::distance(gpus.cbegin(), it) : -1;
    }
    void internal::RenderHardwareInfrastructure::init_debug_layer()
    {
      // Device needs to exist before we can query this
      rex::wrl::ComPtr<ID3D12InfoQueue> dx12_info_queue;
      if (DX_SUCCESS(device->get()->QueryInterface(IID_PPV_ARGS(dx12_info_queue.GetAddressOf()))))
      {
        dx12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_MESSAGE, globals::g_enable_dx12_severity_message);
        dx12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, globals::g_enable_dx12_severity_info);
        dx12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, globals::g_enable_dx12_severity_warning);
        dx12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, globals::g_enable_dx12_severity_error);
        dx12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, globals::g_enable_dx12_severity_corruption);

        /*
         * Bug in the DX12 Debug Layer interaction with the DX12 Debug Layer w/ Windows 11.
         * There's a simple workaround which is to suppress D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE.
         * The bug itself will be fixed in a future Windows update.
         *
         * The Debug Layer has always had quirks when it comes to dealing with 'hybrid graphics' systems
         * (i.e. laptops with both Intel Integrated and discrete GPUs)
         *
         * https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
         * https://github.com/walbourn/directx-vs-templates/commit/2b34dcf9ac764153699058cdc9d4dbc4e837224c
         */
        rsl::array<D3D12_MESSAGE_ID, 7> dx12_hide = { D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
                           D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
                           D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
                           D3D12_MESSAGE_ID_RESOURCE_BARRIER_BEFORE_AFTER_MISMATCH,
                           D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
                           D3D12_MESSAGE_ID_COMMAND_LIST_DRAW_VERTEX_BUFFER_NOT_SET,
                           D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE };

        D3D12_INFO_QUEUE_FILTER dx12_filter = {};
        dx12_filter.DenyList.NumIDs = rsl::safe_numeric_cast<u32>(dx12_hide.size());
        dx12_filter.DenyList.pIDList = dx12_hide.data();
        dx12_info_queue->AddStorageFilterEntries(&dx12_filter);
      }
      else
      {
        REX_WARN(LogRhi, "Unable to get D3D12 Debug Interface");
      }
    }

    bool internal::RenderHardwareInfrastructure::init_d3d_device()
    {
      // Find highest scoring gpu
      const dxgi::AdapterManager adapter_manager(factory.get(), [this](const rsl::vector<GpuDescription>& gpus) { return highest_scoring_gpu(gpus); });
      const dxgi::Adapter* selected_gpu = adapter_manager.selected();
      IDXGIAdapter* adapter = selected_gpu->c_ptr();

      // Create device
      const D3D_FEATURE_LEVEL feature_level = query_feature_level(adapter);

      wrl::ComPtr<ID3D12Device1> d3d_device;
      if (DX_FAILED(D3D12CreateDevice(adapter, static_cast<D3D_FEATURE_LEVEL>(feature_level), IID_PPV_ARGS(&d3d_device))))
      {
        REX_ERROR(LogRhi, "Software adapter not supported");
        REX_ERROR(LogRhi, "Failed to create DX12 Device");
        return false;
      }

      device = rsl::make_unique<DirectXDevice>(d3d_device, feature_level, selected_gpu);
      return true;
    }

    // D3D Command Objects
    bool internal::RenderHardwareInfrastructure::init_command_queue()
    {
      // Command Queue
      wrl::ComPtr<ID3D12CommandQueue> d3d_command_queue;
      D3D12_COMMAND_QUEUE_DESC queue_desc = {};
      queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
      queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
      if (DX_FAILED(device->get()->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(d3d_command_queue.GetAddressOf()))))
      {
        REX_ERROR(LogRhi, "Failed to create command queue");
        return false;
      }

      rhi::set_debug_name_for(d3d_command_queue.Get(), "Global Command Queue");

      // Fence
      wrl::ComPtr<ID3D12Fence> fence;
      if (DX_FAILED(device->get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
      {
        REX_ERROR(LogRhi, "Failed to create DX fence, to synchronize CPU/GPU");
        return false;
      }

      rhi::set_debug_name_for(fence.Get(), "Global Fence");
      command_queue = rsl::make_unique<CommandQueue>(d3d_command_queue, fence);

      return true;
    }

    bool internal::RenderHardwareInfrastructure::init_command_list()
    {
      // Command Allocator
      wrl::ComPtr<ID3D12CommandAllocator> allocator;
      if (DX_FAILED(device->get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocator.GetAddressOf()))))
      {
        REX_ERROR(LogRhi, "Failed to create command allocator");
        return false;
      }

      rhi::set_debug_name_for(allocator.Get(), "Global Command Allocator");

      // Command List
      wrl::ComPtr<ID3D12GraphicsCommandList> cmd_list;
      if (DX_FAILED(device->get()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.Get(), nullptr, IID_PPV_ARGS(cmd_list.GetAddressOf()))))
      {
        REX_ERROR(LogRhi, "Failed to create command list");
        return false;
      }

      rhi::set_debug_name_for(cmd_list.Get(), "Global Command List");
      command_list = rsl::make_unique<CommandList>(cmd_list, allocator);

      // Open the command list to allow further initialization of resources
      command_list->reset();

      return true;
    }

    // DXGI Swapchain
    bool internal::RenderHardwareInfrastructure::init_swapchain(const renderer::OutputWindowUserData& userData)
    {
      DXGI_SWAP_CHAIN_DESC1 sd{};
      sd.Width = userData.window_width;
      sd.Height = userData.window_height;
      sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      sd.SampleDesc.Count = 1;
      sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      sd.BufferCount = s_swapchain_buffer_count;
      sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

      // Note: swap chain uses queue to perform flush.
      rex::wrl::ComPtr<IDXGIFactory4> dxgi_factory = factory->as<IDXGIFactory4>();
      rex::wrl::ComPtr<IDXGISwapChain1> d3d_swapchain;
      if (DX_FAILED(dxgi_factory->CreateSwapChainForHwnd(command_queue->get(), (HWND)userData.primary_display_handle , &sd, nullptr, nullptr, d3d_swapchain.GetAddressOf())))
      {
        REX_ERROR(LogRhi, "Failed to create swap chain");
        return false;
      }

      rhi::set_debug_name_for(d3d_swapchain.Get(), "SwapChain");
      wrl::ComPtr<IDXGISwapChain3> d3d_swapchain_3;
      DX_CALL(d3d_swapchain.As(&d3d_swapchain_3));
      current_swapchain_buffer_idx = d3d_swapchain_3->GetCurrentBackBufferIndex();
      swapchain = rsl::make_unique<Swapchain>(d3d_swapchain_3, sd.Format, sd.BufferCount);
      return true;
    }

    // Resource Heaps
    bool internal::RenderHardwareInfrastructure::init_resource_heaps()
    {
      // Nothing to implement at the moment
      CD3DX12_HEAP_DESC desc(100_mib, D3D12_HEAP_TYPE_DEFAULT);

      wrl::ComPtr<ID3D12Heap> d3d_heap;
      if (DX_FAILED(device->get()->CreateHeap(&desc, IID_PPV_ARGS(&d3d_heap))))
      {
        REX_ERROR(LogRhi, "Failed to create global resource heap");
        return false;
      }

      heap = rsl::make_unique<ResourceHeap>(d3d_heap, device->get());
      return true;
    }

    // Descriptor Heaps
    bool internal::RenderHardwareInfrastructure::init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE type, s32 numDescriptors)
    {
      D3D12_DESCRIPTOR_HEAP_DESC desc{};

      desc.Type = type;
      desc.NumDescriptors = numDescriptors;
      desc.Flags = type != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
        ? D3D12_DESCRIPTOR_HEAP_FLAG_NONE
        : D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
      desc.NodeMask = 0; // For single-adapter operation, set this to zero. ( https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_descriptor_heap_desc )

      wrl::ComPtr<ID3D12DescriptorHeap> desc_heap;
      rsl::string_view type_str = rsl::enum_refl::enum_name(type);
      if (DX_FAILED(device->get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&desc_heap))))
      {
        REX_ERROR(LogRhi, "Failed to create descriptor heap for type: {}", type_str);
        return false;
      }

      rhi::set_debug_name_for(desc_heap.Get(), rsl::format("Descriptor Heap Element - {}", type_str));
      s32 desc_size = device->get()->GetDescriptorHandleIncrementSize(type);
      s32 total_size = desc_size * numDescriptors;

      descriptor_heap_pool.emplace(type, desc_heap, device->get());

      REX_LOG(LogRhi, "Created {0} ( num: {1} descriptors, desc size: {2} bytes, total size: {3} bytes) ", type_str, numDescriptors, desc_size, total_size);

      return true;
    }
    bool internal::RenderHardwareInfrastructure::init_descriptor_heaps()
    {
      if (!init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, s_num_rtv_descs))
      {
        REX_ERROR(LogRhi, "Failed to create descriptor heap for RTV");
        return false;
      }
      if (!init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, s_num_dsv_descs))
      {
        REX_ERROR(LogRhi, "Failed to create descriptor heap for RTV");
        return false;
      }
      if (!init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, s_num_cbv_descs))
      {
        REX_ERROR(LogRhi, "Failed to create descriptor heap for RTV");
        return false;
      }

      return true;
    }

    // Swapchain buffer views
    bool internal::RenderHardwareInfrastructure::init_swapchain_buffer_views(const renderer::OutputWindowUserData& userData)
    {
      if (!init_swapchain_rtvs(userData))
      {
        REX_ERROR(LogRhi, "Failed to create swapchain render target views");
        return false;
      }

      if (!init_swapchain_dsvs(userData))
      {
        REX_ERROR(LogRhi, "Failed to create swapchain depth stencil views");
        return false;
      }

      return true;
    }
    bool internal::RenderHardwareInfrastructure::init_swapchain_rtvs(const renderer::OutputWindowUserData& userData)
    {
      s32 width = userData.window_width;
      s32 height = userData.window_height;
      if (DX_FAILED(swapchain->resize_buffers(width, height, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)))
      {
        REX_ERROR(LogRhi, "Failed to resize swapchain buffers");
        return false;
      }

      for (s32 i = 0; i < swapchain->buffer_count(); ++i)
      {
        wrl::ComPtr<ID3D12Resource> buffer = swapchain->get_buffer(i);
        set_debug_name_for(buffer.Get(), rsl::format("Swapchain Back Buffer {}", i));
        D3D12_CPU_DESCRIPTOR_HANDLE rtv = descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_RTV).create_rtv(buffer.Get());
        swapchain_buffers.emplace_back(buffer, D3D12_RESOURCE_STATE_COMMON, 0);
        swapchain_rtvs[i] = rtv;
      }

      return true;
    }
    bool internal::RenderHardwareInfrastructure::init_swapchain_dsvs(const renderer::OutputWindowUserData& userData)
    {
      s32 width = userData.window_width;
      s32 height = userData.window_height;

      depth_stencil_buffer = heap->create_depth_stencil_resource(width, height);
      set_debug_name_for(depth_stencil_buffer->get(), "Swapchain Depth Stencil Buffer");
      dsv = descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_DSV).create_dsv(depth_stencil_buffer->get(), DXGI_FORMAT_D24_UNORM_S8_UINT);

      CD3DX12_RESOURCE_BARRIER depth_write_transition = CD3DX12_RESOURCE_BARRIER::Transition(depth_stencil_buffer->get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
      command_list->get()->ResourceBarrier(1, &depth_write_transition);

      return true;
    }

    // Pipeline Library
    bool internal::RenderHardwareInfrastructure::init_pipeline_library()
    {
      // A pipeline library is saved to disk
      // Therefore we need to create some file mapping so the pipeline library
      // gets stored to disk right away.
      // At the moment we haven't implement this yet, so we're creating a fresh pipeline library
      // every time
      wrl::ComPtr<ID3D12PipelineLibrary> d3d_pso_lib;
      if (DX_FAILED(device->get()->CreatePipelineLibrary(nullptr, 0, IID_PPV_ARGS(&d3d_pso_lib))))
      {
        REX_ERROR(LogRhi, "Failed to create pipeline library. Is this supported by your OS and driver version?");
        return false;
      }

      pso_lib = rsl::make_unique<PipelineLibrary>(d3d_pso_lib, device->get());
      return true;
    }

    // Upload buffers
    bool internal::RenderHardwareInfrastructure::init_upload_buffers()
    {
      // an intermediate upload heap.
      CD3DX12_HEAP_PROPERTIES heap_properties_upload(D3D12_HEAP_TYPE_UPLOAD);
      auto buffer_upload = CD3DX12_RESOURCE_DESC::Buffer(100_mib);

      wrl::ComPtr<ID3D12Resource> d3d_upload_buffer;
      if (DX_FAILED(device->get()->CreateCommittedResource(&heap_properties_upload, D3D12_HEAP_FLAG_NONE, &buffer_upload, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(d3d_upload_buffer.GetAddressOf()))))
      {
        REX_ERROR(LogDirectX, "Failed to create committed resource for intermediate upload heap.");
        return {};
      }

      set_debug_name_for(d3d_upload_buffer.Get(), "Upload Buffer");
      upload_buffer = rsl::make_unique<UploadBuffer>(d3d_upload_buffer, D3D12_RESOURCE_STATE_COMMON);

      return true;
    }

#pragma endregion
  }
}