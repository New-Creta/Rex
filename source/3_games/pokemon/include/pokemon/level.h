#pragma once

#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/vector.h"

#include "glm/glm.hpp"

namespace pokemon
{
  // High level information on a level.
  struct LevelInfo
  {
    rsl::string name;
    rsl::string background;
    rsl::vector<rsl::string> connections;
    glm::vec3 pos;
  };

  // A level a high level section of the game
  // for example: front end, in game, credits, options
  //class Level
  //{
  //public:
  //  Level(const LevelInfo& levelInfo);

  //  void start(rsl::string_view mapName);

  //private:
  //  void load_map_names(rsl::string_view iniFilePath);

  //private:
  //  rsl::string m_name;
  //  rsl::vector<rsl::string> m_map_names;
  //  rsl::unique_ptr<Map> m_current_map;
  //};
}