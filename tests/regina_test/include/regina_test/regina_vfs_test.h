#pragma once

namespace rex
{
  struct PlatformCreationParams;
  struct ApplicationCreationParams;
}

namespace regina_vfs_test
{
  bool initialize();
  void update();
  void shutdown();

  rex::ApplicationCreationParams vfs_test_entry(rex::PlatformCreationParams&& platformParams);
} // namespace regina_test