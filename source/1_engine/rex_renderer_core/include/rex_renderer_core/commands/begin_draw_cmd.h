#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct BeginDrawDesc
      {
        RenderCommandDesc command;
      };

      class BeginDraw : public RenderCommand
      {
      public:
        BeginDraw(BeginDrawDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~BeginDraw() override = default;

        bool execute() override 
        {
          return backend::begin_draw();  
        }

      private:
        BeginDrawDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex