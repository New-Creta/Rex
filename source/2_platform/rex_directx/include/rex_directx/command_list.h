#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/object_counter.h"

#include "rex_directx/directx_util.h"
#include "rex_directx/resources/buffer.h"

#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/memory.h"
#include "rex_std/map.h"
#include "rex_std/mutex.h"
#include "rex_std/vector.h"

namespace rex
{
    namespace renderer
    {
        class Resource;
        class VertexBuffer;
        class IndexBuffer;
        class Texture;
        class ByteAddressBuffer;
        class RootSignature;
        class RenderTarget;
        class UploadBuffer;
        class DynamicDescriptorHeap;
        class ResourceStateTracker;
        class ConstantBuffer;
        class ConstantBufferView;
        class ShaderResourceView;
        class UnorderedAccessView;
        class PipelineStateObject;

        class CommandList : public rsl::enable_shared_from_this<CommandList>, public ObjectCounter<CommandList>
        {
        public:
            /**
             * Get the type of command list.
             */
            D3D12_COMMAND_LIST_TYPE command_list_type() const;

            /**
            * Get the device that was used to create this command list.
            */
            Device* device() const;

            /**
             * Get direct access to the ID3D12GraphicsCommandList2 interface.
             */
            wrl::ComPtr<ID3D12GraphicsCommandList2> graphics_command_list() const;

            /**
             * Transition a resource to a particular state.
             *
             * @param resource The resource to transition.
             * @param stateAfter The state to transition the resource to. The before state is resolved by the resource state tracker.
             * @param subresource The subresource to transition. By default, this is D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES which indicates that all subresources are transitioned to the same state.
             * @param flushBarriers Force flush any barriers. resource barriers need to be flushed before a command (draw, dispatch, or copy) that expects the resource to be in a particular state can run.
             */
            void transition_barrier(const rsl::shared_ptr<Resource>& resource, D3D12_RESOURCE_STATES stateAfter, u32 subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flushBarriers = false);
            void transition_barrier(wrl::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES stateAfter, u32 subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flushBarriers = false);

            /**
             * Flush any barriers that have been pushed to the command list.
             */
            void flush_resource_barriers();

            /**
             * Copy resources.
             */
            void copy_resource(const rsl::shared_ptr<Resource>& dstRes, const rsl::shared_ptr<Resource>& srcRes);
            void copy_resource(wrl::ComPtr<ID3D12Resource> dstRes, wrl::ComPtr<ID3D12Resource> srcRes);

            /**
             * Resolve a multisampled resource into a non-multisampled resource.
             */
            void resolve_subresource(const rsl::shared_ptr<Resource>&, const rsl::shared_ptr<Resource>&, u32 dstSubresource = 0, u32 srcSubresource = 0);

            /**
             * Copy the contents to a vertex buffer in GPU memory.
             */
            rsl::shared_ptr<VertexBuffer> copy_vertex_buffer(size_t numVertices, size_t vertexStride, const void* vertexBufferData);
            template<typename T>
            rsl::shared_ptr<VertexBuffer> copy_vertex_buffer(const rsl::vector<T>& vertexBufferData)
            {
                return copy_vertex_buffer(vertexBufferData.size(), sizeof(T), vertexBufferData.data());
            }

            /**
             * Copy the contents to a index buffer in GPU memory.
             */
            rsl::shared_ptr<IndexBuffer> copy_index_buffer(size_t numIndices, DXGI_FORMAT indexFormat, const void* indexBufferData);
            template<typename T>
            rsl::shared_ptr<IndexBuffer> copy_index_buffer(const rsl::vector<T>& indexBufferData)
            {
                assert(sizeof(T) == 2 || sizeof(T) == 4);

                DXGI_FORMAT index_format = (sizeof(T) == 2) ? DXGI_FORMAT_R16_u32 : DXGI_FORMAT_R32_u32;
                return copy_index_buffer(indexBufferData.size(), index_format, indexBufferData.data());
            }

            /**
             * Copy the contents to a constant buffer in GPU memory.
             */
            rsl::shared_ptr<ConstantBuffer> copy_constant_buffer(rsl::memory_size bufferSize, const void* bufferData);
            template<typename T>
            rsl::shared_ptr<ConstantBuffer> copy_constant_buffer(const T& data)
            {
                return copy_constant_buffer(sizeof(T), &data);
            }

            /**
             * Copy the contents to a byte address buffer in GPU memory.
             */
            rsl::shared_ptr<ByteAddressBuffer> copy_byte_address_buffer(rsl::memory_size bufferSize, const void* bufferData);
            template<typename T>
            rsl::shared_ptr<ByteAddressBuffer> copy_byte_address_buffer(const T& data)
            {
                return copy_byte_address_buffer(sizeof(T), &data);
            }

            /**
             * Set the current primitive topology for the rendering pipeline.
             */
            void set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY primitiveTopology);

            /**
            * Clear a texture.
            */
            void clear_texture(const rsl::shared_ptr<Texture>& texture, const float clearColor[4]);

            /**
             * Clear depth/stencil texture.
             */
            void clear_depth_stencil_texture(const rsl::shared_ptr<Texture>& texture, D3D12_CLEAR_FLAGS clearFlags, float depth = 1.0f, u8 stencil = 0);

            /**
             * Copy subresource data to a texture.
            */
            bool copy_texture_subresource(const rsl::shared_ptr<Texture>& texture, u32 firstSubresource, u32 numSubresources, D3D12_SUBRESOURCE_DATA* subresourceData);

            /**
             * Set a dynamic constant buffer data to an inline descriptor in the root
             * signature.
             */
            void set_graphics_dynamic_constant_buffer(u32 rootParameterIndex, rsl::memory_size sizeInBytes, const void* bufferData);
            template<typename T>
            void set_graphics_dynamic_constant_buffer(u32 rootParameterIndex, const T& data)
            {
                set_graphics_dynamic_constant_buffer(rootParameterIndex, sizeof(T), &data);
            }

            /**
             * Set a set of 32-bit constants on the graphics pipeline.
             */
            void set_graphics_32_bit_constants(u32 rootParameterIndex, u32 numConstants, const void* constants);
            template<typename T>
            void set_graphics_32_bit_constants(u32 rootParameterIndex, const T& constants)
            {
                static_assert(sizeof(T) % sizeof(u32) == 0, "Size of type must be a multiple of 4 bytes");
                set_graphics_32_bit_constants(rootParameterIndex, sizeof(T) / sizeof(u32), &constants);
            }

            /**
             * Set the vertex buffer to the rendering pipeline.
             */
            void set_vertex_buffer(u32 slot, const rsl::shared_ptr<VertexBuffer>& vertexBuffer);
            void set_vertex_buffers(u32 startSlot, const rsl::vector<rsl::shared_ptr<VertexBuffer>>& vertexBuffers);

            /**
             * Set dynamic vertex buffer data to the rendering pipeline.
             */
            void set_dynamic_vertex_buffer(u32 slot, size_t numVertices, rsl::memory_size vertexSize, const void* vertexBufferData);
            template<typename T>
            void set_dynamic_vertex_buffer(u32 slot, const rsl::vector<T>& vertexBufferData)
            {
                set_dynamic_vertex_buffer(slot, vertexBufferData.size(), rsl::memory_size(sizeof(T)), vertexBufferData.data());
            }

            /**
             * Bind the index buffer to the rendering pipeline.
             */
            void set_index_buffer(const rsl::shared_ptr<IndexBuffer>& indexBuffer);

            /**
             * Bind dynamic index buffer data to the rendering pipeline.
             */
            void set_dynamic_index_buffer(size_t numIndicies, DXGI_FORMAT indexFormat, const void* indexBufferData);
            template<typename T>
            void set_dynamic_index_buffer(const rsl::vector<T>& indexBufferData)
            {
                static_assert(sizeof(T) == 2 || sizeof(T) == 4);

                DXGI_FORMAT indexFormat = (sizeof(T) == 2) ? DXGI_FORMAT_R16_u32 : DXGI_FORMAT_R32_u32;
                set_dynamic_index_buffer(indexBufferData.size(), indexFormat, indexBufferData.data());
            }

            /**
             * Set viewports.
             */
            void set_viewport(const D3D12_VIEWPORT& viewport);
            void set_viewports(const rsl::vector<D3D12_VIEWPORT>& viewports);

            /**
             * Set scissor rects.
             */
            void set_scissor_rect(const D3D12_RECT& scissorRect);
            void set_scissor_rects(const rsl::vector<D3D12_RECT>& scissorRects);

            /**
             * Set the pipeline state object on the command list.
             */
            void set_pipeline_state(const rsl::shared_ptr<PipelineStateObject>& pipelineState);

            /**
             * Set the current root signature on the command list.
             */
            void set_graphics_root_signature(const rsl::shared_ptr<RootSignature>& rootSignature);

            /**
             * Set an inline CBV.
             *
             * Note: Only ConstantBuffer's can be used with inline CBV's.
             */
            void set_constant_buffer_view(u32 rootParameterIndex, const rsl::shared_ptr<ConstantBuffer>& buffer, D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, size_t bufferOffset = 0);

            /**
             * Set the CBV on the rendering pipeline.
             */
            void set_constant_buffer_view(u32 rootParameterIndex, u32 descriptorOffset, const rsl::shared_ptr<ConstantBufferView>& cbv, D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

            /**
             * Set an inline SRV.
             *
             * Note: Only Buffer resources can be used with inline SRV's
             */
            void set_shader_resource_view(u32 rootParameterIndex, const rsl::shared_ptr<Buffer>& buffer, D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, size_t bufferOffset = 0);
            /**
             * Set the SRV on the graphics pipeline.
             */
            void set_shader_resource_view_with_SRV(u32 rootParameterIndex, u32 descriptorOffset, const rsl::shared_ptr<ShaderResourceView>& srv, D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, u32 firstSubresource = 0, u32 numSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

            /**
             * Set an SRV on the graphics pipeline using the default SRV for the texture.
             */
            void set_shader_resource_view_with_texture(s32 rootParameterIndex, u32 descriptorOffset, const rsl::shared_ptr<Texture>& texture, D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, u32 firstSubresource = 0, u32 numSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);


            /**
             * Set an inline UAV.
             *
             * Note: Only Buffer resoruces can be used with inline UAV's.
             */
            void set_unordered_access_view(u32 rootParameterIndex, const rsl::shared_ptr<Buffer>& buffer, D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS, size_t bufferOffset = 0);
            /**
             * Set the UAV on the graphics pipeline.
             */
            void set_unordered_access_view(u32 rootParameterIndex, u32 descriptorOffset, const rsl::shared_ptr<UnorderedAccessView>& uav, D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS, u32 firstSubresource = 0, u32 numSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

            /**
             * Set the UAV on the graphics pipline using a specific mip of the texture.
             */
            void set_unordered_access_view(u32 rootParameterIndex, u32 descriptorOffset, const rsl::shared_ptr<Texture>& texture, u32 mip, D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS, u32 firstSubresource = 0, u32 numSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

            /**
             * Set the render targets for the graphics rendering pipeline.
             */
            void set_render_target(const RenderTarget& renderTarget);

            /**
             * Draw geometry.
             */
            void draw(u32 vertexCount, u32 instanceCount = 1, u32 startVertex = 0, u32 startInstance = 0);
            void draw_indexed(u32 indexCount, u32 instanceCount = 1, u32 startIndex = 0, s32 baseVertex = 0, u32 startInstance = 0);


        protected:
            friend class CommandQueue;
            friend class DynamicDescriptorHeap;

            friend struct rsl::default_delete<CommandList>;

            CommandList(Device& device, D3D12_COMMAND_LIST_TYPE type);
            virtual ~CommandList();

            /**
             * Close the command list.
             * Used by the command queue.
             *
             * @param pendingCommandList The command list that is used to execute pending
             * resource barriers (if any) for this command list.
             *
             * @return true if there are any pending resource barriers that need to be
             * processed.
             */
            bool close(const rsl::shared_ptr<CommandList>& pendingCommandList);
            // Just close the command list. This is useful for pending command lists.
            void close();

            /**
             * Reset the command list. This should only be called by the CommandQueue
             * before the command list is returned from CommandQueue::GetCommandList.
             */
            bool reset();

            /**
             * Release tracked objects. Useful if the swap chain needs to be resized.
             */
            void release_tracked_objects();

            /**
             * Set the currently bound descriptor heap.
             * Should only be called by the DynamicDescriptorHeap class.
             */
            void set_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, ID3D12DescriptorHeap* heap);


        private:
            void track_resource(wrl::ComPtr<ID3D12Object> object);
            void track_resource(const rsl::shared_ptr<Resource>& res);

            // Copy the contents of a CPU buffer to a GPU buffer (possibly replacing the previous buffer contents).
            wrl::ComPtr<ID3D12Resource> copy_buffer(rsl::memory_size bufferSize, const void* bufferData, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

            // Binds the current descriptor heaps to the command list.
            void bind_descriptor_heaps();

            using TrackedObjects = rsl::vector <wrl::ComPtr<ID3D12Object> >;

            // The device that is used to create this command list.
            Device& m_device;

            D3D12_COMMAND_LIST_TYPE m_d3d_command_list_type;

            wrl::ComPtr<ID3D12GraphicsCommandList2> m_d3d_command_list;
            wrl::ComPtr<ID3D12CommandAllocator> m_d3d_command_allocator;

            // Keep track of the currently bound root signatures to minimize root
            // signature changes.
            ID3D12RootSignature* m_root_signature;
            // Keep track of the currently bond pipeline state object to minimize PSO changes.
            ID3D12PipelineState* m_pipeline_state;

            // resource created in an upload heap. Useful for drawing of dynamic geometry
            // or for uploading constant buffer data that changes every draw call.
            rsl::unique_ptr<UploadBuffer> m_upload_buffer;

            // resource state tracker is used by the command list to track (per command list)
            // the current state of a resource. The resource state tracker also tracks the 
            // global state of a resource in order to minimize resource state transitions.
            rsl::unique_ptr<ResourceStateTracker> m_resource_state_tracker;

            // The dynamic descriptor heap allows for descriptors to be staged before
            // being committed to the command list. Dynamic descriptors need to be
            // committed before a Draw or Dispatch.
            rsl::unique_ptr<DynamicDescriptorHeap> m_dynamic_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

            // Keep track of the currently bound descriptor heaps. Only change descriptor 
            // heaps if they are different than the currently bound descriptor heaps.
            ID3D12DescriptorHeap* m_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

            // Objects that are being tracked by a command list that is "in-flight" on 
            // the command-queue and cannot be deleted. To ensure objects are not deleted 
            // until the command list is finished executing, a reference to the object
            // is stored. The referenced objects are released when the command list is 
            // reset.
            TrackedObjects m_tracked_objects;
        };
    }
}