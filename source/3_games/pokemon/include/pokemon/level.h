#pragma once

#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/vector.h"

#include "pokemon/map.h"

namespace pokemon
{
  // High level information on a level.
  class LevelInfo
  {
  public:
    LevelInfo(rsl::string_view name, rsl::string_view iniFilePath);

    rsl::string_view name() const;
    rsl::string_view ini_filepath() const;

  private:
    rsl::string m_name;
    rsl::string m_ini_filepath;
  };

  // A level a high level section of the game
  // for example: front end, in game, credits, options
  class Level
  {
  public:
    Level(const LevelInfo& levelInfo);

    void start(rsl::string_view mapName);

  private:
    void load_map_names(rsl::string_view iniFilePath);

  private:
    rsl::string m_name;
    rsl::vector<rsl::string> m_map_names;
    rsl::unique_ptr<Map> m_current_map;
  };
}