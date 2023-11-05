#pragma once

#include "rex_directx/resources/resource.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/wrl/wrl_types.h"

namespace rex
{
    namespace renderer
    {
        class ConstantBufferResource : BaseResource<ID3D12Resource>
        {
        public:
            ConstantBufferResource(const wrl::com_ptr<ID3D12Resource>& uploader)
                :m_uploader(uploader)
            {}
            ~ConstantBufferResource() override
            {
                m_uploader->Unmap(0, nullptr);
            }

            ID3D12Resource* get()
            {
                return m_uploader.Get();
            }
            const ID3D12Resource* get() const
            {
                return m_uploader.Get();
            }

        private:
            wrl::com_ptr<ID3D12Resource> m_uploader;
        };
    }
}