#include "rex_engine/engine/entrypoint.h"
#include "rex_engine/engine/asset_db.h"

#include "rex_engine/diagnostics/log.h"

#include "rex_engine/system/process.h"

#include "pokemon/game_session.h"
#include "pokemon/serialization/binary_loaders/tileset_loader.h"
#include "pokemon/old_code/tileset.h"

#include "imgui/imgui.h"

namespace pokemon
{
  DEFINE_LOG_CATEGORY(LogPokemon);

  rsl::unique_ptr<GameSession> g_game_session;

  void init_data_importers()
  {
    rex::asset_db::instance()->add_loader<Tileset>(rsl::make_unique<TilesetLoader>());
  }

  bool initialise(const rex::ApplicationCreationParams& /*appCreationParams*/)
  {
    init_data_importers();

    g_game_session = rsl::make_unique<GameSession>();

    REX_INFO(LogPokemon, "Pokemon initialized");

    g_game_session->post_init();

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
  ApplicationCreationParams app_entry(PlatformCreationParams& platformParams)
  {
    rex::ApplicationCreationParams app_params(platformParams);

    app_params.gui_params.window_width = 800;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title.assign("Pokemon: Rex");

    app_params.is_gui_app = true;

    app_params.engine_params.app_init_func = pokemon::initialise;
    app_params.engine_params.app_update_func = pokemon::update;
    app_params.engine_params.app_shutdown_func = pokemon::shutdown;
    app_params.engine_params.app_name.assign("Pokemon: Rex");

    return app_params;
  }
}