#include "rex_engine/diagnostics/assert.h"
#include "rex_opengl/opengl_error.h"
#include "rex_renderer_core_legacy/renderer.h"
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
    None               = -1,
    ClearColorBuffer   = GL_COLOR_BUFFER_BIT,
    ClearDepthBuffer   = GL_DEPTH_BUFFER_BIT,
    ClearStencilBuffer = GL_STENCIL_BUFFER_BIT,
  };

  //-------------------------------------------------------------------------
  bool operator&(ClearBits bits1, ClearBits bits2)
  {
    return (static_cast<u32>(bits1) & static_cast<u32>(bits2)) != 0;
  }

  //-------------------------------------------------------------------------
  bool operator|(ClearBits bits1, ClearBits bits2)
  {
    return (static_cast<u32>(bits1) | static_cast<u32>(bits2)) != 0;
  }

  //-------------------------------------------------------------------------
  struct ClearState
  {
    ClearState()
        : rgba(0.0f, 0.0f, 0.0f, 1.0f)
        , depth(1.0f)
        , stencil(0x00)
        , flags(ClearBits::ClearColorBuffer)
    {
    }

    rsl::Color4f rgba;
    f32 depth;
    u8 stencil;
    ClearBits flags;
  };

  //-------------------------------------------------------------------------
  ClearState make_default_clear_state()
  {
    ClearState cs;

    cs.rgba    = rsl::colors::MediumSeaGreen;
    cs.depth   = 1.0f;
    cs.stencil = 0x00;
    cs.flags   = ClearBits::ClearColorBuffer;

    return cs;
  }

  namespace renderer
  {
    namespace opengl
    {
      RendererInfo g_renderer_info; // NOLINT (fuchsia-statically-constructed-objects,-warnings-as-errors, cppcoreguidelines-avoid-non-const-global-variables,-warnings-as-errors)
    }                               // namespace opengl

    //-------------------------------------------------------------------------
    // general accessors
    const RendererInfo& info()
    {
      return opengl::g_renderer_info;
    }

    //-------------------------------------------------------------------------
    ShaderPlatform shader_platform()
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
      ClearState g_clear_state = make_default_clear_state(); // NOLINT (fuchsia-statically-constructed-objects,-warnings-as-errors, cppcoreguidelines-avoid-non-const-global-variables,-warnings-as-errors)
      s32 g_backbuffer_fbo     = -1;                         // NOLINT (fuchsia-statically-constructed-objects,-warnings-as-errors, cppcoreguidelines-avoid-non-const-global-variables,-warnings-as-errors)

      //-------------------------------------------------------------------------
      bool initialize()
      {
        const GLubyte* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
        const GLubyte* gl_version   = glGetString(GL_VERSION);
        const GLubyte* gl_renderer  = glGetString(GL_RENDERER);
        const GLubyte* gl_vendor    = glGetString(GL_VENDOR);

        // need to reinterpret cast here to convert them to actual string literals (const char8*)
        // providing no lint here to make clang-tidy happy
        opengl::g_renderer_info.shader_version = reinterpret_cast<const char8*>(glsl_version); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        opengl::g_renderer_info.api_version    = reinterpret_cast<const char8*>(gl_version);   // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        opengl::g_renderer_info.adaptor        = reinterpret_cast<const char8*>(gl_renderer);  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        opengl::g_renderer_info.vendor         = reinterpret_cast<const char8*>(gl_vendor);    // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

        // gles base fbo is not 0
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &g_backbuffer_fbo);

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
        const ClearState& cs = g_clear_state;

        REX_ASSERT_X(cs.flags != ClearBits::None, "No clear flags given but renderer::backend::clear was called.");

        GL_CALL(glClearDepth(cs.depth));
        GL_CALL(glClearStencil(cs.stencil));

        if(cs.flags & ClearBits::ClearDepthBuffer)
        {
          GL_CALL(glEnable(GL_DEPTH_TEST));
          GL_CALL(glDepthMask(true));
        }

        if(cs.flags & ClearBits::ClearStencilBuffer)
        {
          GL_CALL(glEnable(GL_STENCIL_TEST));
          GL_CALL(glStencilMask(0xff));
        }

        GL_CALL(glClearColor(cs.rgba.red, cs.rgba.green, cs.rgba.blue, cs.rgba.alpha)); // NOLINT(cppcoreguidelines-pro-type-union-access,-warnings-as-errors)
        GL_CALL(glClear((u32)cs.flags));
      }

      // swap / present / vsync
      void present()
      {
        rex_gl_swap_buffers();
      }
    } // namespace backend
  }   // namespace renderer
} // namespace rex