#pragma once

#include "rex_std/vector.h"

#include "rex_engine/pooling/scoped_pool_object.h"
#include "rex_engine/pooling/growing_pool.h"
#include "rex_renderer_core/gfx/sync_info.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace rhi
  {
    class CommandAllocator;
  }

  namespace gfx
  {
    class SyncInfoPool
    {
    public:
      SyncInfoPool();
      ScopedPoolObject<rhi::SyncInfo> request(u64 fenceValue, void* fenceObject);

    private:
      GrowingPool<rhi::SyncInfo> m_sync_info_pool;
    };
  }
}