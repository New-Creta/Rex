#include "rex_opengl/resources/vertex_array.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_opengl/log.h"

#if REX_PLATFORM_X64
  #include <glad/gl.h>
#else
  #error "Unsupported platform"
#endif

namespace rex
{
  //-----------------------------------------------------------------------
  VertexArray::VertexArray()
      : m_resource_id(0)
  {
    glGenVertexArrays(1, &m_resource_id);
  }
  //-----------------------------------------------------------------------
  VertexArray::VertexArray(VertexArray&& other) noexcept
      : m_resource_id(rsl::exchange(other.m_resource_id, 0))
  {
  }
  //-----------------------------------------------------------------------
  VertexArray::~VertexArray()
  {
    release();
  }

  //-----------------------------------------------------------------------
  VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
  {
    REX_ASSERT_X(this != &other, "can't move to yourself");

    this->m_resource_id = rsl::exchange(other.m_resource_id, 0);

    return *this;
  }

  //-----------------------------------------------------------------------
  u32 VertexArray::resource_id() const
  {
    return m_resource_id;
  }

  //-----------------------------------------------------------------------
  void VertexArray::bind() const
  {
    REX_ASSERT_X(m_resource_id != 0, "Binding an invalid resource id");

    glBindVertexArray(m_resource_id);
  }
  //-----------------------------------------------------------------------
  void VertexArray::unbind() const
  {
    glBindVertexArray(0);
  }

  //-----------------------------------------------------------------------
  void VertexArray::release()
  {
    glDeleteVertexArrays(1, &m_resource_id);

    m_resource_id = 0;
  }
} // namespace rex