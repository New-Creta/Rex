#pragma once

#include "rex_std/memory.h"
#include "glm/glm.hpp"
#include "rex_std/string_view.h"

#include "rex_engine/entities/entity.h"
#include "rex_engine/gfx/materials/material.h"
#include "rex_engine/gfx/resources/texture_2d.h"
#include "rex_engine/gfx/resources/sampler_2d.h"

namespace pokemon
{
  struct ChunkInfo
  {
    rsl::string name;
    rsl::string background;
    rsl::vector<rsl::string> connections;
    glm::vec3 pos;
  };

  class Chunk
  {
  public:
    Chunk(rex::gfx::Entity entity, rsl::string_view name, rsl::string_view background, glm::vec3 pos);

  private:
    rsl::unique_ptr<rex::gfx::Material> m_material;
    rsl::unique_ptr<rex::gfx::Texture2D> m_texture;
    rsl::unique_ptr<rex::gfx::Sampler2D> m_sampler;
    glm::vec3 m_pos;
    rex::gfx::Entity m_entity;
  };
}