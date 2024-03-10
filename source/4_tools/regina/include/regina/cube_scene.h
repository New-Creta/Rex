#pragma once

#include "rex_renderer_core/rendering/scene.h"

#include "rex_renderer_core/resources/mesh.h"

namespace regina
{
  class CubeScene : public rex::renderer::Scene
  {
  public:
    CubeScene();

  private:
    void build_geometry();
    void build_render_items();

  private:
    rsl::unique_ptr<rex::renderer::Mesh> m_mesh_cube;
  };
}