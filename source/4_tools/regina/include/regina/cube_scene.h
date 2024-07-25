#pragma once

#include "rex_renderer_core/rendering/render_item.h"
#include "rex_renderer_core/scenegraph/scene.h"
#include "rex_renderer_core/resources/mesh.h"

namespace regina
{
  class CubeScene : public rex::gfx::Scene
  {
  public:
    CubeScene();

  protected:
    void update_object_constant_buffers();

  private:
    void build_geometry();

  private:
    rsl::unique_ptr<rex::gfx::Mesh> m_mesh_cube;
    //rex::RenderItem* m_cube_render_item;
    glm::mat4 m_cube_world;
    ;
  };
} // namespace regina