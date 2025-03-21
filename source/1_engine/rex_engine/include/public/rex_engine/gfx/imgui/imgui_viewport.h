#pragma once

#include "imgui/imgui.h"

#include "rex_engine/gfx/imgui/imgui_frame_context.h"
#include "rex_engine/gfx/core/primitive_topology.h"

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace gfx
  {
    class RenderContext;
    class RenderPass;

    // A viewport is a single standalone imgui widget with one or more widgets inside of it. (eg. the demo window is a viewport)
    class RexImGuiViewport
    {
    public:
      RexImGuiViewport(ImGuiViewport* imguiViewport);

      // Render the viewport using the given render context to queue gpu commands to
      void render(RenderContext* renderContext, RenderPass* renderPass);

    private:
      // Get the current frame context to use to render the viewport
      ImGuiFrameContext& current_frame_ctx();
      // Advance the index so it points to the next frame context
      void advance_frame_ctx();

      // Setup the render state of the viewport, it for rendering
      void setup_render_state(RenderContext* ctx, ImGuiFrameContext& frameCtx, s32 constantBufferSlot);
      // Draw the current viewport
      void draw(RenderContext* ctx, ImDrawData* drawData, s32 textureSlot);

    private:
      ImGuiViewport* m_imgui_viewport;

      // There's no special reason for using 3, it's mainly because the main window is also tripple buffered
      // If this causes any issus, just change it
      constexpr static s32 s_number_of_contexts = 3;
      rsl::array<ImGuiFrameContext, s_number_of_contexts> m_frame_contexts;
      s32 m_frame_idx;
    };
  }
}