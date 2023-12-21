#pragma once

#include "rex_renderer_core/resource.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/wrl/wrl_types.h"
#include "rex_engine/types.h"
#include "rex_engine/memory/memory_allocation.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct CommitedBuffer
            {
                CommitedBuffer(const wrl::com_ptr<ID3D12Resource>& uploader, s32 elementDataByteSize, s32 mappedDataByteSize)
                    :uploader(uploader)
                    ,mapped_data(nullptr)
                    ,element_data_byte_size(elementDataByteSize)
                    ,mapped_data_byte_size(mappedDataByteSize)
                {

                }

                wrl::com_ptr<ID3D12Resource> uploader;

                u8* mapped_data;
                s32 mapped_data_byte_size;

                s32 element_data_byte_size;
            };
        }

        class CommitedBufferResource : public BaseResource<resources::CommitedBuffer>
        {
        public:
            RESOURCE_CLASS_TYPE(CommitedBufferResource);

            CommitedBufferResource(const wrl::com_ptr<ID3D12Resource>& uploader, s32 elementDataByteSize, s32 mappedDataByteSize)
                :m_commited_buffer(uploader, elementDataByteSize, mappedDataByteSize)
            {}
            ~CommitedBufferResource() override
            {
                if (m_commited_buffer.uploader)
                {
                    m_commited_buffer.uploader->Unmap(0, nullptr);
                }

                m_commited_buffer.mapped_data = nullptr;
                m_commited_buffer.mapped_data_byte_size = 0;
            }

            resources::CommitedBuffer* get()
            {
                return &m_commited_buffer;
            }
            const resources::CommitedBuffer* get() const
            {
                return &m_commited_buffer;
            }

        private:
            resources::CommitedBuffer m_commited_buffer;
        };
    }
}