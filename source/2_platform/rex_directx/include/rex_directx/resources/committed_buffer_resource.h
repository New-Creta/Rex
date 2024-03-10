#pragma once

#include "rex_directx/utility/directx_util.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_renderer_core/resource_management/resource.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct CommittedBuffer
            {
                CommittedBuffer(const wrl::ComPtr<ID3D12Resource>& uploader, s32 elementDataByteSize, s32 mappedDataByteSize)
                    :uploader(uploader)
                    ,mapped_data(nullptr)
                    ,element_data_byte_size(elementDataByteSize)
                    ,mapped_data_byte_size(mappedDataByteSize)
                {

                }

                wrl::ComPtr<ID3D12Resource> uploader{};

                u8* mapped_data;
                s32 mapped_data_byte_size;

                s32 element_data_byte_size;
            };
        } // namespace resources

        class CommittedBufferResource : public BaseResource<resources::CommittedBuffer>
        {
        public:
            RESOURCE_CLASS_TYPE(CommittedBufferResource);

            CommittedBufferResource(const wrl::ComPtr<ID3D12Resource>& uploader, s32 elementDataByteSize, s32 mappedDataByteSize)
                : BaseResource(&m_committed_buffer)
                , m_committed_buffer(uploader, elementDataByteSize, mappedDataByteSize)
            {}
            ~CommittedBufferResource() override
            {
                if (m_committed_buffer.uploader)
                {
                    m_committed_buffer.uploader->Unmap(0, nullptr);
                }

                m_committed_buffer.mapped_data = nullptr;
                m_committed_buffer.mapped_data_byte_size = 0;
            }

        private:
            resources::CommittedBuffer m_committed_buffer;
        };
    } // namespace renderer
} // namespace rex