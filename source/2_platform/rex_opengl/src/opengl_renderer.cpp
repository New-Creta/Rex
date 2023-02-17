#include "rex_renderer_core/renderer.h"

#include "rex_opengl/opengl_error.h"

#include "rex_std/string.h"
#include "rex_std_extra/math/color.h"

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
  enum class ClearBits
  {
    REX_CLEAR_COLOUR_BUFFER = GL_COLOR_BUFFER_BIT,
    REX_CLEAR_DEPTH_BUFFER = GL_DEPTH_BUFFER_BIT,
    REX_CLEAR_STENCIL_BUFFER = GL_STENCIL_BUFFER_BIT,
  };

  //-------------------------------------------------------------------------
  bool operator& (ClearBits bits1, ClearBits bits2)
  {
    return static_cast<s32>(bits1) & static_cast<s32>(bits2);
  }

  //-------------------------------------------------------------------------
  bool operator| (ClearBits bits1, ClearBits bits2)
  {
    return static_cast<s32>(bits1) | static_cast<s32>(bits2);
  }

  //-------------------------------------------------------------------------
  struct ClearState
  {
    rsl::Color4f rgba;
    f32 depth;
    u8  stencil;
    u32 flags;
  };

  //-------------------------------------------------------------------------
  ClearState make_default_clear_state()
  {
    ClearState cs;

    cs.rgba = rsl::colors::MediumSeaGreen;
    cs.depth = 1.0f;
    cs.stencil = 0x00;
    cs.flags = REX_CLEAR_COLOUR_BUFFER;

    return cs;
  }

  namespace renderer
  {
    namespace opengl
    {
      RendererInfo s_renderer_info;
    }

    //-------------------------------------------------------------------------
    // general accessors
    const RendererInfo& get_info()
    {
      return opengl::s_renderer_info;
    }

    //-------------------------------------------------------------------------
    const ShaderPlatform shader_platform()
    {
      return ShaderPlatform::GLSL;
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

    namespace backend
    {
      ClearState s_clear_state = make_default_clear_state();
      s32 s_backbuffer_fbo = -1;

      //-------------------------------------------------------------------------
      bool initialize()
      {
        // todo renderer caps
        const GLubyte* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
        const GLubyte* gl_version = glGetString(GL_VERSION);
        const GLubyte* gl_renderer = glGetString(GL_RENDERER);
        const GLubyte* gl_vendor = glGetString(GL_VENDOR);

        opengl::s_renderer_info.shader_version = rsl::small_stack_string((const char8*)glsl_version);
        opengl::s_renderer_info.api_version = rsl::small_stack_string((const char8*)gl_version);
        opengl::s_renderer_info.adaptor = rsl::small_stack_string((const char8*)gl_renderer);
        opengl::s_renderer_info.vendor = rsl::small_stack_string((const char8*)gl_vendor);

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
        // Nothing to implement
      }

      void clear()
      {
        ClearState& cs = s_clear_state;

        REX_ASSERT_X(cs.flags, "No clear flags given but renderer::backend::clear was called.");

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

        GL_CALL(glClearColor(cs.rgba.red, cs.rgba.green, cs.rgba.blue, cs.rgba.alpha));
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