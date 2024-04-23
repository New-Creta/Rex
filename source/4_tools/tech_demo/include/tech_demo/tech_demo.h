#include "rex_engine/engine/types.h"

namespace rex
{
  struct ApplicationCreationParams;
  struct PlatformCreationParams;
} // namespace rex

namespace tech_demo
{
  bool initialize();
  void update();
  void on_gui();
  void shutdown();

  // we put the generation of the app creation params into a separate function
  // so that auto tests can always start of with the same settings as regina and change them where needed
  rex::ApplicationCreationParams create_tech_demo_app_creation_params(rex::PlatformCreationParams&& platformParams);
} // namespace regina