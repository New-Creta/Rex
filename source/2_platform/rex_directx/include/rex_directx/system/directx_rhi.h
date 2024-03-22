#pragma once

#include "rex_directx/dxgi/util.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/utility/directx_util.h"
#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_renderer_core/system/gpu_description.h"
#include "rex_renderer_core/system/rhi.h"
#include "rex_renderer_core/resource_management/resource_pool.h"

#include "rex_directx/system/directx_device.h"
#include "rex_directx/system/directx_debug_interface.h"
#include "rex_directx/system/directx_resource_heap.h"
#include "rex_directx/system/directx_descriptor_heap.h"
#include "rex_directx/system/directx_commandlist.h"

#include "rex_std/string_view.h"
#include "rex_engine/engine/defines.h"
#include "rex_engine/engine/types.h"

namespace rex
{
  namespace renderer
  {
    struct OutputWindowUserData;
  }

  namespace rhi
  {
    // Useful function that assigns a name to a resource.
    // This is useful for debugging
    template<typename TResourceType>
    void set_debug_name_for(TResourceType* resource, rsl::string_view name)
    {
#ifdef REX_ENABLE_DEBUG_RESOURCE_NAMES
      resource->SetPrivateData(WKPDID_D3DDebugObjectName, name.length(), name.data());
#else
      UNUSED_PARAM(resource);
      UNUSED_PARAM(name);
#endif
    }

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

      ScopedCommandList create_scoped_cmd_list();

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
      ResourcePool resource_pool;

      wrl::ComPtr<IDXGIInfoQueue> debug_info_queue;
      rsl::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, DescriptorHeap> descriptor_heap_pool;
      rsl::array<renderer::ResourceSlot, s_swapchain_buffer_count> swapchain_rt_buffer_slots;   // swapchain render target buffer indices

    };

    namespace d3d
    {
      wrl::ComPtr<ID3D12RootSignature> create_shader_root_signature(const rsl::vector<ConstantLayoutDescription>& constants);
    }
  }
}