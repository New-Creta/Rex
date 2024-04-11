#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  struct Viewport;
  struct ScissorRect;

  namespace renderer
  {
    struct OutputWindowUserData;
    struct RenderItemDesc;
    class RenderItem;

    enum class PrimitiveTopology;
    enum class IndexBufferFormat;

    namespace backend
    {
      bool flush_command_queue();

      bool initialize(const OutputWindowUserData& userData);

      void render();
      void shutdown();

      RenderItem* add_render_item(const RenderItemDesc& desc);

      s32 max_frames_in_flight();

      bool set_viewport(const Viewport& viewport);
      bool set_scissor_rect(const ScissorRect& rect);

      // functions to synchronize the render and main threads
      bool new_frame();
      bool end_frame();

      bool begin_draw();
      bool end_draw();

      bool present();
    } // namespace backend
  }   // namespace renderer
} // namespace rex