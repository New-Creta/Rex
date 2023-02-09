#pragma once

#include "rex_engine/types.h"
#include "rex_std/memory.h"

namespace rex
{
  struct FrameInfo;

  class RexApplication
  {
  public:
    RexApplication();
    RexApplication(const RexApplication&) = delete;
    RexApplication(RexApplication&&)      = delete;
    virtual ~RexApplication();

    RexApplication& operator=(const RexApplication&) = delete;
    RexApplication& operator=(RexApplication&&)      = delete;

    virtual bool is_running() const = 0;

    virtual s32 run()   = 0;
    virtual void quit() = 0;

  private:
    struct Internal;
    rsl::unique_ptr<Internal> m_internal_ptr;
  };
} // namespace rex