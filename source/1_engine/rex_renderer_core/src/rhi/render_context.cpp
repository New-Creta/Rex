#include "rex_renderer_core/rhi/render_context.h"

#include "rex_renderer_core/gfx/graphics.h"

namespace rex
{
  namespace rhi
  {
    RenderContext::RenderContext(rhi::CommandAllocator* alloc)
      : m_cmd_allocator(alloc)
    {
            
    }

    RenderContext::~RenderContext()
    {
      //m_cmd_list->stop_recording_commands();
      //m_owning_engine->exec_cmdlist(m_cmd_list.get());
    }

    void RenderContext::assign_allocator(rhi::CommandAllocator* cmdAlloc)
    {
      m_cmd_allocator = cmdAlloc;
    }

    void RenderContext::release_allocator(u64 fenceValue)
    {
      m_owning_engine->release_allocator(fenceValue, m_cmd_allocator);

      m_cmd_allocator = nullptr;
    }

    void RenderContext::execute_on_gpu()
    {
      flush_render_states();

      u64 fenceValue = m_owning_engine->execute_context(this);
      release_allocator(fenceValue);
    }
  }
}