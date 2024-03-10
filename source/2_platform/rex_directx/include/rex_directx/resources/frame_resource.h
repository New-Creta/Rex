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
            struct Frame
            {
                wrl::ComPtr<ID3D12CommandAllocator> cmd_list_allocator{};

                u64 fence{};
            };
        } // namespace resources

        class FrameResource : public BaseResource<resources::Frame>
        {
        public:
            RESOURCE_CLASS_TYPE(FrameResource);

            explicit FrameResource(const wrl::ComPtr<ID3D12CommandAllocator>& allocator)
                : BaseResource(&m_frame)
                ,m_frame({allocator, 0})
            {}
            ~FrameResource() override = default;

        private:
            resources::Frame m_frame;
        };
    } // namespace renderer
} // namespace rex