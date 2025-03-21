#include "rex_engine/gfx/system/sync_info_pool.h"

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: OBJECT WITH DESTRUCTION CALLBACK

namespace rex
{
  namespace gfx
  {
    SyncInfoPool::SyncInfoPool()
      : m_sync_info_pool([]() { return rsl::make_unique<SyncInfo>(); })
    {}

    // Returns a new sync info object or create a new one if none can be found
    ObjectWithDestructionCallback<SyncInfo> SyncInfoPool::request(u64 fenceValue, Fence* fenceObject)
    {
      auto find_free_sync_info = [](const rsl::unique_ptr<SyncInfo>&) { return true; }; // any idle one will do
      SyncInfo* sync_info = m_sync_info_pool.request(find_free_sync_info);
      sync_info->reset(fenceValue, fenceObject);

      ObjectWithDestructionCallback<SyncInfo> pooled_sync_info(sync_info,
        [this](SyncInfo* syncInfo)
        {
          m_sync_info_pool.discard(syncInfo);
        });

      return pooled_sync_info;
    }

  }
}