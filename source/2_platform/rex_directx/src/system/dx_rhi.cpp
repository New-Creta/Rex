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
#include "rex_directx/utility/dx_util.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/pointer_math.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/casting.h"

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
    DEFINE_LOG_CATEGORY(LogRhi);

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

      ID3D12DescriptorHeap* heap = internal::get()->descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).get();
      internal::get()->command_list->get()->SetDescriptorHeaps(1, &heap);
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
      ResourceID id = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(id))
      {
        return internal::get()->resource_pool.at(id);
      }

      // If we don't have it already, compile the shader
      ShaderCompiler compiler;
      wrl::ComPtr<ID3DBlob> byte_code = compiler.compile_shader(desc);

      if (!byte_code)
      {
        REX_ERROR(LogRhi, "Failed to compile shader");
        return ResourceSlot::make_invalid();
      }

      // store the new compiled shader with its id in the resource pool
      switch (desc.shader_type)
      {
      case ShaderType::Vertex:
        return internal::get()->resource_pool.insert(rsl::make_unique<VertexShaderResource>(id, byte_code));
        break;
      case ShaderType::Pixel:
        return internal::get()->resource_pool.insert(rsl::make_unique<PixelShaderResource>(id, byte_code));
        break;

      default:
        REX_ERROR(LogDirectX, "Unsupported Shader Type was given");
      }

      return ResourceSlot::make_invalid();

    }
    ResourceSlot link_shader(const LinkShaderDesc& desc)
    {
      // If there's already a linked shader, return it.
      ResourceID id = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(id))
      {
        return internal::get()->resource_pool.at(id);
      }

      // If not link the shader with the params provided
      auto root_sig = d3d::create_shader_root_signature(desc.constants, desc.desc_tables, desc.samplers);

      if (!root_sig)
      {
        REX_ERROR(LogRhi, "Failed to create root signature");
        return ResourceSlot::make_invalid();
      }

      auto* vertex_shader = internal::get()->resource_pool.as<VertexShaderResource>(desc.vertex_shader);
      auto* pixel_shader = internal::get()->resource_pool.as<PixelShaderResource>(desc.pixel_shader);

      rsl::unique_array<ShaderParameterLayoutDescription> constants = rsl::make_unique<ShaderParameterLayoutDescription[]>(desc.constants.count());
      rsl::memcpy(constants.get(), desc.constants.get(), desc.constants.byte_size());

      rsl::unique_array<DescriptorTableDescription> desc_tables = rsl::make_unique<DescriptorTableDescription[]>(desc.desc_tables.count());
      rsl::memcpy(desc_tables.get(), desc.desc_tables.get(), desc.desc_tables.byte_size());

      rsl::unique_array<ShaderSamplerDescription> samplers = rsl::make_unique<ShaderSamplerDescription[]>(desc.samplers.count());
      rsl::memcpy(samplers.get(), desc.samplers.get(), desc.samplers.byte_size());

      // create a combined shader object with the root sig, the vertex shader and the pixel shader
      return internal::get()->resource_pool.insert(rsl::make_unique<ShaderProgramResource>(id, root_sig, vertex_shader, pixel_shader, rsl::move(constants), rsl::move(desc_tables), rsl::move(samplers)));
    }
    ResourceSlot create_shader(const ShaderDesc& desc)
    {
      // Check if the shader isn't already loaded
      ResourceID id = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(id))
      {
        return internal::get()->resource_pool.at(id);
      }

      // If the shader hasn't been loaded before
      wrl::ComPtr<ID3DBlob> byte_code = rex::d3d::create_blob(desc.shader_byte_code);

      switch (desc.shader_type)
      {
      case ShaderType::Vertex: return internal::get()->resource_pool.insert(rsl::make_unique<VertexShaderResource>(id, byte_code));
      case ShaderType::Pixel: return internal::get()->resource_pool.insert(rsl::make_unique<PixelShaderResource>(id, byte_code));

      default:
        REX_ERROR(LogDirectX, "Unsupported Shader Type was given");
      }

      return ResourceSlot::make_invalid();
    }

    // A clear state is just a struct holding different values to clear a buffer
    // Flags control which part of the buffer (color, depths or stencil) should be cleared
    ResourceSlot create_clear_state(const ClearStateDesc& desc)
    {
      ResourceID id = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(id))
      {
        return internal::get()->resource_pool.at(id);
      }

      return internal::get()->resource_pool.insert(rsl::make_unique<ClearStateResource>(id, desc));
    }
    // A raster state holds rasterization settings
    // settings like cull mode, fill mode, depth bias, normal orientation, ..
    // are all included in the raster state
    ResourceSlot create_raster_state(const RasterStateDesc& desc)
    {
      ResourceID id = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(id))
      {
        return internal::get()->resource_pool.at(id);
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

      return internal::get()->resource_pool.insert(rsl::make_unique<RasterStateResource>(id, d3d_rs));
    }
    // An input layout determines the format of vertices
    // It determines where a shader can find the position, normal, color
    // of a vertex.
    ResourceSlot create_input_layout(const InputLayoutDesc& desc)
    {
      ResourceID id = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(id))
      {
        return internal::get()->resource_pool.at(id);
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

      return internal::get()->resource_pool.insert(rsl::make_unique<InputLayoutResource>(id, input_element_descriptions));
    }
    // A vertex buffer is a buffer holding vertices of 1 or more objects
    ResourceSlot create_vertex_buffer(const VertexBufferDesc& desc)
    {
      ResourceID id = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(id))
      {
        return internal::get()->resource_pool.at(id);
      }

      // 1) Create the resource on the gpu that'll hold the data of the vertex buffer
      rsl::memory_size size = desc.blob_view.size();
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
    // A vertex buffer is a buffer holding vertices of 1 or more objects
    ResourceSlot create_vertex_buffer(s32 totalSize, s32 vertexSize)
    {
      // 1) Create the resource on the gpu that'll hold the data of the vertex buffer
      wrl::ComPtr<ID3D12Resource> buffer = internal::get()->heap->create_buffer(totalSize * vertexSize);
      set_debug_name_for(buffer.Get(), "Vertex Buffer");
      rsl::unique_ptr<VertexBuffer> vb = rsl::make_unique<VertexBuffer>(buffer, totalSize * vertexSize, vertexSize);

      // 2) Copy the data into the upload buffer
      //internal::get()->upload_buffer->write(internal::get()->command_list.get(), vb.get(), desc.blob_view.data(), size);

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
      ResourceID id = hash_resource_desc(desc);
      if (internal::get()->resource_pool.has_resource(id))
      {
        return internal::get()->resource_pool.at(id);
      }

      // 1) Create the resource on the gpu that'll hold the data of the vertex buffer
      rsl::memory_size size = desc.blob_view.size();
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
    // An index buffer is a buffer holding indices of 1 or more objects
    ResourceSlot create_index_buffer(s32 totalSize, renderer::IndexBufferFormat format)
    {
      // 1) Create the resource on the gpu that'll hold the data of the vertex buffer
      DXGI_FORMAT d3d_format = rex::d3d::to_d3d12_index_format(format);
      wrl::ComPtr<ID3D12Resource> buffer = internal::get()->heap->create_buffer(totalSize, 0);
      set_debug_name_for(buffer.Get(), "Index Buffer");
      rsl::unique_ptr<IndexBuffer> ib = rsl::make_unique<IndexBuffer>(buffer, D3D12_RESOURCE_STATE_INDEX_BUFFER, totalSize, d3d_format);

      // 2) Copy the data into the upload buffer
      //internal::get()->upload_buffer->write(internal::get()->command_list.get(), ib.get(), desc.blob_view.data(), size);

      // 3) Upload the data from the upload buffer onto the gpu
      //internal::get()->upload_buffer->upload(internal::get()->command_list.get());

      // 4) add the resource to the resource pool
      return internal::get()->resource_pool.insert(rsl::move(ib));
    }
    // A constant buffer is a buffer holding data that's accessible to a shader
    // This can hold data like ints, floats, vectors and matrices
    ResourceSlot create_constant_buffer(const ConstantBufferDesc& desc)
    {
      return create_constant_buffer(desc.blob_view.size());
    }
    // A constant buffer is a buffer holding data that's accessible to a shader
    // This can hold data like ints, floats, vectors and matrices
    ResourceSlot create_constant_buffer(rsl::memory_size size)
    {
      //ResourceID id = hash_resource_desc();
      //if (internal::get()->resource_pool.has_resource(id))
      //{
      //  return internal::get()->resource_pool.at(id);
      //}

      // 1) Create the resource on the gpu that'll hold the data of the vertex buffer
      rsl::memory_size aligned_size = rex::align(size.size_in_bytes(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
      wrl::ComPtr<ID3D12Resource> buffer = internal::get()->heap->create_buffer(aligned_size);
      set_debug_name_for(buffer.Get(), "Constant Buffer");
      rsl::unique_ptr<ConstantBuffer> cb = rsl::make_unique<ConstantBuffer>(buffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, size);

      // 2) Copy the data into the upload buffer
      //internal::get()->upload_buffer->write(internal::get()->command_list.get(), cb.get(), desc.blob_view.data(), aligned_size);

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
      if (desc.blend_state.has_value())
      {
        d3d_blend_state = rex::d3d::to_d3d12_blend_desc(desc.blend_state.value());
      }

      D3D12_DEPTH_STENCIL_DESC d3d_depth_stencil_state = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
      if (desc.depth_stencil_state.has_value())
      {
        d3d_depth_stencil_state = rex::d3d::to_d3d12_depth_stencil(desc.depth_stencil_state.value());
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
      pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
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
    // Create a 2D texture on the gpu and add the data to the upload buffer
    // to be uploaded the next time the upload buffer
    ResourceSlot create_texture2d(const char* data, DXGI_FORMAT format, s32 width, s32 height)
    {
      wrl::ComPtr<ID3D12Resource> d3d_texture = internal::get()->heap->create_texture2d(format, width, height);
      rsl::unique_ptr<Texture2D> texture = rsl::make_unique<Texture2D>(d3d_texture, width, height, format);

      internal::get()->upload_buffer->write_texture(internal::get()->command_list.get(), texture.get(), data, format, width, height);
      
      return internal::get()->resource_pool.insert(rsl::move(texture));
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
      internal::get()->swapchain->transition_backbuffer(internal::get()->command_list->get(), state);
    }
    void clear_backbuffer(const ResourceSlot& clearState)
    {
      ClearStateResource* clear_state = internal::get()->resource_pool.as<ClearStateResource>(clearState);
      auto& clear_flags = clear_state->get()->flags;
      if (clear_flags.has_state(renderer::ClearBits::ClearColorBuffer))
      {
        DescriptorHandle rtv = internal::get()->swapchain->backbuffer_view();
        internal::get()->command_list->get()->ClearRenderTargetView(rtv.get(), clear_state->get()->rgba.data(), 0, nullptr);
      }

      if (clear_flags.has_state(renderer::ClearBits::ClearDepthBuffer) || clear_flags.has_state(renderer::ClearBits::ClearStencilBuffer))
      {
        s32 d3d_clear_flags = 0;
        d3d_clear_flags |= clear_flags.has_state(renderer::ClearBits::ClearDepthBuffer) ? D3D12_CLEAR_FLAG_DEPTH : 0;
        d3d_clear_flags |= clear_flags.has_state(renderer::ClearBits::ClearStencilBuffer) ? D3D12_CLEAR_FLAG_STENCIL : 0;

        //DescriptorHandle dsv = internal::get()->swapchain->depth_stencil_view();
        //internal::get()->command_list->get()->ClearDepthStencilView(dsv.get(), (D3D12_CLEAR_FLAGS)d3d_clear_flags, clear_state->get()->depth, clear_state->get()->stencil, 0, nullptr);
      }
    }
    void present()
    {
      internal::get()->swapchain->present();
    }

    DescriptorHandle create_texture2d_srv(const ResourceSlot& textureSlot)
    {
      Texture2D* texture = internal::get()->resource_pool.as<Texture2D>(textureSlot);
      return internal::get()->descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).create_texture2d_srv(texture->get());
    }

    void set_vertex_buffer(const rhi::ResourceSlot& vb)
    {
      D3D12_VERTEX_BUFFER_VIEW view{};
      VertexBuffer* vertex_buffer = internal::get()->resource_pool.as<VertexBuffer>(vb);
      view.BufferLocation = vertex_buffer->get()->GetGPUVirtualAddress();
      view.SizeInBytes = narrow_cast<s32>(vertex_buffer->size());
      view.StrideInBytes = vertex_buffer->stride();
      
      internal::get()->command_list->get()->IASetVertexBuffers(0, 1, &view);
    }
    void set_index_buffer(const rhi::ResourceSlot& ib)
    {
      D3D12_INDEX_BUFFER_VIEW view{};
      IndexBuffer* index_buffer = internal::get()->resource_pool.as<IndexBuffer>(ib);
      view.BufferLocation = index_buffer->get()->GetGPUVirtualAddress();
      view.Format = index_buffer->format();
      view.SizeInBytes = narrow_cast<s32>(index_buffer->size());

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

    void bind_backbuffer_rendertarget()
    {
      auto rtv = internal::get()->swapchain->backbuffer_view();
      //auto dsv = internal::get()->swapchain->depth_stencil_view();
      internal::get()->command_list->get()->OMSetRenderTargets(1, &rtv.get(), true, nullptr);
    }

    void reset_upload_buffer()
    {
      internal::get()->upload_buffer->reset();
    }

    void update_buffer(const ResourceSlot& slot, const void* data, s64 size)
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
      wrl::ComPtr<ID3D12RootSignature> create_shader_root_signature(const rsl::unique_array<ShaderParameterLayoutDescription>& constants, const rsl::unique_array<rhi::DescriptorTableDescription>& tables, const rsl::unique_array<rhi::ShaderSamplerDescription>& samplers)
      {
        // Root parameter can be a table, root descriptor or root constants.
        auto root_parameters = rsl::vector<CD3DX12_ROOT_PARAMETER>(rsl::Capacity(constants.count()));

        for (s32 i = 0; i < constants.count(); ++i)
        {
          const auto& param = constants[i];

          D3D12_SHADER_VISIBILITY visibility = rex::d3d::to_d3d12_shader_visibility(param.visibility);
          switch (param.type)
          {
          case ShaderParameterType::CBuffer: root_parameters.emplace_back().InitAsConstantBufferView(param.reg, param.space, visibility); break;
          case ShaderParameterType::Constant: root_parameters.emplace_back().InitAsConstants(param.num_32bits_for_constant, param.reg, param.space, visibility); break;
          case ShaderParameterType::ShaderResourceView: root_parameters.emplace_back().InitAsShaderResourceView(param.reg, param.space, visibility); break;
          case ShaderParameterType::UnorderedAccessView: root_parameters.emplace_back().InitAsUnorderedAccessView(param.reg, param.space, visibility); break;
          }
        }

        rsl::vector<D3D12_DESCRIPTOR_RANGE> ranges;
        for (s32 i = 0; i < tables.count(); ++i)
        {
          const auto& table = tables[i];
          D3D12_SHADER_VISIBILITY visibility = rex::d3d::to_d3d12_shader_visibility(table.visibility);
          for (s32 range_idx = 0; range_idx < table.ranges.count(); ++range_idx)
          {
            ranges.push_back(rex::d3d::to_d3d12_descriptor_range(table.ranges[range_idx]));
          }

          root_parameters.emplace_back().InitAsDescriptorTable(ranges.size(), ranges.data(), visibility);
        }

        auto root_samplers = rsl::vector<D3D12_STATIC_SAMPLER_DESC>(rsl::Capacity(samplers.count()));

        for (s32 i = 0; i < samplers.count(); ++i)
        {
          const auto& sampler = samplers[i];

          D3D12_STATIC_SAMPLER_DESC desc{};
          desc.Filter = rex::d3d::to_d3d12_sampler_filtering(sampler.filtering);
          desc.AddressU = rex::d3d::to_d3d12_texture_address_mode(sampler.address_mode_u);
          desc.AddressV = rex::d3d::to_d3d12_texture_address_mode(sampler.address_mode_v);
          desc.AddressW = rex::d3d::to_d3d12_texture_address_mode(sampler.address_mode_w);
          desc.MipLODBias =sampler.mip_lod_bias;
          desc.MaxAnisotropy = sampler.max_anisotropy;
          desc.ComparisonFunc = rex::d3d::to_d3d12_comparison_func(sampler.comparison_func);
          desc.BorderColor = rex::d3d::to_d3d12_border_color(sampler.border_color);
          desc.MinLOD = sampler.min_lod;
          desc.MaxLOD = sampler.max_lod;
          desc.ShaderRegister = sampler.shader_register;
          desc.RegisterSpace = sampler.register_space;
          desc.ShaderVisibility = rex::d3d::to_d3d12_shader_visibility(sampler.shader_visibility);
          root_samplers.emplace_back(desc);
        }

        // A root signature is an array of root parameters.
        CD3DX12_ROOT_SIGNATURE_DESC root_sig_desc(
          root_parameters.size(),
          root_parameters.data(),
          root_samplers.size(),
          root_samplers.data(),
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

    Texture2D* get_texture(const ResourceSlot& slot)
    {
      return internal::get()->resource_pool.as<Texture2D>(slot);
    }


#pragma region RHI Class
    internal::RenderHardwareInfrastructure::RenderHardwareInfrastructure(const renderer::OutputWindowUserData& userData)
      : init_successful(true)
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

      // 5) We need to create initial heaps that store the data that'll be used for rendering
      // These heaps only hold the data, but letting the graphics pipeline know which resource to use
      // is never done with the heaps directly, but instead are only used to store the data
      // To let graphics pipeline know which resources to use, we use descriptors, which are stored
      // in their own heap
      if (!init_resource_heaps())
      {
        REX_ERROR(LogRhi, "Failed to create resource heaps");
        return;
      }

      // 6) We need a few heaps to store descriptors which point to the actual resources in other heaps
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

      // 7) we need to create a swapchain which is responsible of presenting.
      // There's no benefit in creating all the above systems if we don't have anything
      // to actually present something on screen, that's what the swapchain is for.
      if (!init_swapchain(userData))
      {
        REX_ERROR(LogRhi, "Failed to create swapchain");
        return;
      }

      // 8) Create the pipeline library
      // This holds all the pso objects we need.
      // Ideally this should get prefilled on boot.
      // However on the very first boot time, all pso need to get compiled first
      if (!init_pipeline_library())
      {
        REX_ERROR(LogRhi, "Failed to create pipeline library");
        return;
      }

      // 9) Create the upload buffers
      // To make sure we can upload data to the gpu
      // we need to make sure the upload buffers 
      // are create which perform this upload for us
      if (!init_upload_buffers())
      {
        REX_ERROR(LogRhi, "Failed to create upload buffers");
        return;
      }

      // 10) Execute the commandlist to finish initialization
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
      sd.Width = 0;
      sd.Height = 0;
      sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      sd.SampleDesc.Count = 1;
      sd.SampleDesc.Quality = 0;
      sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      sd.BufferCount = s_swapchain_buffer_count;
      sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
      sd.AlphaMode - DXGI_ALPHA_MODE_UNSPECIFIED;
      sd.Stereo = false;
      sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

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

      DescriptorHeap* rtv_desc_heap = &descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
      DescriptorHeap* dsv_desc_heap = &descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
      swapchain = rsl::make_unique<Swapchain>(d3d_swapchain_3, sd.Format, sd.BufferCount, rtv_desc_heap, dsv_desc_heap, heap.get());

      //swapchain->resize_buffers(userData.window_width, userData.window_height, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);

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

      REX_INFO(LogRhi, "Created {0} ( num: {1} descriptors, desc size: {2} bytes, total size: {3} bytes) ", type_str, numDescriptors, desc_size, total_size);

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

    ID3D12GraphicsCommandList* cmd_list()
    {
      return internal::get()->command_list->get();
    }

    void set_graphics_root_descriptor_table(ResourceID id)
    {
      ResourceSlot slot(id);
      
    }

    ID3D12Device* get_device()
    {
      return internal::get()->device->get();
    }
    ID3D12DescriptorHeap* get_cbv_uav_srv_heap()
    {
      return internal::get()->descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).get();
    }
    DescriptorHandle get_free_handle()
    {
      return internal::get()->descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).new_free_handle();
    }

#pragma endregion
  }
}