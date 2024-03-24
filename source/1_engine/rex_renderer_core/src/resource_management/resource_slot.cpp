#include "rex_renderer_core/resource_management/resource_slot.h"

#include "rex_renderer_core/rendering/renderer.h"

namespace rex
{
  namespace rhi
  {
    //-------------------------------------------------------------------------
    ResourceSlot ResourceSlot::make_invalid()
    {
      return ResourceSlot(globals::g_invalid_slot_id);
    }

    //-------------------------------------------------------------------------
    ResourceSlot::ResourceSlot()
        : m_about_to_be_removed(false)
        , m_slot_id(globals::g_invalid_slot_id)
        , m_ref_count(nullptr)
    {
    }

    //-------------------------------------------------------------------------
    ResourceSlot::ResourceSlot(const ResourceSlot& other)
        : m_about_to_be_removed(false)
        , m_slot_id(other.m_slot_id)
        , m_ref_count(other.m_ref_count)
    {
      rsl::atomic_increment(*m_ref_count);
    }

    //-------------------------------------------------------------------------
    ResourceSlot::ResourceSlot(ResourceSlot&& other) noexcept
        : m_about_to_be_removed(rsl::exchange(other.m_about_to_be_removed, false))
        , m_slot_id(rsl::exchange(other.m_slot_id, globals::g_invalid_slot_id))
        , m_ref_count(rsl::exchange(other.m_ref_count, nullptr)) // A moved ResourceSlot should leave the remaining ResourceSlot invalid
    {
    }

    //-------------------------------------------------------------------------
    ResourceSlot::~ResourceSlot()
    {
      release();
    }

    //-------------------------------------------------------------------------
    ResourceSlot::ResourceSlot(ResourceHash slotId)
        : m_about_to_be_removed(false)
        , m_slot_id(slotId)
        , m_ref_count(new s32(1))
    {
    }

    //-------------------------------------------------------------------------
    ResourceSlot& ResourceSlot::operator=(const ResourceSlot& other)
    {
      if(this == &other)
      {
        return *this;
      }

      m_about_to_be_removed = other.m_about_to_be_removed;
      m_slot_id             = other.m_slot_id;
      m_ref_count           = other.m_ref_count;

      rsl::atomic_increment(*m_ref_count);

      return *this;
    }

    //-------------------------------------------------------------------------
    ResourceSlot& ResourceSlot::operator=(ResourceSlot&& other) noexcept
    {
      if(this == &other)
      {
        return *this;
      }

      m_about_to_be_removed = rsl::exchange(other.m_about_to_be_removed, false);
      m_slot_id             = rsl::exchange(other.m_slot_id, globals::g_invalid_slot_id);
      m_ref_count           = rsl::exchange(other.m_ref_count, nullptr); // A moved ResourceSlot should leave the remaining ResourceSlot invalid

      return *this;
    }

    //-------------------------------------------------------------------------
    bool ResourceSlot::operator==(const ResourceSlot& other) const
    {
      return this->m_slot_id == other.m_slot_id;
    }

    //-------------------------------------------------------------------------
    bool ResourceSlot::operator!=(const ResourceSlot& other) const
    {
      return !(*this == other);
    }

    //-------------------------------------------------------------------------
    bool ResourceSlot::operator==(s32 other) const
    {
      return this->m_slot_id == other;
    }

    //-------------------------------------------------------------------------
    bool ResourceSlot::operator!=(s32 other) const
    {
      return !(*this == other);
    }

    //-------------------------------------------------------------------------
    bool ResourceSlot::is_valid() const
    {
      // when the slot id is globals::g_invalid_slot_id, something went wrong during the creation of this slot
      return m_slot_id != globals::g_invalid_slot_id && m_ref_count != nullptr;
    }

    //-------------------------------------------------------------------------
    bool ResourceSlot::is_about_to_be_removed() const
    {
      return m_about_to_be_removed;
    }

    //-------------------------------------------------------------------------
    s32 ResourceSlot::release()
    {
      s32 ref_count = 0;

      // Only release valid ResourceSlots
      if(is_valid())
      {
        // Cache ref count to return
        ref_count = *m_ref_count;

        if(rsl::atomic_decrement(*m_ref_count) == 0)
        {
          m_about_to_be_removed = true;

          //renderer::release_resource(*this);

          m_about_to_be_removed = false;
          m_slot_id             = globals::g_invalid_slot_id;

          delete m_ref_count;
          m_ref_count = nullptr;
        }
      }

      return ref_count;
    }

    //-------------------------------------------------------------------------
    s32 ResourceSlot::slot_id() const
    {
      return m_slot_id;
    }
  } // namespace renderer
} // namespace rex