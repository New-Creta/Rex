#include "rex_renderer_core/system/copy_engine.h"

#include "rex_renderer_core/rhi/rhi.h"

namespace rex
{
  CopyEngine::CopyEngine()
    : m_cmd_list(rhi::create_copy_commandlist())
  {

  }

  void CopyEngine::update_buffer(Resource* buffer, const void* data, rsl::memory_size size, s32 dstOffset)
  {
    s32 src_offset = m_upload_buffer->cpu_buffer_write(data, size);
    m_cmd_list->transition_buffer(buffer, renderer::ResourceState::CopyDest);
    m_cmd_list->update_buffer(buffer, dstOffset, m_upload_buffer.get(), src_offset);
  }
  void CopyEngine::update_texture2d(Texture2D* texture, const void* data, s32 width, s32 height, renderer::TextureFormat format)
  {
    s32 src_offset = m_upload_buffer->cpu_texture2d_write(data, width, height, format);
    m_cmd_list->transition_buffer(buffer, renderer::ResourceState::CopyDest);
    m_cmd_list->update_buffer(buffer, dstOffset, m_upload_buffer.get(), src_offset);
  }

  CopyEngine& copy_engine()
  {
    static CopyEngine copy_engine;
    return copy_engine;
  }

}