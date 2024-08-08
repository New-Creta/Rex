#include "rex_directx/system/dx_copy_engine.h"

namespace rex
{
  namespace gfx
  {
    // Upload Buffer Lock
    // -------------------------------------

    UploadBufferLock::UploadBufferLock(DxCopyEngine* owningEngine, DxUploadBuffer* uploadBuffer)
      : m_owning_copy_engine(owningEngine)
      , m_upload_buffer(uploadBuffer)
    {}

    UploadBufferLock::~UploadBufferLock()
    {
      m_owning_copy_engine->unlock_upload_buffer();
    }

    DxUploadBuffer* UploadBufferLock::upload_buffer()
    {
      return m_upload_buffer;
    }

    // Copy Engine
    // -------------------------------------

    DxCopyEngine::DxCopyEngine(ResourceStateTracker* globalResourceStateTracker)
      : gfx::CopyEngine(globalResourceStateTracker)
    {}

    // Initialize all the resources required for the compute engine
    void DxCopyEngine::init()
    {
      m_upload_buffer = rhi::create_upload_buffer(1_mib);
    }

    // Prepare a new frame
    void DxCopyEngine::new_frame()
    {
      m_upload_buffer->reset();
    }

    // Finalise the frame
    void DxCopyEngine::end_frame()
    {
      flush();
    }

    // Lock the upload buffer, other threads cannot write to it
    // This is blocking if the upload buffer is currently locked
    UploadBufferLock DxCopyEngine::lock_upload_buffer()
    {
      m_upload_buffer_access_mtx.lock();
      return UploadBufferLock(this, d3d::to_dx12(m_upload_buffer.get()));
    }
    // Unlock the upload buffer, other threads can write to it again
    void DxCopyEngine::unlock_upload_buffer()
    {
      m_upload_buffer_access_mtx.unlock();
    }

    // Allocate a copy context
    rsl::unique_ptr<GraphicsContext> DxCopyEngine::allocate_new_context(CommandAllocator* alloc)
    {
      wrl::ComPtr<ID3D12GraphicsCommandList> cmdlist = rhi::create_commandlist(alloc, GraphicsEngineType::Copy);
      cmdlist->Close();
      return rsl::make_unique<DxCopyContext>(this, cmdlist);
    }

  }
}