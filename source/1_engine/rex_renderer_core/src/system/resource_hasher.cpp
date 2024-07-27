#include "rex_renderer_core/system/resource_hasher.h"

#include "rex_std/bonus/functional.h"
#include "rex_std/bonus/utility.h"
#include "rex_std/type_traits.h"

namespace rex
{
  namespace gfx
  {
    ResourceID hash_resource_desc(const ClearStateDesc& desc)
    {
      u32 hash = rsl::type_id<ClearStateDesc>().hash_code();

      hash = rsl::internal::hash_combine(hash, rsl::crc32::compute((const char*)&desc, sizeof(desc))); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting)

      return hash;
    }
    ResourceID hash_resource_desc(const CompileShaderDesc& desc)
    {
      u32 hash = rsl::type_id<CompileShaderDesc>().hash_code();

      hash = rsl::internal::hash_combine(hash, rsl::crc32::compute((const char*)desc.shader_source_code.data(), desc.shader_source_code.size())); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting)
      hash = rsl::internal::hash_combine(hash, static_cast<u64>(desc.shader_type));

      return hash;
    }
    ResourceID hash_resource_desc(const InputLayoutDesc& desc)
    {
      u32 hash = rsl::type_id<InputLayoutDesc>().hash_code();

      hash = rsl::internal::hash_combine(hash, rsl::crc32::compute((const char*)desc.data(), desc.size() * sizeof(InputLayoutElementDesc))); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting)

      return hash;
    }
    ResourceID hash_resource_desc(const RasterStateDesc& desc)
    {
      u32 hash = rsl::type_id<RasterStateDesc>().hash_code();

      hash = rsl::internal::hash_combine(hash, rsl::crc32::compute((const char*)&desc, sizeof(desc))); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting)

      return hash;
    }
  } // namespace gfx
} // namespace rex