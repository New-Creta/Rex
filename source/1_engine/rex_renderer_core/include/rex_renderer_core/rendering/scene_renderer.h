//#pragma once
//
//#include "rex_std/memory.h"
//
//#include "rex_renderer_core/rhi/viewport.h"
//#include "rex_renderer_core/rhi/scissor_rect.h"
//#include "rex_renderer_core/resources/render_target.h"
//
//namespace rex
//{
//  class SceneRenderer
//  {
//  public:
//    SceneRenderer();
//    SceneRenderer(const SceneRenderer&) = delete;
//    SceneRenderer(SceneRenderer&&) = delete;
//    ~SceneRenderer();
//
//    SceneRenderer& operator=(const SceneRenderer&) = delete;
//    SceneRenderer& operator=(SceneRenderer&&) = delete;
//    
//    void render();
//
//  private:
//    void init_gpu_resources();
//
//  private:
//    Viewport m_viewport;
//    ScissorRect m_scissor_rect;
//
//  };
//}