#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

#include "rex_engine/memory/blob.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct CreateBufferCommandDesc
            {
                memory::Blob buffer_data;
            };

            class CreateBuffer : public RenderCommand
            {
            public:
                CreateBuffer(CreateBufferCommandDesc&& desc, const ResourceSlot& slot)
                    : RenderCommand()
                    , m_desc(rsl::move(desc))
                    , m_resource_slot(slot)
                {}

                ~CreateBuffer() override = default;

            protected:
                const CreateBufferCommandDesc& description() const { return m_desc; }
                const ResourceSlot& resource_slot() const { return m_resource_slot; }

            private:
                CreateBufferCommandDesc m_desc;
                ResourceSlot m_resource_slot;
            };
        } // namespace commands
    }   // namespace renderer
} // namespace rex