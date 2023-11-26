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
            struct ConstantBuffer
            {
                ~ConstantBuffer()
                {
                    m_uploader->Unmap(0, nullptr);
                }

                wrl::com_ptr<ID3D12Resource> m_uploader;

                u8* m_mapped_data;
                s32 m_mapped_data_byte_size;
            };
        }

        class ConstantBufferResource : public BaseResource<resources::ConstantBuffer>
        {
        public:
            ConstantBufferResource(const wrl::com_ptr<ID3D12Resource>& uploader, u8* mappedData, s32 mappedDataByteSize)
                :m_constant_buffer({uploader, mappedData, mappedDataByteSize})
            {}
            ~ConstantBufferResource() override = default;

            resources::ConstantBuffer* get()
            {
                return &m_constant_buffer;
            }
            const resources::ConstantBuffer* get() const
            {
                return &m_constant_buffer;
            }

        private:
            resources::ConstantBuffer m_constant_buffer;
        };
    }
}