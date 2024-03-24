#include "rex_renderer_core/system/resource_hasher.h"

#include "rex_std/bonus/functional.h"
#include "rex_std/bonus/utility.h"
#include "rex_std/type_traits.h"

namespace rex
{
  namespace rhi
  {
    ResourceHash hash_resource_desc(const BufferDesc& desc)
    {
      u32 hash = rsl::type_id<BufferDesc>().hash_code();
      
      hash = rsl::internal::hash_combine(hash, rsl::crc32c::Crc32c((const char*)desc.blob_view.data(), desc.blob_view.size()));

      return hash;
    }
    ResourceHash hash_resource_desc(const ClearStateDesc& desc)
    {
      u32 hash = rsl::type_id<ClearStateDesc>().hash_code();

      hash = rsl::internal::hash_combine(hash, rsl::crc32c::Crc32c((const char*)&desc, sizeof(desc)));

      return hash;
    }
    ResourceHash hash_resource_desc(const CompileShaderDesc& desc)
    {
      u32 hash = rsl::type_id<CompileShaderDesc>().hash_code();

      hash = rsl::internal::hash_combine(hash, rsl::crc32c::Crc32c((const char*)desc.shader_code.data(), desc.shader_code.size()));

      return hash;
    }
    ResourceHash hash_resource_desc(const InputLayoutDesc& desc)
    {
      u32 hash = rsl::type_id<InputLayoutDesc>().hash_code();

      hash = rsl::internal::hash_combine(hash, rsl::crc32c::Crc32c((const char*)desc.input_layout.data(), desc.input_layout.size() * sizeof(InputLayoutElementDesc)));

      return hash;
    }
    ResourceHash hash_resource_desc(const LinkShaderDesc& desc)
    {
      u32 hash = rsl::type_id<LinkShaderDesc>().hash_code();

      hash = rsl::internal::hash_combine(hash, desc.vertex_shader.slot_id());
      hash = rsl::internal::hash_combine(hash, desc.pixel_shader.slot_id());
      hash = rsl::internal::hash_combine(hash, rsl::crc32c::Crc32c((const char*)desc.constants.get(), desc.constants.byte_size()));

      return hash;
    }
    ResourceHash hash_resource_desc(const LoadShaderDesc& desc)
    {
      u32 hash = rsl::type_id<LoadShaderDesc>().hash_code();

      hash = rsl::internal::hash_combine(hash, rsl::crc32c::Crc32c((const char*)desc.shader_byte_code.data(), desc.shader_byte_code.size()));
      hash = rsl::internal::hash_combine(hash, static_cast<rsl::hash_result>(desc.shader_type));

      return hash;
    }
    ResourceHash hash_resource_desc(const renderer::VertexBufferDesc& desc)
    {
      u32 hash = rsl::type_id<renderer::VertexBufferDesc>().hash_code();

      hash = rsl::internal::hash_combine(hash, rsl::crc32c::Crc32c((const char*)desc.blob.data(), desc.blob.size()));

      return hash;
    }
    ResourceHash hash_resource_desc(const renderer::IndexBufferDesc& desc)
    {
      u32 hash = rsl::type_id<renderer::IndexBufferDesc>().hash_code();

      hash = rsl::internal::hash_combine(hash, rsl::crc32c::Crc32c((const char*)desc.blob.data(), desc.blob.size()));

      return hash;
    }
    ResourceHash hash_resource_desc(const RasterStateDesc& desc)
    {
      u32 hash = rsl::type_id<RasterStateDesc>().hash_code();

      hash = rsl::internal::hash_combine(hash, rsl::crc32c::Crc32c((const char*)&desc, sizeof(desc)));

      return hash;
    }
    ResourceHash hash_resource_desc(const PipelineStateDesc& desc)
    {
      u32 hash = rsl::type_id<PipelineStateDesc>().hash_code();

      hash = rsl::internal::hash_combine(hash, rsl::crc32c::Crc32c((const char*)&desc, sizeof(desc)));

      return hash;
    }
  }
}