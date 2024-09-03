#pragma once

#include "rex_engine/gfx/system/copy_engine.h"
#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_directx/gfx/dx_copy_context.h"
#include "rex_directx/system/dx_rhi.h"
#include "rex_std/mutex.h"

namespace rex
{
  namespace gfx
  {
    class DxUploadBuffer;
    class DxCopyEngine;

    // A lock around the upload buffer
    // Returned from the engine when the upload buffer gets locked
    // It automatically unlocks the upload buffer when it goes out of scope
    class UploadBufferLock
    {
    public:
      UploadBufferLock(DxCopyEngine* owningEngine, DxUploadBuffer* uploadBuffer);
      UploadBufferLock(const UploadBufferLock&) = delete;
      UploadBufferLock(UploadBuffer&&) = delete;

      ~UploadBufferLock();

      UploadBufferLock& operator=(const UploadBufferLock&) = delete;
      UploadBufferLock& operator=(UploadBufferLock&&) = delete;

      // Return the locked upload buffer
      DxUploadBuffer* upload_buffer();

    private:
      DxCopyEngine* m_owning_copy_engine;
      DxUploadBuffer* m_upload_buffer;
    };

    class DxCopyEngine : public gfx::CopyEngine
    {
    public:
      DxCopyEngine(ResourceStateTracker* globalResourceStateTracker);

      // Initialize all the resources required for the compute engine
      void init() override;

      // Prepare a new frame
      void new_frame() override;
      // Finalise the frame
      void end_frame() override;

      // Lock the upload buffer, other threads cannot write to it
      // This is blocking if the upload buffer is currently locked
      UploadBufferLock lock_upload_buffer();
      // Unlock the upload buffer, other threads can write to it again
      void unlock_upload_buffer();

    protected:
      // Allocate a copy context
      rsl::unique_ptr<GraphicsContext> allocate_new_context(CommandAllocator* alloc) override;

    private:
      rsl::unique_ptr<UploadBuffer> m_upload_buffer;
      rsl::mutex m_upload_buffer_access_mtx;
    };
  }
}