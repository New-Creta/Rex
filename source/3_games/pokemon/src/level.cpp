#include "pokemon/level.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/text_processing/text_processing.h"

#include "rex_engine/memory/blob.h"
#include "rex_engine/text_processing/ini.h"
#include "rex_engine/filesystem/vfs.h"

#include "pokemon/poke_map.h"

namespace pokemon
{
  DEFINE_LOG_CATEGORY(LogLevel);

  //LevelInfo::LevelInfo(rsl::string_view name, rsl::string_view iniFilePath)
  //  : m_name(name)
  //  , m_ini_filepath(iniFilePath)
  //{
  //}

  //rsl::string_view LevelInfo::name() const
  //{
  //  return m_name;
  //}
  //rsl::string_view LevelInfo::ini_filepath() const
  //{
  //  return m_ini_filepath;
  //}

  //Level::Level(const LevelInfo& levelInfo)
  //  : m_name(levelInfo.name())
  //{
  //  load_map_names(levelInfo.ini_filepath());
  //}

  //void Level::start(rsl::string_view mapName)
  //{
  //  auto it = m_map_names.begin();
  //  if (!mapName.empty())
  //  {
  //    it = rsl::find(m_map_names.begin(), m_map_names.end(), mapName);
  //    REX_ASSERT_X(it != m_map_names.end(), "Level {} does not have a map with name {} and therefore cannot start it.", rex::quoted(m_name), rex::quoted(mapName));
  //  }

  //  REX_INFO(LogLevel, "Starting map: {}", rex::quoted(*it));

  //  //m_current_map = rsl::make_unique<Map>(*it);
  //  //m_current_map->load_hardcoded_scene();
  //}

  //void Level::load_map_names(rsl::string_view iniFilePath)
  //{
  //  rex::memory::Blob ini_data = rex::vfs::read_file(iniFilePath);
  //  rex::IniProcessor ini_processor = rex::IniProcessor(ini_data);
  //  rex::Error err = ini_processor.process();

  //  REX_ASSERT_X(!err.has_error(), "Failed to load level ini file at {}", rex::quoted(iniFilePath));

  //  rsl::vector<rsl::string_view> maps = rsl::split(ini_processor.get("", "Maps"), ",");
  //  for (rsl::string_view map : maps)
  //  {
  //    m_map_names.emplace_back(map);
  //  }

  //  REX_INFO(LogLevel, "{} maps loaded", m_map_names.size());
  //}
}