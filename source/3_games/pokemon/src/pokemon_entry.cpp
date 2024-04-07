#include "rex_engine/engine/entrypoint.h"

#include "rex_engine/diagnostics/log.h"

namespace pokemon
{
  DEFINE_LOG_CATEGORY(LogPokemon);

  bool initialise()
  {
    REX_INFO(LogPokemon, "Pokemon initialized");

    return true;
  }

  void update()
  {

  }

  void shutdown()
  {

  }
}

namespace rex
{
  rsl::string_view project_name()
  {
    return "Pokemon";
  }

  ApplicationCreationParams app_entry(PlatformCreationParams& platformParams)
  {
    rex::ApplicationCreationParams app_params(platformParams);

    app_params.gui_params.window_width = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title = "Pokemon: Rex";

    app_params.create_window = true;

    app_params.engine_params.app_init_func = pokemon::initialise;
    app_params.engine_params.app_update_func = pokemon::update;
    app_params.engine_params.app_shutdown_func = pokemon::shutdown;

    return app_params;
  }
}