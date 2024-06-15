#pragma once

#include "imgui/imgui.h"

#include "rex_renderer_core/imgui/imgui_frame_context.h"
#include "rex_renderer_core/imgui/imgui_render_state.h"
#include "rex_renderer_core/gfx/primitive_topology.h"

namespace rex
{
  namespace gfx
  {
    class RenderContext;

    // A viewport is a single standalone imgui widget with one or more widgets inside of it. (eg. the demo window is a viewport)
    class RexImGuiViewport
    {
    public:
      RexImGuiViewport(ImGuiViewport* imguiViewport);

      // Render the viewport using the given render context to queue gpu commands to
      void render(RenderContext& renderContext);

    private:
      // Get the current frame context to use to render the viewport
      ImGuiFrameContext& current_frame_ctx();
      // Advance the index so it points to the next frame context
      void advance_frame_ctx();

      // Setup the render state of the viewport, it for rendering
      void setup_render_state(RenderContext& ctx, ImDrawData* drawData, ImGuiFrameContext& frameCtx);
      // Draw the current viewport
      void draw(RenderContext& ctx, ImDrawData* drawData);

    private:
      ImGuiViewport* m_imgui_viewport;
      rsl::vector<ImGuiFrameContext> m_frame_contexts;
      s32 m_frame_idx;
    };
  }
}