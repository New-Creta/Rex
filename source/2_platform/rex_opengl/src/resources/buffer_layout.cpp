#include "rex_opengl/resources/buffer_layout.h"

#include "rex_opengl/log.h"

namespace rex
{
  namespace buffer_layout
  {
    //-----------------------------------------------------------------------
    u64 data_type_byte_size(const DataType& type)
    {
      return type.to_byte_size();
    }

    //-----------------------------------------------------------------------
    u32 data_type_component_count(const DataType::Value& type)
    {
      switch(type)
      {
        case DataType::Value::Int8:
        case DataType::Value::Int16:
        case DataType::Value::Int32:
        case DataType::Value::Int64:

        case DataType::Value::UnsignedInt8:
        case DataType::Value::UnsignedInt16:
        case DataType::Value::UnsignedInt32:
        case DataType::Value::UnsignedInt64:

        case DataType::Value::Float32:
        case DataType::Value::Float64:
        case DataType::Value::Bool:
        case DataType::Value::Sampler2D:
        case DataType::Value::Sampler3D:
        case DataType::Value::SamplerCube: return 1u;

        case DataType::Value::Mat3: return 3u * 3u;
        case DataType::Value::Mat4: return 4u * 4u;

        case DataType::Value::Vec2:
        case DataType::Value::Ivec2:
        case DataType::Value::Uvec2: return 2u;

        case DataType::Value::Ivec3:
        case DataType::Value::Uvec3:
        case DataType::Value::Vec3: return 3u;

        case DataType::Value::Ivec4:
        case DataType::Value::Uvec4:
        case DataType::Value::Vec4: return 4u;

        case DataType::Value::String:
        case DataType::Value::None:
          // Nothing to implement
          break;
      }

      REX_ERROR(LogOpenGL, "Unsupported Data Type to retrieve the component count: {0}", (s32)type);
      return 0;
    }
  } // namespace buffer_layout

  //-------------------------------------------------------------------------
  bool operator==(const BufferElement& element1, const BufferElement& element2)
  {
    return element1.type == element2.type && element1.component_count == element2.component_count && element1.normalized == element2.normalized;
  }

  //-------------------------------------------------------------------------
  bool operator!=(const BufferElement& element1, const BufferElement& element2)
  {
    return !(element1 == element2);
  }

  //-----------------------------------------------------------------------
  BufferLayoutCreator::BufferLayoutCreator()
      : m_elements({})
  {
  }
  //-----------------------------------------------------------------------
  void BufferLayoutCreator::add_buffer_element(DataType::Value inType, ShouldNormalize inShouldNormalize /*= ShouldNormalize::no*/)
  {
    m_elements.emplace_back(inType, buffer_layout::data_type_component_count(inType), inShouldNormalize);
  }
  //-----------------------------------------------------------------------
  void BufferLayoutCreator::clear()
  {
    m_elements.clear();
  }
  //-----------------------------------------------------------------------
  BufferLayout BufferLayoutCreator::build()
  {
    return BufferLayout(m_elements);
  }

  //-----------------------------------------------------------------------
  BufferLayout::BufferLayout(BufferElements elements)
      : m_elements(rsl::move(elements))
  {
  }

  //-----------------------------------------------------------------------
  bool BufferLayout::empty() const
  {
    return m_elements.empty();
  }
  //-----------------------------------------------------------------------
  s32 BufferLayout::size() const
  {
    return m_elements.size();
  }

  //-----------------------------------------------------------------------
  const BufferElements& BufferLayout::buffer_elements() const
  {
    return m_elements;
  }

  //-----------------------------------------------------------------------
  bool operator==(const BufferLayout& layout1, const BufferLayout& layout2)
  {
    if(layout1.buffer_elements().size() != layout2.buffer_elements().size())
    {
      return false;
    }

    return layout1.buffer_elements() == layout2.buffer_elements();
  }
  //-----------------------------------------------------------------------
  bool operator!=(const BufferLayout& layout1, const BufferLayout& layout2)
  {
    return !(layout1 == layout2);
  }
} // namespace rex