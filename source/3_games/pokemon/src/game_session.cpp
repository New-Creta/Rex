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

    // the levels directory is the root directory containing all the levels.
    // The content of these levels is not located here, only the information of what's needed to load these levels
    // rsl::string levels_directory = rex::vfs::mount(rex::path::join(rex::vfs::project_root(), rex::MountingPoint::Levels), "levels");
    rsl::string levels_directory = rex::path::join(rex::vfs::project_root(), "levels");

    // the levels file is the root file listing all the levels of the game together with a bit of information on how to initialize them
    rsl::string levels_file = rex::path::join(levels_directory, "levels_list.ini");
    rex::memory::Blob levels_file_content = rex::vfs::read_file(levels_file);
    rex::IniProcessor ini_processor = rex::IniProcessor(levels_file_content);

    // If there are any errors processing this root levels file, we cannot start the game, therefore we assert.
    rex::Error err = ini_processor.process();
    REX_ASSERT_X(!err.has_error(), "failed to load levels file, game cannot boot. Please investigate {}", rex::quoted(levels_file));

    // Go over all the levels information and store them. This will be needed later to fully load a level
    for (const rsl::key_value<const rsl::string_view, rex::IniHeaderWithItems>& item : ini_processor.all_items())
    {
      rsl::string full_ini_filepath = rex::path::join(levels_directory, item.value.get("File"));
      m_levels_info.emplace_back(item.value.header(), full_ini_filepath);
    }

    // If no levels are loaded, that'd be very weird, so we assert here in case that'd happen
    REX_ASSERT_X(m_levels_info.size() > 0, "0 levels loaded from levels file. This is not supported as the engine doesn't know where to start the game.");
    REX_INFO(LogGameSession, "{} levels loaded", m_levels_info.size());

    // By default the first level in the list is loaded, however, the user has the option to provide a start level
    rsl::string_view start_level_name = rex::cmdline::get_argument("StartLevel").value_or("");
    auto level_info_it = m_levels_info.begin();
    if (!start_level_name.empty())
    {
      level_info_it = rsl::find_if(m_levels_info.begin(), m_levels_info.end(), 
        [start_level_name](const LevelInfo& levelInfo) 
        {
          return levelInfo.name() == start_level_name; 
        });

      if (level_info_it == m_levels_info.end())
      {
        REX_ERROR(LogGameSession, "Failed to find level {} in list of levels. Using first level as default.", rex::quoted(start_level_name));
        level_info_it = m_levels_info.begin();
      }
    }

    REX_INFO(LogGameSession, "Starting level {}", rex::quoted(level_info_it->name()));

    // We found which level we need to start the game with, load it and start it
    m_current_level = rsl::make_unique<Level>(*level_info_it);
    m_current_level->start(rex::cmdline::get_argument("StartMap").value_or(""));
  }
}