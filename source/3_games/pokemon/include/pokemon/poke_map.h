#pragma once

#include "rex_std/string_view.h"
#include "rex_renderer_core/scenegraph/scene.h"
#include "rex_std/unordered_map.h"
#include "pokemon/chunk.h"

#include "rex_renderer_core/scenegraph/entity.h"

#include "rex_engine/text_processing/json.h"

#include "glm/glm.hpp"

namespace rex
{
  namespace gfx
  {
    class Scene;
  }
}

namespace pokemon
{
  // A map is a section for the in game level
  // a game consists of multiple levels
  // a level consists of 1 or more maps
  // for menus, the map is often empty
  // for the actual game, the map is filled with objects
  // In pokemon's case, a map is an area like Johto, Kanto, Hoenn

  // Maps cannot have chunks neigbouring other map's chunks
  // A transfer from one map to another should always be a hard transfer
  // often done through a black screen, animation, or a special map connector
  // Which is an area not tied to a map but shared between maps it connects to
  class Map
  {
  public: 
    Map(rex::gfx::Scene* scene, const rex::json::json& jsonBlob);

    // Load the player into a particular chunk with a given name
    void load_chunk(rsl::string_view name);

  private:
    rsl::string m_name;
    rsl::unordered_map<rsl::string, ChunkInfo> m_level_infos;
    const ChunkInfo* m_current_chunk_info;
    rex::gfx::Scene* m_scene;

    rsl::vector<Chunk> m_active_chunks;
  };
}