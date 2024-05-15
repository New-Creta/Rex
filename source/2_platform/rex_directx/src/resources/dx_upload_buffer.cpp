#include "rex_directx/resources/dx_upload_buffer.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/system/dx_commandlist.h"
#include "rex_directx/system/dx_resource.h"
#include "rex_directx/utility/d3dx12.h"

#include "rex_engine/memory/pointer_math.h"

#include "rex_directx/diagnostics/log.h"

namespace rex
{
  namespace rhi
  {
    UploadBuffer::UploadBuffer(const wrl::ComPtr<ID3D12Resource>& uploadBuffer, D3D12_RESOURCE_STATES startState)
        : BaseResource(uploadBuffer.Get(), make_new_hash())
        , m_upload_buffer(uploadBuffer)
        , m_mapped_data(nullptr)
        , m_resource_state(startState)
        , m_offset(0)
    {
      CD3DX12_RANGE read_range(0, 0); // We do not intend to read from this resource on the CPU
      DX_CALL(m_upload_buffer->Map(0, &read_range, &m_mapped_data));
      m_size = m_upload_buffer->GetDesc().Width;
    }

    UploadBuffer::~UploadBuffer()
    {
      m_upload_buffer->Unmap(0, nullptr);
    }

    void UploadBuffer::write(CommandList* cmdList, Resource2* dstResource, const void* data, s64 size, s32 alignment, s32 offset)
    {
      //// Write the data into our mapped memory
      //REX_ASSERT_X(m_offset + size < m_size, "Upload buffer overflow. Would write more into the upload buffer than is supported");

      //rsl::byte* start = (rsl::byte*)m_mapped_data + m_offset; // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting)
      //start = align(start, alignment);
      //rsl::memcpy(start, data, size);

      //// Fill the commandlist with the commands to update this resource
      //const D3D12_RESOURCE_STATES original_state = dstResource->resource_state(); 
      //dstResource->transition(cmdList, D3D12_RESOURCE_STATE_COPY_DEST);
      //dstResource->write_buffer(cmdList, this, m_offset, size, offset);
      //dstResource->transition(cmdList, original_state);

      //// m_upload_infos.emplace_back(UploadInfo{ dstResource, m_offset, size });
      //m_offset += size;
    }

    s32 UploadBuffer::prepare_for_new_write(const void* data, s64 size, s32 alignment, s32 offset)
    {
      // Write the data into our mapped memory
      REX_ASSERT_X(m_offset + size <= m_size, "Upload buffer overflow. Would write more into the upload buffer than is supported");

      rsl::byte* start = (rsl::byte*)m_mapped_data + m_offset; // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting)
      start = align(start, alignment);
      rsl::memcpy(start, data, size);

      s32 old_offset = m_offset;
      m_offset += size;

      return old_offset;
    }

    void UploadBuffer::write_buffer(CommandList* cmdList, Resource2* dstResource, const void* data, s64 size, s32 alignment, s32 offset)
    {
      // Write the data into our mapped memory
      REX_ASSERT_X(m_offset + size <= m_size, "Upload buffer overflow. Would write more into the upload buffer than is supported");

      rsl::byte* start = (rsl::byte*)m_mapped_data + m_offset; // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting)
      start = align(start, alignment);
      rsl::memcpy(start, data, size);

      cmdList->transition_buffer(dstResource, ResourceState::CopyDest);
      cmdList->dx_object()->CopyBufferRegion(dstResource->dx_object(), offset, m_upload_buffer.Get(), m_offset, size);

      m_offset += size;
    }

    void UploadBuffer::write_texture(ID3D12GraphicsCommandList* cmdList, Resource* dstResource, const void* data, DXGI_FORMAT format, s64 width, s64 height)
    {
      // Write the data into our mapped memory
      s32 format_size = rex::d3d::format_byte_size(format);
      s32 pitch_size = width * format_size;
      s32 alignment = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
      pitch_size = align(pitch_size, alignment);
      REX_ASSERT_X((m_offset + (pitch_size * height)) <= m_size, "Upload buffer overflow. Would write more into the upload buffer than is supported");

      // Texture data needs to get written 1 row at a time
      const char* src = (const char*)data;
      m_offset = align(m_offset, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
      rsl::byte* dst = (rsl::byte*)m_mapped_data + m_offset; // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting)
      dst = align(dst, alignment);
      for (s32 y = 0; y < height; ++y)
      {
        rsl::memcpy(dst, src, pitch_size);
        src += pitch_size;
        dst += pitch_size;
        //rsl::memcpy((void*)((uintptr_t)dst + y * pitch_size), src + y * pitch_size, pitch_size);
      }

      // Fill the commandlist with the commands to update this resource
      const D3D12_RESOURCE_STATES original_state = dstResource->resource_state();
      dstResource->transition(cmdList, D3D12_RESOURCE_STATE_COPY_DEST);
      dstResource->write_texture(cmdList, this, width, height, pitch_size, m_offset);
      dstResource->transition(cmdList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

      // m_upload_infos.emplace_back(UploadInfo{ dstResource, m_offset, size });
      m_offset += (pitch_size * height);
    }

    s32 UploadBuffer::prepare_for_new_texture_write(const void* data, s32 width, s32 height, renderer::TextureFormat format, s32 offset)
    {
      // Write the data into our mapped memory
      s32 alignment = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
      s32 pitch_size = d3d::texture_pitch_size(width, format);

      REX_ASSERT_X((m_offset + (pitch_size * height)) <= m_size, "Upload buffer overflow. Would write more into the upload buffer than is supported");

      // Texture data needs to get written 1 row at a time
      const char* src = (const char*)data;
      m_offset = align(m_offset, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
      rsl::byte* dst = (rsl::byte*)m_mapped_data + m_offset; // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting)
      dst = align(dst, alignment);
      for (s32 y = 0; y < height; ++y)
      {
        rsl::memcpy(dst, src, pitch_size);
        src += pitch_size;
        dst += pitch_size;
      }

      return m_offset;
    }



    void UploadBuffer::reset()
    {
      m_offset = 0;
    }

  } // namespace rhi
} // namespace rex