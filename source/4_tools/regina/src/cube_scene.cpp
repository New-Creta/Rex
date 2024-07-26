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
    rex::gfx::Entity cube_entity = create_empty_entity();

    // 2. Add a mesh to the cube
    rex::gfx::StaticMesh box = rex::gfx::mesh_factory::create_box(1.5f, 1.5f, 1.5f);
    rex::gfx::StaticMeshComponent& smc = cube_entity.add_component<rex::gfx::StaticMeshComponent>(rsl::move(box));

    // 3. Initialize the static mesh with a material
    m_cube_material = rex::gfx::load_material(rex::vfs::create_full_path(rex::MountingPoint::EngineMaterials, "default.material"));
    smc.set_material(m_cube_material.get());

    //// Create the vertex buffer
    //rex::memory::Blob vb(rsl::make_unique<rex::VertexPosNormCol[]>(box.vertices().size()));
    //vb.write(box.vertices().data(), vb.size());

    //// Create the index buffer
    //rex::memory::Blob ib(rsl::make_unique<u16[]>(box.indices().size()));
    //ib.write(box.indices().data(), ib.size());

    //// Fill in the constant buffer
    //m_cube_world = glm::scale(m_cube_world, glm::vec3(2.0f, 2.0f, 2.0f));
    //rex::memory::Blob cb(rsl::make_unique<rsl::byte[]>(sizeof(m_cube_world)));
    //cb.write(&m_cube_world, rsl::memory_size(sizeof(m_cube_world)));    

    //rex::VertexBufferDesc vb_desc{ rex::memory::BlobView(vb), sizeof(rex::VertexPosNormCol)/*, rex::VertexPosNormCol::layout()*/ };
    //rex::IndexBufferDesc ib_desc{rex::memory::BlobView(ib), rex::IndexBufferFormat::Uint16, box.indices().size()};

    //// Create the cube mesh object
    //m_mesh_cube = rsl::make_unique<rex::Mesh>("box_geometry"_med, vb_desc, ib_desc);

    //// Meshes can have multiple submeshes.
    //// In this case the submesh points to the entire mesh, therefore we configure it as such
    //rex::Submesh submesh{};
    //submesh.base_vertex_location = 0;
    //submesh.start_index_location = 0;
    //submesh.index_count = box.indices().size();
    //m_mesh_cube->add_submesh("box"_small, submesh);

    //// Pass the mesh to the renderer so it'll render it next frame
    //m_cube_render_item = rex::add_mesh(m_mesh_cube.get(), submesh, rex::memory::BlobView(cb));
  }
}