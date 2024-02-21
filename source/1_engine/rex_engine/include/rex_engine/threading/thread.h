#pragma once

#include "rex_std/thread.h"

namespace rex
{
  class Thread
  {
  public:
    Thread();

  private:
    rsl::thread m_thread;
  };
}