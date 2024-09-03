#pragma once

#include "rex_engine/gfx/scenegraph/scene.h"
#include "rex_engine/gfx/materials/material.h"

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
    rsl::unique_ptr<rex::gfx::Material> m_cube_material;
    ;
  };
} // namespace regina