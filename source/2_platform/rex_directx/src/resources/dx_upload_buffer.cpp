#include "rex_directx/resources/dx_upload_buffer.h"

#include "rex_directx/diagnostics/dx_call.h"


#include "rex_directx/utility/d3dx12.h"

#include "rex_engine/memory/pointer_math.h"

namespace rex
{
  namespace gfx
  {
    DxUploadBuffer::DxUploadBuffer(const wrl::ComPtr<ID3D12Resource>& uploadBuffer)
      : UploadBuffer(uploadBuffer->GetDesc().Width)
      , m_upload_buffer(uploadBuffer)
      , m_mapped_data(nullptr)
    {
      CD3DX12_RANGE read_range(0, 0); // We do not intend to read from this resource on the CPU
      DX_CALL(m_upload_buffer->Map(0, &read_range, &m_mapped_data));
    }

    DxUploadBuffer::~DxUploadBuffer()
    {
      m_upload_buffer->Unmap(0, nullptr);
    }

    // Write data on cpu side, it returns the offset into the upload buffer where data was written to
    s64 DxUploadBuffer::write_buffer_data_from_cpu(const void* data, s64 size, s32 alignment)
    {
      // Write the data into our mapped memory
      REX_ASSERT_X(can_fit_alloc(size, alignment), "Upload buffer overflow. Would write more into the upload buffer than is supported");

      rsl::byte* start = (rsl::byte*)m_mapped_data + current_offset(); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting)
      start = align(start, alignment);
      rsl::memcpy(start, data, size);

      return inc_offset(size, alignment);
    }

    // Write data on cpu side, it returns the offset into the upload buffer where data was written to
    s64 DxUploadBuffer::write_texture_data_from_cpu(const void* data, s32 width, s32 height, TextureFormat format)
    {
      // It's possible we lose bytes here as texture, who need to be aligned and buffers who don't need to be aligned
      // are written to the same buffer. If you write buffers first and only afterwards you write textures
      // you lose data between the padding.
      // Perhaps we can fix this by writing buffers at the end and textures at the front
      REX_STATIC_WARNING("Look into more optimal upload buffer usage");

      // Write the data into our mapped memory
      s32 alignment = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;
      s32 pitch_size = d3d::texture_pitch_size(width, format);
      s64 total_size = static_cast<s64>(pitch_size) * height;
      REX_ASSERT_X(can_fit_alloc(total_size, alignment), "Upload buffer overflow. Would write more into the upload buffer than is supported");

      // Texture data needs to get written 1 row at a time
      const char* src = (const char*)data;
      rsl::byte* dst = (rsl::byte*)m_mapped_data + current_offset();
      dst = align(dst, alignment);
      for (s32 y = 0; y < height; ++y)
      {
        rsl::memcpy(dst, src, pitch_size);
        src += pitch_size;
        dst += pitch_size;
      }

      return inc_offset(total_size, alignment);
    }

    ID3D12Resource* DxUploadBuffer::dx_object()
    {
      return m_upload_buffer.Get();
    }


  } // namespace gfx
} // namespace rex