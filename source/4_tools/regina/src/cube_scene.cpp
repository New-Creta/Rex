#include "regina/cube_scene.h"

#include "rex_engine/gfx/primitives/mesh_factory.h"
#include "rex_engine/gfx/primitives/box.h"
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

#include "rex_engine/gfx/primitives/sphere.h"

namespace regina
{
  CubeScene::CubeScene()
  {
    build_geometry();   
  }

  void CubeScene::build_geometry()
  {
    // The cube scene is hardcoded, only holding a single cube

    {
      // 1. Create an empty entity
      rex::gfx::Entity cube_entity = add_entity();

      // 2. Add a mesh to the cube
      rex::gfx::StaticMesh box = rex::gfx::mesh_factory::create_box(1.5f, 1.5f, 1.5f);
      rex::gfx::StaticMeshComponent& smc = cube_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(box));

      // 3. Initialize the static mesh with a material
      m_cube_material = rex::gfx::load_material(rex::vfs::abs_path(rex::MountingPoint::EngineMaterials, "default.material"));
      smc.set_material(m_cube_material.get());
    }

    {
      rex::gfx::Entity cube_entity = add_entity();
      cube_entity.component<rex::gfx::TransformComponent>().translation.x -= 2.0f;
      // 2. Add a mesh to the cube
      rex::gfx::StaticMesh box = rex::gfx::mesh_factory::create_box(1.5f, 1.5f, 1.5f);

      rex::gfx::StaticMeshComponent& smc = cube_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(box));
      smc.set_material(m_cube_material.get());

    }
  }
}