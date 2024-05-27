#pragma once

namespace rex
{
  enum class TargetPlatform
  {
    Windows
  };

#ifdef REX_PLATFORM_WINDOWS

  namespace win
  {

  }                    // namespace win

  using namespace win; // NOLINT(google-build-using-namespace)
#endif
} // namespace rex