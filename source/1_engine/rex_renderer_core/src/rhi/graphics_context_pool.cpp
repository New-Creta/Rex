#include "rex_renderer_core/rhi/graphics_context_pool.h"

namespace rex
{
  namespace rhi
  {
    GraphicsContextPool::GraphicsContextPool(rhi::GraphicsEngineType type, alloc_context_func&& allocContextFunc)
      : m_context_pool(rsl::move(allocContextFunc))
    {}

    ScopedPoolObject<rhi::GraphicsContext> GraphicsContextPool::request(u64 lastCompletedFence, CommandAllocator* alloc)
    {
      // We don't care which one we get, so we'll just get first we can find
      auto find_free_ctx = [](const rsl::unique_ptr<GraphicsContext>& ctx) { return true; }; // any idle one will do
      GraphicsContext* ctx = m_context_pool.request(find_free_ctx, alloc);

      ScopedPoolObject<rhi::GraphicsContext> pooled_ctx(ctx,
        [this](rhi::GraphicsContext* ctx)
        {
          ctx->execute_on_gpu();
          m_context_pool.discard(ctx);
        });

      return pooled_ctx;
    }
  }
}