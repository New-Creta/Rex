#pragma once

#include "rex_directx/resources/resource.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/wrl/wrl_types.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct Buffer
            {
                wrl::com_ptr<ID3DBlob> cpu_buffer;
                wrl::com_ptr<ID3D12Resource> gpu_buffer;

                wrl::com_ptr<ID3D12Resource> uploader;

                u32 size_in_bytes;
            };
        }

        class BufferResource : public BaseResource<resources::Buffer>
        {
        public:
            BufferResource(const wrl::com_ptr<ID3DBlob>& cpuBuffer, const wrl::com_ptr<ID3D12Resource>& gpuBuffer, const wrl::com_ptr<ID3D12Resource>& uploader, u32 sizeInBytes)
                :m_buffer_resource({ cpuBuffer , gpuBuffer, uploader, sizeInBytes })
            {}
            ~BufferResource() override
            {
                m_buffer_resource.uploader->Unmap(0, nullptr);
            }

            resources::Buffer* get()
            {
                return &m_buffer_resource;
            }
            const resources::Buffer* get() const
            {
                return &m_buffer_resource;
            }

        private:
            resources::Buffer m_buffer_resource;
        };
    }
}