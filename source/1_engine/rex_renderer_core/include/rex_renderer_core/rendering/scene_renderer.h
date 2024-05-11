//#pragma once
//
//#include "rex_std/memory.h"
//
//#include "rex_renderer_core/rendering/viewport.h"
//#include "rex_renderer_core/rendering/scissor_rect.h"
//
//namespace rex
//{
//  class FrameContext
//  {
//  public:
//
//
//  private:
//    rsl::unique_ptr<RenderTarget> m_render_target;
//
//  };
//
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
//    Viewport m_viewport;
//    ScissorRect m_scissor_rect;
//
//  };
//}