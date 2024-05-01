#include "regina/cube_scene.h"

#include "rex_renderer_core/primitives/mesh_factory.h"
#include "rex_renderer_core/primitives/box.h"
#include "rex_renderer_core/rendering/vertex.h"

#include "rex_engine/memory/blob_writer.h"

#include "rex_windows/app/gui_application.h"

#include <glm/gtc/matrix_transform.hpp>

#include "rex_renderer_core/system/rhi.h"

namespace regina
{
  CubeScene::CubeScene()
  {
    build_geometry();   

    m_cube_world = glm::mat4(1.0f);
  }

  void CubeScene::update_object_constant_buffers()
  {
    m_cube_world = glm::rotate(m_cube_world, 3.14f / 400, glm::vec3(01.0f, 1.0f, 0.0f));
    rex::rhi::update_buffer(m_cube_render_item->cb(), &m_cube_world, sizeof(m_cube_world));
  }

  void CubeScene::build_geometry()
  {
    // Create the box geometry
    auto box = rex::mesh_factory::create_box(1.5f, 1.5f, 1.5f);

    // Create the vertex buffer
    rex::memory::Blob vb(rsl::make_unique<rex::renderer::VertexPosNormCol[]>(box.vertices().size()));
    vb.write(box.vertices().data(), vb.size());

    // Create the index buffer
    rex::memory::Blob ib(rsl::make_unique<u16[]>(box.indices().size()));
    ib.write(box.indices().data(), ib.size());

    // Fill in the constant buffer
    m_cube_world = glm::scale(m_cube_world, glm::vec3(2.0f, 2.0f, 2.0f));
    rex::memory::Blob cb(rsl::make_unique<rsl::byte[]>(sizeof(m_cube_world)));
    cb.write(&m_cube_world, rsl::memory_size(sizeof(m_cube_world)));    

    rex::rhi::VertexBufferDesc vb_desc{ rex::memory::BlobView(vb), sizeof(rex::renderer::VertexPosNormCol)/*, rex::renderer::VertexPosNormCol::layout()*/ };
    rex::rhi::IndexBufferDesc ib_desc{rex::memory::BlobView(ib), rex::renderer::IndexBufferFormat::Uint16, box.indices().size()};

    // Create the cube mesh object
    m_mesh_cube = rsl::make_unique<rex::renderer::Mesh>("box_geometry"_med, vb_desc, ib_desc);

    // Meshes can have multiple submeshes.
    // In this case the submesh points to the entire mesh, therefore we configure it as such
    rex::renderer::Submesh submesh{};
    submesh.base_vertex_location = 0;
    submesh.start_index_location = 0;
    submesh.index_count = box.indices().size();
    m_mesh_cube->add_submesh("box"_small, submesh);

    // Pass the mesh to the renderer so it'll render it next frame
    m_cube_render_item = rex::renderer::add_mesh(m_mesh_cube.get(), submesh, rex::memory::BlobView(cb));
  }
}