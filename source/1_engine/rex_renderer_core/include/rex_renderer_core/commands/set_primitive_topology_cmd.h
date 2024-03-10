#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/rendering/primitive_topology.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct SetPrimitiveTopologyCommandDesc
      {
        SetPrimitiveTopologyCommandDesc()
            : topology(PrimitiveTopology::None)
        {
        }

        explicit SetPrimitiveTopologyCommandDesc(PrimitiveTopology primitiveTopology)
            : topology(primitiveTopology)
        {
        }

        PrimitiveTopology topology;
      };

      class SetPrimitiveTopology : public RenderCommand
      {
      public:
        explicit SetPrimitiveTopology(SetPrimitiveTopologyCommandDesc&& desc)
            : RenderCommand()
            , m_desc(rsl::move(desc))
        {
        }

        ~SetPrimitiveTopology() override = default;

        bool execute() override
        {
          return backend::set_primitive_topology(m_desc.topology);
        }

      private:
        SetPrimitiveTopologyCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex