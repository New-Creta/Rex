#include "rex_engine/gfx/system/graphics_context_pool.h"

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: OBJECT WITH DESTRUCTION CALLBACK

namespace rex
{
  namespace gfx
  {
    GraphicsContextPool::GraphicsContextPool(alloc_context_func&& allocContextFunc)
      : m_context_pool(rsl::move(allocContextFunc))
    {}

    ObjectWithDestructionCallback<GraphicsContext> GraphicsContextPool::request(CommandAllocator* alloc)
    {
      // We don't care which one we get, so we'll just get first we can find
      auto find_free_ctx = [](const rsl::unique_ptr<GraphicsContext>&) { return true; }; // any idle one will do
      GraphicsContext* ctx = m_context_pool.request(find_free_ctx, alloc);

      ObjectWithDestructionCallback<GraphicsContext> pooled_ctx(ctx,
        [this](GraphicsContext* ctx)
        {
          ctx->execute_on_gpu();
          m_context_pool.discard(ctx);
        });

      return pooled_ctx;
    }
  }
}