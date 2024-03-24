//#pragma once
//
//#include "rex_renderer_core/rendering/scene.h"
//#include "rex_renderer_core/resources/mesh.h"
//
//#include "rex_std/unordered_map.h"
//#include "rex_std/memory.h"
//#include "rex_std/bonus/string.h"
//
//#include "rex_renderer_core/resource_management/resource.h"
//#include "rex_renderer_core/commands/attach_committed_resource_to_frame_cmd.h"
//#include "rex_renderer_core/commands/create_constant_buffer_cmd.h"
//
//namespace regina
//{
//  class SampleScene : public rex::renderer::Scene
//  {
//  public:
//    SampleScene();
//
//  private:
//    void build_geometry();
//    void build_render_items();
//
//  private:
//    rsl::unordered_map<rsl::medium_stack_string, rsl::unique_ptr<rex::renderer::Mesh>> m_meshes;
//
//  };
//}