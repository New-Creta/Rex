#include "rex_opengl/resources/buffer_layout.h"

#include "rex_engine/diagnostics/logging.h"

namespace rex
{
  namespace buffer_layout
  {
    //-----------------------------------------------------------------------
    u64 get_data_type_byte_size(const DataType& type)
    {
      return type.to_byte_size();
    }

    //-----------------------------------------------------------------------
    u32 get_data_type_component_count(const DataType& type)
    {
      switch (type.get())
      {
      case DataType::Value::INT8:
      case DataType::Value::INT16:
      case DataType::Value::INT32:
      case DataType::Value::INT64:

      case DataType::Value::UNSIGNED_INT8:
      case DataType::Value::UNSIGNED_INT16:
      case DataType::Value::UNSIGNED_INT32:
      case DataType::Value::UNSIGNED_INT64:

      case DataType::Value::FLOAT32:
      case DataType::Value::FLOAT64:
      case DataType::Value::BOOL:
        return 1u;

      case DataType::Value::MAT3:
        return 3u * 3u;
      case DataType::Value::MAT4:
        return 4u * 4u;

      case DataType::Value::VEC2:
      case DataType::Value::IVEC2:
      case DataType::Value::UIVEC2:
        return 2u;
      case DataType::Value::IVEC3:
      case DataType::Value::UIVEC3:
      case DataType::Value::VEC3:
        return 3u;
      case DataType::Value::IVEC4:
      case DataType::Value::UIVEC4:
      case DataType::Value::VEC4:
        return 4u;
      }

      REX_ERROR("Unsupported Data Type to retrieve the component count: {0}", (s32)type.get());
      return 0;
    }
  }

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
  BufferLayoutBuilder::BufferLayoutBuilder()
    :m_layout({})
  {

  }
  //-----------------------------------------------------------------------
  BufferLayout& BufferLayoutBuilder::add_buffer_element(DataType inType, ShouldNormalize inShouldNormalize /*= ShouldNormalize::no*/)
  {
    BufferElements new_buffer_elements;

    new_buffer_elements.reserve(m_layout.size() + 1);
    new_buffer_elements.insert(rsl::end(new_buffer_elements), rsl::begin(m_layout.get_buffer_elements()), rsl::end(m_layout.get_buffer_elements()));
    new_buffer_elements.emplace_back(BufferElement(inType, buffer_layout::get_data_type_component_count(inType), inShouldNormalize));

    m_layout = BufferLayout(rsl::move(new_buffer_elements));

    return m_layout;
  }
  //-----------------------------------------------------------------------
  BufferLayout& BufferLayoutBuilder::build()
  {
    return m_layout;
  }

  //-----------------------------------------------------------------------
  BufferLayout::BufferLayout(const BufferElements& elements)
    :m_elements(elements)
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
  const BufferElements& BufferLayout::get_buffer_elements() const
  {
    return m_elements;
  }

  //-----------------------------------------------------------------------
  bool operator==(const BufferLayout& layout1, const BufferLayout& layout2)
  {
    if (layout1.get_buffer_elements().size() != layout2.get_buffer_elements().size())
    {
      return false;
    }

    return layout1.get_buffer_elements() == layout2.get_buffer_elements();
  }
  //-----------------------------------------------------------------------
  bool operator!=(const BufferLayout& layout1, const BufferLayout& layout2)
  {
    return !(layout1 == layout2);
  }
}