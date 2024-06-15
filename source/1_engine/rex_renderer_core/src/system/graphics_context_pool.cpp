#include "rex_renderer_core/system/graphics_context_pool.h"

namespace rex
{
  namespace gfx
  {
    GraphicsContextPool::GraphicsContextPool(alloc_context_func&& allocContextFunc)
      : m_context_pool(rsl::move(allocContextFunc))
    {}

    ScopedPoolObject<GraphicsContext> GraphicsContextPool::request(CommandAllocator* alloc)
    {
      // We don't care which one we get, so we'll just get first we can find
      auto find_free_ctx = [](const rsl::unique_ptr<GraphicsContext>&) { return true; }; // any idle one will do
      GraphicsContext* ctx = m_context_pool.request(find_free_ctx, alloc);

      ScopedPoolObject<GraphicsContext> pooled_ctx(ctx,
        [this](GraphicsContext* ctx)
        {
          ctx->execute_on_gpu();
          m_context_pool.discard(ctx);
        });

      return pooled_ctx;
    }
  }
}