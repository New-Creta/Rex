#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct BeginDrawCommandDesc
      {
      };

      class BeginDraw : public RenderCommand
      {
      public:
        explicit BeginDraw(BeginDrawCommandDesc&& desc)
            : RenderCommand()
            , m_desc(rsl::move(desc))
        {
        }

        ~BeginDraw() override = default;

        bool execute() override 
        {
          return backend::begin_draw();  
        }

      private:
          BeginDrawCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex