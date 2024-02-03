#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct PresentCommandDesc
      {
        PresentCommandDesc()
            : front_buffer_color(nullptr)
            , back_buffer_color(nullptr)
        {
        }

        PresentCommandDesc(ResourceSlot* fbc, ResourceSlot* bbc)
            : front_buffer_color(fbc)
            , back_buffer_color(bbc)
        {
        }

        ResourceSlot* front_buffer_color;
        ResourceSlot* back_buffer_color;
      };

      class Present : public RenderCommand
      {
      public:
        //-------------------------------------------------------------------------
        explicit Present(PresentCommandDesc&& desc)
            : RenderCommand()
            , m_desc(rsl::move(desc))
        {
        }

        //-------------------------------------------------------------------------
        bool execute() override
        {
          const bool result = backend::present();

          rsl::swap(*m_desc.front_buffer_color, *m_desc.back_buffer_color);

          return result;
        }

      private:
        PresentCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex