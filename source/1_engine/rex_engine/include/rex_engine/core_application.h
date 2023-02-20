#pragma once

#include "rex_engine/cmd_line_args.h"
#include "rex_engine/types.h"
#include "rex_std/memory.h"
#include "rex_std_extra/memory/memory_size.h"

namespace rex
{
  struct RexEngineParams
  {
    // how much memory is the entire app allowed to use.
    // by default, there's no limit on this.
    rsl::memory_size max_memory = rsl::memory_size(rsl::numeric_limits<s64>::max());
  };

  class CoreApplication
  {
  public:
    CoreApplication(const RexEngineParams& engineParams, const CommandLineArguments& cmdArgs);
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