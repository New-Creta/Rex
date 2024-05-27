#pragma once

#include "rex_std/vector.h"

#include "rex_renderer_core/rhi/graphics_context.h"
#include "rex_engine/pooling/scoped_pool_object.h"

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
      template <typename SyncInfoClass, typename ... Args>
      ScopedPoolObject<rhi::SyncInfo> request_sync_info(u64 fenceValue, void* fenceObject)
      {
        if (!m_idle_sync_infos.empty())
        {
          return use_existing_sync_info(fenceValue, fenceObject);
        }

        return create_new_active_sync_info(fenceValue, fenceObject);
      }
      void discard_sync_info(rhi::SyncInfo* syncInfo)
      {
        auto it = rsl::find_if(m_active_sync_infos.cbegin(), m_active_sync_infos.cend(), [syncInfo](const rsl::unique_ptr<rhi::SyncInfo> info) { return info.get() == syncInfo; });
        REX_ASSERT_X(it != m_active_sync_infos.cend(), "Trying to return sync info to pool that doesn't own it");
        s32 idx = rsl::distance(m_active_sync_infos.cbegin(), it);
        transfer_context(idx, m_active_sync_infos, m_idle_sync_infos);
      }

    private:
      template <typename ... Args>
      ScopedPoolObject<rhi::SyncInfo> use_existing_sync_info(u64 fenceValue, void* fenceObject)
      {
        s32 idx = m_idle_sync_infos.size() - 1;
        rhi::SyncInfo* info = transfer_context(idx, m_idle_sync_infos, m_active_sync_infos);
        info->reset(fenceValue, fenceObject);

        return ScopedPoolObject<rhi::SyncInfo>(info, [this](rhi::SyncInfo* ctx) { discard_sync_info(ctx); });
      }

      template <typename SyncInfoClass>
      ScopedPoolObject<rhi::SyncInfo> create_new_active_sync_info(u64 fenceValue, void* fenceObject)
      {
        auto sync_info = rsl::make_unique<SyncInfoClass>(fenceValue, fenceObject);
        m_active_sync_infos.emplace_back(rsl::move(sync_info));
        rhi::SyncInfo* new_sync_info = m_active_sync_infos.back().get();
        return ScopedPoolObject<rhi::SyncInfo>(new_sync_info, [this](rhi::SyncInfo* ctx) { discard_sync_info(ctx); });
      }

      // Transfers a context object from one vector to the other
      rhi::SyncInfo* transfer_context(s32 srcIdx, rsl::vector<rsl::unique_ptr<rhi::SyncInfo>>& src, rsl::vector<rsl::unique_ptr<rhi::SyncInfo>>& dst)
      {
        rsl::unique_ptr<rhi::SyncInfo> ctx = rsl::move(src[srcIdx]);
        src.pop_back();
        dst.emplace_back(rsl::move(ctx));
        return dst.back().get();
      }


    private:
      rsl::vector<rsl::unique_ptr<rhi::SyncInfo>> m_idle_sync_infos;
      rsl::vector<rsl::unique_ptr<rhi::SyncInfo>> m_active_sync_infos;
    };
  }
}