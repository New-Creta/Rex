#pragma once

#include "rex_renderer_core/resource_descriptions/create_clear_state_desc.h"
#include "rex_renderer_core/iresource.h"

namespace rex
{
    namespace renderer
    {
        class ClearState : public IResource
        {
        public:
          RESOURCE_CLASS_TYPE(ClearState);

          virtual ~ClearState();

          rsl::Color4f  rgba()  const;
          f32           depth() const;
          u8            stencil() const;
          ClearBits     flags() const;

        protected:
          ClearState();
          ClearState(CreateClearStateDesc&& desc);

        private:
          CreateClearStateDesc m_desc;
        };
    }
}