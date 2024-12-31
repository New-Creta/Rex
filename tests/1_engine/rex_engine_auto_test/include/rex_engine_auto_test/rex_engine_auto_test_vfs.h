#pragma once

namespace rex
{
  struct PlatformCreationParams;
  struct ApplicationCreationParams;

  namespace auto_tests
  {
    bool initialize(const ApplicationCreationParams&);
    void update();
    void shutdown();

    rex::ApplicationCreationParams vfs_test_entry(rex::PlatformCreationParams& platformParams);
  }
}
