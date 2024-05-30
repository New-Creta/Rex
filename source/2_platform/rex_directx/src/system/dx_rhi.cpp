#include "rex_directx/system/dx_rhi.h"

#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/adapter_manager.h"
#include "rex_directx/dxgi/adapter.h"

#include "rex_directx/utility/dx_util.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_device.h"
#include "rex_directx/system/dx_feature_level.h"
#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/system/dx_commandlist.h"
#include "rex_directx/system/dx_commandlist.h"
#include "rex_directx/system/dx_descriptor_heap.h"
#include "rex_directx/system/dx_resource_heap.h"
#include "rex_directx/system/dx_resource.h"
#include "rex_directx/system/dx_swapchain.h"
#include "rex_directx/system/dx_shader_compiler.h"
#include "rex_directx/resources/dx_vertex_buffer.h"
#include "rex_directx/resources/dx_index_buffer.h"
#include "rex_directx/system/dx_feature_shader_model.h"
#include "rex_directx/system/dx_fence.h"
#include "rex_directx/resources/dx_constant_buffer.h"

#include "rex_directx/resources/dx_vertex_shader_resource.h"
#include "rex_directx/resources/dx_pixel_shader_resource.h"
#include "rex_directx/resources/dx_shader_program_resource.h"
#include "rex_directx/resources/dx_clear_state_resource.h"
#include "rex_directx/resources/dx_raster_state_resource.h"
#include "rex_directx/resources/dx_input_layout_resource.h"
#include "rex_directx/resources/dx_rendertarget.h"
#include "rex_directx/resources/dx_vertex_shader.h"
#include "rex_directx/resources/dx_pixel_shader.h"

#include "rex_directx/system/dx_copy_context.h"
#include "rex_renderer_core/gfx/graphics.h"

#include "rex_directx/resources/dx_root_signature.h"

#include "rex_directx/utility/d3dx12.h"
#include "rex_directx/utility/dx_util.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/pointer_math.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/casting.h"

#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"
#include "rex_renderer_core/rhi/viewport.h"
#include "rex_renderer_core/rhi/scissor_rect.h"
#include "rex_renderer_core/system/resource_hasher.h"

#include "rex_std/bonus/utility.h"

#include "rex_directx/gfx/dx_gpu_engine.h"

#include <D3Dcompiler.h>
#include "rex_directx/dxgi/includes.h"

namespace rex
{
  namespace rhi
  {
    DEFINE_LOG_CATEGORY(LogDxRhi);

    //namespace d3d
    //{
    //  wrl::ComPtr<ID3D12RootSignature> create_shader_root_signature(const rsl::unique_array<ShaderParameterLayoutDescription>& constants, const rsl::unique_array<rhi::DescriptorTableDescription>& tables, const rsl::unique_array<rhi::ShaderSamplerDescription>& samplers);
    //}
    //namespace internal
    //{
    //  // The RHI class. 
    //  // This manages rhi initializes and shutdown
    //  // It owns the lifetime of GPU objects.
    //  class RenderHardwareInfrastructure
    //  {
    //  public:
    //    RenderHardwareInfrastructure(const renderer::OutputWindowUserData& userData, s32 maxFramesInFlight);
    //    ~RenderHardwareInfrastructure();
    //  private:
    //    // DXGI Factory
    //    s32 init_debug_interface();
    //    bool init_dxgi_factory();
    //    void init_debug_controller();
    //    // D3D Device
    //    count_t highest_scoring_gpu(const rsl::vector<GpuDescription>& gpus);
    //    void init_debug_layer();
    //    bool init_d3d_device();
    //    // D3D Command Objects
    //    //bool init_command_queue(CommandType type);
    //    //bool init_command_queues();
    //    //bool init_command_list();
    //    // Resource Heaps
    //    //bool init_resource_heaps();
    //    // Descriptor Heaps
    //    //bool init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE type, s32 numDescriptors);
    //    //bool init_descriptor_heaps();
    //    // Pipeline Library
    //    //bool init_pipeline_library();
    //    // Upload buffers
    //    //bool init_upload_buffers();
    //  private:
    //    constexpr static s32 s_num_rtv_descs = 8;
    //    constexpr static s32 s_num_dsv_descs = 1;
    //    constexpr static s32 s_num_cbv_descs = 128;
    //    constexpr static rsl::array m_expected_feature_levels = { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1 };
    //  public:
    //    bool init_successful;
    //    // Keep the debug interface as the last resource to be destroyed
    //    // it automatically reports if any resources are still active on destruction
    //    DebugInterface debug_interface;
    //    rsl::unique_ptr<dxgi::Factory> factory;
    //    rsl::unique_ptr<DxDevice> device;
    //    //rsl::unique_ptr<CommandAllocator> command_allocator;
    //    //rsl::unordered_map<CommandType, rsl::unique_ptr<CommandQueue>> command_queue_pool;
    //    //rsl::unique_ptr<ResourceHeap> heap;
    //    //rsl::unique_ptr<UploadBuffer> upload_buffer;
    //    //rsl::unique_ptr<PipelineLibrary> pso_lib;
    //    //ResourcePool resource_pool;
    //    //wrl::ComPtr<IDXGIInfoQueue> debug_info_queue;
    //    //rsl::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, DescriptorHeap> descriptor_heap_pool;
    //  };
    //  rsl::unique_ptr<RenderHardwareInfrastructure> g_rhi;
    //  RenderHardwareInfrastructure* get()
    //  {
    //    return g_rhi.get();
    //  }
    //}

    namespace api
    {
      // Some resources need to be accessible directly by the rhi.
      // We could defer all resource creation to the gpu engine as well, but that'd 
      // cause double wrapping (once in the rhi namespace and once in the gpu engine class)
      // To avoid this we have direct access to the resources required by the rhi to create gpu resources
      struct RhiResources
      {
        rsl::unique_ptr<rhi::DebugInterface> debug_interface; // Used to determine if we have any leaking resource on shutdown
        DxDevice* device = nullptr;                           // Used as the factory object to create gpu resources
        DxCommandQueue* render_command_queue = nullptr;       // Used as the object the swapchain speaks to queue a present command
        rsl::unique_ptr<dxgi::Factory> factory;
      };

      rsl::unique_ptr<RhiResources> g_rhi_resources;
      rsl::unique_ptr<gfx::DxGpuEngine> g_gpu_engine;


      const Info& info()
      {
        return g_rhi_resources->device->info();
      }

      void init_debug_controller()
      {
        // Enable extra debugging and send debug messages to the VC++ output window
        rex::wrl::ComPtr<ID3D12Debug> debug_controller;
        if (DX_SUCCESS(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
        {
          debug_controller->EnableDebugLayer();
        }
        else
        {
          REX_WARN(LogDxRhi, "Failed to create DX debug controller");
        }
      }

      s32 init_debug_interface()
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

        init_debug_controller();

        s32 dxgi_factory_flags = 0;

        wrl::ComPtr<IDXGIInfoQueue> debug_info_queue;
        if (DX_SUCCESS(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug_info_queue.GetAddressOf()))))
        {
          dxgi_factory_flags = DXGI_CREATE_FACTORY_DEBUG;

          debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE, globals::g_enable_dxgi_severity_message);
          debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO, globals::g_enable_dxgi_severity_info);
          debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, globals::g_enable_dxgi_severity_warning);
          debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, globals::g_enable_dxgi_severity_error);
          debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, globals::g_enable_dxgi_severity_corruption);

          rsl::array<DXGI_INFO_QUEUE_MESSAGE_ID, 1> dxgi_hide =
          {
              80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
          };

          DXGI_INFO_QUEUE_FILTER dxgi_filter = {};
          dxgi_filter.DenyList.NumIDs = rsl::safe_numeric_cast<u32>(dxgi_hide.size());
          dxgi_filter.DenyList.pIDList = dxgi_hide.data();

          debug_info_queue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &dxgi_filter);
        }
        else
        {
          REX_WARN(LogDxRhi, "Unable to get GXGI Debug Interface");
        }

        return dxgi_factory_flags;
      }

      rsl::unique_ptr<dxgi::Factory> create_dxgi_factory()
      {
        s32 dxgi_factory_flags = 0;

#ifdef REX_ENABLE_DXGI_DEBUG_LAYER
        dxgi_factory_flags = init_debug_interface();
#endif

        return dxgi::Factory::create(dxgi_factory_flags);
      }

      rsl::unique_ptr<DxDevice> create_d3d_device(dxgi::AdapterManager* adapterManager)
      {
        // Find highest scoring gpu
        const dxgi::Adapter* selected_gpu = adapterManager->selected();
        IDXGIAdapter* adapter = selected_gpu->c_ptr();

        // Create device
        const D3D_FEATURE_LEVEL feature_level = query_feature_level(adapter);

        wrl::ComPtr<ID3D12Device1> d3d_device;
        if (DX_FAILED(D3D12CreateDevice(adapter, static_cast<D3D_FEATURE_LEVEL>(feature_level), IID_PPV_ARGS(&d3d_device))))
        {
          REX_ERROR(LogDxRhi, "Software adapter not supported");
          REX_ERROR(LogDxRhi, "Failed to create DX12 Device");
          return nullptr;
        }

        return rsl::make_unique<rhi::DxDevice>(d3d_device, feature_level, selected_gpu);
      }

      // Initialize graphics systems and create a gpu engine
      gfx::GpuEngine* init(const renderer::OutputWindowUserData& userData)
      {
        g_rhi_resources = rsl::make_unique<RhiResources>();

        // The debug interface needs to get created first (and destroyed last)
        // to make sure all resources are tracked and it won't warn about resources
        // not yet destroyed if they'd get destroyed at a later point in time.
        g_rhi_resources->debug_interface = rsl::make_unique<DebugInterface>();

        // 1) we need to init the dxgi factory.
        // This is the system we use to create most other systems.
        g_rhi_resources->factory = create_dxgi_factory();

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
        auto adapter_manager = rsl::make_unique<dxgi::AdapterManager>(g_rhi_resources->factory.get(), [](const GpuDescription& gpu) { return calc_gpu_score(gpu); });
        rsl::unique_ptr<rhi::DxDevice> device = create_d3d_device(adapter_manager.get());
        if (!device)
        {
          REX_ERROR(LogDxRhi, "Failed to create D3D Device");
          return;
        }

        // Initialize the device to be used by the rhi
        g_rhi_resources->device = device.get();

        // Now create the gpu engine which the backend of all our graphics systems
        g_gpu_engine = rsl::make_unique<gfx::DxGpuEngine>(userData, rsl::move(device), rsl::move(adapter_manager));

        return g_gpu_engine.get();
      }

      // Destroy all graphics systems
      void shutdown()
      {
        g_gpu_engine.reset();
        g_rhi_resources.reset();
      }
      // Return the shader platform used for this API.
      renderer::ShaderPlatform shader_platform()
      {
        return renderer::ShaderPlatform::Hlsl;
      }



      //void prepare_user_initialization()
      //{
      //  //reset_command_list(ResourceSlot::make_invalid());
      //}
      //void finish_user_initialization()
      //{
      //  //exec_command_list();
      //  //flush_command_queue();
      //}
      //// Command line interface
      //void reset_command_list(const ResourceSlot& psoSlot)
      //{
      //  ID3D12PipelineState* pso = nullptr;
      //  if (psoSlot.is_valid())
      //  {
      //    pso = internal::get()->resource_pool.as<PipelineState>(psoSlot)->get();
      //  }
      //  internal::get()->command_list->start_recording_commands(internal::get()->command_allocator.get(), pso);
      //  ID3D12DescriptorHeap* heap = internal::get()->descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).get();
      //  internal::get()->command_list->get()->SetDescriptorHeaps(1, &heap);
      //}
      //void exec_command_list()
      //{
      //  internal::get()->command_list->stop_recording_commands();
      //  ID3D12CommandList* cmdlist = internal::get()->command_list->get();
      //  internal::get()->command_queue->get()->ExecuteCommandLists(1, &cmdlist);
      //}
      //void flush_command_queue()
      //{
      //  internal::get()->command_queue->flush();
      //}
      // shader API
      //ResourceSlot compile_shader(const CompileShaderDesc& desc)
      //{
      //  // Check if we don't have the compiled shader already
      //  ResourceID id = hash_resource_desc(desc);
      //  if (internal::get()->resource_pool.has_resource(id))
      //  {
      //    return internal::get()->resource_pool.at(id);
      //  }
      //  // If we don't have it already, compile the shader
      //  ShaderCompiler compiler;
      //  wrl::ComPtr<ID3DBlob> byte_code = compiler.compile_shader(desc);
      //  if (!byte_code)
      //  {
      //    REX_ERROR(LogDxRhi, "Failed to compile shader");
      //    return ResourceSlot::make_invalid();
      //  }
      //  // store the new compiled shader with its id in the resource pool
      //  switch (desc.shader_type)
      //  {
      //  case ShaderType::Vertex:
      //    return internal::get()->resource_pool.insert(rsl::make_unique<VertexShaderResource>(id, byte_code));
      //    break;
      //  case ShaderType::Pixel:
      //    return internal::get()->resource_pool.insert(rsl::make_unique<PixelShaderResource>(id, byte_code));
      //    break;
      //  default:
      //    REX_ERROR(LogDirectX, "Unsupported Shader Type was given");
      //  }
      //  return ResourceSlot::make_invalid();
      //}
      //ResourceSlot link_shader(const LinkShaderDesc& desc)
      //{
      //  // If there's already a linked shader, return it.
      //  ResourceID id = hash_resource_desc(desc);
      //  if (internal::get()->resource_pool.has_resource(id))
      //  {
      //    return internal::get()->resource_pool.at(id);
      //  }
      //  // If not link the shader with the params provided
      //  auto root_sig = d3d::create_shader_root_signature(desc.constants, desc.desc_tables, desc.samplers);
      //  if (!root_sig)
      //  {
      //    REX_ERROR(LogDxRhi, "Failed to create root signature");
      //    return ResourceSlot::make_invalid();
      //  }
      //  auto* vertex_shader = internal::get()->resource_pool.as<VertexShaderResource>(desc.vertex_shader);
      //  auto* pixel_shader = internal::get()->resource_pool.as<PixelShaderResource>(desc.pixel_shader);
      //  rsl::unique_array<ShaderParameterLayoutDescription> constants = rsl::make_unique<ShaderParameterLayoutDescription[]>(desc.constants.count());
      //  rsl::memcpy(constants.get(), desc.constants.get(), desc.constants.byte_size());
      //  rsl::unique_array<DescriptorTableDescription> desc_tables = rsl::make_unique<DescriptorTableDescription[]>(desc.desc_tables.count());
      //  rsl::memcpy(desc_tables.get(), desc.desc_tables.get(), desc.desc_tables.byte_size());
      //  rsl::unique_array<ShaderSamplerDescription> samplers = rsl::make_unique<ShaderSamplerDescription[]>(desc.samplers.count());
      //  rsl::memcpy(samplers.get(), desc.samplers.get(), desc.samplers.byte_size());
      //  // create a combined shader object with the root sig, the vertex shader and the pixel shader
      //  return internal::get()->resource_pool.insert(rsl::make_unique<ShaderProgramResource>(id, root_sig, vertex_shader, pixel_shader, rsl::move(constants), rsl::move(desc_tables), rsl::move(samplers)));
      //}
      //ResourceSlot create_shader(const ShaderDesc& desc)
      //{
      //  // Check if the shader isn't already loaded
      //  ResourceID id = hash_resource_desc(desc);
      //  if (internal::get()->resource_pool.has_resource(id))
      //  {
      //    return internal::get()->resource_pool.at(id);
      //  }
      //  // If the shader hasn't been loaded before
      //  wrl::ComPtr<ID3DBlob> byte_code = rex::d3d::create_blob(desc.shader_byte_code);
      //  switch (desc.shader_type)
      //  {
      //  case ShaderType::Vertex: return internal::get()->resource_pool.insert(rsl::make_unique<VertexShaderResource>(id, byte_code));
      //  case ShaderType::Pixel: return internal::get()->resource_pool.insert(rsl::make_unique<PixelShaderResource>(id, byte_code));
      //  default:
      //    REX_ERROR(LogDirectX, "Unsupported Shader Type was given");
      //  }
      //  return ResourceSlot::make_invalid();
      //}
      //s32 back_buffer_index()
      //{
      //  return internal::get()->swapchain->get()->GetCurrentBackBufferIndex();
      //}
      //void execute_commandlist(CommandList* cmdList)
      //{
      //  ID3D12CommandList* d3d_cmdlist = cmdList->dx_object();
      //  internal::get()->command_queue->get()->ExecuteCommandLists(1, &d3d_cmdlist);
      //  internal::get()->command_queue->flush();
      //}




      rsl::unique_ptr<CommandList> create_commandlist(CommandAllocator* alloc, CommandType type, ResourceStateTracker* resourceStateTracker)
      {
        DxCommandAllocator* dx_alloc = d3d::to_dx12(alloc);

        wrl::ComPtr<ID3D12GraphicsCommandList> cmd_list;
        if (DX_FAILED(g_rhi_resources->device->get()->CreateCommandList(0, d3d::to_dx12(type), dx_alloc->get(), nullptr, IID_PPV_ARGS(cmd_list.GetAddressOf()))))
        {
          REX_ERROR(LogDxRhi, "Failed to create command list");
          return nullptr;
        }

        return rsl::make_unique<CommandList>(cmd_list, resourceStateTracker);
      }
      rsl::unique_ptr<DxFence> create_fence()
      {
        wrl::ComPtr<ID3D12Fence> fence;
        if (DX_FAILED(g_rhi_resources->device->get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
        {
          REX_ERROR(LogDxRhi, "Failed to create DX fence, to synchronize CPU/GPU");
          return nullptr;
        }

        return rsl::make_unique<DxFence>(fence);
      }
      rsl::unique_ptr<CommandQueue> create_command_queue(CommandType type)
      {
        // Command Queue
        wrl::ComPtr<ID3D12CommandQueue> d3d_command_queue;
        D3D12_COMMAND_QUEUE_DESC queue_desc = {};
        queue_desc.Type = d3d::to_dx12(type);
        queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        if (DX_FAILED(g_rhi_resources->device->get()->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(d3d_command_queue.GetAddressOf()))))
        {
          REX_ERROR(LogDxRhi, "Failed to create command queue");
          return false;
        }

        rsl::unique_ptr<DxFence> fence = create_fence();
        return rsl::make_unique<DxCommandQueue>(type, d3d_command_queue, rsl::move(fence));
      }
      rsl::unique_ptr<Swapchain> create_swapchain(void* apiDevice, s32 bufferCount, void* primaryDisplayHandle)
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
        sd.AlphaMode - DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Stereo = false;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

        // Note: swap chain uses queue to perform flush.
        rex::wrl::ComPtr<IDXGIFactory4> dxgi_factory = g_rhi_resources->factory->as<IDXGIFactory4>();
        rex::wrl::ComPtr<IDXGISwapChain1> d3d_swapchain;
        IUnknown* d3d_device = (IUnknown*)apiDevice;
        if (DX_FAILED(dxgi_factory->CreateSwapChainForHwnd(d3d_device, (HWND)primaryDisplayHandle, &sd, nullptr, nullptr, d3d_swapchain.GetAddressOf())))
        {
          REX_ERROR(LogDxRhi, "Failed to create swap chain");
          return false;
        }

        wrl::ComPtr<IDXGISwapChain3> d3d_swapchain_3;
        DX_CALL(d3d_swapchain.As(&d3d_swapchain_3));

        return rsl::make_unique<DxSwapchain>(d3d_swapchain_3, sd.Format, sd.BufferCount);
      }
      rsl::unique_ptr<CommandAllocator> create_command_allocator()
      {
        wrl::ComPtr<ID3D12CommandAllocator> allocator;
        if (DX_FAILED(g_rhi_resources->device->get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocator.GetAddressOf()))))
        {
          REX_ERROR(LogDxRhi, "Failed to create command allocator");
          return false;
        }
        return rsl::make_unique<rex::rhi::CommandAllocator>(allocator);
      }

      //rsl::unique_ptr<RenderTarget> create_render_target_from_backbuffer(Resource2* resource)
      //{
      //  DescriptorHandle rtv = internal::get()->descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_RTV).create_rtv(resource->dx_object());
      //  return rsl::make_unique<RenderTarget>(resource->dx_object(), rtv);
      //}
      rsl::unique_ptr<RenderTarget> create_render_target(Texture2D* texture)
      {
        DescriptorHandle rtv = g_gpu_engine->create_rtv(texture);
        return rsl::make_unique<DxRenderTarget>(texture, rtv);
      }
      rsl::unique_ptr<VertexBuffer> create_vertex_buffer(s32 numVertices, s32 vertexSize)
      {
        s32 total_size = numVertices * vertexSize;
        wrl::ComPtr<ID3D12Resource> d3d_buffer = g_gpu_engine->allocate_buffer(total_size);
        return rsl::make_unique<DxVertexBuffer>(d3d_buffer, numVertices, vertexSize);

        //wrl::ComPtr<ID3D12Resource> buffer = internal::get()->heap->create_buffer(total_size);
        //set_debug_name_for(buffer.Get(), "Vertex Buffer");
        //return rsl::make_unique<DxVertexBuffer>(buffer, numVertices, vertexSize);
      }
      rsl::unique_ptr<IndexBuffer> create_index_buffer(s32 numIndices, renderer::IndexBufferFormat format)
      {
        s32 index_size = renderer::index_format_size(format);
        s32 total_size = numIndices * index_size;
        wrl::ComPtr<ID3D12Resource> buffer = g_gpu_engine->allocate_buffer(total_size);
        //set_debug_name_for(buffer.Get(), "Index Buffer");
        return rsl::make_unique<DxIndexBuffer>(buffer, numIndices, format);
      }
      rsl::unique_ptr<RootSignature> create_root_signature(const RootSignatureDesc& desc)
      {
        // Root parameter can be a table, root descriptor or root constants.
        auto root_parameters = rsl::vector<CD3DX12_ROOT_PARAMETER>(rsl::Capacity(desc.views.count()));

        for (s32 i = 0; i < desc.constants.count(); ++i)
        {
          const auto& param = desc.constants[i];

          D3D12_SHADER_VISIBILITY visibility = rex::d3d::to_dx12(param.visibility);
          root_parameters.emplace_back().InitAsConstants(param.num_32bits, param.reg, param.reg_space, visibility);
        }

        for (s32 i = 0; i < desc.views.count(); ++i)
        {
          const auto& param = desc.views[i];

          D3D12_SHADER_VISIBILITY visibility = rex::d3d::to_dx12(param.visibility);
          switch (param.type)
          {
          case ShaderViewType::ConstantBufferView: root_parameters.emplace_back().InitAsConstantBufferView(param.reg, param.reg_space, visibility); break;
          case ShaderViewType::ShaderResourceView: root_parameters.emplace_back().InitAsShaderResourceView(param.reg, param.reg_space, visibility); break;
          case ShaderViewType::UnorderedAccessView: root_parameters.emplace_back().InitAsUnorderedAccessView(param.reg, param.reg_space, visibility); break;
          }
        }

        rsl::vector<D3D12_DESCRIPTOR_RANGE> ranges;
        for (s32 i = 0; i < desc.desc_tables.count(); ++i)
        {
          const auto& table = desc.desc_tables[i];
          D3D12_SHADER_VISIBILITY visibility = rex::d3d::to_dx12(table.visibility);
          for (s32 range_idx = 0; range_idx < table.ranges.count(); ++range_idx)
          {
            ranges.push_back(rex::d3d::to_dx12(table.ranges[range_idx]));
          }

          root_parameters.emplace_back().InitAsDescriptorTable(ranges.size(), ranges.data(), visibility);
        }

        auto root_samplers = rsl::vector<D3D12_STATIC_SAMPLER_DESC>(rsl::Capacity(desc.samplers.count()));

        for (s32 i = 0; i < desc.samplers.count(); ++i)
        {
          const auto& sampler = desc.samplers[i];

          D3D12_STATIC_SAMPLER_DESC desc{};
          desc.Filter = rex::d3d::to_dx12(sampler.filtering);
          desc.AddressU = rex::d3d::to_dx12(sampler.address_mode_u);
          desc.AddressV = rex::d3d::to_dx12(sampler.address_mode_v);
          desc.AddressW = rex::d3d::to_dx12(sampler.address_mode_w);
          desc.MipLODBias = sampler.mip_lod_bias;
          desc.MaxAnisotropy = sampler.max_anisotropy;
          desc.ComparisonFunc = rex::d3d::to_dx12(sampler.comparison_func);
          desc.BorderColor = rex::d3d::to_dx12(sampler.border_color);
          desc.MinLOD = sampler.min_lod;
          desc.MaxLOD = sampler.max_lod;
          desc.ShaderRegister = sampler.shader_register;
          desc.RegisterSpace = sampler.register_space;
          desc.ShaderVisibility = rex::d3d::to_dx12(sampler.shader_visibility);
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
          REX_ERROR(LogDxRhi, "{}", (char*)error_blob->GetBufferPointer());
          return nullptr;
        }

        if (DX_FAILED(hr))
        {
          REX_ERROR(LogDxRhi, "Failed to serialize root signature");
          return nullptr;
        }

        wrl::ComPtr<ID3D12RootSignature> root_signature;
        if (DX_FAILED(g_rhi_resources->device->get()->CreateRootSignature(0, serialized_root_sig->GetBufferPointer(), serialized_root_sig->GetBufferSize(), IID_PPV_ARGS(&root_signature))))
        {
          REX_ERROR(LogDxRhi, "Failed to create root signature");
          return nullptr;
        }

        //rhi::set_debug_name_for(root_signature.Get(), "Root Signature");

        return rsl::make_unique<RootSignature>(root_signature);
      }

      rsl::unique_ptr<PipelineState> create_pso(const PipelineStateDesc& desc)
      {
        // 1) Load the resources from the resource pool
        D3D12_RASTERIZER_DESC d3d_raster_state = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        if (desc.raster_state)
        {
          d3d_raster_state = *desc.raster_state->get();
        }

        D3D12_BLEND_DESC d3d_blend_state = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        if (desc.blend_state.has_value())
        {
          d3d_blend_state = rex::d3d::to_dx12(desc.blend_state.value());
        }

        D3D12_DEPTH_STENCIL_DESC d3d_depth_stencil_state = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        if (desc.depth_stencil_state.has_value())
        {
          d3d_depth_stencil_state = rex::d3d::to_dx12(desc.depth_stencil_state.value());
        }

        // 2) Fill in the PSO desc
        D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc{};
        pso_desc.InputLayout = *desc.input_layout->get();
        pso_desc.pRootSignature = d3d::to_dx12(desc.root_signature)->dx_object();
        pso_desc.VS = d3d::to_dx12(desc.vertex_shader)->dx_bytecode();
        pso_desc.PS = d3d::to_dx12(desc.pixel_shader)->dx_bytecode();
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

        wrl::ComPtr<ID3D12PipelineState> pso;
        g_rhi_resources->device->get()->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pso));

        return rsl::make_unique<PipelineState>(pso);
      }

      rsl::unique_ptr<Texture2D> create_texture2d(s32 width, s32 height, renderer::TextureFormat format, const void* data)
      {
        wrl::ComPtr<ID3D12Resource> d3d_texture = g_gpu_engine->allocate_texture2d(format, width, height);
        DescriptorHandle desc_handle = g_gpu_engine->create_texture2d_srv(d3d_texture.Get());

        auto texture = rsl::make_unique<Texture2D>(d3d_texture, desc_handle);

        if (data)
        {
          auto copy_context = gfx::new_copy_ctx();

          copy_context->update_texture2d(texture.get(), data);
        }

        //wrl::ComPtr<ID3D12Resource> d3d_texture = internal::get()->heap->create_texture2d(d3d_format, width, height);
        //DescriptorHandle desc_handle = internal::get()->descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).create_texture2d_srv(d3d_texture.Get());

        //auto texture = rsl::make_unique<Texture2D>(d3d_texture, desc_handle, width, height, format);
        //if (data)
        //{
        //  gfx_copy_engine->copy_to_texture(texture.get(), data);
        //}

        return texture;
      }
      rsl::unique_ptr<RasterStateResource> create_raster_state(const RasterStateDesc& desc)
      {
        D3D12_RASTERIZER_DESC d3d_rs;

        d3d_rs.FillMode = rex::d3d::to_dx12(desc.fill_mode);
        d3d_rs.CullMode = rex::d3d::to_dx12(desc.cull_mode);
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

        return rsl::make_unique<RasterStateResource>(d3d_rs);
      }
      rsl::unique_ptr<ConstantBuffer> create_constant_buffer(rsl::memory_size size)
      {
        // 1) Create the resource on the gpu that'll hold the data of the vertex buffer
        wrl::ComPtr<ID3D12Resource> d3d_constant_buffer = g_gpu_engine->allocate_buffer(size);
        DescriptorHandle desc_handle = g_gpu_engine->create_cbv(d3d_constant_buffer.Get());

        return rsl::make_unique<DxConstantBuffer>(d3d_constant_buffer, desc_handle, size);

        //rsl::memory_size aligned_size = rex::align(size.size_in_bytes(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
        //wrl::ComPtr<ID3D12Resource> buffer = internal::get()->heap->create_buffer(aligned_size);
        //set_debug_name_for(buffer.Get(), "Constant Buffer");

        //// 2) Copy the data into the upload buffer
        ////internal::get()->upload_buffer->write(internal::get()->command_list.get(), cb.get(), desc.blob_view.data(), aligned_size);

        //// 3) Create a view to this constant buffer
        //DescriptorHandle desc_handle = internal::get()->descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).create_cbv(buffer.Get(), aligned_size);

        //return rsl::make_unique<ConstantBuffer>(buffer, desc_handle, size);
      }
      rsl::unique_ptr<InputLayoutResource> create_input_layout(const InputLayoutDesc& desc)
      {
        rsl::vector<D3D12_INPUT_ELEMENT_DESC> input_element_descriptions(rsl::Size(desc.input_layout.size()));
        REX_ASSERT_X(!input_element_descriptions.empty(), "No input elements provided for input layout");

        for (s32 i = 0; i < desc.input_layout.size(); ++i)
        {
          input_element_descriptions[i].SemanticName = desc.input_layout[i].semantic_name.data();
          input_element_descriptions[i].SemanticIndex = desc.input_layout[i].semantic_index;
          input_element_descriptions[i].Format = rex::d3d::to_dx12(desc.input_layout[i].format);
          input_element_descriptions[i].InputSlot = desc.input_layout[i].input_slot;
          input_element_descriptions[i].AlignedByteOffset = desc.input_layout[i].aligned_byte_offset;
          input_element_descriptions[i].InputSlotClass = rex::d3d::to_dx12(desc.input_layout[i].input_slot_class);
          input_element_descriptions[i].InstanceDataStepRate = desc.input_layout[i].instance_data_step_rate;
        }

        return rsl::make_unique<InputLayoutResource>(input_element_descriptions);
      }
      rsl::unique_ptr<Shader> create_vertex_shader(rsl::string_view sourceCode)
      {
        rex::rhi::CompileShaderDesc compile_vs_desc{};
        compile_vs_desc.shader_code = rex::memory::Blob(rsl::make_unique<char[]>(sourceCode.length()));
        compile_vs_desc.shader_code.write(sourceCode.data(), sourceCode.length());
        compile_vs_desc.shader_entry_point = "main";
        compile_vs_desc.shader_feature_target = "vs_5_0";
        compile_vs_desc.shader_name = "imgui_vertex_shader";
        compile_vs_desc.shader_type = rex::rhi::ShaderType::Vertex;
        wrl::ComPtr<ID3DBlob> compiled_vs_blob = rex::rhi::compile_shader(compile_vs_desc);

        return rsl::make_unique<VertexShader>(compiled_vs_blob);
      }
      rsl::unique_ptr<Shader> create_pixel_shader(rsl::string_view sourceCode)
      {
        rex::rhi::CompileShaderDesc compile_ps_desc{};
        compile_ps_desc.shader_code = rex::memory::Blob(rsl::make_unique<char[]>(sourceCode.length()));
        compile_ps_desc.shader_code.write(sourceCode.data(), sourceCode.length());
        compile_ps_desc.shader_entry_point = "main";
        compile_ps_desc.shader_feature_target = "ps_5_0";
        compile_ps_desc.shader_name = "imgui_pixel_shader";
        compile_ps_desc.shader_type = rex::rhi::ShaderType::Pixel;
        wrl::ComPtr<ID3DBlob> compiled_ps_blob = rex::rhi::compile_shader(compile_ps_desc);

        return rsl::make_unique<PixelShader>(compiled_ps_blob);
      }
      wrl::ComPtr<ID3DBlob> compile_shader(const CompileShaderDesc& desc)
      {
        ShaderCompiler compiler;
        wrl::ComPtr<ID3DBlob> byte_code = compiler.compile_shader(desc);

        if (!byte_code)
        {
          REX_ERROR(LogDxRhi, "Failed to compile shader");
          return nullptr;
        }

        return byte_code;
      }
      //DescriptorHeap* cbv_uav_srv_desc_heap()
      //{
      //  return &internal::get()->descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
      //}

      rsl::unique_ptr<UploadBuffer> create_upload_buffer(rsl::memory_size size)
      {
        // an intermediate upload heap.
        CD3DX12_HEAP_PROPERTIES heap_properties_upload(D3D12_HEAP_TYPE_UPLOAD);
        auto buffer_upload = CD3DX12_RESOURCE_DESC::Buffer(size);

        wrl::ComPtr<ID3D12Resource> d3d_upload_buffer;
        if (DX_FAILED(g_rhi_resources->device->get()->CreateCommittedResource(&heap_properties_upload, D3D12_HEAP_FLAG_NONE, &buffer_upload, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(d3d_upload_buffer.GetAddressOf()))))
        {
          REX_ERROR(LogDxRhi, "Failed to create committed resource for intermediate upload heap.");
          return {};
        }

        set_debug_name_for(d3d_upload_buffer.Get(), "Upload Buffer");
        return rsl::make_unique<UploadBuffer>(d3d_upload_buffer, D3D12_RESOURCE_STATE_COMMON);
      }

      //UploadBuffer* global_upload_buffer()
      //{
      //  return internal::get()->upload_buffer.get();
      //}

      //void execute_command_list(CommandList* cmdList)
      //{
      //  CommandType type = cmdList->type();
      //  internal::get()->command_queue_pool.at(type)->execute(cmdList);
      //}
      //void wait_for_gpu(CommandType type)
      //{
      //  internal::get()->command_queue_pool.at(type)->flush();
      //}





    }












    //// A clear state is just a struct holding different values to clear a buffer
    //// Flags control which part of the buffer (color, depths or stencil) should be cleared
    //ResourceSlot create_clear_state(const ClearStateDesc& desc)
    //{
    //  ResourceID id = hash_resource_desc(desc);
    //  if (internal::get()->resource_pool.has_resource(id))
    //  {
    //    return internal::get()->resource_pool.at(id);
    //  }

    //  return internal::get()->resource_pool.insert(rsl::make_unique<ClearStateResource>(id, desc));
    //}
    //// A raster state holds rasterization settings
    //// settings like cull mode, fill mode, depth bias, normal orientation, ..
    //// are all included in the raster state
    //ResourceSlot create_raster_state(const RasterStateDesc& desc)
    //{
    //  ResourceID id = hash_resource_desc(desc);
    //  if (internal::get()->resource_pool.has_resource(id))
    //  {
    //    return internal::get()->resource_pool.at(id);
    //  }

    //  D3D12_RASTERIZER_DESC d3d_rs;

    //  d3d_rs.FillMode = rex::d3d::to_dx12(desc.fill_mode);
    //  d3d_rs.CullMode = rex::d3d::to_dx12(desc.cull_mode);
    //  d3d_rs.FrontCounterClockwise = desc.front_ccw;
    //  d3d_rs.DepthBias = desc.depth_bias;
    //  d3d_rs.DepthBiasClamp = desc.depth_bias_clamp;
    //  d3d_rs.SlopeScaledDepthBias = desc.sloped_scale_depth_bias;
    //  d3d_rs.DepthClipEnable = desc.depth_clip_enable;
    //  d3d_rs.ForcedSampleCount = desc.forced_sample_count;

    //  /**
    //   * Conservative rasterization means that all pixels that are at least partially covered by a rendered primitive are rasterized, which means that the pixel shader is invoked.
    //   * Normal behavior is sampling, which is not used if conservative rasterization is enabled.
    //   *
    //   * Conservative rasterization is useful in a number of situations outside of rendering (collision detection, occlusion culling, and visibility detection).
    //   *
    //   * https://learn.microsoft.com/en-us/windows/win32/direct3d11/conservative-rasterization
    //   */
    //  d3d_rs.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    //  d3d_rs.MultisampleEnable = desc.multisample;
    //  d3d_rs.AntialiasedLineEnable = desc.aa_lines;

    //  return internal::get()->resource_pool.insert(rsl::make_unique<RasterStateResource>(id, d3d_rs));
    //}
    //// An input layout determines the format of vertices
    //// It determines where a shader can find the position, normal, color
    //// of a vertex.
    //ResourceSlot create_input_layout(const InputLayoutDesc& desc)
    //{
    //  ResourceID id = hash_resource_desc(desc);
    //  if (internal::get()->resource_pool.has_resource(id))
    //  {
    //    return internal::get()->resource_pool.at(id);
    //  }
    //  rsl::vector<D3D12_INPUT_ELEMENT_DESC> input_element_descriptions(rsl::Size(desc.input_layout.size()));
    //  REX_ASSERT_X(!input_element_descriptions.empty(), "No input elements provided for input layout");

    //  for (s32 i = 0; i < desc.input_layout.size(); ++i)
    //  {
    //    input_element_descriptions[i].SemanticName = desc.input_layout[i].semantic_name.data();
    //    input_element_descriptions[i].SemanticIndex = desc.input_layout[i].semantic_index;
    //    input_element_descriptions[i].Format = rex::d3d::to_dx12(desc.input_layout[i].format);
    //    input_element_descriptions[i].InputSlot = desc.input_layout[i].input_slot;
    //    input_element_descriptions[i].AlignedByteOffset = desc.input_layout[i].aligned_byte_offset;
    //    input_element_descriptions[i].InputSlotClass = rex::d3d::to_dx12(desc.input_layout[i].input_slot_class);
    //    input_element_descriptions[i].InstanceDataStepRate = desc.input_layout[i].instance_data_step_rate;
    //  }

    //  return internal::get()->resource_pool.insert(rsl::make_unique<InputLayoutResource>(id, input_element_descriptions));
    //}
    //// A vertex buffer is a buffer holding vertices of 1 or more objects
    //ResourceSlot create_vertex_buffer(const VertexBufferDesc& desc)
    //{
    //  ResourceID id = hash_resource_desc(desc);
    //  if (internal::get()->resource_pool.has_resource(id))
    //  {
    //    return internal::get()->resource_pool.at(id);
    //  }

    //  // 1) Create the resource on the gpu that'll hold the data of the vertex buffer
    //  rsl::memory_size size = desc.blob_view.size();
    //  wrl::ComPtr<ID3D12Resource> buffer = internal::get()->heap->create_buffer(size);
    //  set_debug_name_for(buffer.Get(), "Vertex Buffer");
    //  rsl::unique_ptr<VertexBuffer> vb = rsl::make_unique<VertexBuffer>(buffer, desc.blob_view.size(), desc.vertex_size);

    //  // 2) Copy the data into the upload buffer
    //  internal::get()->upload_buffer->write(internal::get()->command_list->get(), vb.get(), desc.blob_view.data(), size);

    //  //// 3) Upload the data from the upload buffer onto the gpu
    //  //internal::get()->upload_buffer->upload(internal::get()->command_list.get());

    //  // 4) Cache that the data is available on the gpu (or will be soon)
    //  // so if another request comes in for the same data, we don't upload it again

    //  // 5) Add the resource to the resource pool
    //  return internal::get()->resource_pool.insert(rsl::move(vb));
    //}
    //// A vertex buffer is a buffer holding vertices of 1 or more objects
    //ResourceSlot create_vertex_buffer(s32 totalSize, s32 vertexSize)
    //{
    //  // 1) Create the resource on the gpu that'll hold the data of the vertex buffer
    //  wrl::ComPtr<ID3D12Resource> buffer = internal::get()->heap->create_buffer(totalSize * vertexSize);
    //  set_debug_name_for(buffer.Get(), "Vertex Buffer");
    //  rsl::unique_ptr<VertexBuffer> vb = rsl::make_unique<VertexBuffer>(buffer, totalSize * vertexSize, vertexSize);

    //  // 2) Copy the data into the upload buffer
    //  //internal::get()->upload_buffer->write(internal::get()->command_list.get(), vb.get(), desc.blob_view.data(), size);

    //  //// 3) Upload the data from the upload buffer onto the gpu
    //  //internal::get()->upload_buffer->upload(internal::get()->command_list.get());

    //  // 4) Cache that the data is available on the gpu (or will be soon)
    //  // so if another request comes in for the same data, we don't upload it again

    //  // 5) Add the resource to the resource pool
    //  return internal::get()->resource_pool.insert(rsl::move(vb));
    //}

    //// An index buffer is a buffer holding indices of 1 or more objects
    //ResourceSlot create_index_buffer(const IndexBufferDesc& desc)
    //{
    //  ResourceID id = hash_resource_desc(desc);
    //  if (internal::get()->resource_pool.has_resource(id))
    //  {
    //    return internal::get()->resource_pool.at(id);
    //  }

    //  // 1) Create the resource on the gpu that'll hold the data of the vertex buffer
    //  rsl::memory_size size = desc.blob_view.size();
    //  DXGI_FORMAT d3d_format = rex::d3d::to_dx12(desc.format);
    //  wrl::ComPtr<ID3D12Resource> buffer = internal::get()->heap->create_buffer(size, 0);
    //  set_debug_name_for(buffer.Get(), "Index Buffer");
    //  rsl::unique_ptr<IndexBuffer> ib = rsl::make_unique<IndexBuffer>(buffer, D3D12_RESOURCE_STATE_INDEX_BUFFER, desc.blob_view.size(), d3d_format);

    //  // 2) Copy the data into the upload buffer
    //  internal::get()->upload_buffer->write(internal::get()->command_list->get(), ib.get(), desc.blob_view.data(), size);

    //  // 3) Upload the data from the upload buffer onto the gpu
    //  //internal::get()->upload_buffer->upload(internal::get()->command_list.get());

    //  // 4) add the resource to the resource pool
    //  return internal::get()->resource_pool.insert(rsl::move(ib));
    //}
    //// An index buffer is a buffer holding indices of 1 or more objects
    //ResourceSlot create_index_buffer(s32 totalSize, renderer::IndexBufferFormat format)
    //{
    //  // 1) Create the resource on the gpu that'll hold the data of the vertex buffer
    //  DXGI_FORMAT d3d_format = rex::d3d::to_dx12(format);
    //  wrl::ComPtr<ID3D12Resource> buffer = internal::get()->heap->create_buffer(totalSize, 0);
    //  set_debug_name_for(buffer.Get(), "Index Buffer");
    //  rsl::unique_ptr<IndexBuffer> ib = rsl::make_unique<IndexBuffer>(buffer, D3D12_RESOURCE_STATE_INDEX_BUFFER, totalSize, d3d_format);

    //  // 2) Copy the data into the upload buffer
    //  //internal::get()->upload_buffer->write(internal::get()->command_list.get(), ib.get(), desc.blob_view.data(), size);

    //  // 3) Upload the data from the upload buffer onto the gpu
    //  //internal::get()->upload_buffer->upload(internal::get()->command_list.get());

    //  // 4) add the resource to the resource pool
    //  return internal::get()->resource_pool.insert(rsl::move(ib));
    //}
    //// A constant buffer is a buffer holding data that's accessible to a shader
    //// This can hold data like ints, floats, vectors and matrices
    //ResourceSlot create_constant_buffer(const ConstantBufferDesc& desc)
    //{
    //  return create_constant_buffer(desc.blob_view.size());
    //}
    //// A constant buffer is a buffer holding data that's accessible to a shader
    //// This can hold data like ints, floats, vectors and matrices
    //ResourceSlot create_constant_buffer(rsl::memory_size size)
    //{
    //  //ResourceID id = hash_resource_desc();
    //  //if (internal::get()->resource_pool.has_resource(id))
    //  //{
    //  //  return internal::get()->resource_pool.at(id);
    //  //}

    //  // 1) Create the resource on the gpu that'll hold the data of the vertex buffer
    //  rsl::memory_size aligned_size = rex::align(size.size_in_bytes(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
    //  wrl::ComPtr<ID3D12Resource> buffer = internal::get()->heap->create_buffer(aligned_size);
    //  set_debug_name_for(buffer.Get(), "Constant Buffer");
    //  rsl::unique_ptr<ConstantBuffer> cb = rsl::make_unique<ConstantBuffer>(buffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, size);

    //  // 2) Copy the data into the upload buffer
    //  //internal::get()->upload_buffer->write(internal::get()->command_list.get(), cb.get(), desc.blob_view.data(), aligned_size);

    //  // 3) Create a view to this constant buffer
    //  internal::get()->descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).create_cbv(buffer.Get(), aligned_size);

    //  // 4) add the buffer to the resource pool
    //  return internal::get()->resource_pool.insert(rsl::move(cb));
    //}
    //// A pipeline state object defines a state for the graphics pipeline.
    //// It holds the input layout, root signature, shaders, raster state, blend state ..
    //// needed for a draw call.
    //ResourceSlot create_pso(const PipelineStateDesc& desc)
    //{
    //  // 1) Load the resources from the resource pool
    //  InputLayoutResource* input_layout = internal::get()->resource_pool.as<InputLayoutResource>(desc.input_layout);
    //  ShaderProgramResource* shader = internal::get()->resource_pool.as<ShaderProgramResource>(desc.shader);

    //  D3D12_RASTERIZER_DESC d3d_raster_state = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    //  if (desc.raster_state.is_valid())
    //  {
    //    RasterStateResource* raster_state = internal::get()->resource_pool.as<RasterStateResource>(desc.raster_state);
    //    d3d_raster_state = *raster_state->get();
    //  }

    //  D3D12_BLEND_DESC d3d_blend_state = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    //  if (desc.blend_state.has_value())
    //  {
    //    d3d_blend_state = rex::d3d::to_dx12(desc.blend_state.value());
    //  }

    //  D3D12_DEPTH_STENCIL_DESC d3d_depth_stencil_state = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    //  if (desc.depth_stencil_state.has_value())
    //  {
    //    d3d_depth_stencil_state = rex::d3d::to_dx12(desc.depth_stencil_state.value());
    //  }

    //  // 2) Fill in the PSO desc
    //  D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc{};
    //  pso_desc.InputLayout = *input_layout->get();
    //  pso_desc.pRootSignature = shader->root_signature();
    //  pso_desc.VS = shader->vs();
    //  pso_desc.PS = shader->ps();
    //  pso_desc.RasterizerState = d3d_raster_state;
    //  pso_desc.BlendState = d3d_blend_state;
    //  pso_desc.DepthStencilState = d3d_depth_stencil_state;
    //  pso_desc.SampleMask = UINT_MAX;
    //  pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    //  pso_desc.NumRenderTargets = 1;
    //  pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    //  pso_desc.SampleDesc.Count = 1;
    //  pso_desc.SampleDesc.Quality = 0;
    //  pso_desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    //  // 3) Create a resource slot for a pso if it already exists in the lib
    //  rsl::unique_ptr<PipelineState> pso = internal::get()->pso_lib->load_pso(pso_desc);
    //  if (pso)
    //  {
    //    return internal::get()->resource_pool.insert(rsl::move(pso));
    //  }

    //  // 4) Ohterwise store the new PSO in the pipeline library
    //  pso = internal::get()->pso_lib->store_pso(pso_desc);

    //  return internal::get()->resource_pool.insert(rsl::move(pso));
    //}
    //// Create a 2D texture on the gpu and add the data to the upload buffer
    //// to be uploaded the next time the upload buffer
    //ResourceSlot create_texture2d(const char* data, DXGI_FORMAT format, s32 width, s32 height)
    //{
    //  wrl::ComPtr<ID3D12Resource> d3d_texture = internal::get()->heap->create_texture2d(format, width, height);
    //  rsl::unique_ptr<Texture2D> texture = rsl::make_unique<Texture2D>(d3d_texture, width, height, format);

    //  internal::get()->upload_buffer->write_texture(internal::get()->command_list->get(), texture.get(), data, format, width, height);
    //  
    //  return internal::get()->resource_pool.insert(rsl::move(texture));
    //}


    //void set_viewport(const Viewport& viewport)
    //{
    //  D3D12_VIEWPORT d3d_viewport;
    //  d3d_viewport.TopLeftX = viewport.top_left_x;
    //  d3d_viewport.TopLeftY = viewport.top_left_y;
    //  d3d_viewport.Width    = viewport.width;
    //  d3d_viewport.Height   = viewport.height;
    //  d3d_viewport.MinDepth = viewport.min_depth;
    //  d3d_viewport.MaxDepth = viewport.max_depth;

    //  internal::get()->command_list->get()->RSSetViewports(1, &d3d_viewport);    
    //}
    //void set_scissor_rect(const ScissorRect& rect)
    //{
    //  RECT scissor_rect{};
    //  scissor_rect.top    = (LONG)rect.top;
    //  scissor_rect.left   = (LONG)rect.left;
    //  scissor_rect.bottom = (LONG)rect.bottom;
    //  scissor_rect.right  = (LONG)rect.right;

    //  internal::get()->command_list->get()->RSSetScissorRects(1, &scissor_rect);
    //}

    //void transition_backbuffer(D3D12_RESOURCE_STATES state)
    //{
    //  internal::get()->swapchain->transition_backbuffer(internal::get()->command_list->get(), state);
    //}
    //void clear_backbuffer(const ResourceSlot& clearState)
    //{
    //  ClearStateResource* clear_state = internal::get()->resource_pool.as<ClearStateResource>(clearState);
    //  auto& clear_flags = clear_state->get()->flags;
    //  if (clear_flags.has_state(renderer::ClearBits::ClearColorBuffer))
    //  {
    //    DescriptorHandle rtv = internal::get()->swapchain->backbuffer_view();
    //    internal::get()->command_list->get()->ClearRenderTargetView(rtv.get(), clear_state->get()->rgba.data(), 0, nullptr);
    //  }

    //  if (clear_flags.has_state(renderer::ClearBits::ClearDepthBuffer) || clear_flags.has_state(renderer::ClearBits::ClearStencilBuffer))
    //  {
    //    s32 d3d_clear_flags = 0;
    //    d3d_clear_flags |= clear_flags.has_state(renderer::ClearBits::ClearDepthBuffer) ? D3D12_CLEAR_FLAG_DEPTH : 0;
    //    d3d_clear_flags |= clear_flags.has_state(renderer::ClearBits::ClearStencilBuffer) ? D3D12_CLEAR_FLAG_STENCIL : 0;

    //    //DescriptorHandle dsv = internal::get()->swapchain->depth_stencil_view();
    //    //internal::get()->command_list->get()->ClearDepthStencilView(dsv.get(), (D3D12_CLEAR_FLAGS)d3d_clear_flags, clear_state->get()->depth, clear_state->get()->stencil, 0, nullptr);
    //  }
    //}
    //void present()
    //{
    //  internal::get()->swapchain->present();
    //}

    //DescriptorHandle create_texture2d_srv(const ResourceSlot& textureSlot)
    //{
    //  DescriptorHeap* desc_heap = &internal::get()->descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    //  return create_texture2d_srv(desc_heap, textureSlot);
    //}
    //DescriptorHandle create_texture2d_srv(DescriptorHeap* descriptorHeap, const ResourceSlot& textureSlot)
    //{
    //  Texture2D* texture = internal::get()->resource_pool.as<Texture2D>(textureSlot);
    //  return descriptorHeap->create_texture2d_srv(texture->get());
    //}

    //void set_vertex_buffer(const rhi::ResourceSlot& vb, struct ID3D12GraphicsCommandList* cmdList)
    //{
    //  if (!cmdList)
    //  {
    //    cmdList = internal::get()->command_list->get();
    //  }
    //  D3D12_VERTEX_BUFFER_VIEW view{};
    //  VertexBuffer* vertex_buffer = internal::get()->resource_pool.as<VertexBuffer>(vb);
    //  view.BufferLocation = vertex_buffer->get()->GetGPUVirtualAddress();
    //  view.SizeInBytes = narrow_cast<s32>(vertex_buffer->size());
    //  view.StrideInBytes = vertex_buffer->stride();
    //  
    //  cmdList->IASetVertexBuffers(0, 1, &view);
    //}
    //void set_index_buffer(const rhi::ResourceSlot& ib, struct ID3D12GraphicsCommandList* cmdList)
    //{
    //  if (!cmdList)
    //  {
    //    cmdList = internal::get()->command_list->get();
    //  }
    //  D3D12_INDEX_BUFFER_VIEW view{};
    //  IndexBuffer* index_buffer = internal::get()->resource_pool.as<IndexBuffer>(ib);
    //  view.BufferLocation = index_buffer->get()->GetGPUVirtualAddress();
    //  view.Format = index_buffer->format();
    //  view.SizeInBytes = narrow_cast<s32>(index_buffer->size());

    //  cmdList->IASetIndexBuffer(&view);
    //}
    //void set_constant_buffer(s32 idx, const rhi::ResourceSlot& cb, struct ID3D12GraphicsCommandList* cmdList)
    //{
    //  ConstantBuffer* constant_buffer = internal::get()->resource_pool.as<ConstantBuffer>(cb);

    //  static auto cbv = internal::get()->descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).gpu_heap_start();

    //  if (!cmdList)
    //  {
    //    cmdList = internal::get()->command_list->get();
    //  }
    //  cmdList->SetGraphicsRootConstantBufferView(idx, constant_buffer->get()->GetGPUVirtualAddress());
    //}
    //void set_primitive_topology(renderer::PrimitiveTopology topology)
    //{
    //  auto d3d_topology = rex::d3d::to_dx12(topology);
    //  internal::get()->command_list->get()->IASetPrimitiveTopology(d3d_topology);
    //}


    //void set_shader(const rhi::ResourceSlot& slot)
    //{
    //  auto* shader_program = internal::get()->resource_pool.as<ShaderProgramResource>(slot);

    //  internal::get()->command_list->get()->SetGraphicsRootSignature(shader_program->get()->root_signature.Get());
    //}
    //void set_pso(const rhi::ResourceSlot& slot)
    //{
    //  auto* pso = internal::get()->resource_pool.as<PipelineState>(slot);
    //  internal::get()->command_list->get()->SetPipelineState(pso->get());
    //}

    //void bind_backbuffer_rendertarget()
    //{
    //  auto rtv = internal::get()->swapchain->backbuffer_view();
    //  //auto dsv = internal::get()->swapchain->depth_stencil_view();
    //  internal::get()->command_list->get()->OMSetRenderTargets(1, &rtv.get(), true, nullptr);
    //}


    //void set_viewport(CommandList* cmdList, const Viewport& viewport)
    //{
    //  D3D12_VIEWPORT d3d_viewport;
    //  d3d_viewport.TopLeftX = viewport.top_left_x;
    //  d3d_viewport.TopLeftY = viewport.top_left_y;
    //  d3d_viewport.Width = viewport.width;
    //  d3d_viewport.Height = viewport.height;
    //  d3d_viewport.MinDepth = viewport.min_depth;
    //  d3d_viewport.MaxDepth = viewport.max_depth;

    //  cmdList->get()->RSSetViewports(1, &d3d_viewport);
    //}
    //void set_scissor_rect(CommandList* cmdList, const ScissorRect& rect)
    //{
    //  RECT scissor_rect{};
    //  scissor_rect.top = (LONG)rect.top;
    //  scissor_rect.left = (LONG)rect.left;
    //  scissor_rect.bottom = (LONG)rect.bottom;
    //  scissor_rect.right = (LONG)rect.right;

    //  cmdList->get()->RSSetScissorRects(1, &scissor_rect);
    //}

    //void transition_backbuffer(CommandList* cmdList, D3D12_RESOURCE_STATES state)
    //{
    //  internal::get()->swapchain->transition_backbuffer(cmdList->get(), state);
    //}
    //void clear_backbuffer(CommandList* cmdList, const ResourceSlot& clearState)
    //{
    //  ClearStateResource* clear_state = internal::get()->resource_pool.as<ClearStateResource>(clearState);
    //  auto& clear_flags = clear_state->get()->flags;
    //  if (clear_flags.has_state(renderer::ClearBits::ClearColorBuffer))
    //  {
    //    DescriptorHandle rtv = internal::get()->swapchain->backbuffer_view();
    //    cmdList->get()->ClearRenderTargetView(rtv.get(), clear_state->get()->rgba.data(), 0, nullptr);
    //  }

    //  if (clear_flags.has_state(renderer::ClearBits::ClearDepthBuffer) || clear_flags.has_state(renderer::ClearBits::ClearStencilBuffer))
    //  {
    //    s32 d3d_clear_flags = 0;
    //    d3d_clear_flags |= clear_flags.has_state(renderer::ClearBits::ClearDepthBuffer) ? D3D12_CLEAR_FLAG_DEPTH : 0;
    //    d3d_clear_flags |= clear_flags.has_state(renderer::ClearBits::ClearStencilBuffer) ? D3D12_CLEAR_FLAG_STENCIL : 0;

    //    DescriptorHandle dsv = internal::get()->swapchain->depth_stencil_view();
    //    cmdList->get()->ClearDepthStencilView(dsv.get(), (D3D12_CLEAR_FLAGS)d3d_clear_flags, clear_state->get()->depth, clear_state->get()->stencil, 0, nullptr);
    //  }
    //}

    //void set_vertex_buffer(CommandList* cmdList, const ResourceSlot& vb)
    //{
    //  D3D12_VERTEX_BUFFER_VIEW view{};
    //  VertexBuffer* vertex_buffer = internal::get()->resource_pool.as<VertexBuffer>(vb);
    //  view.BufferLocation = vertex_buffer->get()->GetGPUVirtualAddress();
    //  view.SizeInBytes = narrow_cast<s32>(vertex_buffer->size());
    //  view.StrideInBytes = vertex_buffer->stride();

    //  cmdList->get()->IASetVertexBuffers(0, 1, &view);
    //}
    //void set_index_buffer(CommandList* cmdList, const ResourceSlot& ib)
    //{
    //  D3D12_INDEX_BUFFER_VIEW view{};
    //  IndexBuffer* index_buffer = internal::get()->resource_pool.as<IndexBuffer>(ib);
    //  view.BufferLocation = index_buffer->get()->GetGPUVirtualAddress();
    //  view.Format = index_buffer->format();
    //  view.SizeInBytes = narrow_cast<s32>(index_buffer->size());

    //  cmdList->get()->IASetIndexBuffer(&view);
    //}
    //void set_constant_buffer(CommandList* cmdList, s32 idx, const ResourceSlot& cb)
    //{
    //  ConstantBuffer* constant_buffer = internal::get()->resource_pool.as<ConstantBuffer>(cb);

    //  static auto cbv = internal::get()->descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).gpu_heap_start();

    //  cmdList->get()->SetGraphicsRootConstantBufferView(idx, constant_buffer->get()->GetGPUVirtualAddress());
    //}
    //void set_primitive_topology(CommandList* cmdList, renderer::PrimitiveTopology topology)
    //{
    //  auto d3d_topology = rex::d3d::to_dx12(topology);
    //  cmdList->get()->IASetPrimitiveTopology(d3d_topology);
    //}

    //void draw_indexed(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertexLoc)
    //{
    //  internal::get()->command_list->get()->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertexLoc, startInstance);
    //}
    //void draw_indexed_instanced(CommandList* cmdList, s32 indexCountPerInstance, s32 instanceCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation)
    //{
    //  cmdList->get()->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
    //}
    //void set_shader(CommandList* cmdList, const ResourceSlot& slot)
    //{
    //  auto* shader_program = internal::get()->resource_pool.as<ShaderProgramResource>(slot);
    //  cmdList->get()->SetGraphicsRootSignature(shader_program->get()->root_signature.Get());
    //}
    //void set_pso(CommandList* cmdList, const ResourceSlot& slot)
    //{
    //  auto* pso = internal::get()->resource_pool.as<PipelineState>(slot);
    //  cmdList->get()->SetPipelineState(pso->get());
    //}
    //void set_blend_factor(CommandList* cmdList, const f32 blendFactor[4])
    //{
    //  cmdList->get()->OMSetBlendFactor(blendFactor);
    //}

    //void reset_upload_buffer()
    //{
    //  internal::get()->upload_buffer->reset();
    //}

    //void update_buffer(const ResourceSlot& slot, const void* data, s64 size, ID3D12GraphicsCommandList* cmdList, s32 offset)
    //{
    //  Resource* resource = internal::get()->resource_pool.as<Resource>(slot);
    //  if (!cmdList)
    //  {
    //    cmdList = internal::get()->command_list->get();
    //  }
    //  internal::get()->upload_buffer->write(cmdList, resource, data, size, 1, offset);
    //}

    //namespace d3d
    //{
    //  wrl::ComPtr<ID3D12RootSignature> create_shader_root_signature(const rsl::unique_array<ShaderParameterLayoutDescription>& constants, const rsl::unique_array<rhi::DescriptorTableDescription>& tables, const rsl::unique_array<rhi::ShaderSamplerDescription>& samplers)
    //  {
    //    // Root parameter can be a table, root descriptor or root constants.
    //    auto root_parameters = rsl::vector<CD3DX12_ROOT_PARAMETER>(rsl::Capacity(constants.count()));

    //    for (s32 i = 0; i < constants.count(); ++i)
    //    {
    //      const auto& param = constants[i];

    //      D3D12_SHADER_VISIBILITY visibility = rex::d3d::to_dx12(param.visibility);
    //      switch (param.type)
    //      {
    //      case ShaderParameterType::CBuffer: root_parameters.emplace_back().InitAsConstantBufferView(param.reg, param.space, visibility); break;
    //      case ShaderParameterType::Constant: root_parameters.emplace_back().InitAsConstants(param.num_32bits_for_constant, param.reg, param.space, visibility); break;
    //      case ShaderParameterType::ShaderResourceView: root_parameters.emplace_back().InitAsShaderResourceView(param.reg, param.space, visibility); break;
    //      case ShaderParameterType::UnorderedAccessView: root_parameters.emplace_back().InitAsUnorderedAccessView(param.reg, param.space, visibility); break;
    //      }
    //    }

    //    rsl::vector<D3D12_DESCRIPTOR_RANGE> ranges;
    //    for (s32 i = 0; i < tables.count(); ++i)
    //    {
    //      const auto& table = tables[i];
    //      D3D12_SHADER_VISIBILITY visibility = rex::d3d::to_dx12(table.visibility);
    //      for (s32 range_idx = 0; range_idx < table.ranges.count(); ++range_idx)
    //      {
    //        ranges.push_back(rex::d3d::to_dx12(table.ranges[range_idx]));
    //      }

    //      root_parameters.emplace_back().InitAsDescriptorTable(ranges.size(), ranges.data(), visibility);
    //    }

    //    auto root_samplers = rsl::vector<D3D12_STATIC_SAMPLER_DESC>(rsl::Capacity(samplers.count()));

    //    for (s32 i = 0; i < samplers.count(); ++i)
    //    {
    //      const auto& sampler = samplers[i];

    //      D3D12_STATIC_SAMPLER_DESC desc{};
    //      desc.Filter = rex::d3d::to_dx12(sampler.filtering);
    //      desc.AddressU = rex::d3d::to_dx12(sampler.address_mode_u);
    //      desc.AddressV = rex::d3d::to_dx12(sampler.address_mode_v);
    //      desc.AddressW = rex::d3d::to_dx12(sampler.address_mode_w);
    //      desc.MipLODBias =sampler.mip_lod_bias;
    //      desc.MaxAnisotropy = sampler.max_anisotropy;
    //      desc.ComparisonFunc = rex::d3d::to_dx12(sampler.comparison_func);
    //      desc.BorderColor = rex::d3d::to_dx12(sampler.border_color);
    //      desc.MinLOD = sampler.min_lod;
    //      desc.MaxLOD = sampler.max_lod;
    //      desc.ShaderRegister = sampler.shader_register;
    //      desc.RegisterSpace = sampler.register_space;
    //      desc.ShaderVisibility = rex::d3d::to_dx12(sampler.shader_visibility);
    //      root_samplers.emplace_back(desc);
    //    }

    //    // A root signature is an array of root parameters.
    //    CD3DX12_ROOT_SIGNATURE_DESC root_sig_desc(
    //      root_parameters.size(),
    //      root_parameters.data(),
    //      root_samplers.size(),
    //      root_samplers.data(),
    //      D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    //    // Create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    //    wrl::ComPtr<ID3DBlob> serialized_root_sig = nullptr;
    //    wrl::ComPtr<ID3DBlob> error_blob = nullptr;

    //    HRESULT hr = D3D12SerializeRootSignature(&root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1, serialized_root_sig.GetAddressOf(), error_blob.GetAddressOf());
    //    if (error_blob != nullptr)
    //    {
    //      REX_ERROR(LogDirectX, "{}", (char*)error_blob->GetBufferPointer());
    //      return nullptr;
    //    }

    //    if (DX_FAILED(hr))
    //    {
    //      REX_ERROR(LogDirectX, "Failed to serialize root signature");
    //      return nullptr;
    //    }

    //    wrl::ComPtr<ID3D12RootSignature> root_signature;
    //    if (DX_FAILED(g_rhi_resources->device->get()->CreateRootSignature(0, serialized_root_sig->GetBufferPointer(), serialized_root_sig->GetBufferSize(), IID_PPV_ARGS(&root_signature))))
    //    {
    //      REX_ERROR(LogDirectX, "Failed to create root signature");
    //      return nullptr;
    //    }

    //    rhi::set_debug_name_for(root_signature.Get(), "Root Signature");

    //    return root_signature;
    //  }
    //}

    //Texture2D* get_texture(const ResourceSlot& slot)
    //{
    //  return internal::get()->resource_pool.as<Texture2D>(slot);
    //}


#pragma region RHI Class
//    internal::RenderHardwareInfrastructure::RenderHardwareInfrastructure(const renderer::OutputWindowUserData& userData, s32 maxFramesInFlight)
//      : init_successful(true)
//    {
//      // Create a scopeguard that automatically marks initialization as failed
//      // This is to make it easy to extend initialization where needed with only
//      // needing 1 place where we mark initialization as successful.
//      rsl::scopeguard mark_init_failed = [this]() { init_successful = false; };
//
//      // 1) we need to init the dxgi factory.
//      // This is the system we use to create most other systems.
//      if (!init_dxgi_factory())
//      {
//        REX_ERROR(LogDxRhi, "Failed to create DXGI Factory");
//        return;
//      }
//
//#ifdef REX_ENABLE_DX12_DEBUG_LAYER
//      // 1.1) Create the debug controller before the device gets created
//      // This way we can have some additional debugging information if something goes wrong
//      init_debug_controller();
//#endif
//
//      // 2) we need to init the device.
//      // A DirectX Device is used for creation of other resources
//      // You can think of it as an abstraction of the GPU,
//      // but just an abstraction over resource creation,
//      // not an abstraction of the gpu itself, that's what an IDXGIAdapter is for.
//      if (!init_d3d_device())
//      {
//        REX_ERROR(LogDxRhi, "Failed to create D3D Device");
//        return;
//      }
//
//#ifdef REX_ENABLE_DX12_DEBUG_LAYER
//      // 2.1) Create the debug layer aftet device gets created
//      // The debug layer is linked to the device and can therefore
//      // only get created when the device is created
//      init_debug_layer();
//#endif
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//      // Everything below this line should be removed
//      // --------------------------------------------
//
//
//
//
//
//      // 3) we create the command queue.
//      // The command queue is used to send commands to the gpu
//      // It also holds the fence for syncronysation between cpu and gpu
//      //if (!init_command_queues())
//      //{
//      //  REX_ERROR(LogDxRhi, "Failed to create command queue");
//      //  return;
//      //}
//
//      // 4) Create the global command list
//      // A gpu works with commands, that get added to a command list.
//      // command lists themselves are not executable, but need to be added
//      // to a command queue, which in turn executes the commands.
//      //if (!init_command_list())
//      //{
//      //  REX_ERROR(LogDxRhi, "Failed to create command list");
//      //  return;
//      //}
//
//      // 5) We need to create initial heaps that store the data that'll be used for rendering
//      // These heaps only hold the data, but letting the graphics pipeline know which resource to use
//      // is never done with the heaps directly, but instead are only used to store the data
//      // To let graphics pipeline know which resources to use, we use descriptors, which are stored
//      // in their own heap
//      //if (!init_resource_heaps())
//      //{
//      //  REX_ERROR(LogDxRhi, "Failed to create resource heaps");
//      //  return;
//      //}
//
//      // 6) We need a few heaps to store descriptors which point to the actual resources in other heaps
//      // A descriptor is just some metadata about the resource, holding all the information
//      // the gpu needs to use a resource.
//      // A heap on the gpu is just the same as on the cpu except it's more limited to what it can store
//      // You don't get random access into it, a heap can only store specific types and you access into the heap by offset
//      // Where each offset is the multiplied by the size of the type the heap stores.
//      // The heap doesn't store the content of a resource but stores the view to the resource.
//      //if (!init_descriptor_heaps())
//      //{
//      //  REX_ERROR(LogDxRhi, "Failed to create descriptor heaps");
//      //  return;
//      //}
//
//      // 7) we need to create a swapchain which is responsible of presenting.
//      // There's no benefit in creating all the above systems if we don't have anything
//      // to actually present something on screen, that's what the swapchain is for.
//      //if (!init_swapchain(userData, maxFramesInFlight))
//      //{
//      //  REX_ERROR(LogDxRhi, "Failed to create swapchain");
//      //  return;
//      //}
//
//      // 8) Create the pipeline library
//      // This holds all the pso objects we need.
//      // Ideally this should get prefilled on boot.
//      // However on the very first boot time, all pso need to get compiled first
//      //if (!init_pipeline_library())
//      //{
//      //  REX_ERROR(LogDxRhi, "Failed to create pipeline library");
//      //  return;
//      //}
//
//      // 9) Create the upload buffers
//      // To make sure we can upload data to the gpu
//      // we need to make sure the upload buffers 
//      // are create which perform this upload for us
//      //if (!init_upload_buffers())
//      //{
//      //  REX_ERROR(LogDxRhi, "Failed to create upload buffers");
//      //  return;
//      //}
//
//      // 10) Execute the commandlist to finish initialization
//      //command_list->stop_recording_commands();
//      //ID3D12CommandList* cmdlist = command_list->get();
//      //command_queue->get()->ExecuteCommandLists(1, &cmdlist);
//
//      // release scopeguard so that init gets marked successful
//      mark_init_failed.release();
//    }
//    internal::RenderHardwareInfrastructure::~RenderHardwareInfrastructure()
//    {
//      if (device)
//      {
//        // A command queue needs to be flushed to shutdown properly
//        //command_queue->flush();
//
//        // Now that all gpu commands are cleared, we can release all resources
//      }
//    }
//
//    // DXGI Factory
//    s32 internal::RenderHardwareInfrastructure::init_debug_interface()
//    {
//      /*
//      * Bug in the DXGI Debug Layer interaction with the DX12 Debug Layer w/ Windows 11.
//      * There's a simple workaround which is to suppress D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE.
//      * The bug itself will be fixed in a future Windows update.
//      *
//      * The Debug Layer has always had quirks when it comes to dealing with 'hybrid graphics' systems
//      * (i.e. laptops with both Intel Integrated and discrete GPUs)
//      *
//      * https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
//      * https://github.com/walbourn/directx-vs-templates/commit/18e3eaa444e98ba75d37d506ab18df8db0b82441
//      */
//      s32 dxgi_factory_flags = 0;
//
//      if (DX_SUCCESS(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug_info_queue.GetAddressOf()))))
//      {
//        dxgi_factory_flags = DXGI_CREATE_FACTORY_DEBUG;
//
//        debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE, globals::g_enable_dxgi_severity_message);
//        debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO, globals::g_enable_dxgi_severity_info);
//        debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, globals::g_enable_dxgi_severity_warning);
//        debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, globals::g_enable_dxgi_severity_error);
//        debug_info_queue->SetBreakOnSeverity(DXGI_DEBUG_DXGI, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, globals::g_enable_dxgi_severity_corruption);
//
//        rsl::array<DXGI_INFO_QUEUE_MESSAGE_ID, 1> dxgi_hide = {
//            80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
//        };
//        DXGI_INFO_QUEUE_FILTER dxgi_filter = {};
//        dxgi_filter.DenyList.NumIDs = rsl::safe_numeric_cast<u32>(dxgi_hide.size());
//        dxgi_filter.DenyList.pIDList = dxgi_hide.data();
//
//        debug_info_queue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &dxgi_filter);
//      }
//      else
//      {
//        REX_WARN(LogDxRhi, "Unable to get GXGI Debug Interface");
//      }
//
//      return dxgi_factory_flags;
//    }
//    bool internal::RenderHardwareInfrastructure::init_dxgi_factory()
//    {
//      s32 dxgi_factory_flags = 0;
//
//#ifdef REX_ENABLE_DXGI_DEBUG_LAYER
//      dxgi_factory_flags = init_debug_interface();
//#endif
//
//      factory = dxgi::Factory::create(dxgi_factory_flags);
//      return true;
//    }
//    void internal::RenderHardwareInfrastructure::init_debug_controller()
//    {
//      // Enable extra debugging and send debug messages to the VC++ output window
//      rex::wrl::ComPtr<ID3D12Debug> debug_controller;
//      if (DX_SUCCESS(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
//      {
//        debug_controller->EnableDebugLayer();
//      }
//      else
//      {
//        REX_WARN(LogDxRhi, "Failed to create DX debug controller");
//      }
//    }
//
//    // D3D Device
//    count_t internal::RenderHardwareInfrastructure::highest_scoring_gpu(const rsl::vector<GpuDescription>& gpus)
//    {
//      auto it = rsl::max_element(gpus.cbegin(), gpus.cend(),
//        [](const GpuDescription& lhs, const GpuDescription& rhs)
//        {
//          const size_t lhs_vram = lhs.dedicated_video_memory.size_in_bytes();
//          const size_t rhs_vram = rhs.dedicated_video_memory.size_in_bytes();
//
//          return rhs_vram > lhs_vram;
//        });
//
//      return it != gpus.cend() ? rsl::distance(gpus.cbegin(), it) : -1;
//    }
//    void internal::RenderHardwareInfrastructure::init_debug_layer()
//    {
//      // Device needs to exist before we can query this
//      rex::wrl::ComPtr<ID3D12InfoQueue> dx12_info_queue;
//      if (DX_SUCCESS(device->get()->QueryInterface(IID_PPV_ARGS(dx12_info_queue.GetAddressOf()))))
//      {
//        dx12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_MESSAGE, globals::g_enable_dx12_severity_message);
//        dx12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, globals::g_enable_dx12_severity_info);
//        dx12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, globals::g_enable_dx12_severity_warning);
//        dx12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, globals::g_enable_dx12_severity_error);
//        dx12_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, globals::g_enable_dx12_severity_corruption);
//
//        /*
//         * Bug in the DX12 Debug Layer interaction with the DX12 Debug Layer w/ Windows 11.
//         * There's a simple workaround which is to suppress D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE.
//         * The bug itself will be fixed in a future Windows update.
//         *
//         * The Debug Layer has always had quirks when it comes to dealing with 'hybrid graphics' systems
//         * (i.e. laptops with both Intel Integrated and discrete GPUs)
//         *
//         * https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
//         * https://github.com/walbourn/directx-vs-templates/commit/2b34dcf9ac764153699058cdc9d4dbc4e837224c
//         */
//        rsl::array<D3D12_MESSAGE_ID, 7> dx12_hide = { D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
//                           D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
//                           D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
//                           D3D12_MESSAGE_ID_RESOURCE_BARRIER_BEFORE_AFTER_MISMATCH,
//                           D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
//                           D3D12_MESSAGE_ID_COMMAND_LIST_DRAW_VERTEX_BUFFER_NOT_SET,
//                           D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE };
//
//        D3D12_INFO_QUEUE_FILTER dx12_filter = {};
//        dx12_filter.DenyList.NumIDs = rsl::safe_numeric_cast<u32>(dx12_hide.size());
//        dx12_filter.DenyList.pIDList = dx12_hide.data();
//        dx12_info_queue->AddStorageFilterEntries(&dx12_filter);
//      }
//      else
//      {
//        REX_WARN(LogDxRhi, "Unable to get D3D12 Debug Interface");
//      }
//    }
//
//    bool internal::RenderHardwareInfrastructure::init_d3d_device()
//    {
//      // Find highest scoring gpu
//      REX_STATIC_WARNING("Give callbacks for the adapter manager to score a gpu, that way we can found the lowest, higher, medium, ...");
//      const dxgi::AdapterManager adapter_manager(factory.get(), [this](const rsl::vector<GpuDescription>& gpus) { return highest_scoring_gpu(gpus); });
//      const dxgi::Adapter* selected_gpu = adapter_manager.selected();
//      IDXGIAdapter* adapter = selected_gpu->c_ptr();
//
//      // Create device
//      const D3D_FEATURE_LEVEL feature_level = query_feature_level(adapter);
//
//      wrl::ComPtr<ID3D12Device1> d3d_device;
//      if (DX_FAILED(D3D12CreateDevice(adapter, static_cast<D3D_FEATURE_LEVEL>(feature_level), IID_PPV_ARGS(&d3d_device))))
//      {
//        REX_ERROR(LogDxRhi, "Software adapter not supported");
//        REX_ERROR(LogDxRhi, "Failed to create DX12 Device");
//        return false;
//      }
//
//      device = rsl::make_unique<DxDevice>(d3d_device, feature_level, selected_gpu);
//      return true;
//    }
//
//    // D3D Command Objects
//    bool internal::RenderHardwareInfrastructure::init_command_queue(CommandType type)
//    {
//      // Command Queue
//      wrl::ComPtr<ID3D12CommandQueue> d3d_command_queue;
//      D3D12_COMMAND_QUEUE_DESC queue_desc = {};
//      queue_desc.Type = d3d::to_dx12(type);
//      queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//      if (DX_FAILED(device->get()->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(d3d_command_queue.GetAddressOf()))))
//      {
//        REX_ERROR(LogDxRhi, "Failed to create command queue");
//        return false;
//      }
//
//      rsl::string_view type_str = rsl::enum_refl::enum_name(type);
//      rhi::set_debug_name_for(d3d_command_queue.Get(), rsl::format("Global Command Queue For {}", type_str));
//
//      // Fence
//      wrl::ComPtr<ID3D12Fence> fence;
//      if (DX_FAILED(device->get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
//      {
//        REX_ERROR(LogDxRhi, "Failed to create DX fence, to synchronize CPU/GPU");
//        return false;
//      }
//
//      rhi::set_debug_name_for(fence.Get(), "Global Fence");
//      command_queue_pool.emplace(type, rsl::make_unique<CommandQueue>(d3d_command_queue, fence));
//
//      return true;
//    }
//    bool internal::RenderHardwareInfrastructure::init_command_queues()
//    {
//      bool result = true;
//
//      result &= init_command_queue(CommandType::Render);
//      result &= init_command_queue(CommandType::Copy);
//      result &= init_command_queue(CommandType::Compute);
//
//      return result;
//    }
//
//    bool internal::RenderHardwareInfrastructure::init_command_list()
//    {
//      // Command Allocator
//      wrl::ComPtr<ID3D12CommandAllocator> allocator;
//      if (DX_FAILED(device->get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocator.GetAddressOf()))))
//      {
//        REX_ERROR(LogDxRhi, "Failed to create command allocator");
//        return false;
//      }
//      rhi::set_debug_name_for(allocator.Get(), "Global Command Allocator");
//      command_allocator = rsl::make_unique<rex::rhi::CommandAllocator>(allocator);
//
//      // Command List
//      //wrl::ComPtr<ID3D12GraphicsCommandList> cmd_list;
//      //if (DX_FAILED(device->get()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.Get(), nullptr, IID_PPV_ARGS(cmd_list.GetAddressOf()))))
//      //{
//      //  REX_ERROR(LogDxRhi, "Failed to create command list");
//      //  return false;
//      //}
//
//      //rhi::set_debug_name_for(cmd_list.Get(), "Global Command List");
//      //command_list = rsl::make_unique<CommandList>(cmd_list);
//
//      //// Open the command list to allow further initialization of resources
//      //command_list->start_recording_commands(command_allocator.get());
//
//      return true;
//    }
//
//    // Resource Heaps
//    bool internal::RenderHardwareInfrastructure::init_resource_heaps()
//    {
//      // Nothing to implement at the moment
//      CD3DX12_HEAP_DESC desc(100_mib, D3D12_HEAP_TYPE_DEFAULT);
//
//      wrl::ComPtr<ID3D12Heap> d3d_heap;
//      if (DX_FAILED(device->get()->CreateHeap(&desc, IID_PPV_ARGS(&d3d_heap))))
//      {
//        REX_ERROR(LogDxRhi, "Failed to create global resource heap");
//        return false;
//      }
//
//      heap = rsl::make_unique<ResourceHeap>(d3d_heap, device->get());
//      return true;
//    }
//
//    // Descriptor Heaps
//    bool internal::RenderHardwareInfrastructure::init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE type, s32 numDescriptors)
//    {
//      D3D12_DESCRIPTOR_HEAP_DESC desc{};
//
//      desc.Type = type;
//      desc.NumDescriptors = numDescriptors;
//      desc.Flags = type != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
//        ? D3D12_DESCRIPTOR_HEAP_FLAG_NONE
//        : D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//      desc.NodeMask = 0; // For single-adapter operation, set this to zero. ( https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_descriptor_heap_desc )
//
//      wrl::ComPtr<ID3D12DescriptorHeap> desc_heap;
//      rsl::string_view type_str = rsl::enum_refl::enum_name(type);
//      if (DX_FAILED(device->get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&desc_heap))))
//      {
//        REX_ERROR(LogDxRhi, "Failed to create descriptor heap for type: {}", type_str);
//        return false;
//      }
//
//      rhi::set_debug_name_for(desc_heap.Get(), rsl::format("Descriptor Heap Element - {}", type_str));
//      s32 desc_size = device->get()->GetDescriptorHandleIncrementSize(type);
//      s32 total_size = desc_size * numDescriptors;
//
//      descriptor_heap_pool.emplace(type, desc_heap, device->get());
//
//      REX_INFO(LogDxRhi, "Created {0} ( num: {1} descriptors, desc size: {2} bytes, total size: {3} bytes) ", type_str, numDescriptors, desc_size, total_size);
//
//      return true;
//    }
//    bool internal::RenderHardwareInfrastructure::init_descriptor_heaps()
//    {
//      if (!init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, s_num_rtv_descs))
//      {
//        REX_ERROR(LogDxRhi, "Failed to create descriptor heap for RTV");
//        return false;
//      }
//      if (!init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, s_num_dsv_descs))
//      {
//        REX_ERROR(LogDxRhi, "Failed to create descriptor heap for RTV");
//        return false;
//      }
//      if (!init_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, s_num_cbv_descs))
//      {
//        REX_ERROR(LogDxRhi, "Failed to create descriptor heap for RTV");
//        return false;
//      }
//
//      return true;
//    }
//
//    // Pipeline Library
//    bool internal::RenderHardwareInfrastructure::init_pipeline_library()
//    {
//      // A pipeline library is saved to disk
//      // Therefore we need to create some file mapping so the pipeline library
//      // gets stored to disk right away.
//      // At the moment we haven't implement this yet, so we're creating a fresh pipeline library
//      // every time
//      wrl::ComPtr<ID3D12PipelineLibrary> d3d_pso_lib;
//      if (DX_FAILED(device->get()->CreatePipelineLibrary(nullptr, 0, IID_PPV_ARGS(&d3d_pso_lib))))
//      {
//        REX_ERROR(LogDxRhi, "Failed to create pipeline library. Is this supported by your OS and driver version?");
//        return false;
//      }
//
//      pso_lib = rsl::make_unique<PipelineLibrary>(d3d_pso_lib, device->get());
//      return true;
//    }
//
//    // Upload buffers
//    bool internal::RenderHardwareInfrastructure::init_upload_buffers()
//    {
//      // an intermediate upload heap.
//      CD3DX12_HEAP_PROPERTIES heap_properties_upload(D3D12_HEAP_TYPE_UPLOAD);
//      auto buffer_upload = CD3DX12_RESOURCE_DESC::Buffer(100_mib);
//
//      wrl::ComPtr<ID3D12Resource> d3d_upload_buffer;
//      if (DX_FAILED(device->get()->CreateCommittedResource(&heap_properties_upload, D3D12_HEAP_FLAG_NONE, &buffer_upload, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(d3d_upload_buffer.GetAddressOf()))))
//      {
//        REX_ERROR(LogDirectX, "Failed to create committed resource for intermediate upload heap.");
//        return {};
//      }
//
//      set_debug_name_for(d3d_upload_buffer.Get(), "Upload Buffer");
//      upload_buffer = rsl::make_unique<UploadBuffer>(d3d_upload_buffer, D3D12_RESOURCE_STATE_COMMON);
//
//      return true;
//    }


#pragma endregion
    //ID3D12Device1* get_device()
    //{
    //  return g_rhi_resources->device->get();
    //}

    //DescriptorHandle get_rtv()
    //{
    //  return internal::get()->swapchain->backbuffer_view();
    //}
  }
}