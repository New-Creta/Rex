#pragma once

#include "rex_directx/dxgi/util.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_renderer_core/system/gpu_description.h"
#include "rex_renderer_core/system/rhi.h"
#include "rex_renderer_core/resource_management/resource_pool.h"

#include "rex_directx/system/dx_device.h"
#include "rex_directx/system/dx_debug_interface.h"
#include "rex_directx/system/dx_resource_heap.h"
#include "rex_directx/system/dx_descriptor_heap.h"
#include "rex_directx/system/dx_commandlist.h"
#include "rex_directx/system/dx_pipeline_library.h"
#include "rex_directx/system/dx_texture_2d.h"
#include "rex_directx/resources/dx_upload_buffer.h"

#include "rex_std/string_view.h"
#include "rex_engine/engine/defines.h"
#include "rex_engine/engine/types.h"

struct ID3D12GraphicsCommandList;

namespace rex
{
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

    void transition_backbuffer(D3D12_RESOURCE_STATES state);
    void clear_backbuffer(const ResourceSlot& clearState);
    void present();

    // Create a 2D texture on the gpu and add the data to the upload buffer
    // to be uploaded the next time the upload buffer
    ResourceSlot create_texture2d(const char* data, DXGI_FORMAT format, s32 width, s32 height);

    DescriptorHandle create_texture2d_srv(const ResourceSlot& textureSlot);

    ScopedCommandList create_scoped_commandlist();

    class CommandList2* cmd_list();

    void set_graphics_root_descriptor_table(ResourceID id);
    Texture2D* get_texture(const ResourceSlot& slot);

    ID3D12Device1* get_device();
    DescriptorHeap* get_cbv_uav_srv_heap();
    DescriptorHandle get_free_handle();

    class ShaderProgramResource* get_shader(const ResourceSlot& slot);
    class PipelineState* get_pso(const ResourceSlot& slot);
    //class VertexBuffer* get_vertex_buffer(const ResourceSlot* slot);
    //class IndexBuffer* get_index_buffer(const ResourceSlot* slot);

    DescriptorHandle get_rtv();

    void set_viewport(CommandList2* cmdList, const Viewport& viewport);
    void set_scissor_rect(CommandList2* cmdList, const ScissorRect& rect);

    void transition_backbuffer(CommandList2* cmdList, D3D12_RESOURCE_STATES state);
    void clear_backbuffer(CommandList2* cmdList, const ResourceSlot& clearState);

    void set_vertex_buffer(CommandList2* cmdList, const ResourceSlot& vb);
    void set_index_buffer(CommandList2* cmdList, const ResourceSlot& ib);
    void set_constant_buffer(CommandList2* cmdList, s32 idx, const ResourceSlot& cb);
    void set_primitive_topology(CommandList2* cmdList, renderer::PrimitiveTopology topology);

    void draw_indexed(CommandList2* cmdList, s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertexLoc);
    void set_shader(CommandList2* cmdList, const ResourceSlot& slot);
    void set_pso(CommandList2* cmdList, const ResourceSlot& slot);

    void set_render_target(CommandList2* cmdList, DescriptorHandle rtv);

    namespace d3d
    {
      wrl::ComPtr<ID3D12RootSignature> create_shader_root_signature(const rsl::unique_array<ShaderParameterLayoutDescription>& constants, const rsl::unique_array<rhi::DescriptorTableDescription>& tables, const rsl::unique_array<rhi::ShaderSamplerDescription>& samplers);
    }
  }
}