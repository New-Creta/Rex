#include "regina/sample_scene2.h"

#include "rex_renderer_core/primitives/mesh_factory.h"
#include "rex_renderer_core/primitives/box.h"
#include "rex_renderer_core/primitives/cylinder.h"
#include "rex_renderer_core/primitives/grid.h"
#include "rex_renderer_core/primitives/quad.h"
#include "rex_renderer_core/primitives/sphere.h"
#include "rex_renderer_core/gfx/vertex.h"
#include "rex_renderer_core/components/transform_component.h"

#include "rex_engine/memory/blob_writer.h"

#include "rex_windows/app/gui_application.h"

#include "glm/gtc/matrix_transform.hpp"

#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/scenegraph/entity.h"
#include "rex_renderer_core/components/mesh_component.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_renderer_core/materials/material_system.h"

namespace regina
{
  SampleScene2::SampleScene2()
  {
    build_geometry();
  }

  void SampleScene2::build_geometry()
  {
    // The sample scene is hardcoded, only holding a few primitives

    // A default material, used by all objects
    m_primitive_material = rex::gfx::load_material(rex::vfs::abs_path(rex::MountingPoint::EngineMaterials, "default.material"));

    create_cube(    glm::vec3(+0.0f, 0.0f, 0.0f));
    create_sphere(  glm::vec3(+2.0f, 0.0f, 0.0f));
    create_quad(    glm::vec3(+4.0f, 0.0f, 0.0f));
    //create_grid(    glm::vec3(-1.0f, 0.0f, 0.0f));
    create_cylinder(glm::vec3(-4.0f, 0.0f, 0.0f));
  }

  void SampleScene2::create_cube(const glm::vec3& pos)
  {
    rex::gfx::Entity entity = add_entity();
    rex::gfx::StaticMesh mesh = rex::gfx::mesh_factory::create_box(1.5f, 1.5f, 1.5f);
    rex::gfx::StaticMeshComponent& smc = entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(mesh));
    entity.component<rex::gfx::TransformComponent>().translation = pos;
    smc.set_material(m_primitive_material.get());
  }
  void SampleScene2::create_sphere(const glm::vec3& pos)
  {
    rex::gfx::Entity entity = add_entity();
    rex::gfx::StaticMesh mesh = rex::gfx::mesh_factory::create_sphere(1.0f, 20, 20);
    rex::gfx::StaticMeshComponent& smc = entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(mesh));
    entity.component<rex::gfx::TransformComponent>().translation = pos;
    smc.set_material(m_primitive_material.get());
  }
  void SampleScene2::create_cylinder(const glm::vec3& pos)
  {
    rex::gfx::Entity entity = add_entity();
    rex::gfx::StaticMesh mesh = rex::gfx::mesh_factory::create_cylinder(1.5f, 1.5f, 5.0f, 20, 5);
    rex::gfx::StaticMeshComponent& smc = entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(mesh));
    entity.component<rex::gfx::TransformComponent>().translation = pos;
    smc.set_material(m_primitive_material.get());
  }
  void SampleScene2::create_grid(const glm::vec3& pos)
  {
    rex::gfx::Entity entity = add_entity();
    rex::gfx::StaticMesh mesh = rex::gfx::mesh_factory::create_grid(20.0f, 30.0f, 60, 40);
    rex::gfx::StaticMeshComponent& smc = entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(mesh));
    entity.component<rex::gfx::TransformComponent>().translation = pos;
    smc.set_material(m_primitive_material.get());
  }
  void SampleScene2::create_quad(const glm::vec3& pos)
  {
    rex::gfx::Entity entity = add_entity();
    rex::gfx::StaticMesh mesh = rex::gfx::mesh_factory::create_quad(1.5f, 1.5f);
    rex::gfx::StaticMeshComponent& smc = entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(mesh));
    entity.component<rex::gfx::TransformComponent>().translation = pos;
    smc.set_material(m_primitive_material.get());
  }

}