#pragma once

#include "rex_std/vector.h"

#include "rex_engine/pooling/scoped_pool_object.h"
#include "rex_engine/pooling/growing_pool.h"
#include "rex_renderer_core/gfx/sync_info.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace gfx
  {
    // The pool managed per command queue that manages sync info objects
    class SyncInfoPool
    {
    public:
      SyncInfoPool();
      // Returns a new sync info object or create a new one if none can be found
      ScopedPoolObject<rhi::SyncInfo> request(u64 fenceValue, void* fenceObject);

    private:
      GrowingPool<rhi::SyncInfo> m_sync_info_pool;
    };
  }
}