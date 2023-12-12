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
        RenderCommandDesc command;

        s32 num_color;
        ResourceSlot color[s_max_color_targets];
        ResourceSlot depth;
        s32 array_index;
      };

      class SetRenderTarget : public RenderCommand
      {
      public:
        SetRenderTarget(SetRenderTargetCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~SetRenderTarget() override = default;

        bool execute() override
        {
          rsl::vector<s32> render_targets((rsl::Capacity)renderer::commands::s_max_color_targets);
          for(s32 i = 0; i < renderer::commands::s_max_color_targets; ++i)
            render_targets.push_back(cmd.set_render_target.color[i].slot_id());

          return backend::set_render_targets(render_targets.data(), cmd.set_render_target.num_color, cmd.set_render_target.depth.slot_id());
        }

      private:
        SetRenderTargetCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex