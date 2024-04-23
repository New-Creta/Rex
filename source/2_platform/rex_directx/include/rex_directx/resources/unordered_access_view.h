#pragma once

#include "rex_engine/engine/types.h"

#include "rex_directx/directx_util.h"
#include "rex_directx/descriptors/descriptor_allocation.h"
#include "rex_renderer_core/iresource.h"

#include "rex_std/memory.h"

namespace rex
{
    namespace renderer
    {
        class Resource;
        class Device;

        class UnorderedAccessView : public IResource
        {
        public:
            RESOURCE_CLASS_TYPE(UnorderedAccessView);

            rsl::shared_ptr<Resource> resource() const;
            rsl::shared_ptr<Resource> counter_resource() const;

            D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle() const;

        protected:
            UnorderedAccessView(Device& device, const rsl::shared_ptr<Resource>& inResource, const rsl::shared_ptr<Resource>& inCounterResource = nullptr, const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav = nullptr);
            virtual ~UnorderedAccessView();

        private:
            Device& m_device;
            rsl::shared_ptr<Resource> m_resource;
            rsl::shared_ptr<Resource> m_counter_resource;
            DescriptorAllocation m_descriptor_allocation;
        };
    }
}