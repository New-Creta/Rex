#pragma once

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/types.h"
#include "rex_std/atomic.h"
#include "rex_std/functional.h"

namespace rex
{
  namespace rhi
  {
    using ResourceID = s32;

    ResourceID make_new_hash();

    class ResourceSlot
    {
    public:
      static ResourceSlot make_invalid();

    public:
      ResourceSlot();
      ResourceSlot(const ResourceSlot& other);
      ResourceSlot(ResourceSlot&& other) noexcept;
      ~ResourceSlot();

      explicit ResourceSlot(ResourceID slotId);

    public:
      ResourceSlot& operator=(const ResourceSlot& other);
      ResourceSlot& operator=(ResourceSlot&& other) noexcept;

      bool operator==(const ResourceSlot& other) const;
      bool operator!=(const ResourceSlot& other) const;
      bool operator==(s32 other) const;
      bool operator!=(s32 other) const;

    public:
      bool is_valid() const;
      bool is_about_to_be_removed() const;

      s32 release();
      s32 slot_id() const;
      static ResourceID invalid_id();

    private:
      bool m_about_to_be_removed;
      s32 m_slot_id;
      s32* m_ref_count;
      static constexpr ResourceID s_invalid_slot = static_cast<ResourceID>(-1);
    };

  } // namespace renderer
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