#pragma once

#include "rex_renderer_core/primitives/mesh_factory.h"
#include "rex_renderer_core/scenegraph/scene.h"
#include "rex_renderer_core/gfx/vertex.h"
#include "rex_renderer_core/materials/material.h"

#include "rex_std/bonus/string.h"
#include "rex_std/memory.h"
#include "rex_std/unordered_map.h"

namespace regina
{
 class SampleScene : public rex::gfx::Scene
 {
 public:
   SampleScene();

 private:
   void build_geometry();

   void create_pillar(const glm::vec3& pos);

 private:
   rsl::unique_ptr<rex::gfx::Material> m_primitive_material;

 };
} // namespace regina