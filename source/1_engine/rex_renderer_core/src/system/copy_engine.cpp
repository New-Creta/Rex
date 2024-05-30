#include "rex_renderer_core/system/copy_engine.h"

#include "rex_renderer_core/rhi/rhi.h"

#include "rex_renderer_core/rhi/resource_state.h"

namespace rex
{
  //CopyEngine::CopyEngine()
  //{

  //}

  //void CopyEngine::update_buffer(rhi::Buffer* buffer, const void* data, rsl::memory_size size, s32 dstOffset)
  //{
  //  s32 src_offset = m_upload_buffer->cpu_buffer_write(data, size);
  //  m_cmd_list->transition_buffer(buffer, rhi::ResourceState::CopyDest);
  //  m_cmd_list->update_buffer(buffer, dstOffset, m_upload_buffer.get(), src_offset);
  //}
  //void CopyEngine::update_texture2d(rhi::Texture2D* texture, const void* data, s32 width, s32 height, renderer::TextureFormat format)
  //{
  //  s32 src_offset = m_upload_buffer->cpu_texture2d_write(data, width, height, format);
  //  s32 dst_offset = 0;
  //  m_cmd_list->transition_buffer(texture, rhi::ResourceState::CopyDest);
  //  m_cmd_list->update_buffer(texture, dst_offset, m_upload_buffer.get(), src_offset);
  //}

  //CopyEngine& copy_engine()
  //{
  //  static CopyEngine copy_engine;
  //  return copy_engine;
  //}

}