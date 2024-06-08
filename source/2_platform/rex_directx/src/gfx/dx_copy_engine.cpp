#include "rex_directx/gfx/dx_copy_engine.h"

namespace rex
{
  namespace rhi
  {
    DxCopyEngine::DxCopyEngine()
    {

    }

    void DxCopyEngine::init()
    {
      m_upload_buffer = rhi::create_upload_buffer(100_mib);
    }

    void DxCopyEngine::new_frame()
    {
      m_upload_buffer->reset();
    }

    UploadBuffer* DxCopyEngine::lock_upload_buffer()
    {
      return m_upload_buffer.get();
    }
    void DxCopyEngine::unlock_upload_buffer()
    {
      // Nothing to implement
    }

    rsl::unique_ptr<GraphicsContext> DxCopyEngine::allocate_new_context(rhi::CommandAllocator* alloc)
    {
      wrl::ComPtr<ID3D12GraphicsCommandList> cmdlist = rhi::create_commandlist(alloc, rhi::CommandType::Copy);
      cmdlist->Close();
      return rsl::make_unique<rhi::DxCopyContext>(this, cmdlist, alloc);
    }

  }
}