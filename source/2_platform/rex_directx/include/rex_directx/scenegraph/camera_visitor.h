#pragma once

#include "rex_engine/scenegraph/scene_visitor.h"
#include "rex_renderer_core/resource_slot.h"
#include "rex_renderer_core/viewport.h"
#include "rex_renderer_core/scissor_rect.h"

namespace rex
{
    class CameraVisitor : public SceneVisitor
    {
    public:
      CameraVisitor(const renderer::Viewport& viewport, const renderer::ScissorRect& rect);

      void set_clear_state(const renderer::ResourceSlot& clearTarget, const renderer::ResourceSlot& clearState);
      void set_clear_depth_state(const renderer::ResourceSlot& clearTarget, const renderer::ResourceSlot& clearState);

      void set_viewport(const renderer::Viewport& viewport);
      void set_scissor_rect(const renderer::ScissorRect& rect);

      void visit(AbstractSceneNode& sceneNode) override;

    private:
      renderer::ResourceSlot m_clear_target;
      renderer::ResourceSlot m_clear_depth_target;
      renderer::ResourceSlot m_clear_state;
      renderer::ResourceSlot m_clear_depth_state;
      
      renderer::Viewport m_viewport;
      renderer::ScissorRect m_scissor_rect;
    };
}