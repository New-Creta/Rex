#pragma once

#include "imgui/imgui.h"

#include "rex_renderer_core/imgui/imgui_frame_context.h"
#include "rex_renderer_core/imgui/imgui_render_state.h"
#include "rex_renderer_core/rhi/primitive_topology.h"

namespace rex
{
  namespace rhi
  {
    class RootSignature;
    class PipelineState;
    class ConstantBuffer;
    class CommandList;
    class RenderContext;
  };

  class RexImGuiViewport
  {
  public:
    RexImGuiViewport(ImGuiViewport* imguiViewport);

    void render(rhi::RenderContext& renderContext);

  private:
    ImGuiFrameContext& current_frame_ctx();
    void advance_frame_ctx();

    void setup_render_state(rhi::RenderContext& ctx, ImDrawData* drawData, ImGuiFrameContext& frameCtx);
    void draw(rhi::RenderContext& ctx, ImDrawData* drawData);

  private:
    ImGuiViewport* m_imgui_viewport;
    rsl::vector<ImGuiFrameContext> m_frame_contexts;
    s32 m_frame_idx;
  };
}