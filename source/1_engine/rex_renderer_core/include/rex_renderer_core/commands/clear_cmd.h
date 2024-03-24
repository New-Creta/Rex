//#pragma once
//
//#include "rex_renderer_core/commands/render_cmd.h"
//#include "rex_renderer_core/resource_management/resource_slot.h"
//
//namespace rex
//{
//  namespace renderer
//  {
//    namespace commands
//    {
//      struct ClearCommandDesc
//      {
//      };
//
//      class Clear : public RenderCommand
//      {
//      public:
//        Clear(ClearCommandDesc&& desc, const ResourceSlot& resourceSlot)
//            : RenderCommand()
//            , m_desc(rsl::move(desc))
//            , m_resource_slot(resourceSlot)
//        {
//        }
//
//        ~Clear() override = default;
//
//        bool execute() override
//        {
//          backend::clear(m_resource_slot);
//
//          return true;
//        }
//
//      private:
//        ClearCommandDesc m_desc;
//        ResourceSlot m_resource_slot;
//      };
//    } // namespace commands
//  }   // namespace renderer
//} // namespace rex