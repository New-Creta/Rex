#include "regina/sample_scene.h"

#include "rex_engine/app/core_application.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_renderer_core/primitives/box.h"
#include "rex_renderer_core/primitives/cylinder.h"
#include "rex_renderer_core/primitives/grid.h"
#include "rex_renderer_core/primitives/mesh_factory.h"
#include "rex_renderer_core/primitives/sphere.h"
#include "rex_renderer_core/gfx/depth_info.h"
#include "rex_renderer_core/gfx/vertex.h"
#include "rex_renderer_core/rendering/renderer.h"
#include "rex_renderer_core/gfx/rhi.h"
#include "rex_std/bonus/math/color.h"
#include "rex_windows/app/gui_application.h"

#include <glm/gtc/matrix_transform.hpp>

#include "rex_renderer_core/materials/material_system.h"
#include "rex_renderer_core/components/mesh_component.h"
#include "rex_renderer_core/components/transform_component.h"

namespace regina
{
  SampleScene::SampleScene()
  {
    build_geometry();

    // input layout should be tied to the render item

    // shader and pso should be part of the material system
  }

  void SampleScene::build_geometry()
  {
    m_primitive_material = rex::gfx::load_material(rex::vfs::abs_path(rex::MountingPoint::EngineMaterials, "default.material"));

    auto box      = rex::gfx::mesh_factory::create_box(1.5f, 1.5f, 1.5f);
    rex::gfx::Entity box_entity = add_entity();
    box_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(box)).set_material(m_primitive_material.get());
    box_entity.component<rex::gfx::TransformComponent>().scale *= 2.0f;

    auto grid     = rex::gfx::mesh_factory::create_grid(20.0f, 30.0f, 60, 40);
    rex::gfx::Entity grid_entity = add_entity();
    grid_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(grid)).set_material(m_primitive_material.get());

    // Spheres and Cylinders
    for(s32 i = 0; i < 5; ++i)
    {
      // Left
      create_pillar(glm::vec3(-5.0f, 0.0f, -10.0f + static_cast<f32>(i) * 5.0f));
      // Right
      create_pillar(glm::vec3(+5.0f, 0.0f, -10.0f + static_cast<f32>(i) * 5.0f));
    }
  }

  void SampleScene::create_pillar(const glm::vec3& pos)
  {
    rex::gfx::Entity sphere_entity = add_entity();
    rex::gfx::Entity cylinder_entity = add_entity();

    auto sphere = rex::gfx::mesh_factory::create_sphere(0.5f, 20, 20);
    auto cylinder = rex::gfx::mesh_factory::create_cylinder(0.5f, 0.3f, 3.0f, 20, 20);

    sphere_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(sphere)).set_material(m_primitive_material.get());
    cylinder_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(cylinder)).set_material(m_primitive_material.get());

    sphere_entity.component<rex::gfx::TransformComponent>().translation = pos;
    sphere_entity.component<rex::gfx::TransformComponent>().translation.y = 1.5f;

    cylinder_entity.component<rex::gfx::TransformComponent>().translation = pos;
    cylinder_entity.component<rex::gfx::TransformComponent>().translation.y = 3.5f;
  }


} // namespace regina
