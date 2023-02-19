#include "rex_opengl/resources/input_assembler.h"

#if REX_PLATFORM_X64
  #include <glad/gl.h>
#else
  #error "Unsupported platform"
#endif

namespace rex
{
  namespace input_assembler
  {
    //-----------------------------------------------------------------------
    u32 gl_data_type(const DataType& type)
    {
      switch(type.get())
      {
        case DataType::Value::Int8:
        case DataType::Value::Int16:
        case DataType::Value::Int32:
        case DataType::Value::Int64:
        case DataType::Value::Bool:
        case DataType::Value::Ivec2:
        case DataType::Value::Ivec3:
        case DataType::Value::Ivec4: return GL_INT;
        case DataType::Value::UnsignedInt8:
        case DataType::Value::UnsignedInt16:
        case DataType::Value::UnsignedInt32:
        case DataType::Value::UnsignedInt64:
        case DataType::Value::Uvec2:
        case DataType::Value::Uvec3:
        case DataType::Value::Uvec4: return GL_UNSIGNED_INT;
        case DataType::Value::Float32:
        case DataType::Value::Float64:
        case DataType::Value::Mat3:
        case DataType::Value::Mat4:
        case DataType::Value::Vec2:
        case DataType::Value::Vec3:
        case DataType::Value::Vec4: return GL_FLOAT;
        case DataType::Value::Sampler2D: return GL_SAMPLER_2D;
        case DataType::Value::Sampler3D: return GL_SAMPLER_3D;
        case DataType::Value::SamplerCube: return GL_SAMPLER_CUBE;

        case DataType::Value::String:
        case DataType::Value::NONE: break;
      }

      REX_ERROR("Unsupported Data Type to retrieve the OpenGL data type: {0}", (s32)type.get());
      return 0;
    }
  } // namespace input_assembler

  //-----------------------------------------------------------------------
  void InputAssembler::enable_vertex_attribute_array(const BufferLayout& bufferLayout)
  {
    size_t offset = 0;

    for(s32 i = 0; i < bufferLayout.size(); ++i)
    {
      const BufferElement& element = bufferLayout.buffer_elements()[i];

      glVertexAttribPointer(i, element.component_count, input_assembler::gl_data_type(element.type), static_cast<GLboolean>(element.normalized), 0,
                            reinterpret_cast<void*>(offset)); // NOLINT (cppcoreguidelines-pro-type-reinterpret-cast)
      glEnableVertexAttribArray(i);

      offset += element.type.to_byte_size();
    }
  }

} // namespace rex