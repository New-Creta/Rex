#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
  namespace renderer
  {
    //-------------------------------------------------------------------------
    RenderCommand::RenderCommand(RenderCommand&& desc)
        : RenderCommand(rsl::move(desc.command))
, m_desc(rsl::move(desc))
    {
    }

    //-------------------------------------------------------------------------
    RenderCommand::RenderCommand(const RenderCommand& other) = default;
    //-------------------------------------------------------------------------
    RenderCommand::RenderCommand(RenderCommand&& other) = default;
    //-------------------------------------------------------------------------
    RenderCommand::~RenderCommand() = default;

    //-------------------------------------------------------------------------
    RenderCommand& RenderCommand::operator=(const RenderCommand& other) = default;
    //-------------------------------------------------------------------------
    RenderCommand& RenderCommand::operator=(RenderCommand&& other) = default;

    //-------------------------------------------------------------------------
    s64 RenderCommand::frame_id() const
    {
      return m_desc.frame_index;
    }
  } // namespace renderer
} // namespace rex