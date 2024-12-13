#include "regina/sample_scene2.h"

#include "rex_engine/gfx/primitives/mesh_factory.h"
#include "rex_engine/gfx/primitives/box.h"
#include "rex_engine/gfx/primitives/cylinder.h"
#include "rex_engine/gfx/primitives/grid.h"
#include "rex_engine/gfx/primitives/quad.h"
#include "rex_engine/gfx/primitives/sphere.h"
#include "rex_engine/gfx/core/vertex.h"
#include "rex_engine/components/transform_component.h"

#include "rex_engine/memory/blob_writer.h"

#include "rex_windows/app/gui_application.h"

#include "glm/gtc/matrix_transform.hpp"

#include "rex_engine/gfx/system/gal.h"
#include "rex_engine/entities/entity.h"
#include "rex_engine/gfx/components/mesh_component.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/gfx/materials/material_system.h"

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
    rex::gfx::StaticMesh mesh = rex::gfx::mesh_factory::create_grid(10, 30, 2.0f);
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