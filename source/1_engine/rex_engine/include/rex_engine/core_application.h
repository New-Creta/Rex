#pragma once

#include "rex_engine/types.h"
#include "rex_std/memory.h"

namespace rex
{
  struct FrameInfo;

  class CoreApplication
  {
  public:
    CoreApplication();
    CoreApplication(const CoreApplication&) = delete;
    CoreApplication(CoreApplication&&)      = delete;
    virtual ~CoreApplication();

    CoreApplication& operator=(const CoreApplication&) = delete;
    CoreApplication& operator=(CoreApplication&&)      = delete;

    virtual bool is_running() const = 0;

    virtual s32 run()   = 0;
    virtual void quit() = 0;

  private:
    struct Internal;
    rsl::unique_ptr<Internal> m_internal_ptr;
  };
} // namespace rex