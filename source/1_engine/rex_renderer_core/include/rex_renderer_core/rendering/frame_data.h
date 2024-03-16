#pragma once

#include "rex_renderer_core/resource_management/resource_slot.h"

#include "rex_std/vector.h"

namespace rex
{
  namespace renderer
  {
    class FrameData
    {
    public:
      explicit FrameData(const rex::renderer::ResourceSlot& frameSlot);

      FrameData(const FrameData& other) = delete;
      FrameData(FrameData&& other) noexcept = default;

      ~FrameData();

      FrameData& operator=(const FrameData& other) = delete;
      FrameData& operator=(FrameData&& other) noexcept = default;

      void set_object_committed_resource_slot(s32 frameIndex, s32 numItems, s32 bufferByteSize);
      void set_pass_committed_resource_slot(s32 frameIndex, s32 numItems, s32 bufferByteSize);

      void add_object_constant_buffer_slot(s32 frameIndex, const rex::renderer::ResourceSlot& committedResourceSlot, s32 bufferSize);
      void set_pass_constant_buffer_slot(s32 frameIndex, const rex::renderer::ResourceSlot& committedResourceSlot, s32 bufferSize);

      const rex::renderer::ResourceSlot& frame_slot() const;
      const rex::renderer::ResourceSlot& object_committed_resource_slot() const;
      const rex::renderer::ResourceSlot& pass_committed_resource_slot() const;
      const rsl::vector<rex::renderer::ResourceSlot>& object_constant_buffer_slots() const;
      const rex::renderer::ResourceSlot& pass_constant_buffer_slot() const;

    private:
      rex::renderer::ResourceSlot m_frame_slot; // holds the slot of this object
      rex::renderer::ResourceSlot m_object_committed_resource_slot; // holds the resource slot of the upload buffer for constant buffers per object
      rex::renderer::ResourceSlot m_pass_committed_resource_slot; // holds the resource slot of the upload buffer for constant buffers per pass

      rsl::vector<rex::renderer::ResourceSlot> m_object_constant_buffer_slots; // holds the resource slots for the constant buffer views, pointing into the constant buffers resource per objects
      rex::renderer::ResourceSlot m_pass_constant_buffer_slot; // holds the resource slots for the constant buffer views, pointing into the constant buffers resource per pass
    };

  }
}