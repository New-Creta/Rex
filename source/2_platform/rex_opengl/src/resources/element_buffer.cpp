#include "rex_opengl/resources/element_buffer.h"

#include "rex_opengl/opengl_error.h"

#if REX_PLATFORM_X64
  #include <glad/gl.h>
#else
  #error "Unsupported platform"
#endif

namespace rex 
{
  //-----------------------------------------------------------------------
  ElementBuffer::ElementBuffer()
      : Buffer(GL_ELEMENT_ARRAY_BUFFER)
  {
  }

  //-----------------------------------------------------------------------
  void ElementBuffer::set_data(u64 size, const void* data, BufferUsage usage)
  {
    GL_CALL(glBufferData(resource_target(), size, data, gl_usage(usage)));
  }

} // namespace rex 