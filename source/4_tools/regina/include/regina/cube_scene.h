#pragma once

#include "rex_renderer_core/rendering/scene.h"

#include "rex_renderer_core/resources/mesh.h"
#include "rex_renderer_core/rendering/render_item.h"

namespace regina
{
  class CubeScene : public rex::renderer::Scene
  {
  public:
    CubeScene();

  protected:
    void update_object_constant_buffers() override;

  private:
    void build_geometry();

  private:
    rsl::unique_ptr<rex::renderer::Mesh> m_mesh_cube;
    rex::renderer::RenderItem* m_cube_render_item;
    glm::mat4 m_cube_world;;
  };
}