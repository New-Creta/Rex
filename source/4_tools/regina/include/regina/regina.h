#include "rex_engine/types.h"

namespace rex
{
  struct ApplicationCreationParams;
  struct PlatformCreationParams;
}

namespace regina
{
  bool initialize();
  void update();
  void draw();
  void shutdown();

  // we put the generation of the app creation params into a separate function
  // so that auto tests can always start of with the same settings as regina and change them where needed
  rex::ApplicationCreationParams create_regina_app_creation_params(rex::PlatformCreationParams&& platformParams);
} // namespace rex