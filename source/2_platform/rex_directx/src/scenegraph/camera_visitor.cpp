#include "rex_directx/scenegraph/camera_visitor.h"

#include "rex_directx/scenegraph/camera_node.h"

namespace rex
{
  CameraVisitor::CameraVisitor(const renderer::Viewport& viewport, const renderer::ScissorRect& rect)
      : m_viewport(viewport)
      , m_scissor_rect(rect)
  {
  }

  void CameraVisitor::set_clear_state(const renderer::ResourceSlot& clearTarget, const renderer::ResourceSlot& clearState)
  {
    m_clear_target = clearTarget;
    m_clear_state  = clearState;
  }

  void CameraVisitor::set_clear_depth_state(const renderer::ResourceSlot& clearTarget, const renderer::ResourceSlot& clearState)
  {
    m_clear_depth_target = clearTarget;
    m_clear_depth_state  = clearState;
  }

  void CameraVisitor::set_viewport(const renderer::Viewport& viewport)
  {
    m_viewport = viewport;
  }
  void CameraVisitor::set_scissor_rect(const renderer::ScissorRect& rect)
  {
    m_scissor_rect = rect;
  }

  void CameraVisitor::visit(AbstractSceneNode& sceneNode)
  {
    if(sceneNode.type() == rsl::type_id<CameraNode>().hash_code())
    {
      CameraNode& camera_node = static_cast<CameraNode&>(sceneNode);

      camera_node.clear(m_clear_target, m_clear_state);
      camera_node.clear_depth(m_clear_depth_target, m_clear_depth_state);

      camera_node.set_viewport(m_viewport);
      camera_node.set_scissor_rect(m_scissor_rect);
    }
  }
} // namespace rex