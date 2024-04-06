#pragma once

#include "rex_renderer_core/primitives/mesh_factory.h"
#include "rex_renderer_core/rendering/scene.h"
#include "rex_renderer_core/rendering/vertex.h"
#include "rex_renderer_core/resource_management/resource.h"
#include "rex_renderer_core/resources/mesh.h"
#include "rex_std/bonus/string.h"
#include "rex_std/memory.h"
#include "rex_std/unordered_map.h"

namespace regina
{
  class SampleScene : public rex::renderer::Scene
  {
  public:
    SampleScene();

  protected:
    void update_object_constant_buffers() override;

  private:
    void build_geometry();

    rex::renderer::Submesh build_submesh(const rex::mesh_factory::MeshData16& meshData, rsl::vector<rex::renderer::VertexPosNormCol>& vertices, rsl::vector<u16>& indices);

  private:
    rsl::unique_ptr<rex::renderer::Mesh> m_geometry;
    rex::memory::Blob m_vb;
    rex::memory::Blob m_ib;
  };
} // namespace regina