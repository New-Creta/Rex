#include "rex_directx/scenegraph/camera_node.h"

#include "rex_renderer_core/renderer.h"

#include <glm/gtc/matrix_transform.hpp>

namespace rex
{
  CameraNode::CameraNode(const glm::mat4& localTransformation, const rsl::string& name)
      : SceneNode(localTransformation, name)
      , m_view_dirty(true)
      , m_inv_view_dirty(true)
      , m_proj_dirty(true)
      , m_inv_proj_dirty(true)
      , m_vfov(45.0f)
      , m_aspect(1.0f)
      , m_z_near(0.1f)
      , m_z_far(100.0f)
  {
  }

  void CameraNode::clear(const renderer::ResourceSlot& clearTarget, const renderer::ResourceSlot& clearState)
  {
    renderer::clear_texture(clearTarget, clearState);
  }

  void CameraNode::clear_depth(const renderer::ResourceSlot& clearTarget, const renderer::ResourceSlot& clearState)
  {
    renderer::clear_depth_stencil_texture(clearTarget, clearState);
  }

  void CameraNode::set_lookat(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up)
  {
    m_view = glm::lookAt(eye, target, up);

    set_local_transform(glm::translate(glm::mat4(1.0f), eye));

    m_inv_view_dirty = true;
    m_view_dirty     = false;
  }

  void CameraNode::set_projection(f32 fovy, f32 aspect, f32 zNear, f32 zFar)
  {
    m_vfov        = fovy;
    m_aspect      = aspect;
    m_z_near      = zNear;
    m_z_far       = zFar;

    m_proj_dirty     = true;
    m_inv_proj_dirty  = true;
  }

  void CameraNode::set_fov(f32 fovy)
  {
    if(m_vfov != fovy)
    {
      m_vfov                   = fovy;
      m_proj_dirty             = true;
      m_inv_proj_dirty         = true;
    }
  }

  void CameraNode::set_viewport(const renderer::Viewport& viewport)
  {
    renderer::set_viewport(viewport);
  }
  void CameraNode::set_scissor_rect(const renderer::ScissorRect& rect)
  {
    renderer::set_scissor_rect(rect);
  }

  const glm::mat4& CameraNode::view()
  {
    if(m_view_dirty)
    {
      update_view_matrix();
    }
    return m_view;
  }

  const glm::mat4& CameraNode::inv_view()
  {
    if(m_inv_view_dirty)
    {
      m_inv_view = glm::inverse(m_view);
      m_inv_view_dirty = false;
    }

    return m_inv_view;
  }

  const glm::mat4& CameraNode::proj()
  {
    if(m_proj_dirty)
    {
      update_proj_matrix();
    }

    return m_proj;
  }

  const glm::mat4& CameraNode::inv_proj()
  {
    if(m_inv_proj_dirty)
    {
      update_inv_proj_matrix();
    }

    return m_inv_proj;
  }

  void CameraNode::update_view_matrix()
  {
    m_view = get_local_transform() * -1.0f;

    m_inv_view_dirty = true;
    m_view_dirty       = false;
  }

  void CameraNode::update_inv_view_matrix()
  {
    if(m_view_dirty)
    {
      update_view_matrix();
    }

    m_inv_view = glm::inverse(m_view);
    m_inv_view_dirty           = false;
  }

  void CameraNode::update_proj_matrix()
  {
    m_proj = glm::perspective(glm::radians(m_vfov), m_aspect, m_z_near, m_z_far);

    m_proj_dirty     = false;
    m_inv_proj_dirty  = true;
  }

  void CameraNode::update_inv_proj_matrix()
  {
    if(m_proj_dirty)
    {
      update_proj_matrix();
    }

    m_inv_proj = glm::inverse(m_proj);
    m_inv_proj_dirty                 = false;
  }
} // namespace rex