#include "rex_renderer_core/renderer.h"

#include "rex_opengl/opengl_error.h"

#include "rex_std/string.h"

#if REX_PLATFORM_X64
#include <glad/gl.h>
#else
#error "Unsupported platform"
#endif

namespace rex
{
  // these are required for platform specific gl implementation calls.
  extern bool rex_make_gl_context_current();
  extern void rex_gl_swap_buffers();

  //-------------------------------------------------------------------------
  enum ClearBits
  {
    REX_CLEAR_COLOUR_BUFFER = BIT(0),
    REX_CLEAR_DEPTH_BUFFER = BIT(1),
    REX_CLEAR_STENCIL_BUFFER = BIT(2),
  };

  //-------------------------------------------------------------------------
  struct ClearState
  {
    f32 rgba[4];
    f32 depth;
    u8  stencil;
    u32 flags;
  };

  RendererInfo s_renderer_info;
  ClearState s_clear_state{ 0.0f, 0.3f, 0.2f, 1.0f, 1.0f, 0x00, REX_CLEAR_COLOUR_BUFFER };

  //-------------------------------------------------------------------------
  // general accessors
  const RendererInfo& get_info()
  {
    return s_renderer_info;
  }

  //-------------------------------------------------------------------------
  const char8* shader_platform()
  {
    return "glsl";
  }

  //-------------------------------------------------------------------------
  bool is_y_up()
  {
    return true;
  }

  //-------------------------------------------------------------------------
  bool is_depth_0_to_1()
  {
    return false; // opengl has -1 to 1 z value
  }
  namespace renderer
  {
    namespace backend
    {
      GLint s_backbuffer_fbo = -1;

      //-------------------------------------------------------------------------
      bool initialize()
      {
        rsl::string str_glsl_version;
        rsl::string str_gl_version;
        rsl::string str_gl_renderer;
        rsl::string str_gl_vendor;

        // todo renderer caps
        const GLubyte* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
        const GLubyte* gl_version = glGetString(GL_VERSION);
        const GLubyte* gl_renderer = glGetString(GL_RENDERER);
        const GLubyte* gl_vendor = glGetString(GL_VENDOR);

        str_glsl_version = rsl::string((const char8*)glsl_version);
        str_gl_version = rsl::string((const char8*)gl_version);
        str_gl_renderer = rsl::string((const char8*)gl_renderer);
        str_gl_vendor = rsl::string((const char8*)gl_vendor);

        s_renderer_info.shader_version = str_glsl_version.c_str();
        s_renderer_info.api_version = str_gl_version.c_str();
        s_renderer_info.renderer = str_gl_renderer.c_str();
        s_renderer_info.vendor = str_gl_vendor.c_str();

        // gles base fbo is not 0
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &s_backbuffer_fbo);

        return true;
      }

      //-------------------------------------------------------------------------
      void shutdown()
      {
        // Nothing to implement
      }

      //-------------------------------------------------------------------------
      // functions to syncronise the render and main threads
      void new_frame()
      {
        rex_make_gl_context_current();
      }
      //-------------------------------------------------------------------------
      void end_frame()
      {
        // unused on this platform
      }

      // clears
      void clear()
      {
        ClearState& cs = s_clear_state;

        GL_CALL(glClearDepth(cs.depth));
        GL_CALL(glClearStencil(cs.stencil));

        if (cs.flags & GL_DEPTH_BUFFER_BIT)
        {
          GL_CALL(glEnable(GL_DEPTH_TEST));
          GL_CALL(glDepthMask(true));
        }

        if (cs.flags & GL_STENCIL_BUFFER_BIT)
        {
          GL_CALL(glEnable(GL_STENCIL_TEST));
          GL_CALL(glStencilMask(0xff));
        }

        GL_CALL(glClearColor(cs.rgba[0], cs.rgba[1], cs.rgba[2], cs.rgba[3]));

        if (!cs.flags)
        {
          return;
        }

        GL_CALL(glClear(cs.flags));
      }

      // swap / present / vsync
      extern void present()
      {
        rex_gl_swap_buffers();
      }
    }
  }
}