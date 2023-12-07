#pragma once

#include "rex_directx/resources/resource.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/wrl/wrl_types.h"
#include "rex_engine/memory/memory_allocation.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct ConstantBuffer
            {
                ConstantBuffer(const wrl::com_ptr<ID3D12Resource>& uploader, s32 elementDataByteSize, s32 mappedDataByteSize, s32 bufferIndex)
                    :uploader(uploader)
                    ,mapped_data(nullptr)
                    ,element_data_byte_size(elementDataByteSize)
                    ,mapped_data_byte_size(mappedDataByteSize)
                    ,buffer_index(bufferIndex)
                {

                }

                wrl::com_ptr<ID3D12Resource> uploader;

                u8* mapped_data;
                s32 mapped_data_byte_size;

                s32 element_data_byte_size;

                s32 buffer_index;
            };
        }

        class ConstantBufferResource : public BaseResource<resources::ConstantBuffer>
        {
        public:
            RESOURCE_CLASS_TYPE(ConstantBufferResource);

            ConstantBufferResource(const wrl::com_ptr<ID3D12Resource>& uploader, s32 elementDataByteSize, s32 mappedDataByteSize, s32 bufferIndex)
                :m_constant_buffer(uploader, elementDataByteSize, mappedDataByteSize, bufferIndex)
            {}
            ~ConstantBufferResource() override
            {
                if (m_constant_buffer.uploader)
                {
                    m_constant_buffer.uploader->Unmap(0, nullptr);
                }

                m_constant_buffer.mapped_data = nullptr;
                m_constant_buffer.mapped_data_byte_size = 0;
            }

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