#pragma once

#include "rex_renderer_core/scenegraph/scene.h"
#include "rex_renderer_core/materials/material.h"

#include "glm/glm.hpp"

namespace regina
{
  // The cube scene only exists of a single cube and nothing else
  // It can be used for integration testing, making sure all system are still operational
  class SampleScene2 : public rex::gfx::Scene
  {
  public:
    SampleScene2();

  private:
    void build_geometry();

    void create_cube(const glm::vec3& pos);
    void create_sphere(const glm::vec3& pos);
    void create_cylinder(const glm::vec3& pos);
    void create_grid(const glm::vec3& pos);
    void create_quad(const glm::vec3& pos);

  private:
    rsl::unique_ptr<rex::gfx::Material> m_primitive_material;
  };
} // namespace regina