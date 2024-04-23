#pragma once

#include "rex_engine/engine/types.h"

#include "rex_directx/directx_util.h"
#include "rex_directx/descriptors/descriptor_allocation.h"
#include "rex_renderer_core/iresource.h"

namespace rex
{
    namespace renderer
    {
        class Resource;
        class Device;

        class ShaderResourceView : public IResource
        {
        public:
            RESOURCE_CLASS_TYPE(ShaderResourceView);

            rsl::shared_ptr<Resource> resource() const;

            D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle() const;

        protected:
            ShaderResourceView(Device& device, const rsl::shared_ptr<Resource>& resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srv = nullptr);
            virtual ~ShaderResourceView();

        private:
            Device& m_device;
            rsl::shared_ptr<Resource> m_resource;
            DescriptorAllocation m_descriptor_allocation;
        };
    }
}