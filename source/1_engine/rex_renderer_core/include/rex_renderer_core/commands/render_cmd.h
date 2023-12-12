#pragma once

#include "rex_engine/types.h"
#include "rex_renderer_core/renderer.h"

namespace rex
{
  namespace renderer
  {
    class RenderCommand
    {
    public:
      RenderCommand();      
      RenderCommand(const RenderCommand& other);
      RenderCommand(RenderCommand&& other);
      virtual ~RenderCommand();

      RenderCommand& operator=(const RenderCommand& other);
      RenderCommand& operator=(RenderCommand&& other);

      virtual bool execute() = 0;
    };
  } // namespace renderer
} // namespace rex