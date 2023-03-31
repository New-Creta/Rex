#include "rex_engine/memory/memory_manager.h"

#include "rex_engine/diagnostics/legacy/logging.h"
#include "rex_std/limits.h"

namespace rex
{
  // we need to have an internal error reporting here
  // so that we don't allocate heap memory when overflowing
  // as this would cause a stack overflow
#define REX_ENABLE_MEM_TRACKING
#ifdef REX_ENABLE_MEM_TRACKING
  #define REX_HEAP_TRACK_ERR(cond, msg)                                                                                                                                                                                                                  \
    if(!(cond))                                                                                                                                                                                                                                          \
    {                                                                                                                                                                                                                                                    \
      rsl::cout << "Err: " << msg << "\n";                                                                                                                                                                                                               \
    }
#else
  #define REX_HEAP_TRACK_ERR(cond, msg)
#endif

  //-------------------------------------------------------------------------
  MemoryManager::MemoryManager()
      : m_mem_usage(0)
      , m_max_mem_usage(rsl::numeric_limits<s64>::max())
  {
  }

  //-------------------------------------------------------------------------
  void MemoryManager::initialize(rsl::memory_size maxMemUsage)
  {
    m_max_mem_usage = rsl::high_water_mark<s64>(static_cast<s64>(maxMemUsage));
  }

  //-------------------------------------------------------------------------
  void MemoryManager::track_alloc(u64 size)
  {
    m_mem_usage += static_cast<s64>(size);
    REX_HEAP_TRACK_ERR(m_mem_usage.value() <= m_max_mem_usage, "Using more memory than allowed! usage: " << m_mem_usage.value() << " max: " << m_max_mem_usage);
  }
  //-------------------------------------------------------------------------
  void MemoryManager::track_dealloc(u64 size)
  {
    m_mem_usage -= static_cast<s64>(size);
  }

  //-------------------------------------------------------------------------
  MemoryManager& mem_manager()
  {
    static MemoryManager mem_manager;
    return mem_manager;
  }
} // namespace rex