#pragma once

namespace rex
{
  struct PlatformCreationParams;
  struct ApplicationCreationParams;
}

namespace rex_renderer_directx_initialize_test
{
  bool initialize();
  void update();
  void shutdown();

  rex::ApplicationCreationParams boot_test_entry(rex::PlatformCreationParams&& platformParams);
} // namespace regina_test