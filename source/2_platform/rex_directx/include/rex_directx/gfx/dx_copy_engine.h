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
      DxCopyEngine()
      {

      }

      void post_init() override
      {
        m_upload_buffer = rhi::create_upload_buffer(100_mib);
      }

      UploadBuffer* lock_upload_buffer()
      {
        return m_upload_buffer.get();
      }
      void unlock_upload_buffer()
      {
        // Nothing to implement
      }

    protected:
      rsl::unique_ptr<GraphicsContext> allocate_new_context(rhi::CommandAllocator* alloc) override
      {
        wrl::ComPtr<ID3D12GraphicsCommandList> cmdlist = rhi::create_commandlist(alloc, rhi::CommandType::Copy);

        return rsl::make_unique<rhi::DxCopyContext>(this, cmdlist, alloc);
      }

    private:
      rsl::unique_ptr<UploadBuffer> m_upload_buffer;
    };
  }
}