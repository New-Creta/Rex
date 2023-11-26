#pragma once

#include "rex_directx/resources/resource.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/wrl/wrl_types.h"
#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct Frame
            {
                wrl::com_ptr<ID3D12CommandAllocator> cmd_list_allocator;

                s32 fence;
            };
        }

        class FrameResource : public BaseResource<resources::Frame>
        {
        public:
            FrameResource(const wrl::com_ptr<ID3D12CommandAllocator>& allocator)
                :m_frame({allocator, 0})
            {}
            ~FrameResource() override = default;

            resources::Frame* get()
            {
                return &m_frame;
            }
            const resources::Frame* get() const
            {
                return &m_frame;
            }

        private:
            resources::Frame m_frame;
        };
    }
}