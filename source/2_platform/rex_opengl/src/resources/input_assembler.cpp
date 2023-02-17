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
        u32 get_gl_data_type(const DataType& type)
        {
            switch (type.get())
            {
            case DataType::Value::INT8:             return GL_INT;
            case DataType::Value::INT16:            return GL_INT;
            case DataType::Value::INT32:            return GL_INT;
            case DataType::Value::INT64:            return GL_INT;
            case DataType::Value::UNSIGNED_INT8:    return GL_UNSIGNED_INT;
            case DataType::Value::UNSIGNED_INT16:   return GL_UNSIGNED_INT;
            case DataType::Value::UNSIGNED_INT32:   return GL_UNSIGNED_INT;
            case DataType::Value::UNSIGNED_INT64:   return GL_UNSIGNED_INT;
            case DataType::Value::FLOAT32:          return GL_FLOAT;
            case DataType::Value::FLOAT64:          return GL_FLOAT;
            case DataType::Value::BOOL:             return GL_INT;
            case DataType::Value::MAT3:             return GL_FLOAT;
            case DataType::Value::MAT4:             return GL_FLOAT;
            case DataType::Value::VEC2:             return GL_FLOAT;
            case DataType::Value::IVEC2:            return GL_INT;
            case DataType::Value::UIVEC2:           return GL_UNSIGNED_INT;
            case DataType::Value::VEC3:             return GL_FLOAT;
            case DataType::Value::IVEC3:            return GL_INT;
            case DataType::Value::UIVEC3:           return GL_UNSIGNED_INT;
            case DataType::Value::VEC4:             return GL_FLOAT;
            case DataType::Value::IVEC4:            return GL_INT;
            case DataType::Value::UIVEC4:           return GL_UNSIGNED_INT;
            }

            REX_ERROR("Unsupported Data Type to retrieve the OpenGL data type: {0}", (s32)type.get());
            return 0;
        }
    }

    //-----------------------------------------------------------------------
    void InputAssembler::enable_vertex_attribute_array(const BufferLayout& bufferLayout)
    {
        size_t offset = 0;

        for (s32 i = 0; i < bufferLayout.size(); ++i)
        {
            const BufferElement& element = bufferLayout.get_buffer_elements()[i];

            glVertexAttribPointer(i, element.component_count, input_assembler::get_gl_data_type(element.type), element.normalized, 0, (void*)offset);
            glEnableVertexAttribArray(i);

            offset += element.type.to_byte_size();
        }
    }

}