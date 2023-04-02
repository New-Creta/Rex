#pragma once

namespace rex
{
#ifdef REX_PLATFORM_WINDOWS
  namespace win 
  {
    // We require the namespace "win32" here in order to use the "using namespace"
  }

  using namespace win;
#endif
}