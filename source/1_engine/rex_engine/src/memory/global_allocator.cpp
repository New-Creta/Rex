#include "rex_engine/memory/global_allocator.h"

#include "rex_engine/memory/memory_stats.h"
#include "rex_engine/diagnostics/debug.h"

namespace rex
{
  namespace internal
  {
    bool log_mem_usage_before_tracking_impl()
    {
      log_mem_usage();
      return true;
    }

    void log_mem_usage_before_tracking()
    {
      // Store result in static bool so we only log once
      static bool _ = log_mem_usage_before_tracking_impl();
    }

    template <typename T>
    T create_global_allocator()
    {
      log_mem_usage_before_tracking();

      static UntrackedAllocator untracked_alloc{};
      static T alloc(untracked_alloc);

      return alloc;
    }

  }

  GlobalAllocator& global_allocator()
  {
#ifdef REX_ENABLE_MEM_TRACKING
    static GlobalAllocator alloc = internal::create_global_allocator<GlobalAllocator>();
#else
    static GlobalAllocator alloc;
#endif
    return alloc;
  }

  GlobalDebugAllocator& global_debug_allocator()
  {
    static GlobalDebugAllocator alloc = internal::create_global_allocator<GlobalDebugAllocator>();

    return alloc;
  }
} // namespace rex