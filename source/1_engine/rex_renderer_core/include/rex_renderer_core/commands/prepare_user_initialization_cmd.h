#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct PrepareUserInitializationCommandDesc
      {
      };

      class PrepareUserInitialization : public RenderCommand
      {
      public:
        explicit PrepareUserInitialization(PrepareUserInitializationCommandDesc&& desc)
            : RenderCommand()
            , m_desc(rsl::move(desc))
        {
        }

        ~PrepareUserInitialization() override = default;

        bool execute() override
        {
          return backend::prepare_user_initialization();
        }

      private:
        PrepareUserInitializationCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex