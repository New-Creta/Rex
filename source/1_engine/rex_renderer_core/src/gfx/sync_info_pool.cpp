#include "rex_renderer_core/gfx/sync_info_pool.h"

namespace rex
{
  namespace gfx
  {
    SyncInfoPool::SyncInfoPool()
      : m_sync_info_pool([]() { return rsl::make_unique<rhi::SyncInfo>(); })
    {}

    // Returns a new sync info object or create a new one if none can be found
    ScopedPoolObject<rhi::SyncInfo> SyncInfoPool::request(u64 fenceValue, rhi::Fence* fenceObject)
    {
      auto find_free_sync_info = [](const rsl::unique_ptr<rhi::SyncInfo>& ctx) { return true; }; // any idle one will do
      rhi::SyncInfo* sync_info = m_sync_info_pool.request(find_free_sync_info);
      sync_info->reset(fenceValue, fenceObject);

      ScopedPoolObject<rhi::SyncInfo> pooled_sync_info(sync_info,
        [this](rhi::SyncInfo* syncInfo)
        {
          m_sync_info_pool.discard(syncInfo);
        });

      return pooled_sync_info;
    }

  }
}