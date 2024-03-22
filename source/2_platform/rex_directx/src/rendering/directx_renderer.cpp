#include "rex_directx/d3dx12.h"
#include "rex_directx/diagnostics/directx_call.h"
#include "rex_directx/system/directx_feature_level.h"
#include "rex_directx/system/directx_feature_shader_model.h"
#include "rex_directx/system/directx_swapchain.h"
#include "rex_directx/utility/directx_util.h" // IWYU pragma: keep
#include "rex_directx/dxgi/adapter.h"
#include "rex_directx/dxgi/adapter_manager.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/system/directx_rhi.h"
#include "rex_directx/dxgi/util.h"
#include "rex_directx/diagnostics/log.h"

#include "rex_directx/resources/buffer_resource.h"
#include "rex_directx/resources/clear_state_resource.h"
#include "rex_directx/resources/committed_buffer_resource.h"
#include "rex_directx/resources/constant_buffer_view_resource.h"
#include "rex_directx/resources/depth_stencil_target_resource.h"
#include "rex_directx/resources/frame_resource.h"
#include "rex_directx/resources/input_layout_resource.h"
#include "rex_directx/resources/pipeline_state_object_hasher.h"
#include "rex_directx/resources/pipeline_state_resource.h"
#include "rex_directx/resources/pixel_shader_resource.h"
#include "rex_directx/resources/raster_state_resource.h"
#include "rex_directx/resources/render_target_resource.h"
#include "rex_directx/resources/shader_program_resource.h"
#include "rex_directx/resources/vertex_shader_resource.h"
#include "rex_directx/system/directx_device.h"
#include "rex_directx/system/directx_commandlist.h"

#include "rex_directx/utility/vertex.h"

#include "rex_engine/platform/win/win_com_ptr.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/memory/pointer_math.h"
#include "rex_engine/timing/execution_logger.h"

#include "rex_renderer_core/commands/attach_committed_resource_to_frame_cmd.h"
#include "rex_renderer_core/commands/begin_draw_cmd.h"
#include "rex_renderer_core/commands/clear_cmd.h"
#include "rex_renderer_core/commands/compile_shader_cmd.h"
#include "rex_renderer_core/commands/create_buffer_cmd.h"
#include "rex_renderer_core/commands/create_clear_state_cmd.h"
#include "rex_renderer_core/commands/create_constant_buffer_cmd.h"
#include "rex_renderer_core/commands/create_frame_resource_cmd.h"
#include "rex_renderer_core/commands/create_index_buffer_cmd.h"
#include "rex_renderer_core/commands/create_input_layout_cmd.h"
#include "rex_renderer_core/commands/create_pipeline_state_cmd.h"
#include "rex_renderer_core/commands/create_raster_state_cmd.h"
#include "rex_renderer_core/commands/create_vertex_buffer_cmd.h"
#include "rex_renderer_core/commands/draw_cmd.h"
#include "rex_renderer_core/commands/draw_indexed_cmd.h"
#include "rex_renderer_core/commands/draw_indexed_instanced_cmd.h"
#include "rex_renderer_core/commands/draw_instanced_cmd.h"
#include "rex_renderer_core/commands/end_draw_cmd.h"
#include "rex_renderer_core/commands/end_frame_cmd.h"
#include "rex_renderer_core/commands/finish_user_initialization_cmd.h"
#include "rex_renderer_core/commands/link_shader_cmd.h"
#include "rex_renderer_core/commands/load_shader_cmd.h"
#include "rex_renderer_core/commands/new_frame_cmd.h"
#include "rex_renderer_core/commands/prepare_user_initialization_cmd.h"
#include "rex_renderer_core/commands/present_cmd.h"
#include "rex_renderer_core/commands/release_resource_cmd.h"
#include "rex_renderer_core/commands/set_constant_buffer_cmd.h"
#include "rex_renderer_core/commands/set_index_buffer_cmd.h"
#include "rex_renderer_core/commands/set_input_layout_cmd.h"
#include "rex_renderer_core/commands/set_pipeline_state_cmd.h"
#include "rex_renderer_core/commands/set_primitive_topology_cmd.h"
#include "rex_renderer_core/commands/set_raster_state_cmd.h"
#include "rex_renderer_core/commands/set_render_target_cmd.h"
#include "rex_renderer_core/commands/set_scissor_rect_cmd.h"
#include "rex_renderer_core/commands/set_shader_cmd.h"
#include "rex_renderer_core/commands/set_vertex_buffer_cmd.h"
#include "rex_renderer_core/commands/set_viewport_cmd.h"
#include "rex_renderer_core/commands/update_committed_resource_cmd.h"

#include "rex_renderer_core/rendering/renderer.h"
#include "rex_renderer_core/rendering/renderer_backend.h"
#include "rex_renderer_core/rendering/cull_mode.h"
#include "rex_renderer_core/rendering/fill_mode.h"
#include "rex_renderer_core/system/gpu_description.h"
#include "rex_renderer_core/rendering/index_buffer_format.h"
#include "rex_renderer_core/rendering/input_layout_classification.h"
#include "rex_renderer_core/rendering/renderer_backend.h"
#include "rex_renderer_core/rendering/renderer_info.h"
#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"
#include "rex_renderer_core/resource_management/resource_pool.h"
#include "rex_renderer_core/rendering/scissor_rect.h"
#include "rex_renderer_core/shaders/shader_platform.h"
#include "rex_renderer_core/rendering/texture_format.h"
#include "rex_renderer_core/rendering/vertex_buffer_format.h"
#include "rex_renderer_core/rendering/viewport.h"
#include "rex_renderer_core/rendering/msaa_settings.h"

#include "rex_std/algorithm.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/bonus/platform/windows/handle.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/utility/casting.h"
#include "rex_std/bonus/utility/enum_reflection.h"
#include "rex_std/bonus/utility/scopeguard.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"

#include <optional>

#include <D3Dcompiler.h>
#include <DirectXColors.h>
#include <Windows.h>
#include <cstddef>
#include <d3d12.h>

#ifdef REX_ENABLE_DXGI_DEBUG_LAYER
  #include <dxgidebug.h>
#endif

namespace rex
{
  namespace renderer
  {
    namespace directx
    {
      Info g_renderer_info; // NOLINT (fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

      //-------------------------------------------------------------------------
      D3D12_FILL_MODE to_d3d12_fill_mode(FillMode mode)
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
      D3D12_CULL_MODE to_d3d12_cull_mode(CullMode mode)
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
      //-------------------------------------------------------------------------
      DXGI_FORMAT to_d3d12_vertex_format(VertexBufferFormat format)
      {
        switch(format)
        {
          case VertexBufferFormat::FLOAT1: return DXGI_FORMAT_R32_FLOAT;
          case VertexBufferFormat::FLOAT2: return DXGI_FORMAT_R32G32_FLOAT;
          case VertexBufferFormat::FLOAT3: return DXGI_FORMAT_R32G32B32_FLOAT;
          case VertexBufferFormat::FLOAT4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
          case VertexBufferFormat::UNORM1: return DXGI_FORMAT_R8_UNORM;
          case VertexBufferFormat::UNORM2: return DXGI_FORMAT_R8G8_UNORM;
          case VertexBufferFormat::UNORM4: return DXGI_FORMAT_R8G8B8A8_UNORM;
          default: break;
        }
        REX_ASSERT("Unsupported vertex buffer format given");
        return DXGI_FORMAT_UNKNOWN;
      }
      //-------------------------------------------------------------------------
      DXGI_FORMAT to_d3d12_index_format(IndexBufferFormat format)
      {
        switch(format)
        {
          case IndexBufferFormat::R16Uint: return DXGI_FORMAT_R16_UINT;
          case IndexBufferFormat::R32Uint: return DXGI_FORMAT_R32_UINT;
          default: break;
        }
        REX_ASSERT("Unsupported index buffer format given");
        return DXGI_FORMAT_UNKNOWN;
      }
      //-------------------------------------------------------------------------
      DXGI_FORMAT to_dxd12_texture_format(TextureFormat format)
      {
        switch(format)
        {
          case TextureFormat::Unorm4Srgb: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
          default: break;
        }
        REX_ASSERT("Unsupported vertex buffer format given");
        return DXGI_FORMAT_UNKNOWN;
      }

      //-------------------------------------------------------------------------
      D3D12_PRIMITIVE_TOPOLOGY to_d3d12_topology(PrimitiveTopology topology)
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
      D3D12_INPUT_CLASSIFICATION to_d3d12_input_layout_classification(InputLayoutClassification classification)
      {
        switch(classification)
        {
          case InputLayoutClassification::PerVertexData: return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
          case InputLayoutClassification::PerInstanceData: return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
          default: break;
        }

        REX_ASSERT("Unsupported input layout classification given");
        return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
      }

      //-------------------------------------------------------------------------


      struct DefaultBuffer
      {
          wrl::ComPtr<ID3D12Resource> buffer;
          wrl::ComPtr<ID3D12Resource> upload_buffer;
      };

    } // namespace directx

    //-------------------------------------------------------------------------
    const Info& info()
    {
      return directx::g_renderer_info;
    }
    //-------------------------------------------------------------------------
    ShaderPlatform shader_platform()
    {
      return ShaderPlatform::HLSL;
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

    namespace backend
    {
      static constexpr s32 s_swapchain_buffer_count = 2;

      static constexpr s32 s_max_color_targets = 8;
      static constexpr s32 s_max_depth_targets = 1;

      static constexpr s32 s_constant_buffer_min_allocation_size = 256;

      static constexpr s32 s_rtv_descriptor_count = s_max_color_targets;
      static constexpr s32 s_dsv_descriptor_count = s_max_depth_targets;
      static constexpr s32 s_cbv_descriptor_count = 128;

      static constexpr s32 s_num_frame_resources = 3;

      class Frame
      {
        public:
          //-------------------------------------------------------------------------
          Frame(s32 idx, ResourceSlot s)
              : m_index(idx)
              , m_slot(s)
          {
          }

        public:
          //-------------------------------------------------------------------------
          bool operator==(const Frame& other) const
          {
            return m_index == other.m_index && m_slot == other.m_slot;
          }
          //-------------------------------------------------------------------------
          bool operator!=(const Frame& other) const
          {
            return !(*this == other);
          }

        public:

          //-------------------------------------------------------------------------
          s32 index() const
          {
            return m_index;
          }

          //-------------------------------------------------------------------------
          const ResourceSlot* slot() const
          {
            return &m_slot;
          }

          //-------------------------------------------------------------------------
          void add_committed_resource(const ResourceSlot& slot)
          {          
            REX_ASSERT_X(!has_committed_resource(slot), "Duplicate committed resource added to frame");

            m_committed_resources.push_back(slot);
          }

          //-------------------------------------------------------------------------
          bool has_committed_resource(const ResourceSlot& slot) const
          {
            return rsl::find_if(rsl::cbegin(m_committed_resources), rsl::cend(m_committed_resources), [&slot](const ResourceSlot& crs) { return slot == crs; }) != rsl::cend(m_committed_resources);
          }

          //-------------------------------------------------------------------------
          void increment_amount_active_constant_buffer(const ResourceSlot& slot)
          {
            if(m_active_constant_buffers.find(slot) == rsl::cend(m_active_constant_buffers))
            {
              return;
            }

            ++m_active_constant_buffers[slot];
          }

          //-------------------------------------------------------------------------
          s32 amount_active_constant_buffer(const ResourceSlot& slot)
          {
            if(m_active_constant_buffers.find(slot) == rsl::cend(m_active_constant_buffers))
            {
              m_active_constant_buffers.emplace(slot, 0);
            }

            return m_active_constant_buffers[slot];
          }

          //-------------------------------------------------------------------------
          void clear()
          {
            m_committed_resources.clear();
          }

        private:
          s32 m_index;
          ResourceSlot m_slot;

          rsl::vector<ResourceSlot> m_committed_resources;
          rsl::unordered_map<ResourceSlot, s32> m_active_constant_buffers;
      };

      class FrameContext
      {
        public:
          //-------------------------------------------------------------------------
          FrameContext(s32 maxFrameResources)
              : m_frame_resources()
              , m_curr_frame_resource(nullptr)
              , m_curr_frame_resource_index(0)
          {
            REX_ASSERT_X(maxFrameResources > 0, "A minimum of one frame has to be created in order to render anything.");

            m_frame_resources.reserve(maxFrameResources);
          }

          //-------------------------------------------------------------------------
          void add_frame(const Frame& f)
          {
            REX_ASSERT_X(rsl::find(rsl::cbegin(m_frame_resources), rsl::cend(m_frame_resources), f) == rsl::cend(m_frame_resources), "Duplicate frame resource found");

            m_frame_resources.push_back(f);
          }

          //-------------------------------------------------------------------------
          Frame* find_frame(const ResourceSlot* slot)
          {
            auto it = rsl::find_if(rsl::begin(m_frame_resources), rsl::end(m_frame_resources),
                [&slot](const Frame& f)
                {
                    return *f.slot() == *slot;
                });

            return it != rsl::end(m_frame_resources) 
                ? &(*it) 
                : nullptr;
          }

          //-------------------------------------------------------------------------
          Frame* find_frame(s32 idx)
          {
            REX_ASSERT_X(idx >= 0, "Invalid frame idx given: {}", idx);
            REX_ASSERT_X(idx < m_frame_resources.size(), "Only {0} frame resources are allocated when trying to retrieve at idx: {1}", max_frame_resources_count(), idx);

            return &m_frame_resources[idx];
          }

          //-------------------------------------------------------------------------
          void next_frame()
          {
            REX_ASSERT_X(m_frame_resources.empty() == false, "No frame resources registered");

            m_curr_frame_resource_index = (m_curr_frame_resource_index + 1) % m_frame_resources.size();
            m_curr_frame_resource       = m_frame_resources[m_curr_frame_resource_index].slot();
          }

          //-------------------------------------------------------------------------
          void set_current_frame(const ResourceSlot* slot)
          {
            m_curr_frame_resource = slot;
          }

          //-------------------------------------------------------------------------
          void clear()
          {
            m_frame_resources.clear();
          }

          //-------------------------------------------------------------------------
          s32 frame_resources_count() const
          {
            return m_frame_resources.size();
          }

          //-------------------------------------------------------------------------
          s32 max_frame_resources_count() const
          {
            return m_frame_resources.capacity();
          }

          //-------------------------------------------------------------------------
          const rsl::vector<Frame>& frame_resources() const
          {
            return m_frame_resources;
          }

          //-------------------------------------------------------------------------
          const ResourceSlot* current_frame_resource() const
          {
            return m_curr_frame_resource;
          }

          //-------------------------------------------------------------------------
          s32 current_frame_resource_index() const
          {
            return m_curr_frame_resource_index;
          }

        private:
          rsl::vector<Frame> m_frame_resources;
          const ResourceSlot* m_curr_frame_resource;
          s32 m_curr_frame_resource_index;
      };

      struct DirectXContext
      {
      public:

        rsl::unordered_map<PipelineStateObjectHashData, wrl::ComPtr<ID3D12PipelineState>> pipeline_state_objects;

        ResourcePool resource_pool;

        s32 active_color_targets = 0;                                       // Amount of color targets to write to
        s32 active_constant_buffers = 0;                                    // Available constant buffers

        ResourceSlot active_depth_target;                                   // Current depth buffer to write to
        ResourceSlot active_color_target[s_max_color_targets];              // Current color buffers to write to
        ResourceSlot active_pipeline_state_object;                          // Active pipeline state used for drawing
        ResourceSlot active_shader_program;                                 // Active shader program

        ResourceSlot swapchain_rt_buffer_slots[s_swapchain_buffer_count];   // swapchain render target buffer indices
        ResourceSlot swapchain_ds_buffer_slot;                              // swapchain depth stencil index

        ResourceSlot clear_state;


        bool dx12_debug_layer_enabled = false;
        bool dxgi_debug_layer_enabled = false;
      };

      rsl::unique_ptr<DirectXContext> g_ctx; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

      namespace internal
      {
        //-------------------------------------------------------------------------
        bool has_committed_resource_for_frame(const ResourceSlot* frameSlot, const ResourceSlot& committedResourceSlot)
        {
            const Frame* frame = g_ctx->frame_ctx.find_frame(frameSlot);
            if (frame == nullptr)
            {
                REX_WARN(LogDirectX, "Unable to find frame with slot index: {}", frameSlot->slot_id());
                return false;
            }

            return frame->has_committed_resource(committedResourceSlot);
        }

        //-------------------------------------------------------------------------
        rsl::optional<directx::DefaultBuffer> create_default_buffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT64 byteSize)
        {
          directx::DefaultBuffer default_buffer;

          // Create the actual default buffer resource.
          CD3DX12_HEAP_PROPERTIES heap_properties_default(D3D12_HEAP_TYPE_DEFAULT);
          auto buffer_default = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

          if(DX_FAILED(device->CreateCommittedResource(&heap_properties_default, D3D12_HEAP_FLAG_NONE, &buffer_default, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(default_buffer.buffer.GetAddressOf()))))
          {
            REX_ERROR(LogDirectX, "Failed to create committed resource for a default buffer.");
            return {};
          }

          // In order to copy CPU memory data into our default buffer, we need to create
          // an intermediate upload heap.
          CD3DX12_HEAP_PROPERTIES heap_properties_upload(D3D12_HEAP_TYPE_UPLOAD);
          auto buffer_upload = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

          if(DX_FAILED(device->CreateCommittedResource(&heap_properties_upload, D3D12_HEAP_FLAG_NONE, &buffer_upload, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(default_buffer.upload_buffer.GetAddressOf()))))
          {
            REX_ERROR(LogDirectX, "Failed to create committed resource for intermediate upload heap.");
            return {};
          }

          // Describe the data we want to copy into the default buffer.
          D3D12_SUBRESOURCE_DATA sub_resource_data = {};
          sub_resource_data.pData                  = initData;
          sub_resource_data.RowPitch               = byteSize;
          sub_resource_data.SlicePitch             = sub_resource_data.RowPitch;

          // Schedule to copy the data to the default buffer resource.
          // At a high level, the helper function UpdateSubresources will copy the CPU memory into the intermediate upload heap.
          // Then, using ID3D12CommandList::CopySubresourceRegion, the intermediate upload heap data will be copied to mBuffer.
          auto transition_common_copydest = CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
          cmdList->ResourceBarrier(1, &transition_common_copydest);
          UpdateSubresources<1>(cmdList, default_buffer.buffer.Get(), default_buffer.upload_buffer.Get(), 0, 0, 1, &sub_resource_data);
          auto transition_copydest_generic_read = CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
          cmdList->ResourceBarrier(1, &transition_copydest_generic_read);

          // Note: uploadbuffer has to be kept alive after the above function calls because
          // the command list has not been executed yet that performs the actual copy.
          // The caller can Release the uploadBuffer after it knows the copy has been executed.

          return default_buffer;
        }

        //-------------------------------------------------------------------------
        rsl::unique_ptr<BufferResource> create_buffer(void* bufferData, s32 bufferByteSize)
        {
          wrl::ComPtr<ID3DBlob> buffer_cpu;

          if(DX_FAILED(D3DCreateBlob(bufferByteSize, &buffer_cpu)))
          {
            REX_ERROR(LogDirectX, "Could not create buffer blob");
            return nullptr;
          }
          CopyMemory(buffer_cpu->GetBufferPointer(), bufferData, bufferByteSize);

          auto default_buffer = create_default_buffer(g_ctx->device->get(), g_ctx->command_list.Get(), bufferData, bufferByteSize);
          if(!default_buffer)
          {
            REX_ERROR(LogDirectX, "Could not create GPU buffer");
            return nullptr;
          }

          rhi::set_debug_name_for(default_buffer->buffer.Get(), "Buffer GPU");
          rhi::set_debug_name_for(default_buffer->upload_buffer.Get(), "Buffer Uploader");

          return rsl::make_unique<BufferResource>(buffer_cpu, default_buffer->buffer, default_buffer->upload_buffer, bufferByteSize);
        }

        //-------------------------------------------------------------------------
        rsl::unique_ptr<CommittedBufferResource> create_committed_resource(s32 bufferCount, s32 bufferByteSize)
        {
            s32 obj_cb_byte_size = rex::align(bufferByteSize, s_constant_buffer_min_allocation_size);

            wrl::ComPtr<ID3D12Resource> constant_buffer_uploader;

            CD3DX12_HEAP_PROPERTIES heap_properties_upload(D3D12_HEAP_TYPE_UPLOAD);
            CD3DX12_RESOURCE_DESC buffer_upload = CD3DX12_RESOURCE_DESC::Buffer(obj_cb_byte_size * bufferCount);

            if(DX_FAILED(g_ctx->device->get()->CreateCommittedResource(&heap_properties_upload, D3D12_HEAP_FLAG_NONE, &buffer_upload, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constant_buffer_uploader))))
            {
                REX_ERROR(LogDirectX, "Could not create committed resource ( constant buffer )");
                return nullptr;
            }

            rhi::set_debug_name_for(constant_buffer_uploader.Get(), "Constant Buffer Uploader");

            rsl::unique_ptr<CommittedBufferResource> constant_buffer_resources = rsl::make_unique<CommittedBufferResource>(constant_buffer_uploader, obj_cb_byte_size, obj_cb_byte_size * bufferCount);

            if(DX_FAILED(constant_buffer_uploader->Map(0, nullptr, reinterpret_cast<void**>(&constant_buffer_resources->get()->mapped_data))))
            {
                REX_ERROR(LogDirectX, "Could not map data to committed resource ( constant buffer )");
                return nullptr;
            }

            REX_LOG(LogDirectX, "Create Committed Resource - buffer_count: {0}, buffer_byte_size: {1}, buffer_element_byte_size: {2}", bufferCount, obj_cb_byte_size * bufferCount, obj_cb_byte_size);

            return constant_buffer_resources;
        }

        //-------------------------------------------------------------------------
        rsl::unique_ptr<ConstantBufferViewResource> create_constant_buffer_view(const ResourceSlot* frameSlot, const ResourceSlot& committedResourceSlot, s32 bufferByteSize)
        {
            // Find the frame for the given frame slot
            Frame* frame = g_ctx->frame_ctx.find_frame(frameSlot);

            // Ensure the frame was found for the given slot
            REX_ASSERT_X(frame != nullptr, "Failed to find frame for slot: {}", frameSlot->slot_id());
            // Ensure the committed resource is part of this frame
            REX_ASSERT_X(internal::has_committed_resource_for_frame(frameSlot, committedResourceSlot), "Unable to find committed resource for give frame: {}", frameSlot->slot_id());

            // Retrieve the committed resource
            auto& committed_buffer_resource = g_ctx->resource_pool.as<CommittedBufferResource>(committedResourceSlot);
            auto  committed_resource = committed_buffer_resource.get();

            // Align buffer size to meet DX12 requirements (multiples of 256)
            s32 obj_cb_byte_size = rex::align(bufferByteSize, s_constant_buffer_min_allocation_size);

            // Calculate GPU memory block starting address
            D3D12_GPU_VIRTUAL_ADDRESS cb_address = committed_resource->uploader->GetGPUVirtualAddress();
            cb_address += frame->amount_active_constant_buffer(committedResourceSlot) * obj_cb_byte_size;

            // Calculate CPU memory block starting handle
            s32 heap_index = g_ctx->active_constant_buffers;
            CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(g_ctx->descriptor_heap_pool[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->GetCPUDescriptorHandleForHeapStart());
            handle.Offset(heap_index, g_ctx->cbv_srv_uav_desc_size);

            // Create a new constant buffer view given the offsetted GPU address and the size of the constant buffer in bytes
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc;
            cbv_desc.BufferLocation = cb_address;
            cbv_desc.SizeInBytes = obj_cb_byte_size;

            g_ctx->device->get()->CreateConstantBufferView(&cbv_desc, handle);

            // Create an internal Constant Buffer View Resource and track it in the resource pool
            // Pass the committed resource, data size, and CBV heap index
            rsl::unique_ptr<ConstantBufferViewResource> constant_buffer_resources = rsl::make_unique<ConstantBufferViewResource>(committedResourceSlot, obj_cb_byte_size, g_ctx->active_constant_buffers);

            // Increment the amount of active constant buffer resources for this frame
            frame->increment_amount_active_constant_buffer(committedResourceSlot);

            // A large heap of constant buffers was allocated at the start of this application
            // Each new constant buffer view requires a different offset in the CBV heap.
            // Increment the count of active constant buffers to keep track of the offset within this heap
            ++g_ctx->active_constant_buffers;

            return constant_buffer_resources;
        }

        //-------------------------------------------------------------------------
        CD3DX12_DESCRIPTOR_RANGE create_constant_buffer_descriptor_range(const commands::ConstantLayoutDescription& desc)
        {
          CD3DX12_DESCRIPTOR_RANGE table;

          table.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, desc.location);

          return table;
        }

        //-------------------------------------------------------------------------
        rsl::unordered_map<commands::ConstantType, rsl::function<CD3DX12_DESCRIPTOR_RANGE(const commands::ConstantLayoutDescription&)>> get_descriptor_ranges_create_func()
        {
          return {{commands::ConstantType::CBuffer, create_constant_buffer_descriptor_range}};
        }

        //-------------------------------------------------------------------------
        wrl::ComPtr<ID3D12RootSignature> create_shader_root_signature(const commands::ConstantLayoutDescription* constants, s32 numConstants)
        {
          auto descriptor_ranges_create_funcs = get_descriptor_ranges_create_func();
          auto descriptor_ranges_map          = rsl::unordered_map<commands::ConstantType, rsl::vector<CD3DX12_DESCRIPTOR_RANGE>>();

          for(s32 i = 0; i < numConstants; ++i)
          {
            const commands::ConstantLayoutDescription& constant = constants[i];
            const commands::ConstantType constant_type          = constant.type;

            if(descriptor_ranges_create_funcs.find(constant_type) != descriptor_ranges_create_funcs.cend())
            {
              CD3DX12_DESCRIPTOR_RANGE descriptor_range = descriptor_ranges_create_funcs[constant_type](constant);
              descriptor_ranges_map[constant_type].push_back(descriptor_range);
            }
          }

          // Root parameter can be a table, root descriptor or root constants.
          auto root_parameters = rsl::vector<CD3DX12_ROOT_PARAMETER>(rsl::Capacity(numConstants));
          for(auto& pair: descriptor_ranges_map)
          {
            for(auto& table: pair.value)
            {
              // Create root CBVs.
              CD3DX12_ROOT_PARAMETER root_param;
              root_param.InitAsDescriptorTable(1, &table);

              root_parameters.push_back(root_param);
            }
          }

          // A root signature is an array of root parameters.
          CD3DX12_ROOT_SIGNATURE_DESC root_sig_desc(root_parameters.size(), root_parameters.data(), 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

          // Create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
          wrl::ComPtr<ID3DBlob> serialized_root_sig = nullptr;
          wrl::ComPtr<ID3DBlob> error_blob          = nullptr;

          HRESULT hr = D3D12SerializeRootSignature(&root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1, serialized_root_sig.GetAddressOf(), error_blob.GetAddressOf());
          if(error_blob != nullptr)
          {
            REX_ERROR(LogDirectX, "{}", (char*)error_blob->GetBufferPointer());
            return nullptr;
          }

          if(DX_FAILED(hr))
          {
            REX_ERROR(LogDirectX, "Failed to serialize root signature");
            return nullptr;
          }

          wrl::ComPtr<ID3D12RootSignature> root_signature;
          if(DX_FAILED(g_ctx->device->get()->CreateRootSignature(0, serialized_root_sig->GetBufferPointer(), serialized_root_sig->GetBufferSize(), IID_PPV_ARGS(&root_signature))))
          {
            REX_ERROR(LogDirectX, "Failed to create root signature");
            return nullptr;
          }

          rhi::set_debug_name_for(root_signature.Get(), "Root Signature");

          return root_signature;
        }

        //-------------------------------------------------------------------------
        D3D12_CPU_DESCRIPTOR_HANDLE rendertarget_buffer_descriptor(s32 idx)
        {
          return CD3DX12_CPU_DESCRIPTOR_HANDLE(g_ctx->descriptor_heap_pool[D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV]->GetCPUDescriptorHandleForHeapStart(), idx, g_ctx->rtv_desc_size);
        }

        //-------------------------------------------------------------------------
        D3D12_CPU_DESCRIPTOR_HANDLE depthstencil_buffer_descriptor(s32 idx)
        {
          return CD3DX12_CPU_DESCRIPTOR_HANDLE(g_ctx->descriptor_heap_pool[D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV]->GetCPUDescriptorHandleForHeapStart(), idx, g_ctx->dsv_desc_size);
        }

        bool init_multi_sampling(DirectXDevice* device, DXGI_FORMAT backbufferFormat)
        {
          // 4x MSAA is supported on all DX11 hardware, since we only support DX12 devices we are guaranteed that 4x MSAA is supported.
          // However, we do have to check the supported quality level.
          D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS ms_quality_levels;
          ms_quality_levels.Format = backbufferFormat;
          ms_quality_levels.SampleCount = 4;
          ms_quality_levels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
          ms_quality_levels.NumQualityLevels = 0;
          if (DX_FAILED(device->get()->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &ms_quality_levels, sizeof(ms_quality_levels))))
          {
            REX_ERROR(LogDirectX, "Given multisample quality level is not supported by this hardware");
            return ms_quality_levels.NumQualityLevels;
          }

          return ms_quality_levels.NumQualityLevels;
        }

        bool build_clear_state(DirectXContext* ctx)
        {
          rex::renderer::commands::CreateClearStateCommandDesc create_clear_state_command_desc{};

          create_clear_state_command_desc.rgba = rsl::colors::LightSteelBlue;
          create_clear_state_command_desc.depth = 1.0f;
          create_clear_state_command_desc.stencil = 0x00;

          rex::StateController<rex::renderer::ClearBits> clear_flags;
          clear_flags.add_state(rex::renderer::ClearBits::ClearColorBuffer);
          clear_flags.add_state(rex::renderer::ClearBits::ClearDepthBuffer);
          clear_flags.add_state(rex::renderer::ClearBits::ClearStencilBuffer);

          create_clear_state_command_desc.flags = clear_flags;

          ctx->clear_state = rex::renderer::create_clear_state(rsl::move(create_clear_state_command_desc));

          return true;
        }

      } // namespace internal

      class DirectXRenderer
      {
      public:
        DirectXRenderer(rhi::RenderHardwareInfrastructure* rhi, const OutputWindowUserData& userData)
          : m_rhi(rhi)
          , m_frame_ctx(s_num_frame_resources)
        {
          // Create a scopeguard so if we exit the renderer too early on
          // We mark it as initialization failed
          rsl::scopeguard mark_init_failed = [this]() { m_is_initialized = false; };

          // Init the clear state
          if (internal::build_clear_state(g_ctx.get()) == false)
          {
            REX_ERROR(LogDirectX, "Failed to create the clear state");
            return;
          }

          // Flush the command queue, making sure all graphic commands are executed at the end of renderer creation
          m_rhi->flush_command_queue();

          init_viewport();
          init_scirssor_rect();

          // release the scopeguard so that init gets marked successful
          mark_init_failed.release();
        }

      private:
        bool build_clear_state()
        {
          rex::renderer::commands::CreateClearStateCommandDesc create_clear_state_command_desc{};

          create_clear_state_command_desc.rgba = rsl::colors::LightSteelBlue;
          create_clear_state_command_desc.depth = 1.0f;
          create_clear_state_command_desc.stencil = 0x00;

          rex::StateController<rex::renderer::ClearBits> clear_flags;
          clear_flags.add_state(rex::renderer::ClearBits::ClearColorBuffer);
          clear_flags.add_state(rex::renderer::ClearBits::ClearDepthBuffer);
          clear_flags.add_state(rex::renderer::ClearBits::ClearStencilBuffer);

          create_clear_state_command_desc.flags = clear_flags;

          m_clear_state = rex::renderer::create_clear_state(rsl::move(create_clear_state_command_desc));

          return true;
        }
        void init_viewport()
        {
          // Create viewport to render our image in
          // A viewport always needs to reset whenever a command list is reset
          m_screen_viewport.TopLeftX = 0.0f;
          m_screen_viewport.TopLeftY = 0.0f;
          m_screen_viewport.Width = static_cast<f32>(userData.window_width);
          m_screen_viewport.Height = static_cast<f32>(userData.window_height);
          m_screen_viewport.MinDepth = 0.0f;
          m_screen_viewport.MaxDepth = 1.0f;
        }
        void init_scirssor_rect()
        {
          // Cull pixels drawn outside of the backbuffer ( such as UI elements )
          m_scissor_rect = { 0, 0, static_cast<s32>(userData.window_width), static_cast<s32>(userData.window_height) };
        }

      private:
        rhi::RenderHardwareInfrastructure* m_rhi;
        D3D12_VIEWPORT m_screen_viewport;
        RECT m_scissor_rect;
        FrameContext m_frame_ctx;
        ResourceSlot m_clear_state;

        bool m_is_initialized;
      };
      
      rsl::unique_ptr<DirectXRenderer> g_renderer;

      //-------------------------------------------------------------------------
      bool initialize(const OutputWindowUserData& userData, s32 maxFramesInflight, const ResourceSlot& fbColorTargetSlot, const ResourceSlot& bbColorTargetSlot, const ResourceSlot& depthTargetSlot)
      {
        // Initialize the render hardware interface
        // This is the first layer of abstraction between the hardware
        // and the software.
        rhi::RenderHardwareInfrastructure* rhi = nullptr;
        {
          ExecutionLogger exec_logger(LogDirectX, "Render Hardware Infrastructure Initialization");
          rhi = rhi::init(userData);
          if (!rhi)
          {
            REX_ERROR(LogDirectX, "Failed to initialize rhi layer.");
            return false;
          }
        }

        // Initialize the renderer with the rhi.
        g_renderer = rsl::make_unique<DirectXRenderer>(rhi, userData);

        return true;
      }

      void shutdown()
      {
        g_renderer.reset();
      }

      //-------------------------------------------------------------------------
      const ResourceSlot* active_frame()
      {
          return g_ctx->frame_ctx.current_frame_resource();
      }

      //-------------------------------------------------------------------------
      const ResourceSlot* frame_at_index(s32 idx)
      {
          REX_ASSERT_X(idx < max_frames_in_flight(), "Frame index exceeds the number of available frames");

          return g_ctx->frame_ctx.find_frame(idx)->slot();
      }
      
      //-------------------------------------------------------------------------
      s32 max_frames_in_flight()
      {
          return g_ctx->frame_ctx.max_frame_resources_count();
      }

      //-------------------------------------------------------------------------
      bool create_clear_state(const commands::CreateClearStateCommandDesc& cs, const ResourceSlot& resourceSlot)
      {
        resources::ClearState rcs;

        rcs.rgba    = cs.rgba;
        rcs.depth   = cs.depth;
        rcs.stencil = cs.stencil;
        rcs.flags   = cs.flags.state();

        g_ctx->resource_pool.insert(resourceSlot, rsl::make_unique<ClearStateResource>(rcs));

        return true;
      }

      //-------------------------------------------------------------------------
      bool create_raster_state(const commands::CreateRasterStateCommandDesc& rs, const ResourceSlot& resourceSlot)
      {
        D3D12_RASTERIZER_DESC d3d_rs;

        d3d_rs.FillMode              = directx::to_d3d12_fill_mode(rs.fill_mode);
        d3d_rs.CullMode              = directx::to_d3d12_cull_mode(rs.cull_mode);
        d3d_rs.FrontCounterClockwise = rs.front_ccw;
        d3d_rs.DepthBias             = rs.depth_bias;
        d3d_rs.DepthBiasClamp        = rs.depth_bias_clamp;
        d3d_rs.SlopeScaledDepthBias  = rs.sloped_scale_depth_bias;
        d3d_rs.DepthClipEnable       = rs.depth_clip_enable;
        d3d_rs.ForcedSampleCount     = rs.forced_sample_count;

        /**
         * Conservative rasterization means that all pixels that are at least partially covered by a rendered primitive are rasterized, which means that the pixel shader is invoked.
         * Normal behavior is sampling, which is not used if conservative rasterization is enabled.
         *
         * Conservative rasterization is useful in a number of situations outside of rendering (collision detection, occlusion culling, and visibility detection).
         *
         * https://learn.microsoft.com/en-us/windows/win32/direct3d11/conservative-rasterization
         */
        d3d_rs.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        d3d_rs.MultisampleEnable     = rs.multisample;
        d3d_rs.AntialiasedLineEnable = rs.aa_lines;

        g_ctx->resource_pool.insert(resourceSlot, rsl::make_unique<RasterStateResource>(d3d_rs));

        return true;
      }

      //-------------------------------------------------------------------------
      bool create_input_layout(const commands::CreateInputLayoutCommandDesc& cil, const ResourceSlot& resourceSlot)
      {
        rsl::vector<D3D12_INPUT_ELEMENT_DESC> input_element_descriptions(rsl::Size(cil.input_layout.size()));

        for(s32 i = 0; i < cil.input_layout.size(); ++i)
        {
          input_element_descriptions[i].SemanticName         = cil.input_layout[i].semantic_name.data();
          input_element_descriptions[i].SemanticIndex        = cil.input_layout[i].semantic_index;
          input_element_descriptions[i].Format               = directx::to_d3d12_vertex_format(cil.input_layout[i].format);
          input_element_descriptions[i].InputSlot            = cil.input_layout[i].input_slot;
          input_element_descriptions[i].AlignedByteOffset    = cil.input_layout[i].aligned_byte_offset;
          input_element_descriptions[i].InputSlotClass       = directx::to_d3d12_input_layout_classification(cil.input_layout[i].input_slot_class);
          input_element_descriptions[i].InstanceDataStepRate = cil.input_layout[i].instance_data_step_rate;
        }

        g_ctx->resource_pool.insert(resourceSlot, rsl::make_unique<InputLayoutResource>(input_element_descriptions));

        return true;
      }

      //-------------------------------------------------------------------------
      bool create_vertex_buffer(const commands::CreateBufferCommandDesc& cb, const ResourceSlot& resourceSlot)
      {
        REX_ASSERT_X(cb.buffer_data.data() != nullptr && cb.buffer_data.size() != 0, "Trying to create an empty vertex buffer");

        rsl::unique_ptr<BufferResource> resource = internal::create_buffer((void*)cb.buffer_data.data(), rsl::safe_numeric_cast<s32>((cb.buffer_data.size().size_in_bytes())));

        if(resource == nullptr)
        {
          REX_ERROR(LogDirectX, "Failed to create vertex buffer");
          return false;
        }

        g_ctx->resource_pool.insert(resourceSlot, rsl::move(resource));

        return true;
      }

      //-------------------------------------------------------------------------
      bool create_index_buffer(const commands::CreateBufferCommandDesc& cb, const ResourceSlot& resourceSlot)
      {
        REX_ASSERT_X(cb.buffer_data.data() != nullptr && cb.buffer_data.size() != 0, "Trying to create an empty index buffer");

        rsl::unique_ptr<BufferResource> resource = internal::create_buffer((void*)cb.buffer_data.data(), rsl::safe_numeric_cast<s32>(cb.buffer_data.size().size_in_bytes()));

        if(resource == nullptr)
        {
          REX_ERROR(LogDirectX, "Failed to create index buffer");
          return false;
        }

        g_ctx->resource_pool.insert(resourceSlot, rsl::move(resource));

        return true;
      }

      //-------------------------------------------------------------------------
      bool create_constant_buffer_view(const commands::CreateConstantBufferViewCommandDesc& cb, const ResourceSlot& resourceSlot)
      {
        REX_ASSERT_X(cb.buffer_size != 0, "Trying to create an empty constant buffer"); // cb.data is allowed to be NULL when creating a constant buffer

        rsl::unique_ptr<ConstantBufferViewResource> resource = internal::create_constant_buffer_view(g_ctx->frame_ctx.find_frame(cb.frame_index)->slot(), cb.committed_resource, cb.buffer_size);

        if(resource == nullptr)
        {
          REX_ERROR(LogDirectX, "Failed to create constant buffer");
          return false;
        }

        g_ctx->resource_pool.insert(resourceSlot, rsl::move(resource));

        return true;
      }

      //-------------------------------------------------------------------------
      bool create_pipeline_state_object(const commands::CreatePipelineStateCommandDesc& cps, const ResourceSlot& resourceSlot)
      {
        REX_ASSERT_X(cps.input_layout.is_valid(), "Invalid input layout resource slot given");
        REX_ASSERT_X(cps.shader_program.is_valid(), "Invalid shader program resource slot given");
        REX_ASSERT_X(g_ctx->active_color_targets != 0, "No render targets have been set, a PSO needs to know the format of the render target(s)");

        auto& input_layout_resource = g_ctx->resource_pool.as<InputLayoutResource>(cps.input_layout);
        auto& shader_program_resource = g_ctx->resource_pool.as<ShaderProgramResource>(cps.shader_program);
        auto& raster_state_resource = g_ctx->resource_pool.as<RasterStateResource>(cps.rasterizer_state);

        D3D12_RASTERIZER_DESC raster_state = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        if(cps.rasterizer_state.is_valid())
        {
          raster_state = *raster_state_resource.get();
        }
        D3D12_BLEND_DESC blend_state = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        if(cps.blend_state.is_valid())
        {
          // TODO:
          // Create a Blend State Resource
        }
        D3D12_DEPTH_STENCIL_DESC depth_stencil_state = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        if(cps.depth_stencil_state.is_valid())
        {
          // TODO:
          // Create a Depth Stencil State Resource
        }

        PipelineStateObjectHashData hash_data;
        hash_data.input_layout_resource = &input_layout_resource;
        hash_data.shader_program_resource = &shader_program_resource;
        hash_data.raster_state_resource = &raster_state_resource;

        auto insert_res = g_ctx->pipeline_state_objects.try_emplace(hash_data);

        // If there's already a PSO with the above values, return that one and don't create a new one
        if(insert_res.emplace_successful == false)
        {
          g_ctx->resource_pool.insert(resourceSlot, rsl::make_unique<PipelineStateResource>(insert_res.inserted_element->value.Get()));

          return true;
        }

        // If no such PSO exists yet, create a new one
        D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc;
        ZeroMemory(&pso_desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

        pso_desc.InputLayout           = {input_layout_resource.get()->data(), rsl::safe_numeric_cast<UINT>(input_layout_resource.get()->size())};
        pso_desc.pRootSignature        = shader_program_resource.get()->root_signature.Get();
        pso_desc.VS                    = {reinterpret_cast<BYTE*>(shader_program_resource.get()->vertex_shader->GetBufferPointer()), shader_program_resource.get()->vertex_shader->GetBufferSize()};
        pso_desc.PS                    = {reinterpret_cast<BYTE*>(shader_program_resource.get()->pixel_shader->GetBufferPointer()), shader_program_resource.get()->pixel_shader->GetBufferSize()};
        pso_desc.RasterizerState       = raster_state;
        pso_desc.BlendState            = blend_state;
        pso_desc.DepthStencilState     = depth_stencil_state;
        pso_desc.SampleMask            = UINT_MAX;
        pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pso_desc.NumRenderTargets      = g_ctx->active_color_targets;
        for(s32 i = 0; i < g_ctx->active_color_targets; ++i)
        {
            auto& render_target_id = g_ctx->active_color_target[i];
            auto& render_target    = g_ctx->resource_pool.as<RenderTargetResource>(render_target_id);

            pso_desc.RTVFormats[i] = directx::to_dxd12_texture_format(render_target.get()->format);
        }
        pso_desc.SampleDesc.Count      = 1;
        pso_desc.SampleDesc.Quality    = 0;
        pso_desc.DSVFormat             = g_ctx->depth_stencil_format;

        if(DX_FAILED(g_ctx->device->get()->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&g_ctx->pipeline_state_objects[hash_data]))))
        {
          REX_ERROR(LogDirectX, "Failed to create pipeline state object");
          return false;
        }

        rhi::set_debug_name_for(g_ctx->pipeline_state_objects[hash_data].Get(), "Pipeline State Object");

        g_ctx->resource_pool.insert(resourceSlot, rsl::make_unique<PipelineStateResource>(g_ctx->pipeline_state_objects.at(hash_data)));

        return true;
      }

      //-------------------------------------------------------------------------
      bool create_frame_resource(const ResourceSlot& resourceSlot)
      {
        wrl::ComPtr<ID3D12CommandAllocator> cmd_list_alloc;

        if(DX_FAILED(g_ctx->device->get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmd_list_alloc.GetAddressOf()))))
        {
          REX_ERROR(LogDirectX, "Failed to create command list allocator for frame resource");
          return false;
        }

        rhi::set_debug_name_for(cmd_list_alloc.Get(), "Frame Command Allocator");

        g_ctx->resource_pool.insert(resourceSlot, rsl::make_unique<FrameResource>(cmd_list_alloc));

        // Activate the first frame resource
        g_ctx->frame_ctx.add_frame(Frame(g_ctx->frame_ctx.frame_resources_count(), resourceSlot));
        if(g_ctx->frame_ctx.current_frame_resource() == nullptr)
        {
          g_ctx->frame_ctx.set_current_frame(&resourceSlot);
        }

        return true;
      }

      //-------------------------------------------------------------------------
      bool attach_committed_resource_to_frame(const commands::AttachCommittedResourceToFrameCommandDesc& acrd, const ResourceSlot& resourceSlot)
      {
          REX_ASSERT_X(acrd.buffer_byte_size != 0, "Trying to create an empty constant buffer"); // cb.data is allowed to be NULL when creating a constant buffer

          rsl::unique_ptr<CommittedBufferResource> resource = internal::create_committed_resource(acrd.buffer_count, acrd.buffer_byte_size);

          if (resource == nullptr)
          {
              REX_ERROR(LogDirectX, "Failed to create constant buffer");
              return false;
          }

          auto frame = g_ctx->frame_ctx.find_frame(acrd.frame_index);
          
          REX_ASSERT_X(frame != nullptr, "Failed to find frame for slot: {}", acrd.frame_index);

          frame->add_committed_resource(resourceSlot);

          g_ctx->resource_pool.insert(resourceSlot, rsl::move(resource));

          return true;
      }

      //-------------------------------------------------------------------------
      bool load_shader(const commands::LoadShaderCommandDesc& desc)
      {
        wrl::ComPtr<ID3DBlob> byte_code = d3d::create_blob(desc.shader_byte_code);
        if(!byte_code)
        {
          REX_ERROR(LogDirectX, "Failed to load shader");
          return false;
        }

        switch(ls.shader_type)
        {
          case ShaderType::VERTEX: g_ctx->resource_pool.insert(rsl::make_unique<VertexShaderResource>(byte_code)); break;
          case ShaderType::PIXEL: g_ctx->resource_pool.insert(rsl::make_unique<PixelShaderResource>(byte_code)); break;

          default: REX_ERROR(LogDirectX, "Unsupported Shader Type was given"); return false;
        }

        return true;
      }

      //-------------------------------------------------------------------------
      bool link_shader(const commands::LinkShaderCommandDesc& ls, const ResourceSlot& resourceSlot)
      {
        auto root_sig = internal::create_shader_root_signature(ls.constants.data(), ls.constants.size());

        if(root_sig == nullptr)
        {
          REX_ERROR(LogDirectX, "Failed to create root signature");
          return false;
        }

        auto& vsr = g_ctx->resource_pool.as<VertexShaderResource>(ls.vertex_shader);
        auto& psr = g_ctx->resource_pool.as<PixelShaderResource>(ls.pixel_shader);

        g_ctx->resource_pool.insert(resourceSlot, rsl::make_unique<ShaderProgramResource>(root_sig, vsr.get()->vertex_shader, psr.get()->pixel_shader, ls.constants.data(), ls.constants.size()));

        // More than one instance of the function matches the arugment list
        // So we have to add the namespace here
        //
        backend::release_resource(ls.vertex_shader); // vertex shader is referenced in the shader program
        backend::release_resource(ls.pixel_shader);  // pixel shader is referenced in the shader program

        return true;
      }

      //-------------------------------------------------------------------------
      bool compile_shader(const commands::CompileShaderCommandDesc& cs, const ResourceSlot& resourceSlot)
      {
        s32 compile_flags = 0;
#ifdef REX_ENABLE_DEBUG_SHADER_COMPILATION
        compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        HRESULT hr = S_OK;

        wrl::ComPtr<ID3DBlob> byte_code = nullptr;
        wrl::ComPtr<ID3DBlob> errors    = nullptr;

        hr = D3DCompile2(cs.shader_code.data(), cs.shader_code.size(), cs.shader_name.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, cs.shader_entry_point.data(), cs.shader_feature_target.data(), compile_flags, 0, 0, 0, 0, &byte_code, &errors);

        if(errors != nullptr)
        {
          REX_ERROR(LogDirectX, "{}", (char*)errors->GetBufferPointer());
          return false;
        }

        if(DX_FAILED(hr))
        {
          REX_ERROR(LogDirectX, "Failed to compile shader");
          return false;
        }

        switch(cs.shader_type)
        {
          case ShaderType::VERTEX: g_ctx->resource_pool.insert(resourceSlot, rsl::make_unique<VertexShaderResource>(byte_code)); break;
          case ShaderType::PIXEL: g_ctx->resource_pool.insert(resourceSlot, rsl::make_unique<PixelShaderResource>(byte_code)); break;

          default: REX_ERROR(LogDirectX, "Unsupported Shader Type was given"); return false;
        }

        return true;
      }

      //-------------------------------------------------------------------------
      void update_committed_resource(const commands::UpdateCommittedResourceCommandDesc& updateConstantBuffer, const ResourceSlot& resourceSlot)
      {
        auto& cbr = g_ctx->resource_pool.as<CommittedBufferResource>(resourceSlot);
        auto cs   = cbr.get();

        memcpy(&cs->mapped_data[updateConstantBuffer.element_index * cs->element_data_byte_size], updateConstantBuffer.buffer_data.data(), updateConstantBuffer.buffer_data.size());
      }

      //-------------------------------------------------------------------------
      void wait_for_active_frame()
      {
        // Cycle through the circular frame resource array.
        g_ctx->frame_ctx.next_frame();

        auto& fr = g_ctx->resource_pool.as<FrameResource>(*g_ctx->frame_ctx.current_frame_resource());
        auto f   = fr.get();

        if(f->fence != 0)
        {
          internal::wait_for_fence(g_ctx->fence.Get(), f->fence);
        }
      }

      //-------------------------------------------------------------------------
      void clear(const ResourceSlot& resourceSlot)
      {
        auto& csr = g_ctx->resource_pool.as<ClearStateResource>(resourceSlot);

        ClearBits flags = csr.get()->flags;

        REX_ASSERT_X(flags != ClearBits::None, "No clear flags given but renderer::backend::clear was called.");

        if(flags & ClearBits::ClearColorBuffer)
        {
          for(s32 i = 0; i < g_ctx->active_color_targets; ++i)
          {
            auto& render_target_id = g_ctx->active_color_target[i];
            auto& render_target    = g_ctx->resource_pool.as<RenderTargetResource>(render_target_id);

            D3D12_CPU_DESCRIPTOR_HANDLE backbuffer_desc = internal::rendertarget_buffer_descriptor(render_target.get()->array_index);

            g_ctx->command_list->ClearRenderTargetView(backbuffer_desc, csr.get()->rgba.getData(), 0, nullptr);
          }
        }

        if(flags & ClearBits::ClearDepthBuffer || flags & ClearBits::ClearStencilBuffer)
        {
          auto& depth_stencil_target_id = g_ctx->active_depth_target;
          auto& depth_stencil_target    = g_ctx->resource_pool.as<DepthStencilTargetResource>(depth_stencil_target_id);

          D3D12_CPU_DESCRIPTOR_HANDLE depthstencil_desc = internal::depthstencil_buffer_descriptor(depth_stencil_target.get()->array_index);

          s32 depth_stencil_clear_flags = 0;
          depth_stencil_clear_flags |= flags & ClearBits::ClearDepthBuffer ? D3D12_CLEAR_FLAG_DEPTH : 0;
          depth_stencil_clear_flags |= flags & ClearBits::ClearStencilBuffer ? D3D12_CLEAR_FLAG_STENCIL : 0;

          g_ctx->command_list->ClearDepthStencilView(depthstencil_desc, (D3D12_CLEAR_FLAGS)depth_stencil_clear_flags, csr.get()->depth, csr.get()->stencil, 0, nullptr);
        }
      }

      //-------------------------------------------------------------------------
      bool release_resource(const ResourceSlot& resourceSlot)
      {
        if (resourceSlot.is_valid() == false)
        {
            REX_WARN(LogDirectX, "Trying to release an invalid resource from ResourcePool");
            return true;
        }

        if (resourceSlot.is_about_to_be_removed() == false && g_ctx->resource_pool.has_slot(resourceSlot))
        {
            if (g_ctx->resource_pool.is<ConstantBufferViewResource>(resourceSlot))
            {
                --g_ctx->active_constant_buffers;
            }

            g_ctx->resource_pool.remove(resourceSlot);
        }
        else
        {
            REX_WARN(LogDirectX, "Trying to release a resource slot that was not registered within the resource pool: {}", resourceSlot.slot_id());
        }

        return true;
      }

      //-------------------------------------------------------------------------
      bool prepare_user_initialization()
      {
        if(DX_FAILED(g_ctx->command_allocator->Reset()))
        {
          REX_ERROR(LogDirectX, "Failed to reset command allocator");
          return false;
        }

        if(internal::reset_command_list(g_ctx->command_allocator.Get(), nullptr) == false)
        {
          REX_ERROR(LogDirectX, "Failed to reset command list");
          return false;
        }

        return true;
      }

      //-------------------------------------------------------------------------
      bool finish_user_initialization()
      {
        // Done recording commands!
        if(internal::close_command_list() == false)
        {
          REX_ERROR(LogDirectX, "Failed to close command list");
          return false;
        }

        // Add the command list to the queue for execution
        internal::exec_command_list();

        // Wait until ready
        flush_command_queue();

        return true;
      }

      //-------------------------------------------------------------------------
      void draw(s32 /*vertexCount*/, s32 /*startVertex*/)
      {
        REX_ASSERT("renderer::draw is unsupported when using DX12, use renderer::draw_indexed_instanced or renderer::draw_instanced");
      }

      //-------------------------------------------------------------------------
      void draw_indexed(s32 /*indexCount*/, s32 /*startIndex*/, s32 /*baseVertex*/)
      {
        REX_ASSERT("renderer::draw is unsupported when using DX12, use renderer::draw_indexed_instanced or renderer::draw_instanced");
      }

      //-------------------------------------------------------------------------
      void draw_indexed_instanced(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertex)
      {
        REX_ASSERT_X(g_ctx->active_shader_program.is_valid(), "Call renderer::set_shader before issuing a draw command");

        g_ctx->command_list->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, startInstance);
      }

      //-------------------------------------------------------------------------
      void draw_instanced(s32 vertexCount, s32 instanceCount, s32 startVertex, s32 startInstance)
      {
        REX_ASSERT_X(g_ctx->active_shader_program.is_valid(), "Call renderer::set_shader before issuing a draw command");

        g_ctx->command_list->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
      }

      //-------------------------------------------------------------------------
      bool set_render_targets(const ResourceSlot* const colorTargets, s32 numColorTargets, const ResourceSlot& depthTarget)
      {
        g_ctx->active_depth_target  = depthTarget;
        g_ctx->active_color_targets = numColorTargets;

        s32 num_views = numColorTargets;
        CD3DX12_CPU_DESCRIPTOR_HANDLE render_target_handles[s_max_color_targets];

        for(s32 i = 0; i < numColorTargets; ++i)
        {
          const ResourceSlot& color_target = colorTargets[i];
          g_ctx->active_color_target[i] = color_target;

          if(color_target.is_valid())
          {
            auto& render_target = g_ctx->resource_pool.as<RenderTargetResource>(color_target);

            render_target_handles[i] = internal::rendertarget_buffer_descriptor(render_target.get()->array_index);
          }
          else
          {
            g_ctx->active_color_target[i] = ResourceSlot(0);

            render_target_handles[i] = internal::rendertarget_buffer_descriptor(0);
          }
        }

        D3D12_CPU_DESCRIPTOR_HANDLE depth_stencil_handle = {};
        if(depthTarget.is_valid())
        {
          auto& depth_stencil_target = g_ctx->resource_pool.as<DepthStencilTargetResource>(depthTarget);

          depth_stencil_handle = internal::depthstencil_buffer_descriptor(depth_stencil_target.get()->array_index);
        }
        else
        {
          g_ctx->active_depth_target = ResourceSlot(0);

          depth_stencil_handle = internal::depthstencil_buffer_descriptor(0);
        }

        g_ctx->command_list->OMSetRenderTargets(num_views, render_target_handles, true, &depth_stencil_handle);

        return true;
      }

      //-------------------------------------------------------------------------
      bool set_input_layout(const ResourceSlot& /*inputLayoutSlot*/)
      {
        // Nothing to to on this platform

        return true;
      }

      //-------------------------------------------------------------------------
      bool set_viewport(const Viewport& viewport)
      {
        g_ctx->screen_viewport.TopLeftX = viewport.top_left_x;
        g_ctx->screen_viewport.TopLeftY = viewport.top_left_y;
        g_ctx->screen_viewport.Width    = viewport.width;
        g_ctx->screen_viewport.Height   = viewport.height;
        g_ctx->screen_viewport.MinDepth = viewport.min_depth;
        g_ctx->screen_viewport.MaxDepth = viewport.max_depth;

        g_ctx->command_list->RSSetViewports(1, &g_ctx->screen_viewport);

        return true;
      }

      //-------------------------------------------------------------------------
      bool set_scissor_rect(const ScissorRect& rect)
      {
        g_ctx->scissor_rect.top    = (LONG)rect.top;
        g_ctx->scissor_rect.left   = (LONG)rect.left;
        g_ctx->scissor_rect.bottom = (LONG)rect.bottom;
        g_ctx->scissor_rect.right  = (LONG)rect.right;

        g_ctx->command_list->RSSetScissorRects(1, &g_ctx->scissor_rect);

        return true;
      }

      //-------------------------------------------------------------------------
      bool set_vertex_buffers(const ResourceSlot* vertexBufferTargets, s32 numBuffers, s32 startSlot, const s32* strides, const s32* offsets)
      {
        auto views = rsl::vector<D3D12_VERTEX_BUFFER_VIEW>(rsl::Capacity(numBuffers));

        for(s32 i = 0; i < numBuffers; ++i)
        {
          auto& buffer_resource = g_ctx->resource_pool.as<BufferResource>(vertexBufferTargets[i]);

          D3D12_VERTEX_BUFFER_VIEW view;

          D3D12_GPU_VIRTUAL_ADDRESS vb_address = buffer_resource.get()->gpu_buffer->GetGPUVirtualAddress();
          vb_address += offsets[i];

          view.BufferLocation = vb_address;
          view.StrideInBytes  = strides[i];
          view.SizeInBytes    = buffer_resource.get()->size_in_bytes;

          views.push_back(view);
        }

        g_ctx->command_list->IASetVertexBuffers(startSlot, numBuffers, views.data());

        return true;
      }

      //-------------------------------------------------------------------------
      bool set_index_buffer(const ResourceSlot& indexBufferTarget, IndexBufferFormat format, s32 offset)
      {
        auto& buffer_resource = g_ctx->resource_pool.as<BufferResource>(indexBufferTarget);

        D3D12_INDEX_BUFFER_VIEW ibv;

        D3D12_GPU_VIRTUAL_ADDRESS ib_address = buffer_resource.get()->gpu_buffer->GetGPUVirtualAddress();
        ib_address += offset;

        ibv.BufferLocation = ib_address;
        ibv.Format         = directx::to_d3d12_index_format(format);
        ibv.SizeInBytes    = buffer_resource.get()->size_in_bytes;

        g_ctx->command_list->IASetIndexBuffer(&ibv);

        return true;
      }

      //-------------------------------------------------------------------------
      bool set_shader(const ResourceSlot& resourceSlot)
      {
        g_ctx->active_shader_program = resourceSlot;

        auto& shader_program = g_ctx->resource_pool.as<ShaderProgramResource>(resourceSlot);

        g_ctx->command_list->SetGraphicsRootSignature(shader_program.get()->root_signature.Get());

        return true;
      }

      //-------------------------------------------------------------------------
      bool set_constant_buffer_view(const ResourceSlot& resourceSlot, s32 location)
      {
        auto& buffer_resource = g_ctx->resource_pool.as<ConstantBufferViewResource>(resourceSlot);

        auto cbv_handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(g_ctx->descriptor_heap_pool[D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->GetGPUDescriptorHandleForHeapStart());
        cbv_handle.Offset(buffer_resource.get()->buffer_index, g_ctx->cbv_srv_uav_desc_size);

        g_ctx->command_list->SetGraphicsRootDescriptorTable(location, cbv_handle);

        return true;
      }

      //-------------------------------------------------------------------------
      bool set_primitive_topology(PrimitiveTopology topology)
      {
        g_ctx->command_list->IASetPrimitiveTopology(directx::to_d3d12_topology(topology));

        return true;
      }

      //-------------------------------------------------------------------------
      bool set_pipeline_state_object(const ResourceSlot& resourceSlot)
      {
        g_ctx->active_pipeline_state_object = resourceSlot;

        return true;
      }

      //-------------------------------------------------------------------------
      bool set_raster_state(const ResourceSlot& /*rasterStateIndex*/)
      {
        // Nothing to to on this platform

        return true;
      }

      //-------------------------------------------------------------------------
      bool new_frame()
      {
        auto& fr = g_ctx->resource_pool.as<FrameResource>(*g_ctx->frame_ctx.current_frame_resource());
        auto f   = fr.get();

        // Reuse the memory assosiated with command recording.
        // We can only reset when the associated command lists have finished
        // execution on the GPU.
        if(DX_FAILED(f->cmd_list_allocator->Reset()))
        {
          REX_ERROR(LogDirectX, "Failed to reset command allocator");
          return false;
        }

        // a command list can be reset after it has been added to the command queue via ExecuteCommandList. Reusing the
        // command list reuses memory.
        ID3D12PipelineState* dx_pso = nullptr;
        if (g_ctx->active_pipeline_state_object.is_valid())
        {
          auto& pso = g_ctx->resource_pool.as<PipelineStateResource>(g_ctx->active_pipeline_state_object);
          dx_pso = pso.get();
        }

        if(internal::reset_command_list(f->cmd_list_allocator.Get(), dx_pso) == false)
        {
          REX_ERROR(LogDirectX, "Failed to reset command list for frame: {0}", g_ctx->frame_ctx.current_frame_resource_index());
          return false;
        }

        return true;
      }

      //-------------------------------------------------------------------------
      bool end_frame()
      {
        auto& fr = g_ctx->resource_pool.as<FrameResource>(*g_ctx->frame_ctx.current_frame_resource());
        auto f   = fr.get();

        // Advance the fence value to mark commands up to this fence point.
        f->fence = ++g_ctx->current_fence;

        // Add an instruction to the command queue to set a new fence point.
        // Because we are on the GPU timeline, the new fence point won't be
        // set until the GPU finishes processing all the commands prior to this Signal().
        g_ctx->command_queue->Signal(g_ctx->fence.Get(), g_ctx->current_fence);

        return true;
      }

      //-------------------------------------------------------------------------
      bool begin_draw()
      {
        // Loop over all active color targets and transition them to the render target state
        for(s32 i = 0; i < g_ctx->active_color_targets; ++i)
        {
          const ResourceSlot& buffer_index = g_ctx->active_color_target[i];

          auto& render_target = g_ctx->resource_pool.as<RenderTargetResource>(buffer_index);

          // Indicate a state transition on the resouce usage.
          D3D12_RESOURCE_BARRIER render_target_transition = CD3DX12_RESOURCE_BARRIER::Transition(render_target.get()->render_target.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
          g_ctx->command_list->ResourceBarrier(1, &render_target_transition);
        }

        // Clear all of them
        backend::clear(g_ctx->clear_state);

        // Set the descriptor heap
        ID3D12DescriptorHeap* desc_heap    = g_ctx->descriptor_heap_pool[D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].Get();
        ID3D12DescriptorHeap* desc_heaps[] = {desc_heap};
        g_ctx->command_list->SetDescriptorHeaps(_countof(desc_heaps), desc_heaps);

        return true;
      }

      //-------------------------------------------------------------------------
      bool end_draw()
      {
        for(s32 i = 0; i < g_ctx->active_color_targets; ++i)
        {
          ResourceSlot buffer_slot = g_ctx->active_color_target[i];

          auto& render_target = g_ctx->resource_pool.as<RenderTargetResource>(buffer_slot);

          // Indicate a state transition on the resouce usage.
          D3D12_RESOURCE_BARRIER present_transition = CD3DX12_RESOURCE_BARRIER::Transition(render_target.get()->render_target.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
          g_ctx->command_list->ResourceBarrier(1, &present_transition);
        }

        return true;
      }

      //-------------------------------------------------------------------------
      bool present()
      {
        // Done recording commands!
        if(internal::close_command_list() == false)
        {
          REX_ERROR(LogDirectX, "Failed to close command list");
          return false;
        }

        // Add the command list to the queue for execution
        internal::exec_command_list();

        // Swap the back and front buffers
        if(DX_FAILED(g_ctx->swapchain->Present(0, 0)))
        {
          REX_ERROR(LogDirectX, "Failed to present swapchain");
          return false;
        }

        return true;
      }
    } // namespace backend
  }   // namespace renderer
} // namespace rex