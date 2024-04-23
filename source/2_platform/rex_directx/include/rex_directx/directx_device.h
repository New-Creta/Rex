#pragma once

#include "rex_engine/engine/types.h"
#include "rex_dxgi/wrl/comobject.h"
#include "rex_dxgi/dxgi_util.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/descriptors/descriptor_allocation.h"

#include "rex_std/memory.h"

namespace rex
{
    namespace dxgi
    {
      class Adapter;
    }

    namespace renderer
    {
        class CommandQueue;
        class DescriptorAllocator;
        class VertexBuffer;
        class IndexBuffer;
        class ConstantBuffer;
        class ByteAddressBuffer;
        class Texture;
        class PipelineStateObject;
        class ConstantBufferView;
        class ShaderResourceView;
        class UnorderedAccessView;
        class RootSignature;
        class Resource;
        class Swapchain;

        struct GpuDescription;

        class Device : public wrl::ComObject<ID3D12Device2>
        {
        public:
            /**
             * Always enable the debug layer before doing anything DX12 related so all possible errors generated while creating
             * DX12 objects are caught by the debug layer.
             */
            static bool enable_debug_layer();

            /**
             * Reports info about the lifetime of an object or objects.
            */
            static void report_live_objects();

            /**
             * Create a new device
            */
            static rsl::shared_ptr<Device> create();

        public:
            /**
            * Get information about the selected adapter
            */
            const GpuDescription& adapter_description() const;

            /**
             * Get the adapter that was used to create this device.
             */
            IDXGIAdapter4* dxgi_adapter();

            /**
            * Get the adapter that was used to create this device.
            */
            const IDXGIAdapter4* dxgi_adapter() const;

            /**
             * Get the d3d device.
             */
            ID3D12Device2* d3d_device();

            /**
            * Get the d3d device.
            */
            const ID3D12Device2* d3d_device() const;

            /**
             * Get a command queue. Valid types are:
             * - D3D12_COMMAND_LIST_TYPE_DIRECT : Can be used for draw, dispatch, or copy commands.
             * - D3D12_COMMAND_LIST_TYPE_COMPUTE: Can be used for dispatch or copy commands.
             * - D3D12_COMMAND_LIST_TYPE_COPY   : Can be used for copy commands.
             * By default, a D3D12_COMMAND_LIST_TYPE_DIRECT queue is returned.
             */
            CommandQueue& command_queue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

            /**
             * Allocate a number of CPU visible descriptors.
             */
            DescriptorAllocation allocate_descriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, u32 numDescriptors = 1);

            /**
             * Release stale descriptors. This should only be called with a completed frame counter.
             */
            void release_stale_descriptors();

            /**
             * Create a ConstantBuffer from a given ID3D12Resoure.
             */
            rsl::shared_ptr<ConstantBuffer> create_constant_buffer(wrl::ComPtr<ID3D12Resource> resource);

            /**
             * Create a ByteAddressBuffer resource.
             *
             * @param resDesc A description of the resource.
             */
            rsl::shared_ptr<ByteAddressBuffer> create_byte_address_buffer(rsl::memory_size bufferSize);
            rsl::shared_ptr<ByteAddressBuffer> create_byte_address_buffer(wrl::ComPtr<ID3D12Resource> resource);

            /**
             * Create a Texture resource.
             *
             * @param resourceDesc A description of the texture to create.
             * @param [clearVlue] Optional optimized clear value for the texture.
             * @param [textureUsage] Optional texture usage flag provides a hint about how the texture will be used.
             *
             * @returns A pointer to the created texture.
             */
            rsl::shared_ptr<Texture> create_texture(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue = nullptr);
            rsl::shared_ptr<Texture> create_texture(wrl::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clearValue = nullptr);

            rsl::shared_ptr<IndexBuffer> create_index_buffer(size_t numIndices, DXGI_FORMAT indexFormat);
            rsl::shared_ptr<IndexBuffer> create_index_buffer(wrl::ComPtr<ID3D12Resource> resource, size_t numIndices, DXGI_FORMAT indexFormat);

            rsl::shared_ptr<VertexBuffer> create_vertex_buffer(size_t numVertices, size_t vertexStride);
            rsl::shared_ptr<VertexBuffer> create_vertex_buffer(wrl::ComPtr<ID3D12Resource> resource, size_t numVertices, size_t vertexStride);

            rsl::shared_ptr<RootSignature> create_root_signature(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc);

            template<class TPipelineStateStream>
            rsl::shared_ptr<PipelineStateObject> create_pipeline_state_object(TPipelineStateStream& pipelineStateStream);

            rsl::shared_ptr<ConstantBufferView> create_constant_buffer_view(const rsl::shared_ptr<ConstantBuffer>& constantBuffer, size_t offset = 0);
            rsl::shared_ptr<ShaderResourceView> create_shader_resource_view(const rsl::shared_ptr<Resource>& resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srv = nullptr);
            rsl::shared_ptr<UnorderedAccessView> create_unordered_access_view(const rsl::shared_ptr<Resource>& inResource, const rsl::shared_ptr<Resource>& inCounterResource = nullptr, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav = nullptr);

            /**
            * Flush all command queues.
            */
            void flush();

            /**
            * Get the highest root signature version
            */
            D3D_ROOT_SIGNATURE_VERSION highest_root_signature_version() const;

            /**
             * Check if the requested multisample quality is supported for the given format.
             */
            DXGI_SAMPLE_DESC multisample_quality_levels(DXGI_FORMAT format, u32 numSamples = D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT, D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE) const;
            
            /**
            * Check if tearing is supported
            */
            bool is_tearing_supported() const;

        protected:
            /**
            * The device's constructor should always be private to prevent direct construction calls
            */
          Device(rsl::shared_ptr<dxgi::Adapter> adaptor, wrl::ComPtr<ID3D12Device2> device);

            virtual ~Device();

        private:
            /**
             * Execute logic to create the pipeline state object
            */
            rsl::shared_ptr<PipelineStateObject> do_create_pipeline_state_object( const D3D12_PIPELINE_STATE_STREAM_DESC& pipelineStateStreamDesc );

        private:
            rsl::shared_ptr<dxgi::Adapter> m_adapter;

            rsl::unique_ptr<CommandQueue> m_direct_command_queue;
            rsl::unique_ptr<CommandQueue> m_compute_command_queue;
            rsl::unique_ptr<CommandQueue> m_copy_command_queue;

            D3D_ROOT_SIGNATURE_VERSION m_highest_root_signature_version;

            bool m_tearing_supported;

            rsl::unique_ptr<DescriptorAllocator> m_descriptor_allocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
        };

        template<class TPipelineStateStream>
        rsl::shared_ptr<PipelineStateObject> Device::create_pipeline_state_object(TPipelineStateStream& pipelineStateStream)
        {
            D3D12_PIPELINE_STATE_STREAM_DESC pipeline_state_stream_desc = { sizeof(TPipelineStateStream), &pipelineStateStream };

            return do_create_pipeline_state_object(pipeline_state_stream_desc);
        }
    }
}