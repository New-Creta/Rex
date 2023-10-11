#pragma once

#include "rex_engine/types.h"
#include "rex_engine/memory/memory_alighment_helper.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/d3dx12.h"
#include "rex_std_extra/utility/yes_no.h"

namespace rex
{
    namespace renderer
    {
        namespace directx
        {
            DEFINE_YES_NO_ENUM(IsConstantBuffer);

            template<typename T>
            class UploadBuffer
            {
            public:
                UploadBuffer(ID3D12Device* device, u32 elementCount, IsConstantBuffer isConstantBuffer);

                UploadBuffer(const UploadBuffer& rhs) = delete;
                UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

                ~UploadBuffer();

            public:
                ID3D12Resource* resource() const;

                void copy_data(int elementIndex, const T& data);

            private:
                wrl::com_ptr<ID3D12Resource> m_upload_buffer;
                BYTE* m_mapped_data = nullptr;

                UINT m_element_byte_size  = 0;
            };

            template<typename T>
            UploadBuffer<T>::UploadBuffer(ID3D12Device* device, u32 elementCount, IsConstantBuffer isConstantBuffer)
            {
                m_element_byte_size = sizeof(T);

                // Constant buffer elements need to be multiples of 256 bytes.
                if(isConstantBuffer)
                {
                  m_element_byte_size = rex::round_up_to_nearest_multiple_of(sizeof(T), 256);
                }

                HRESULT hr = S_OK;

                CD3DX12_HEAP_PROPERTIES upload_heap_properties   = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
                CD3DX12_RESOURCE_DESC contant_buffer_description = CD3DX12_RESOURCE_DESC::Buffer(m_element_byte_size * elementCount);

                hr = (device->CreateCommittedResource(&upload_heap_properties, D3D12_HEAP_FLAG_NONE, &contant_buffer_description, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_upload_buffer));
                REX_ASSERT_X(SUCCEEDED(hr), "Could not created commited resource for constant buffer");

                hr = m_upload_buffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mapped_data));
                REX_ASSERT_X(SUCCEEDED(hr), "Could not map commited resource for constant buffer");

                // We do not need to unmap until we are done with the resource.  However, we must not write to
                // the resource while it is in use by the GPU (so we must use synchronization techniques).
            }

            template <typename T>
            UploadBuffer<T>::~UploadBuffer()
            {
                if(m_upload_buffer != nullptr)
                {
                  m_upload_buffer->Unmap(0, nullptr);
                }

                m_mapped_data = nullptr;
            }

            template <typename T>
            ID3D12Resource* UploadBuffer<T>::resource() const
            {
                return m_upload_buffer.Get();
            }

            template <typename T>
            void UploadBuffer<T>::copy_data(int elementIndex, const T& data)
            {
                memcpy(&m_mapped_data[elementIndex * m_element_byte_size], &data, sizeof(T));
            }
        }
    }
}