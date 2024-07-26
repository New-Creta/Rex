#pragma once

#include "rex_renderer_core/scenegraph/scene.h"
#include "rex_renderer_core/resources/mesh.h"
#include "rex_renderer_core/materials/material.h"

namespace regina
{
  // The cube scene only exists of a single cube and nothing else
  // It can be used for integration testing, making sure all system are still operational
  class CubeScene : public rex::gfx::Scene
  {
  public:
    CubeScene();

  private:
    void build_geometry();

  private:
    rsl::unique_ptr<rex::gfx::Mesh> m_cube_mesh;
    rsl::unique_ptr<rex::gfx::Material> m_cube_material;
    //rex::RenderItem* m_cube_render_item;
    //glm::mat4 m_cube_world;
    ;
  };
} // namespace regina