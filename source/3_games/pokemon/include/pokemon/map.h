#pragma once

#include "rex_std/string_view.h"

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
  class Map
  {
  public: 
    Map(rsl::string_view name);

    // Load a hardcoded scene, this will change in the future
    void load_hardcoded_scene();

  private:
    rsl::string_view m_name;
    rsl::unique_ptr<rex::gfx::Scene> m_scene;
  };
}