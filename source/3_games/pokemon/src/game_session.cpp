#include "pokemon/game_session.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_engine/text_processing/ini_processor.h"
#include "rex_engine/text_processing/text_processing.h"

namespace pokemon
{
  DEFINE_LOG_CATEGORY(LogGameSession);

  GameSession::GameSession()
  {
    initialise_start_level();
  }

  void GameSession::initialise_start_level()
  {
    // We don't want any assets, script, resource, to magically load in another one, all of this needs to be
    // explicitly defined in data, not in code.
    // However, knowing what to load when the game boots up is tricky to specify in data.
    // It's a bit of a chicken and egg problem. To just get it over with, we load whatever level
    // is defined as the start level and continue from there. As level are required to link to other level (otherwise it'd break game flow)
    // We can safely assume that from any level you can transition to another level (perhaps through other level, eg main game -> front end -> new game).

    rsl::string levels_directory = rex::path::join(rex::vfs::project_root(), "levels");
    rsl::string levels_file = rex::path::join(levels_directory, "levels_list.ini");
    rex::IniProcessor ini_processor = rex::IniProcessor(levels_file);
    rex::Error err = ini_processor.process();
    REX_ASSERT_X(!err.has_error(), "failed to load levels file, game cannot boot. Please investigate {}", rex::quoted(levels_file));

    for (const rsl::key_value<const rsl::string_view, rex::IniHeaderWithItems>& item : ini_processor.all_items())
    {
      rsl::string full_ini_filepath = rex::path::join(levels_directory, item.value.get("File"));
      m_levels_info.emplace_back(item.value.header(), full_ini_filepath);
    }

    REX_ASSERT_X(m_levels_info.size() > 0, "0 levels loaded from levels file. This is not supported as the engine doesn't know where to start the game.");
    REX_INFO(LogGameSession, "{} levels loaded", m_levels_info.size());

    rsl::string_view start_level_name = rex::cmdline::get_argument("StartLevel").value_or("");
    auto it = m_levels_info.begin();
    if (!start_level_name.empty())
    {
      it = rsl::find_if(m_levels_info.begin(), m_levels_info.end(), 
        [start_level_name](const LevelInfo& levelInfo) 
        {
          return levelInfo.name() == start_level_name; 
        });

      if (it == m_levels_info.end())
      {
        REX_ERROR(LogGameSession, "Failed to find level {} in list of levels. Using first level as default.", rex::quoted(start_level_name));
        it = m_levels_info.begin();
      }
    }

    REX_INFO(LogGameSession, "Starting level {}", rex::quoted(it->name()));

    m_current_level = rsl::make_unique<Level>(*it);
    m_current_level->start(rex::cmdline::get_argument("StartMap").value_or(""));
  }
}