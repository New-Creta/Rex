#pragma once

#include "rex_renderer_core/resource.h"
#include "rex_renderer_core/resource_slot.h"
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
            struct ContantBufferView
            {
                ContantBufferView(const ResourceSlot* commitedResource, s32 bufferByteSize, s32 bufferIndex)
                    :commited_resource(commitedResource)
                    ,buffer_byte_size(bufferByteSize)
                    ,buffer_index(bufferIndex)
                {}

                const ResourceSlot* commited_resource;

                s32 buffer_byte_size;
                s32 buffer_index;
            };
        }

        class ConstantBufferViewResource : public BaseResource<resources::ContantBufferView>
        {
        public:
            RESOURCE_CLASS_TYPE(ConstantBufferViewResource);

            ConstantBufferViewResource(const ResourceSlot* commitedResource, s32 bufferByteSize, s32 bufferIndex)
                :m_commited_buffer_view(commitedResource, bufferByteSize, bufferIndex)
            {}
            ~ConstantBufferViewResource() override = default;

            resources::ContantBufferView* get()
            {
                return &m_commited_buffer_view;
            }
            const resources::ContantBufferView* get() const
            {
                return &m_commited_buffer_view;
            }

        private:
            resources::ContantBufferView m_commited_buffer_view;
        };
    }
}