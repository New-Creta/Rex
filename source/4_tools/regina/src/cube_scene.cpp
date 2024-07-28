#include "regina/cube_scene.h"

#include "rex_renderer_core/primitives/mesh_factory.h"
#include "rex_renderer_core/primitives/box.h"
#include "rex_renderer_core/gfx/vertex.h"
#include "rex_renderer_core/components/transform_component.h"

#include "rex_engine/memory/blob_writer.h"

#include "rex_windows/app/gui_application.h"

#include <glm/gtc/matrix_transform.hpp>

#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/scenegraph/entity.h"
#include "rex_renderer_core/components/mesh_component.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_renderer_core/materials/material_system.h"

namespace regina
{
  CubeScene::CubeScene()
  {
    build_geometry();   

    //m_cube_world = glm::mat4(1.0f);
  }

  //void CubeScene::update_object_constant_buffers()
  //{
  //  m_cube_world = glm::rotate(m_cube_world, 3.14f / 400, glm::vec3(01.0f, 1.0f, 0.0f));
  //  rex::update_buffer(m_cube_render_item->cb(), &m_cube_world, sizeof(m_cube_world));
  //}

  void CubeScene::build_geometry()
  {
    // The cube scene is hardcoded, only holding a single cube

    // 1. Create an empty entity
    {
      rex::gfx::Entity cube_entity = create_empty_entity();

      // 2. Add a mesh to the cube
      rex::gfx::StaticMesh box = rex::gfx::mesh_factory::create_box(1.5f, 1.5f, 1.5f);
      rex::gfx::StaticMeshComponent& smc = cube_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(box));

      // 3. Initialize the static mesh with a material
      m_cube_material = rex::gfx::load_material(rex::vfs::abs_path(rex::MountingPoint::EngineMaterials, "default.material"));
      smc.set_material(m_cube_material.get());
    }

    {
      rex::gfx::Entity cube_entity = create_empty_entity();
      cube_entity.component<rex::gfx::TransformComponent>().translation.x -= 2.0f;
      // 2. Add a mesh to the cube
      rex::gfx::StaticMesh box = rex::gfx::mesh_factory::create_box(1.5f, 1.5f, 1.5f);
      
      rex::gfx::StaticMeshComponent& smc = cube_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(box));
      smc.set_material(m_cube_material.get());

    }
  }
}