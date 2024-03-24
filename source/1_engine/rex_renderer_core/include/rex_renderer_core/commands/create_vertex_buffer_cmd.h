//#pragma once
//
//#include "rex_renderer_core/commands/create_buffer_cmd.h"
//
//namespace rex
//{
//  namespace renderer
//  {
//    namespace commands
//    {
//      class CreateVertexBuffer : public CreateBuffer
//      {
//      public:
//        CreateVertexBuffer(CreateBufferCommandDesc&& desc, const ResourceSlot& slot)
//            : CreateBuffer(rsl::move(desc), slot)
//        {
//        }
//
//        bool execute() override
//        {
//          return backend::create_vertex_buffer(description(), resource_slot());
//        }
//      };
//    } // namespace commands
//  }   // namespace renderer
//} // namespace rex