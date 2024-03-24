//#pragma once
//
//#include "rex_engine/memory/blob.h"
//#include "rex_renderer_core/commands/render_cmd.h"
//
//namespace rex
//{
//  namespace renderer
//  {
//    namespace commands
//    {
//      struct CreateBufferCommandDesc
//      {
//        memory::Blob buffer_data;
//      };
//
//      template <typename T>
//      rex::renderer::commands::CreateBufferCommandDesc create_buffer_parameters(const T* data, s32 num)
//      {
//        rex::renderer::commands::CreateBufferCommandDesc create_buffer_command_desc{};
//
//        create_buffer_command_desc.buffer_data = rex::memory::make_blob<T>(data, num);
//
//        return create_buffer_command_desc;
//      }
//
//      class CreateBuffer : public RenderCommand
//      {
//      public:
//        CreateBuffer(CreateBufferCommandDesc&& desc, const ResourceSlot& slot)
//            : RenderCommand()
//            , m_desc(rsl::move(desc))
//            , m_resource_slot(slot)
//        {
//        }
//
//        ~CreateBuffer() override = default;
//
//      protected:
//        const CreateBufferCommandDesc& description() const
//        {
//          return m_desc;
//        }
//        const ResourceSlot& resource_slot() const
//        {
//          return m_resource_slot;
//        }
//
//      private:
//        CreateBufferCommandDesc m_desc;
//        ResourceSlot m_resource_slot;
//      };
//    } // namespace commands
//  }   // namespace renderer
//} // namespace rex