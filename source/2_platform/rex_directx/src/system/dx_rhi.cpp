#include "rex_directx/system/dx_rhi.h"

#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/dx_adapter_manager.h"
#include "rex_directx/dxgi/dx_adapter.h"

#include "rex_directx/utility/dx_util.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_device.h"
#include "rex_directx/system/dx_feature_level.h"
#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/system/dx_view_heap.h"
#include "rex_directx/system/dx_resource_heap.h"
#include "rex_directx/system/dx_swapchain.h"
#include "rex_directx/system/dx_shader_compiler.h"
#include "rex_directx/resources/dx_vertex_buffer.h"
#include "rex_directx/resources/dx_index_buffer.h"
#include "rex_directx/resources/dx_texture_2d.h"
#include "rex_directx/system/dx_feature_shader_model.h"
#include "rex_directx/system/dx_fence.h"
#include "rex_directx/resources/dx_constant_buffer.h"
#include "rex_renderer_core/gfx/graphics_context.h"

#include "rex_directx/resources/dx_input_layout.h"
#include "rex_directx/resources/dx_vertex_shader.h"
#include "rex_directx/resources/dx_pixel_shader.h"
#include "rex_directx/resources/dx_pipeline_state.h"
#include "rex_directx/resources/dx_render_target.h"

#include "rex_directx/gfx/dx_copy_context.h"
#include "rex_renderer_core/gfx/graphics.h"

#include "rex_directx/resources/dx_root_signature.h"

#include "rex_directx/utility/d3dx12.h"
#include "rex_directx/utility/dx_util.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/pointer_math.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/casting.h"

#include "rex_renderer_core/gfx/renderer_output_window_user_data.h"
#include "rex_renderer_core/gfx/viewport.h"
#include "rex_renderer_core/gfx/scissor_rect.h"

#include "rex_std/bonus/utility.h"

#include "rex_directx/system/dx_gpu_engine.h"

#include <D3Dcompiler.h>
#include "rex_directx/dxgi/includes.h"

#include "rex_engine/platform/win/diagnostics/hr_call.h"
#include "rex_directx/shader_reflection/dx_shader_reflection.h"
#include "rex_directx/system/dx_shader_root_parameters.h"
#include "rex_directx/resources/dx_sampler_2d.h"

#include "rex_engine/images/stb_image.h"
#include "rex_renderer_core/materials/material_system.h"
#include "rex_renderer_core/system/input_layout_cache.h"
#include "rex_renderer_core/system/root_signature_cache.h"

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogDxRhi);

    namespace rhi
    {
      // Some resources need to be accessible directly by the rhi.
      // We could defer all resource creation to the gpu engine as well, but that'd 
      // cause double wrapping (once in the rhi namespace and once in the gpu engine class)
      // To avoid this we have direct access to the resources required by the rhi to create gpu resources
      struct RhiResources
      {
        rsl::unique_ptr<DebugInterface> debug_interface; // Used to determine if we have any leaking resource on shutdown
        DxDevice* device = nullptr;                           // Used as the factory object to create gpu resources
        DxCommandQueue* render_command_queue = nullptr;       // Used as the object the swapchain speaks to queue a present command
        rsl::unique_ptr<dxgi::Factory> factory;
        rsl::unique_ptr<dxgi::AdapterManager> adapter_manager; // The manager holding all the adapters on this machine
      };

      // Resources needed to create objects
      rsl::unique_ptr<RhiResources> g_rhi_resources;
      // The gpu engine which owns all graphic resources
      rsl::unique_ptr<gfx::DxGpuEngine> g_gpu_engine;

      // Return information about the graphics hardware and software
      const Info& info()
      {
        return g_rhi_resources->device->info();
      }

      // Startup initialization
      DEFINE_YES_NO_ENUM(EnableDebugFactory);
      rsl::unique_ptr<dxgi::Factory> create_dxgi_factory(EnableDebugFactory enableDebugFactory)
      {
        s32 dxgi_factory_flags = 0;
        if (enableDebugFactory)
        {
          dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
        }
        
        return dxgi::Factory::create(dxgi_factory_flags);
      }
      rsl::unique_ptr<DxDevice> create_d3d_device(dxgi::AdapterManager* adapterManager)
      {
        // Select highest scoring gpu
        const dxgi::Adapter* selected_gpu = adapterManager->selected();
        rsl::unique_ptr<DxDevice> device = selected_gpu->create_device();

        if (!device)
        {
          REX_ERROR(LogDxRhi, "Failed to create DX12 Device");
          return nullptr;
        }

        return device;
      }

      // Initialize graphics systems and create a gpu engine
      gfx::GpuEngine* init(const OutputWindowUserData& userData)
      {
        g_rhi_resources = rsl::make_unique<RhiResources>();

        // The debug interface needs to get created first (and destroyed last)
        // to make sure all resources are tracked and it won't warn about resources
        // not yet destroyed if they'd get destroyed at a later point in time.
#ifdef REX_ENABLE_DX12_DEBUG_LAYER
        g_rhi_resources->debug_interface = rsl::make_unique<DebugInterface>();
#endif

        // 1) we need to init the dxgi factory.
        // This is the system we use to create most other systems.
        bool enable_debug_factory = g_rhi_resources->debug_interface != nullptr;
        g_rhi_resources->factory = create_dxgi_factory(enable_debug_factory);

        // 2) Create the adapter manager
        // It'll go over all found GPUs in the system and score them
        g_rhi_resources->adapter_manager = rsl::make_unique<dxgi::AdapterManager>(g_rhi_resources->factory.get(), [](const GpuDescription& gpu) { return calc_gpu_score(gpu); });

        // 3) we need to init the device.
        // A DirectX Device is used for creation of other resources
        // You can think of it as an abstraction of the GPU,
        // but just an abstraction over resource creation,
        // not an abstraction of the gpu itself, that's what an IDXGIAdapter is for.
        rsl::unique_ptr<DxDevice> device = create_d3d_device(g_rhi_resources->adapter_manager.get());
        if (!device)
        {
          REX_ERROR(LogDxRhi, "Failed to create D3D Device");
          return nullptr;
        }

        // Initialize the device to be used by the rhi
        g_rhi_resources->device = device.get();

        // Log the info in case anything goes wrong after this.
        log_info();

        // Now create the gpu engine which the backend of all our graphics systems
        g_gpu_engine = rsl::make_unique<gfx::DxGpuEngine>(userData, rsl::move(device), g_rhi_resources->adapter_manager.get());
        g_gpu_engine->init();

        return g_gpu_engine.get();
      }

      // Destroy all graphics systems
      void shutdown()
      {
        g_gpu_engine.reset();
        g_rhi_resources.reset();
      }
      
      // Return the shader platform used for this API.
      ShaderPlatform shader_platform()
      {
        return ShaderPlatform::Hlsl;
      }

      RenderTarget* current_backbuffer_rt()
      {
        return g_gpu_engine->current_backbuffer_rt();
      }

      // Generic functions, coming from rhi.h
      // -------------------------------------------
      rsl::unique_ptr<DxFence>          create_fence()
      {
        wrl::ComPtr<ID3D12Fence> fence;
        if (DX_FAILED(g_rhi_resources->device->dx_object()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
        {
          REX_ERROR(LogDxRhi, "Failed to create DX fence, to synchronize CPU/GPU");
          return nullptr;
        }

        return rsl::make_unique<DxFence>(fence);
      }
      rsl::unique_ptr<CommandQueue>     create_command_queue(GraphicsEngineType type)
      {
        // Command Queue
        wrl::ComPtr<ID3D12CommandQueue> d3d_command_queue;
        D3D12_COMMAND_QUEUE_DESC queue_desc = {};
        queue_desc.Type = d3d::to_dx12(type);
        queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        if (DX_FAILED(g_rhi_resources->device->dx_object()->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(d3d_command_queue.GetAddressOf()))))
        {
          REX_ERROR(LogDxRhi, "Failed to create command queue");
          return nullptr;
        }

        switch (type)
        {
        case GraphicsEngineType::Render: d3d::set_debug_name_for(d3d_command_queue.Get(), "Render Command Queue"); break;
        case GraphicsEngineType::Copy:     d3d::set_debug_name_for(d3d_command_queue.Get(), "Copy Command Queue"); break;
        case GraphicsEngineType::Compute: d3d::set_debug_name_for(d3d_command_queue.Get(), "Compute Command Queue"); break;
        }

        rsl::unique_ptr<DxFence> fence = create_fence();
        return rsl::make_unique<DxCommandQueue>(type, d3d_command_queue, rsl::move(fence));
      }
      rsl::unique_ptr<Swapchain>        create_swapchain(void* apiDevice, s32 bufferCount, void* primaryDisplayHandle)
      {
        DXGI_SWAP_CHAIN_DESC1 sd{};
        sd.Width = 0;
        sd.Height = 0;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = bufferCount;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Stereo = false;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

        // Note: swap chain uses queue to perform flush.
        wrl::ComPtr<IDXGIFactory4> dxgi_factory = g_rhi_resources->factory->as<IDXGIFactory4>();
        wrl::ComPtr<IDXGISwapChain1> d3d_swapchain;
        CommandQueue* cmd_queue = (CommandQueue*)apiDevice;
        DxCommandQueue* dx_cmd_queue = d3d::to_dx12(cmd_queue);
        if (DX_FAILED(dxgi_factory->CreateSwapChainForHwnd(dx_cmd_queue->dx_object(), (HWND)primaryDisplayHandle, &sd, nullptr, nullptr, d3d_swapchain.GetAddressOf())))
        {
          REX_ERROR(LogDxRhi, "Failed to create swap chain");
          return nullptr;
        }

        wrl::ComPtr<IDXGISwapChain3> d3d_swapchain_3;
        DX_CALL(d3d_swapchain.As(&d3d_swapchain_3));

        d3d_swapchain_3->GetDesc1(&sd);
        s32 width = sd.Width;
        s32 height = sd.Height;

        return rsl::make_unique<DxSwapchain>(d3d_swapchain_3, width, height, sd.Format, sd.BufferCount);
      }
      rsl::unique_ptr<CommandAllocator> create_command_allocator(GraphicsEngineType type)
      {
        wrl::ComPtr<ID3D12CommandAllocator> allocator;
        if (DX_FAILED(g_rhi_resources->device->dx_object()->CreateCommandAllocator(d3d::to_dx12(type), IID_PPV_ARGS(allocator.GetAddressOf()))))
        {
          REX_ERROR(LogDxRhi, "Failed to create command allocator");
          return nullptr;
        }

        switch (type)
        {
        case GraphicsEngineType::Render: d3d::set_debug_name_for(allocator.Get(), "Render Command Allocator"); break;
        case GraphicsEngineType::Copy:     d3d::set_debug_name_for(allocator.Get(), "Copy Command Allocator"); break;
        case GraphicsEngineType::Compute: d3d::set_debug_name_for(allocator.Get(), "Compute Command Allocator"); break;
        }

        return rsl::make_unique<DxCommandAllocator>(allocator);
      }

      // Resource creation
      // -------------------------------------------
      rsl::unique_ptr<VertexBuffer>         create_vertex_buffer(s32 numVertices, s32 vertexSize, const void* data)
      {
        s32 total_size = numVertices * vertexSize;
        wrl::ComPtr<ID3D12Resource> d3d_buffer = g_gpu_engine->allocate_buffer(total_size);
        auto vb = rsl::make_unique<DxVertexBuffer>(d3d_buffer, numVertices, vertexSize);

        if (data)
        {
          auto copy_context = gfx::new_copy_ctx();
          copy_context->update_buffer(vb.get(), data, total_size, 0);
          auto sync_info = copy_context->execute_on_gpu();

          auto render_context = gfx::new_render_ctx();
          render_context->stall(*sync_info.get());
          render_context->transition_buffer(vb.get(), ResourceState::VertexAndConstantBuffer);
        }

        return vb;
      }
      rsl::unique_ptr<IndexBuffer>          create_index_buffer(s32 numIndices, IndexBufferFormat format, const void* data)
      {
        s32 index_size = index_format_size(format);
        s32 total_size = numIndices * index_size;
        wrl::ComPtr<ID3D12Resource> buffer = g_gpu_engine->allocate_buffer(total_size);
        d3d::set_debug_name_for(buffer.Get(), "Index Buffer");

        auto ib = rsl::make_unique<DxIndexBuffer>(buffer, numIndices, format);
        if (data)
        {
          auto copy_context = gfx::new_copy_ctx();
          copy_context->update_buffer(ib.get(), data, total_size, 0);
          auto sync_info = copy_context->execute_on_gpu();

          auto render_context = gfx::new_render_ctx();
          render_context->stall(*sync_info.get());
          render_context->transition_buffer(ib.get(), ResourceState::IndexBuffer);
        }

        return ib;
      }
      rsl::unique_ptr<RootSignature>        create_root_signature(const rsl::vector<ShaderParameterDeclaration>& parameters)
      {
        DxShaderPipelineParameters dx_pipeline_parameters = d3d::to_dx12(parameters);

        // A root signature is an array of root parameters.
        REX_WARN(LogDxRhi, "Use versioned root signature here");
        REX_WARN(LogDxRhi, "Investigate if we can use static samplers here as well..");
        CD3DX12_ROOT_SIGNATURE_DESC root_sig_desc(
          dx_pipeline_parameters.root_parameters.size(),
          dx_pipeline_parameters.root_parameters.data(),
          0,          // As we're creating the root signature from reflection, we cannot infer the static samplers at the moment
          nullptr,    // As we're creating the root signature from reflection, we cannot infer the static samplers at the moment
          D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        // Create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
        wrl::ComPtr<ID3DBlob> serialized_root_sig = nullptr;
        wrl::ComPtr<ID3DBlob> error_blob = nullptr;

        HRESULT hr = D3D12SerializeRootSignature(&root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1, serialized_root_sig.GetAddressOf(), error_blob.GetAddressOf());
        if (error_blob != nullptr)
        {
          REX_ERROR(LogDxRhi, "{}", (char*)error_blob->GetBufferPointer());
          return nullptr;
        }

        if (DX_FAILED(hr))
        {
          REX_ERROR(LogDxRhi, "Failed to serialize root signature");
          return nullptr;
        }

        wrl::ComPtr<ID3D12RootSignature> root_signature;
        if (DX_FAILED(g_rhi_resources->device->dx_object()->CreateRootSignature(0, serialized_root_sig->GetBufferPointer(), serialized_root_sig->GetBufferSize(), IID_PPV_ARGS(&root_signature))))
        {
          HR_CALL(g_rhi_resources->device->dx_object()->GetDeviceRemovedReason());
          REX_ERROR(LogDxRhi, "Failed to create root signature");
          return nullptr;
        }

        return rsl::make_unique<DxRootSignature>(root_signature);
      }

      rsl::unique_ptr<RenderTarget>         create_render_target(s32 width, s32 height, TextureFormat format)
      {
        wrl::ComPtr<ID3D12Resource> d3d_texture = g_gpu_engine->allocate_texture2d(width, height, format);
        return create_render_target(d3d_texture);
      }
      rsl::unique_ptr<PipelineState>        create_pso(const InputLayoutDesc& inputLayoutDesc, Material* material)
      {
        PipelineStateDesc desc{};

        material->fill_pso_desc(desc);
        desc.input_layout = inputLayoutDesc;

        return create_pso(desc);
      }
      rsl::unique_ptr<PipelineState>        create_pso(const PipelineStateDesc& desc)
      {
        REX_ASSERT_X(desc.shader_pipeline.vs, "No vertex shader specified for the pso");
        REX_ASSERT_X(desc.shader_pipeline.ps, "No pixel shader specified for the pso");

        InputLayout* input_layout = input_layout_cache::load(desc.input_layout);
        RootSignature* root_signature = root_signature_cache::load(desc.shader_pipeline);

        // Make sure our critical required parameters are specified
        REX_ASSERT_X(input_layout, "No input layout for the pso");
        REX_ASSERT_X(root_signature, "No root signature for the pso");

        D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc{};
        pso_desc.InputLayout            = d3d::to_dx12(input_layout)->dx_object();
        pso_desc.pRootSignature         = d3d::to_dx12(root_signature)->dx_object();
        pso_desc.VS                     = d3d::to_dx12(desc.shader_pipeline.vs)->dx_bytecode();
        pso_desc.PS                     = d3d::to_dx12(desc.shader_pipeline.ps)->dx_bytecode();
        pso_desc.RasterizerState        = d3d::to_dx12(desc.raster_state);
        pso_desc.BlendState             = d3d::to_dx12(desc.blend_state);
        pso_desc.DepthStencilState      = d3d::to_dx12(desc.depth_stencil_state);
        pso_desc.SampleMask             = UINT_MAX;
        pso_desc.PrimitiveTopologyType  = d3d::to_dx12(desc.primitive_topology);
        pso_desc.NumRenderTargets       = 1;
        pso_desc.RTVFormats[0]          = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        pso_desc.SampleDesc.Count       = 1;
        pso_desc.SampleDesc.Quality     = 0;
        pso_desc.DSVFormat              = DXGI_FORMAT_UNKNOWN; // DXGI_FORMAT_D24_UNORM_S8_UINT;

        wrl::ComPtr<ID3D12PipelineState> pso;
        DX_CALL(g_rhi_resources->device->dx_object()->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pso)));
        if (!pso)
        {
          return nullptr;
        }

        return rsl::make_unique<DxPipelineState>(pso, root_signature);
      }
      rsl::unique_ptr<Texture2D>            create_texture2d(s32 width, s32 height, TextureFormat format, const void* data)
      {
        wrl::ComPtr<ID3D12Resource> d3d_texture = g_gpu_engine->allocate_texture2d(width, height, format);
        DxResourceView desc_handle = g_gpu_engine->create_texture2d_srv(d3d_texture.Get());

        auto texture = rsl::make_unique<DxTexture2D>(d3d_texture, desc_handle, width, height, format);

        // Upload data to gpu and transition to pixel shader resource
        if (data)
        {
          auto copy_context = gfx::new_copy_ctx();
          copy_context->update_texture2d(texture.get(), data);
          auto sync_info = copy_context->execute_on_gpu();

          auto render_context = gfx::new_render_ctx();
          render_context->stall(*sync_info.get());
          render_context->transition_buffer(texture.get(), ResourceState::PixelShaderResource);
        }

        return texture;
      }
      rsl::unique_ptr<ConstantBuffer>       create_constant_buffer(rsl::memory_size size)
      {
        // 1) Create the resource on the gpu that'll hold the data of the vertex buffer
        rsl::memory_size aligned_size = align(size.size_in_bytes(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

        wrl::ComPtr<ID3D12Resource> d3d_constant_buffer = g_gpu_engine->allocate_buffer(aligned_size);
        DxResourceView desc_handle = g_gpu_engine->create_cbv(d3d_constant_buffer.Get(), aligned_size);

        d3d::set_debug_name_for(d3d_constant_buffer.Get(), "Constant Buffer");

        return rsl::make_unique<DxConstantBuffer>(d3d_constant_buffer, desc_handle, size);
      }
      rsl::unique_ptr<InputLayout>          create_input_layout(const InputLayoutDesc& desc)
      {
        rsl::vector<D3D12_INPUT_ELEMENT_DESC> input_element_descriptions(rsl::Size(desc.size()));
        REX_ASSERT_X(!input_element_descriptions.empty(), "No input elements provided for input layout");

        for (s32 i = 0; i < desc.size(); ++i)
        {
          input_element_descriptions[i].SemanticName          = shader_semantic_name(desc[i].semantic).data();
          input_element_descriptions[i].SemanticIndex         = desc[i].semantic_index;
          input_element_descriptions[i].Format                = d3d::to_dx12(desc[i].format);
          input_element_descriptions[i].InputSlot             = desc[i].input_slot;
          input_element_descriptions[i].AlignedByteOffset     = desc[i].aligned_byte_offset;
          input_element_descriptions[i].InputSlotClass        = d3d::to_dx12(desc[i].input_slot_class);
          input_element_descriptions[i].InstanceDataStepRate  = desc[i].instance_data_step_rate;
        }

        return rsl::make_unique<DxInputLayout>(input_element_descriptions, rsl::move(desc));
      }

      rsl::unique_ptr<Shader>               create_vertex_shader(rsl::string_view sourceCode, rsl::string_view shaderName)
      {
        CompileShaderDesc compile_vs_desc{};
        compile_vs_desc.shader_source_code = sourceCode;
        compile_vs_desc.shader_entry_point = "main";
        compile_vs_desc.shader_feature_target = "vs_5_1";
        compile_vs_desc.shader_name = shaderName;
        compile_vs_desc.shader_type = ShaderType::Vertex;
        wrl::ComPtr<ID3DBlob> compiled_vs_blob = compile_shader(compile_vs_desc);

        if (!compiled_vs_blob)
        {
          return nullptr;
        }

        return rsl::make_unique<DxVertexShader>(compiled_vs_blob);
      }
      rsl::unique_ptr<Shader>               create_vertex_shader(const memory::Blob& byteBlob)
      {
        wrl::ComPtr<ID3DBlob> d3d_blob = d3d::create_blob(byteBlob);
        return rsl::make_unique<DxVertexShader>(d3d_blob);
      }
      rsl::unique_ptr<Shader>               create_pixel_shader(rsl::string_view sourceCode, rsl::string_view shaderName)
      {
        CompileShaderDesc compile_ps_desc{};
        compile_ps_desc.shader_source_code = sourceCode;
        compile_ps_desc.shader_entry_point = "main";
        compile_ps_desc.shader_feature_target = "ps_5_1";
        compile_ps_desc.shader_name = shaderName;
        compile_ps_desc.shader_type = ShaderType::Pixel;
        wrl::ComPtr<ID3DBlob> compiled_ps_blob = compile_shader(compile_ps_desc);

        if (!compiled_ps_blob)
        {
          return nullptr;
        }

        return rsl::make_unique<DxPixelShader>(compiled_ps_blob);
      }
      rsl::unique_ptr<Shader>               create_pixel_shader(const memory::Blob& byteBlob)
      {
        wrl::ComPtr<ID3DBlob> d3d_blob = d3d::create_blob(byteBlob);
        return rsl::make_unique<DxPixelShader>(d3d_blob);
      }
      rsl::unique_ptr<UploadBuffer>         create_upload_buffer(rsl::memory_size size)
      {
        // an intermediate upload heap.
        CD3DX12_HEAP_PROPERTIES heap_properties_upload(D3D12_HEAP_TYPE_UPLOAD);
        auto buffer_upload = CD3DX12_RESOURCE_DESC::Buffer(size);

        wrl::ComPtr<ID3D12Resource> d3d_upload_buffer;
        if (DX_FAILED(g_rhi_resources->device->dx_object()->CreateCommittedResource(&heap_properties_upload, D3D12_HEAP_FLAG_NONE, &buffer_upload, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(d3d_upload_buffer.GetAddressOf()))))
        {
          REX_ERROR(LogDxRhi, "Failed to create committed resource for intermediate upload heap.");
          return {};
        }

        d3d::set_debug_name_for(d3d_upload_buffer.Get(), "Upload Buffer");
        return rsl::make_unique<DxUploadBuffer>(d3d_upload_buffer);
      }

      rsl::unique_ptr<Material> create_material(rsl::string_view path)
      {
        return nullptr;
      }

      rsl::unique_ptr<Material> create_material(const ShaderPipeline& shaderPipeline, const MaterialDesc& matDesc)
      {
        return rsl::make_unique<Material>(shaderPipeline, matDesc);
      }
      rsl::unique_ptr<Sampler2D> create_sampler2d(const SamplerDesc& desc)
      {
        return g_gpu_engine->create_sampler2d(desc);
      }

      // API Specific functions
      // -------------------------------------------
      rsl::unique_ptr<RenderTarget> create_render_target(wrl::ComPtr<ID3D12Resource>& resource)
      {
        DxResourceView rtv = g_gpu_engine->create_rtv(resource.Get());
        return rsl::make_unique<DxRenderTarget>(resource, rtv);
      }
      wrl::ComPtr<ID3DBlob>                 compile_shader(const CompileShaderDesc& desc)
      {
        wrl::ComPtr<ID3DBlob> byte_code = g_gpu_engine->compile_shader(desc);

        if (!byte_code)
        { 
          REX_ERROR(LogDxRhi, "Failed to compile shader");
          return nullptr;
        }

        return byte_code;
      }
      wrl::ComPtr<ID3D12GraphicsCommandList> create_commandlist(CommandAllocator* alloc, GraphicsEngineType type)
      {
        DxCommandAllocator* dx_alloc = d3d::to_dx12(alloc);

        wrl::ComPtr<ID3D12GraphicsCommandList> cmd_list;
        if (DX_FAILED(g_rhi_resources->device->dx_object()->CreateCommandList(0, d3d::to_dx12(type), dx_alloc->dx_object(), nullptr, IID_PPV_ARGS(cmd_list.GetAddressOf()))))
        {
          REX_ERROR(LogDxRhi, "Failed to create command list");
          return nullptr;
        }

        switch (type)
        {
        case GraphicsEngineType::Render: d3d::set_debug_name_for(cmd_list.Get(), "Render Command List"); break;
        case GraphicsEngineType::Copy:   d3d::set_debug_name_for(cmd_list.Get(), "Copy Command List"); break;
        case GraphicsEngineType::Compute: d3d::set_debug_name_for(cmd_list.Get(), "Compute Command List"); break;
        };

        return cmd_list;
      }
      rsl::unique_ptr<ViewHeap> create_view_heap(D3D12_DESCRIPTOR_HEAP_TYPE type, IsShaderVisible isShaderVisible)
      {
        D3D12_DESCRIPTOR_HEAP_DESC desc{};
        
        s32 num_descriptors = 128;

        desc.Type = type;
        desc.NumDescriptors = num_descriptors;
        desc.Flags = isShaderVisible
          ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
          : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 0; // For single-adapter operation, set this to zero. ( https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_descriptor_heap_desc )

        wrl::ComPtr<ID3D12DescriptorHeap> desc_heap;
        rsl::string_view type_str = rsl::enum_refl::enum_name(type);
        if (DX_FAILED(g_rhi_resources->device->dx_object()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&desc_heap))))
        {
          REX_ERROR(LogDxRhi, "Failed to create descriptor heap for type: {}", type_str);
          return nullptr;
        }

        d3d::set_debug_name_for(desc_heap.Get(), rsl::format("Descriptor Heap - {} - {}", type_str, isShaderVisible ? "Shader Visible" : "Not Shader Visible"));
        s32 desc_size = g_rhi_resources->device->dx_object()->GetDescriptorHandleIncrementSize(type);
        s32 total_size = desc_size * num_descriptors;
        REX_UNUSED_PARAM(total_size);

        REX_INFO(LogDxRhi, "Created {0} ( num: {1} descriptors, desc size: {2} bytes, total size: {3} bytes) ", type_str, num_descriptors, desc_size, total_size);

        return rsl::make_unique<DxViewHeap>(desc_heap, g_rhi_resources->device->dx_object(), isShaderVisible);
      }
      rsl::unique_ptr<ResourceHeap> create_resource_heap()
      {
        CD3DX12_HEAP_DESC desc(100_mib, D3D12_HEAP_TYPE_DEFAULT);

        wrl::ComPtr<ID3D12Heap> d3d_heap;
        if (DX_FAILED(g_rhi_resources->device->dx_object()->CreateHeap(&desc, IID_PPV_ARGS(&d3d_heap))))
        {
          REX_ERROR(LogDxRhi, "Failed to create global resource heap");
          return nullptr;
        }

        return rsl::make_unique<ResourceHeap>(d3d_heap, g_rhi_resources->device->dx_object());
      }
      void report_live_objects()
      {
        g_rhi_resources->debug_interface->report_live_objects();
      }
    }
  }
}