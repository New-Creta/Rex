#pragma once

#include "rex_engine/types.h"
#include "rex_renderer_core/renderer.h"

namespace rex
{
  namespace renderer
  {
    struct RenderCommandDesc
    {
      s64 frame_index = REX_INVALID_INDEX;
    };

    class RenderCommand
    {
    public:
      explicit RenderCommand(RenderCommandDesc&& desc);
      
      RenderCommand(const RenderCommand& other);
      RenderCommand(RenderCommand&& other);

      virtual ~RenderCommand();

      RenderCommand& operator=(const RenderCommand& other);
      RenderCommand& operator=(RenderCommand&& other);

      virtual bool execute() = 0;

      s64 frame_id() const;

    private:
      RenderCommandDesc m_desc;
    };
  } // namespace renderer
} // namespace rex