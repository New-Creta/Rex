#pragma once

#include "rex_directx/utility/directx_util.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_renderer_core/resource_management/resource.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct Buffer
            {
                wrl::ComPtr<ID3DBlob> cpu_buffer{};
                wrl::ComPtr<ID3D12Resource> gpu_buffer{};

                wrl::ComPtr<ID3D12Resource> uploader{};

                u32 size_in_bytes{};
            };
        } // namespace resources

        class BufferResource : public BaseResource<resources::Buffer>
        {
        public:
            RESOURCE_CLASS_TYPE(BufferResource);

            BufferResource(const wrl::ComPtr<ID3DBlob>& cpuBuffer, const wrl::ComPtr<ID3D12Resource>& gpuBuffer, const wrl::ComPtr<ID3D12Resource>& uploader, u32 sizeInBytes)
                :BaseResource(&m_buffer_resource)
                ,m_buffer_resource({ cpuBuffer , gpuBuffer, uploader, sizeInBytes })
            {}
            ~BufferResource() override = default;

        private:
            resources::Buffer m_buffer_resource;
        };
    } // namespace renderer
} // namespace rex