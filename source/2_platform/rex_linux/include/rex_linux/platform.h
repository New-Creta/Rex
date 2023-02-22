#pragma once

#ifdef REX_PLATFORM_LINUX
namespace rex
{
  namespace linux
  {
    // We require the namespace "linux" here in order to use the "using namespace"
  }

  using namespace linux;
} // namespace rex
#else
  #error Unsupported Platform
#endif