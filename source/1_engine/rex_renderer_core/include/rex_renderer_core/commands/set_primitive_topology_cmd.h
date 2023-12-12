#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/primitive_topology.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct SetPrimitiveTopologyCommandDesc
      {
        RenderCommandDesc command;

        PrimitiveTopology topology;
      };

      class SetPrimitiveTopology : public RenderCommand
      {
      public:
        SetPrimitiveTopology(SetPrimitiveTopologyCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~SetPrimitiveTopology() override = default;

        bool execute() override 
        {
          return backend::set_primitive_topology(cmd.topology);
        }

      private:
        SetPrimitiveTopologyCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex