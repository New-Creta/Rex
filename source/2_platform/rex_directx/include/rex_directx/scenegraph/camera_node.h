#pragma once

#include "rex_engine/scenegraph/scene_node.h"
#include "rex_renderer_core/resource_slot.h"
#include "rex_renderer_core/scissor_rect.h"
#include "rex_renderer_core/viewport.h"

namespace rex
{
  class CameraNode : public SceneNode
  {
  public:
    SCENE_NODE_CLASS_TYPE(CameraNode)

    CameraNode(const glm::mat4& localTransformation = glm::mat4(1.0), const rsl::string& name = rsl::string("CameraNode"));

    void clear(const renderer::ResourceSlot& clearTarget, const renderer::ResourceSlot& clearState);
    void clear_depth(const renderer::ResourceSlot& clearTarget, const renderer::ResourceSlot& clearState);

    void set_lookat(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up);
    void set_projection(f32 fov, f32 aspect, f32 zNear, f32 zFar);
    void set_fov(f32 fov);
    
    void set_viewport(const renderer::Viewport& viewport);
    void set_scissor_rect(const renderer::ScissorRect& rect);

    const glm::mat4& view();
    const glm::mat4& inv_view();
    const glm::mat4& proj();
    const glm::mat4& inv_proj();

  private:
    void update_view_matrix();
    void update_inv_view_matrix();
    void update_proj_matrix();
    void update_inv_proj_matrix();

    glm::mat4 m_view;
    glm::mat4 m_inv_view;
    glm::mat4 m_proj;
    glm::mat4 m_inv_proj;

    bool m_view_dirty;
    bool m_inv_view_dirty;
    bool m_proj_dirty;
    bool m_inv_proj_dirty;

    f32 m_vfov;
    f32 m_aspect;
    f32 m_z_near;
    f32 m_z_far;
  };
} // namespace rex