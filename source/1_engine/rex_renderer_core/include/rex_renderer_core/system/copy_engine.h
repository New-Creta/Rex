#pragma once

#include "rex_std/memory.h"

namespace rex
{
  namespace rhi
  {
    class CopyCommandList;
  }

  class CopyEngine
  {
  public:
    CopyEngine();

    void update_buffer(Resource* buffer, const void* data, rsl::memory_size size, s32 dstOffset);
    void update_texture2d(Texture2D* texture, const void* data, s32 width, s32 height, renderer::TextureFormat format);

  private:
    rsl::unique_ptr<rhi::CopyCommandList> m_cmd_list;
    rsl::unique_ptr<rhi::UploadBuffer> m_upload_buffer;
  };

  CopyEngine& copy_engine();
}