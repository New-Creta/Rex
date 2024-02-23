#include "rex_engine/engine/data_type.h"

#include "glm/ext/matrix_float3x3.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/ext/vector_int2.hpp"
#include "glm/ext/vector_int3.hpp"
#include "glm/ext/vector_int4.hpp"
#include "glm/ext/vector_uint2.hpp"
#include "glm/ext/vector_uint3.hpp"
#include "glm/ext/vector_uint4.hpp"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/types.h"
#include "rex_std/bonus/utility/type_id.h"
#include "rex_std/format.h"

#include <cstddef>

#ifdef REX_COMPILER_MSVC
  #pragma warning(push)
  #pragma warning(disable : 4201) // nonstandard extension used: nameless struct/union
#endif

#ifdef REX_COMPILER_MSVC
  #pragma warning(pop)
#endif

namespace rex
{
  namespace data_type
  {
    //-----------------------------------------------------------------------
    size_t get_data_type_typeid(const DataType::Value& dataTypeValue)
    {
      switch(dataTypeValue)
      {
        case DataType::Value::Int8: return rsl::type_id<s8>().hash_code();
        case DataType::Value::Int16: return rsl::type_id<s16>().hash_code();
        case DataType::Value::Int32: return rsl::type_id<s32>().hash_code();
        case DataType::Value::Int64: return rsl::type_id<s64>().hash_code();

        case DataType::Value::UnsignedInt8: return rsl::type_id<u8>().hash_code();
        case DataType::Value::UnsignedInt16: return rsl::type_id<u16>().hash_code();
        case DataType::Value::UnsignedInt32: return rsl::type_id<u32>().hash_code();
        case DataType::Value::UnsignedInt64: return rsl::type_id<u64>().hash_code();

        case DataType::Value::Float32: return rsl::type_id<f32>().hash_code();
        case DataType::Value::Float64: return rsl::type_id<f64>().hash_code();

        case DataType::Value::Mat3: return rsl::type_id<glm::mat3>().hash_code();
        case DataType::Value::Mat4: return rsl::type_id<glm::mat4>().hash_code();

        case DataType::Value::Vec2: return rsl::type_id<glm::vec2>().hash_code();
        case DataType::Value::Vec3: return rsl::type_id<glm::vec3>().hash_code();
        case DataType::Value::Vec4: return rsl::type_id<glm::vec4>().hash_code();

        case DataType::Value::Ivec2: return rsl::type_id<glm::ivec2>().hash_code();
        case DataType::Value::Ivec3: return rsl::type_id<glm::ivec3>().hash_code();
        case DataType::Value::Ivec4: return rsl::type_id<glm::ivec4>().hash_code();

        case DataType::Value::Uvec2: return rsl::type_id<glm::uvec2>().hash_code();
        case DataType::Value::Uvec3: return rsl::type_id<glm::uvec3>().hash_code();
        case DataType::Value::Uvec4: return rsl::type_id<glm::uvec4>().hash_code();

        case DataType::Value::Bool: return rsl::type_id<bool>().hash_code();

        case DataType::Value::String: return rsl::type_id<rsl::string>().hash_code();

        case DataType::Value::Sampler2D:
        case DataType::Value::Sampler3D:
        case DataType::Value::SamplerCube: return rsl::type_id<s32>().hash_code();

        case DataType::Value::None:
          // Nothing to implement
          break;
      }

      REX_ERROR(LogEngine, "Unknown DataType::Value");
      return 0;
    }

    //-----------------------------------------------------------------------
    size_t get_data_type_size(const DataType::Value& dataTypeValue)
    {
      switch(dataTypeValue)
      {
        case DataType::Value::Int8: return sizeof(s8);
        case DataType::Value::Int16: return sizeof(s16);
        case DataType::Value::Int32: return sizeof(s32);
        case DataType::Value::Int64: return sizeof(s64);

        case DataType::Value::UnsignedInt8: return sizeof(uint8);
        case DataType::Value::UnsignedInt16: return sizeof(uint16);
        case DataType::Value::UnsignedInt32: return sizeof(uint32);
        case DataType::Value::UnsignedInt64: return sizeof(uint64);

        case DataType::Value::Float32: return sizeof(float);
        case DataType::Value::Float64: return sizeof(double);

        case DataType::Value::Mat3: return sizeof(glm::mat3);
        case DataType::Value::Mat4: return sizeof(glm::mat4);

        case DataType::Value::Vec2: return sizeof(glm::vec2);
        case DataType::Value::Vec3: return sizeof(glm::vec3);
        case DataType::Value::Vec4: return sizeof(glm::vec4);

        case DataType::Value::Ivec2: return sizeof(glm::ivec2);
        case DataType::Value::Ivec3: return sizeof(glm::ivec3);
        case DataType::Value::Ivec4: return sizeof(glm::ivec4);

        case DataType::Value::Uvec2: return sizeof(glm::uvec2);
        case DataType::Value::Uvec3: return sizeof(glm::uvec3);
        case DataType::Value::Uvec4: return sizeof(glm::uvec4);

        case DataType::Value::Bool: return sizeof(bool);

        case DataType::Value::String: return sizeof(rsl::string);

        case DataType::Value::Sampler2D:
        case DataType::Value::Sampler3D:
        case DataType::Value::SamplerCube: return sizeof(s32);

        case DataType::Value::None:
          // Nothing to implement
          break;
      }

      REX_ERROR(LogEngine, "Unknown DataType::Value");
      return 0;
    }
  } // namespace data_type

  //-----------------------------------------------------------------------
  DataType::DataType(Value value)
      : m_value(value)
  {
  }

  //-----------------------------------------------------------------------
  DataType::DataType(DataType&& other) noexcept
      : m_value(rsl::exchange(other.m_value, DataType::Value::None))
  {
  }

  //-----------------------------------------------------------------------
  DataType& DataType::operator=(DataType&& other) noexcept
  {
    REX_ASSERT_X(this != &other, "can't move to yourself");

    m_value = rsl::exchange(other.m_value, DataType::Value::None);

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
    const size_t datatype_id = data_type::get_data_type_typeid(m_value);

    if(datatype_id == 0)
    {
      REX_ERROR(LogEngine, "Unsupported DataType to convert to typeid: {0}", (int32)m_value);
    }

    return datatype_id;
  }
  //-----------------------------------------------------------------------
  size_t DataType::to_byte_size() const
  {
    const size_t datatype_size = data_type::get_data_type_size(m_value);

    if(datatype_size == 0)
    {
      REX_ERROR(LogEngine, "Unsupported DataType to convert to byte size: {0}", (int32)m_value);
    }

    return datatype_size;
  }
} // namespace rex