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
//      struct DrawInstanceCommandDesc
//      {
//        DrawInstanceCommandDesc()
//            : instance_count(0)
//            , start_instance(0)
//            , vertex_count(0)
//            , start_vertex(0)
//        {
//        }
//
//        DrawInstanceCommandDesc(s32 instanceCount, s32 startInstance, s32 vertexCount, s32 startVertex)
//            : instance_count(instanceCount)
//            , start_instance(startInstance)
//            , vertex_count(vertexCount)
//            , start_vertex(startVertex)
//        {
//        }
//
//        s32 instance_count;
//        s32 start_instance;
//        s32 vertex_count;
//        s32 start_vertex;
//      };
//
//      class DrawInstanced : public RenderCommand
//      {
//      public:
//        explicit DrawInstanced(DrawInstanceCommandDesc&& desc)
//            : RenderCommand()
//            , m_desc(rsl::move(desc))
//        {
//        }
//
//        ~DrawInstanced() override = default;
//
//        bool execute() override
//        {
//          backend::draw_instanced(m_desc.vertex_count, m_desc.instance_count, m_desc.start_vertex, m_desc.start_instance);
//
//          return true;
//        }
//
//      private:
//        DrawInstanceCommandDesc m_desc;
//      };
//    } // namespace commands
//  }   // namespace renderer
//} // namespace rex