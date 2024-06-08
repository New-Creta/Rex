#pragma once

#include "rex_renderer_core/gfx/copy_engine.h"
#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_directx/system/dx_copy_context.h"
#include "rex_directx/system/dx_rhi.h"

namespace rex
{
  namespace rhi
  {
    class UploadBuffer;

    class DxCopyEngine : public gfx::CopyEngine
    {
    public:
      DxCopyEngine();

      void init() override;
      void new_frame() override;

      UploadBuffer* lock_upload_buffer();
      void unlock_upload_buffer();

    protected:
      rsl::unique_ptr<GraphicsContext> allocate_new_context(rhi::CommandAllocator* alloc) override;
    private:
      rsl::unique_ptr<UploadBuffer> m_upload_buffer;
    };
  }
}