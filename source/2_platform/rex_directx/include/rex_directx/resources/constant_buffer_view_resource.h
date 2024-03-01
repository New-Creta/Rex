#pragma once

#include <utility>


#include "rex_directx/directx_util.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_renderer_resources/resource.h"
#include "rex_renderer_resources/resource_slot.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct ContantBufferView
            {
                ContantBufferView(ResourceSlot committedResource, s32 bufferByteSize, s32 bufferIndex)
                    :committed_resource(std::move(std::move(committedResource)))
                    ,buffer_byte_size(bufferByteSize)
                    ,buffer_index(bufferIndex)
                {}

                ResourceSlot committed_resource;

                s32 buffer_byte_size;
                s32 buffer_index;
            };
        } // namespace resources

        class ConstantBufferViewResource : public BaseResource<resources::ContantBufferView>
        {
        public:
            RESOURCE_CLASS_TYPE(ConstantBufferViewResource);

            ConstantBufferViewResource(ResourceSlot committedResource, s32 bufferByteSize, s32 bufferIndex)
                : BaseResource(&m_committed_buffer_view)
                ,m_committed_buffer_view(std::move(committedResource), bufferByteSize, bufferIndex)
            {}
            ~ConstantBufferViewResource() override = default;

        private:
            resources::ContantBufferView m_committed_buffer_view;
        };
    } // namespace renderer
} // namespace rex