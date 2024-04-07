#pragma once

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/types.h"
#include "rex_std/atomic.h"
#include "rex_std/functional.h"

namespace rex
{
  namespace rhi
  {
    using ResourceHash = u32;

    ResourceHash make_new_hash();

    class ResourceSlot
    {
    public:
      static ResourceSlot make_invalid();

    public:
      ResourceSlot();
      ResourceSlot(const ResourceSlot& other);
      ResourceSlot(ResourceSlot&& other) noexcept;
      ~ResourceSlot();

      explicit ResourceSlot(ResourceHash slotId);

    public:
      ResourceSlot& operator=(const ResourceSlot& other);
      ResourceSlot& operator=(ResourceSlot&& other) noexcept;

      bool operator==(const ResourceSlot& other) const;
      bool operator!=(const ResourceSlot& other) const;
      bool operator==(ResourceHash other) const;
      bool operator!=(ResourceHash other) const;

    public:
      bool is_valid() const;
      bool is_about_to_be_removed() const;

      s32 release();
      ResourceHash slot_id() const;
      static ResourceHash invalid_id();
    private:
      bool m_about_to_be_removed;
      ResourceHash m_slot_id;
      s32* m_ref_count;

      static constexpr ResourceHash s_invalid_slot = static_cast<ResourceHash>(-1);
    };

  } // namespace rhi
} // namespace rex

namespace rsl
{
  inline namespace v1
  {
    template <>
    struct hash<rex::rhi::ResourceSlot>
    {
      rsl::hash_result operator()(const rex::rhi::ResourceSlot& resourceSlot) const
      {
        return resourceSlot.slot_id();
      }
    };
  } // namespace v1
} // namespace rsl