#include "rex_engine/engine/entrypoint.h"

#include "rex_engine/diagnostics/log.h"

#include "rex_engine/system/process.h"
#include "pokemon/game_session.h"

#include "imgui/imgui.h"

namespace pokemon
{
  DEFINE_LOG_CATEGORY(LogPokemon);

  rsl::unique_ptr<GameSession> g_game_session;

  bool initialise(const rex::ApplicationCreationParams& /*appCreationParams*/)
  {
    g_game_session = rsl::make_unique<GameSession>();

    REX_INFO(LogPokemon, "Pokemon initialized");

    return true;
  }

  void update()
  {
    g_game_session->update();
  }

  void shutdown()
  {
    g_game_session.reset();
  }
}

namespace rex
{
  bool g_project_name_set = set_project_name("Pokemon");

  ApplicationCreationParams app_entry(PlatformCreationParams& platformParams)
  {
    rex::ApplicationCreationParams app_params(platformParams);

    app_params.gui_params.window_width = 720;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title.assign(rsl::format("Pokemon: Rex | PID: {}", rex::current_process_id()));

    app_params.is_gui_app = true;

    app_params.engine_params.app_init_func = pokemon::initialise;
    app_params.engine_params.app_update_func = pokemon::update;
    app_params.engine_params.app_shutdown_func = pokemon::shutdown;
    app_params.engine_params.app_name.assign("Pokemon: Rex");

    return app_params;
  }
}