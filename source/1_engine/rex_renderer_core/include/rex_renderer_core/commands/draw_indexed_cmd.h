//#pragma once
//
//#include "rex_renderer_core/commands/render_cmd.h"
//
//namespace rex
//{
//  namespace renderer
//  {
//    namespace commands
//    {
//      struct DrawIndexedCommandDesc
//      {
//        DrawIndexedCommandDesc()
//            : index_count(0)
//            , start_index(0)
//            , base_vertex(0)
//        {
//        }
//
//        DrawIndexedCommandDesc(s32 indexCount, s32 startIndex, s32 baseVertex)
//            : index_count(indexCount)
//            , start_index(startIndex)
//            , base_vertex(baseVertex)
//        {
//        }
//
//        s32 index_count;
//        s32 start_index;
//        s32 base_vertex;
//      };
//
//      class DrawIndexed : public RenderCommand
//      {
//      public:
//        explicit DrawIndexed(DrawIndexedCommandDesc&& desc)
//            : RenderCommand()
//            , m_desc(rsl::move(desc))
//        {
//        }
//
//        ~DrawIndexed() override = default;
//
//        bool execute() override
//        {
//          backend::draw_indexed(m_desc.index_count, m_desc.start_index, m_desc.base_vertex);
//
//          return true;
//        }
//
//      private:
//        DrawIndexedCommandDesc m_desc;
//      };
//    } // namespace commands
//  }   // namespace renderer
//} // namespace rex