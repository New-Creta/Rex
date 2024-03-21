#pragma once

#include "rex_directx/dxgi/util.h"
#include "rex_directx/utility/directx_util.h"
#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_renderer_core/system/gpu_description.h"

#include "rex_directx/system/directx_device.h"
#include "rex_directx/system/directx_debug_interface.h"
#include "rex_directx/system/directx_resource_heap.h"
#include "rex_directx/system/directx_descriptor_heap.h"

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

    // Initializes the render hardware infrastructure
    // Creates the dxgi factory, d3d device, command buffers, heaps and swapchain
    // After this, the rhi is setup to start creating resources (textures, shaders, vertex buffers, ..)
    class RenderHardwareInfrastructure* init(const renderer::OutputWindowUserData& userData);

    // shutdown the internal rhi, all reference to the rhi are invalidated from here on out
    void shutdown();

    class RenderHardwareInfrastructure
    {
    public:
      RenderHardwareInfrastructure(const renderer::OutputWindowUserData& userData);
      ~RenderHardwareInfrastructure();

      bool init_successful() const;

      void reset_command_list();
      void flush_command_queue();
      void exec_command_list();

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

      bool m_is_initialized;
      // Keep the debug interface as the last resource to be destroyed
      // it automatically reports if any resources are still active on destruction
      DebugInterface m_debug_interface;
      rsl::unique_ptr<dxgi::Factory> m_factory;
      rsl::unique_ptr<renderer::DirectXDevice> m_device;
      rsl::unique_ptr<renderer::Swapchain> m_swapchain;
      rsl::unique_ptr<CommandQueue> m_command_queue;
      rsl::unique_ptr<CommandList> m_command_list;
      rsl::unique_ptr<ResourceHeap> m_heap;

      wrl::ComPtr<IDXGIInfoQueue> m_debug_info_queue;
      rsl::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, DescriptorHeap> m_descriptor_heap_pool;
      rsl::array<renderer::ResourceSlot, s_swapchain_buffer_count> swapchain_rt_buffer_slots;   // swapchain render target buffer indices

    };

    rsl::unique_ptr<RenderHardwareInfrastructure> g_rhi;


    void flush_command_queue();

  }
}