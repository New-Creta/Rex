#include "rex_engine/data_type.h"

#include "rex_engine/diagnostics/logging.h"
#include "rex_engine/types.h"
#include "rex_std/unordered_map.h"
#include "rex_std_extra/utility/type_id.h"

#include <glm/glm.hpp>

namespace rex
{
  namespace data_type
  {
    //-----------------------------------------------------------------------
    size_t get_data_type_typeid(const DataType::Value& dataTypeValue)
    {
      switch(dataTypeValue)
      {
        case DataType::Value::INT8: return rsl::type_id<s8>().hash_code();
        case DataType::Value::INT16: return rsl::type_id<s16>().hash_code();
        case DataType::Value::INT32: return rsl::type_id<s32>().hash_code();
        case DataType::Value::INT64: return rsl::type_id<s64>().hash_code();

        case DataType::Value::UNSIGNED_INT8: return rsl::type_id<uint8>().hash_code();
        case DataType::Value::UNSIGNED_INT16: return rsl::type_id<uint16>().hash_code();
        case DataType::Value::UNSIGNED_INT32: return rsl::type_id<uint32>().hash_code();
        case DataType::Value::UNSIGNED_INT64: return rsl::type_id<uint64>().hash_code();

        case DataType::Value::FLOAT32: return rsl::type_id<f32>().hash_code();
        case DataType::Value::FLOAT64: return rsl::type_id<f64>().hash_code();

        case DataType::Value::MAT3: return rsl::type_id<glm::mat3>().hash_code();
        case DataType::Value::MAT4: return rsl::type_id<glm::mat4>().hash_code();

        case DataType::Value::VEC2: return rsl::type_id<glm::vec2>().hash_code();
        case DataType::Value::VEC3: return rsl::type_id<glm::vec3>().hash_code();
        case DataType::Value::VEC4: return rsl::type_id<glm::vec4>().hash_code();

        case DataType::Value::IVEC2: return rsl::type_id<glm::ivec2>().hash_code();
        case DataType::Value::IVEC3: return rsl::type_id<glm::ivec3>().hash_code();
        case DataType::Value::IVEC4: return rsl::type_id<glm::ivec4>().hash_code();

        case DataType::Value::UIVEC2: return rsl::type_id<glm::uvec2>().hash_code();
        case DataType::Value::UIVEC3: return rsl::type_id<glm::uvec3>().hash_code();
        case DataType::Value::UIVEC4: return rsl::type_id<glm::uvec4>().hash_code();

        case DataType::Value::BOOL: return rsl::type_id<bool>().hash_code();

        case DataType::Value::STRING: return rsl::type_id<rsl::string>().hash_code();

        case DataType::Value::SAMPLER2D: return rsl::type_id<s32>().hash_code();
        case DataType::Value::SAMPLER3D: return rsl::type_id<s32>().hash_code();
        case DataType::Value::SAMPLERCUBE: return rsl::type_id<s32>().hash_code();

        case DataType::Value::NONE: return 0;
      }

      REX_ERROR("Unknown DataType::Value");
      return 0;
    }

    //-----------------------------------------------------------------------
    size_t get_data_type_size(const DataType::Value& dataTypeValue)
    {
      switch(dataTypeValue)
      {
        case DataType::Value::INT8: return sizeof(s8);
        case DataType::Value::INT16: return sizeof(s16);
        case DataType::Value::INT32: return sizeof(s32);
        case DataType::Value::INT64: return sizeof(s64);

        case DataType::Value::UNSIGNED_INT8: return sizeof(uint8);
        case DataType::Value::UNSIGNED_INT16: return sizeof(uint16);
        case DataType::Value::UNSIGNED_INT32: return sizeof(uint32);
        case DataType::Value::UNSIGNED_INT64: return sizeof(uint64);

        case DataType::Value::FLOAT32: return sizeof(float);
        case DataType::Value::FLOAT64: return sizeof(double);

        case DataType::Value::MAT3: return sizeof(glm::mat3);
        case DataType::Value::MAT4: return sizeof(glm::mat4);

        case DataType::Value::VEC2: return sizeof(glm::vec2);
        case DataType::Value::VEC3: return sizeof(glm::vec3);
        case DataType::Value::VEC4: return sizeof(glm::vec4);

        case DataType::Value::IVEC2: return sizeof(glm::ivec2);
        case DataType::Value::IVEC3: return sizeof(glm::ivec3);
        case DataType::Value::IVEC4: return sizeof(glm::ivec4);

        case DataType::Value::UIVEC2: return sizeof(glm::uvec2);
        case DataType::Value::UIVEC3: return sizeof(glm::uvec3);
        case DataType::Value::UIVEC4: return sizeof(glm::uvec4);

        case DataType::Value::BOOL: return sizeof(bool);

        case DataType::Value::STRING: return sizeof(rsl::string);

        case DataType::Value::SAMPLER2D: return sizeof(s32);
        case DataType::Value::SAMPLER3D: return sizeof(s32);
        case DataType::Value::SAMPLERCUBE: return sizeof(s32);

        case DataType::Value::NONE: return 0;
      }

      REX_ERROR("Unknown DataType::Value");
      return 0;
    }
  } // namespace data_type

  //-----------------------------------------------------------------------
  DataType::DataType(Value value)
      : m_value(value)
  {
  }

  //-----------------------------------------------------------------------
  DataType& DataType::operator=(const DataType& other)
  {
    REX_ASSERT(*this != other);

    m_value = other.m_value;

    return *this;
  }
  //-----------------------------------------------------------------------
  DataType& DataType::operator=(DataType&& other) noexcept
  {
    REX_ASSERT(*this != other);

    m_value = rsl::exchange(other.m_value, DataType::Value::NONE);

    return *this;
  }

  //-----------------------------------------------------------------------
  bool DataType::operator==(const DataType& other) const
  {
    return this->m_value == other.m_value;
  }
  //-----------------------------------------------------------------------
  bool DataType::operator!=(const DataType& other) const
  {
    return !(*this == other);
  }

  //-----------------------------------------------------------------------
  DataType::Value DataType::get() const
  {
    return m_value;
  }

  //-----------------------------------------------------------------------
  size_t DataType::to_type_id() const
  {
    size_t datatype_id = data_type::get_data_type_typeid(m_value);

    if(datatype_id == 0)
    {
      REX_ERROR("Unsupported DataType to convert to typeid: {0}", (int32)m_value);
    }

    return datatype_id;
  }
  //-----------------------------------------------------------------------
  size_t DataType::to_byte_size() const
  {
    size_t datatype_size = data_type::get_data_type_size(m_value);

    if(datatype_size == 0)
    {
      REX_ERROR("Unsupported DataType to convert to byte size: {0}", (int32)m_value);
    }

    return datatype_size;
  }
} // namespace rex