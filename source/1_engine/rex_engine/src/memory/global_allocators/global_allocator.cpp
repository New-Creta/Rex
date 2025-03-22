#include "rex_engine/memory/global_allocators/global_allocator.h"

#include "rex_engine/diagnostics/debug.h"
#include "rex_engine/memory/memory_stats.h"
#include "rex_engine/memory/alloc_unique.h"

#include "rex_engine/engine/mutable_globals.h"

namespace rex
{
  namespace internal
  {
    bool log_mem_usage_before_tracking()
    {
      output_debug_string("Memory usage before any allocation");
      debug_log_mem_usage();
      return true;
    }

		BackendOSAllocator& backend_allocator()
		{
#ifdef REX_ENABLE_MEM_TRACKING
      static UntrackedAllocator untracked_alloc{};
      static TrackedAllocator alloc(untracked_alloc);
#else
      static UntrackedAllocator alloc{};
#endif
			return alloc;
		}

    bool init_backend_allocator()
    {
      log_mem_usage_before_tracking();

      backend_allocator(); // simply touch it so that the local static get initialized
      return true;
    }

    bool create_minimal_global_allocators(GlobalAllocator& alloc)
    {
      s32 minimal_global_alloc_size = static_cast<s32>(1_kib);
      output_debug_string("Creating a minimal scratch allocator of 1KiB for early startup");

      // Cannot use make_unique or alloc_unique here
      // make_unique would cause a circular dependency and we'd get a deadlock the second time the GlobalAllocator ctor is called
      // alloc_unique cannot be used here as that'd return a unique_ptr without a default_deleter argument
      CircularAllocator<GlobalAllocator>* ptr = alloc.allocate<CircularAllocator<GlobalAllocator>>();
      alloc.construct(ptr, minimal_global_alloc_size, alloc);
      mut_globals().allocators.scratch_allocator = rsl::unique_ptr<CircularAllocator<GlobalAllocator>>(ptr);

      return mut_globals().allocators.scratch_allocator != nullptr;
    }
  } // namespace internal


  GlobalAllocator::GlobalAllocator()
  {
    static bool s_init_backend_allocator = internal::init_backend_allocator();
    static bool s_create_minimal_global_allocator = internal::create_minimal_global_allocators(*this);
  }

  GlobalAllocator::pointer GlobalAllocator::allocate(rsl::memory_size size) // NOLINT(readability-convert-member-functions-to-static)
  {
    return internal::backend_allocator().allocate(size.size_in_bytes());
  }
  GlobalAllocator::pointer GlobalAllocator::allocate(card64 size) // NOLINT(readability-convert-member-functions-to-static)
  {
    return internal::backend_allocator().allocate(size);
  }

  void GlobalAllocator::deallocate(pointer ptr, rsl::memory_size size) // NOLINT(readability-convert-member-functions-to-static)
  {
    internal::backend_allocator().deallocate(ptr, size.size_in_bytes());
  }
  void GlobalAllocator::deallocate(pointer ptr, card64 size) // NOLINT(readability-convert-member-functions-to-static)
  {
    internal::backend_allocator().deallocate(ptr, size);
  }

} // namespace rex