#include "rex_opengl/resources/vertex_buffer.h"

#include "rex_std_extra/utilities/casting.h"

#if REX_PLATFORM_X64
  #include <glad/gl.h>
#else
  #error "Unsupported platform"
#endif

namespace rex
{
  //-----------------------------------------------------------------------
  VertexBuffer::VertexBuffer()
      : Buffer(GL_ARRAY_BUFFER)
  {
  }

  //-----------------------------------------------------------------------
  void VertexBuffer::set_data(u64 size, const void* data, BufferUsage usage)
  {
    glBufferData(get_resource_target(), rsl::safe_numeric_cast<GLsizeiptr>(size), data, get_gl_usage(usage));
  }

} // namespace rex