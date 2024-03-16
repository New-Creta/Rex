#include "rex_renderer_core/rendering/frame_data.h"

#include "rex_renderer_core/commands/attach_committed_resource_to_frame_cmd.h"
#include "rex_renderer_core/commands/create_constant_buffer_cmd.h"
#include "rex_renderer_core/rendering/renderer.h"

namespace rex
{
  namespace renderer
  {
    FrameData::FrameData(const rex::renderer::ResourceSlot& frameSlot)
      : m_frame_slot(frameSlot)
      , m_object_committed_resource_slot()
      , m_pass_committed_resource_slot()
      , m_object_constant_buffer_slots()
      , m_pass_constant_buffer_slot()
    {
    }

    //-------------------------------------------------------------------------
    FrameData::~FrameData()
    {
      m_frame_slot.release();
      m_object_constant_buffer_slots.clear();
      m_pass_constant_buffer_slot.release();
    }

    //-------------------------------------------------------------------------
    void FrameData::set_object_committed_resource_slot(s32 frameIndex, s32 numItems, s32 bufferByteSize)
    {
      rex::renderer::commands::AttachCommittedResourceToFrameCommandDesc attach_object_constants{};
      attach_object_constants.frame_index = frameIndex;
      attach_object_constants.buffer_count = numItems;
      attach_object_constants.buffer_byte_size = bufferByteSize;

      m_object_committed_resource_slot = rex::renderer::attach_committed_resource_to_frame(rsl::move(attach_object_constants));
    }

    //-------------------------------------------------------------------------
    void FrameData::set_pass_committed_resource_slot(s32 frameIndex, s32 numItems, s32 bufferByteSize)
    {
      rex::renderer::commands::AttachCommittedResourceToFrameCommandDesc attach_object_constants{};
      attach_object_constants.frame_index = frameIndex;
      attach_object_constants.buffer_count = numItems;
      attach_object_constants.buffer_byte_size = bufferByteSize;

      m_pass_committed_resource_slot = rex::renderer::attach_committed_resource_to_frame(rsl::move(attach_object_constants));
    }

    //-------------------------------------------------------------------------
    void FrameData::add_object_constant_buffer_slot(s32 frameIndex, const rex::renderer::ResourceSlot & committedResourceSlot, s32 bufferSize)
    {
      rex::renderer::commands::CreateConstantBufferViewCommandDesc create_const_buffer_command_desc{};

      create_const_buffer_command_desc.frame_index = frameIndex;
      create_const_buffer_command_desc.committed_resource = committedResourceSlot;
      create_const_buffer_command_desc.buffer_size = bufferSize;

      rex::renderer::ResourceSlot const object_constant_buffer = rex::renderer::create_constant_buffer_view(rsl::move(create_const_buffer_command_desc));

      m_object_constant_buffer_slots.push_back(object_constant_buffer);
    }

    //-------------------------------------------------------------------------
    void FrameData::set_pass_constant_buffer_slot(s32 frameIndex, const rex::renderer::ResourceSlot & committedResourceSlot, s32 bufferSize)
    {
      rex::renderer::commands::CreateConstantBufferViewCommandDesc create_const_buffer_command_desc{};

      create_const_buffer_command_desc.frame_index = frameIndex;
      create_const_buffer_command_desc.committed_resource = committedResourceSlot;
      create_const_buffer_command_desc.buffer_size = bufferSize;

      rex::renderer::ResourceSlot pass_constant_buffer = rex::renderer::create_constant_buffer_view(rsl::move(create_const_buffer_command_desc));

      m_pass_constant_buffer_slot = rsl::move(pass_constant_buffer);
    }

    //-------------------------------------------------------------------------
    const rex::renderer::ResourceSlot& FrameData::frame_slot() const
    {
      return m_frame_slot;
    }

    //-------------------------------------------------------------------------
    const rex::renderer::ResourceSlot& FrameData::object_committed_resource_slot() const
    {
      return m_object_committed_resource_slot;
    }

    //-------------------------------------------------------------------------
    const rex::renderer::ResourceSlot& FrameData::pass_committed_resource_slot() const
    {
      return m_pass_committed_resource_slot;
    }

    //-------------------------------------------------------------------------
    const rsl::vector<rex::renderer::ResourceSlot>& FrameData::object_constant_buffer_slots() const
    {
      return m_object_constant_buffer_slots;
    }

    //-------------------------------------------------------------------------
    const rex::renderer::ResourceSlot& FrameData::pass_constant_buffer_slot() const
    {
      return m_pass_constant_buffer_slot;
    }
  }
}