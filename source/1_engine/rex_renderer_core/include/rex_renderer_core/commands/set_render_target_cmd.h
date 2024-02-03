#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      static constexpr u32 s_max_color_targets = 8;

      struct SetRenderTargetCommandDesc
      {
        SetRenderTargetCommandDesc()
            :num_color(0)
            ,color()
            ,depth()
        {}

        s32 num_color;
        rsl::array<ResourceSlot, s_max_color_targets> color;
        ResourceSlot depth;
      };

      class SetRenderTarget : public RenderCommand
      {
      public:
        explicit SetRenderTarget(SetRenderTargetCommandDesc&& desc)
            : RenderCommand()
            , m_desc(rsl::move(desc))
        {
        }

        ~SetRenderTarget() override = default;

        bool execute() override
        {
          return backend::set_render_targets(m_desc.color.data(), m_desc.num_color, m_desc.depth);
        }

      private:
        SetRenderTargetCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex