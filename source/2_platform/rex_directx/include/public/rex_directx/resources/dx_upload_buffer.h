#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_engine/gfx/resources/upload_buffer.h"

namespace rex
{
  namespace gfx
  {
    class Resource;

    // An upload buffer is a heap specifically used for uploading data to the gpu
    // it has a gpu resource with read acces
    // on the other hand tt has an internal mapped buffer into this gpu resource
    // so data can be written from the cpu
    // The upload buffer acts as a stack allocator
    class DxUploadBuffer : public UploadBuffer
    {
    public:
      explicit DxUploadBuffer(const wrl::ComPtr<ID3D12Resource>& uploadBuffer);
      DxUploadBuffer(const DxUploadBuffer&) = delete;
      DxUploadBuffer(DxUploadBuffer&&) = delete;
      ~DxUploadBuffer() override;

      DxUploadBuffer& operator=(const DxUploadBuffer&) = delete;
      DxUploadBuffer& operator=(DxUploadBuffer&&) = delete;

      // Write data on cpu side, it returns the offset into the upload buffer where data was written to
      s64 write_buffer_data_from_cpu(const void* data, s64 size, s32 alignment = 1);
      // Write data on cpu side, it returns the offset into the upload buffer where data was written to
      s64 write_texture_data_from_cpu(const void* data, s32 width, s32 height, TextureFormat format);

      ID3D12Resource* dx_object();

    private:
      wrl::ComPtr<ID3D12Resource> m_upload_buffer; // The gpu resource where data will be written in to
      void* m_mapped_data;                         // A pointed pointing to the bit of memory that's mapped between cpu and gpu
    };
  } // namespace gfx
} // namespace rex