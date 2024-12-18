#pragma once

#include "rex_std/type_traits.h"
#include "rex_std/bonus/utility/always_false.h"
#include "rex_std/string.h"
#include "rex_std/string_view.h"

#include "rex_engine/engine/types.h"

#include "glm/glm.hpp"

namespace rex
{
  class DataType
  {
  public:
    enum class Value
    {
      Unknown = 0,

      Int8,
      Int16,
      Int32,
      Int64,

      UnsignedInt8,
      UnsignedInt16,
      UnsignedInt32,
      UnsignedInt64,

      Float32,
      Float64,

      Mat3,
      Mat4,

      Vec2,
      Vec3,
      Vec4,

      Ivec2,
      Ivec3,
      Ivec4,

      Uvec2,
      Uvec3,
      Uvec4,

      Bool,

      String
    };

  public:
    DataType(Value value); // NOLINT(google-explicit-constructor)
    ~DataType() = default;

    DataType(const DataType& other) = default;
    DataType(DataType&& other) noexcept;

    DataType& operator=(const DataType& other) = default;
    DataType& operator=(DataType&& other) noexcept;

    bool operator==(const DataType& other) const;
    bool operator!=(const DataType& other) const;

    Value get() const;

    size_t to_type_id() const;
    size_t to_byte_size() const;

  private:
    Value m_value;
  };

  template <typename T>
  constexpr DataType data_type()
  {
    if constexpr (rsl::is_same_v<T, s8>)
    {
      return DataType(DataType::Value::Int8);
    }
    else if constexpr (rsl::is_same_v<T, s16>)
    {
      return DataType(DataType::Value::Int16);
    }
    else if constexpr (rsl::is_same_v<T, s32>)
    {
      return DataType(DataType::Value::Int32);
    }
    else if constexpr (rsl::is_same_v<T, s64>)
    {
      return DataType(DataType::Value::Int64);
    }
    else if constexpr (rsl::is_same_v<T, u8>)
    {
      return DataType(DataType::Value::UnsignedInt8);
    }
    else if constexpr (rsl::is_same_v<T, u16>)
    {
      return DataType(DataType::Value::UnsignedInt16);
    }
    else if constexpr (rsl::is_same_v<T, u32>)
    {
      return DataType(DataType::Value::UnsignedInt32);
    }
    else if constexpr (rsl::is_same_v<T, u64>)
    {
      return DataType(DataType::Value::UnsignedInt64);
    }
    else if constexpr (rsl::is_same_v<T, bool>)
    {
      return DataType(DataType::Value::Bool);
    }
    else if constexpr (rsl::is_same_v<T, f32>)
    {
      return DataType(DataType::Value::Float32);
    }
    else if constexpr (rsl::is_same_v<T, f64>)
    {
      return DataType(DataType::Value::Float64);
    }
    else if constexpr (rsl::is_same_v<T, glm::mat3>)
    {
      return DataType(DataType::Value::Mat3);
    }
    else if constexpr (rsl::is_same_v<T, glm::mat4>)
    {
      return DataType(DataType::Value::Mat4);
    }
    else if constexpr (rsl::is_same_v<T, glm::vec2>)
    {
      return DataType(DataType::Value::Vec2);
    }
    else if constexpr (rsl::is_same_v<T, glm::vec3>)
    {
      return DataType(DataType::Value::Vec3);
    }
    else if constexpr (rsl::is_same_v<T, glm::vec4>)
    {
      return DataType(DataType::Value::Vec4);
    }
    else if constexpr (rsl::is_same_v<T, glm::ivec2>)
    {
      return DataType(DataType::Value::Ivec2);
    }
    else if constexpr (rsl::is_same_v<T, glm::ivec3>)
    {
      return DataType(DataType::Value::Ivec3);
    }
    else if constexpr (rsl::is_same_v<T, glm::ivec4>)
    {
      return DataType(DataType::Value::Ivec4);
    }
    else if constexpr (rsl::is_same_v<T, glm::uvec2>)
    {
      return DataType(DataType::Value::Uvec2);
    }
    else if constexpr (rsl::is_same_v<T, glm::uvec3>)
    {
      return DataType(DataType::Value::Uvec3);
    }
    else if constexpr (rsl::is_same_v<T, glm::uvec4>)
    {
      return DataType(DataType::Value::Uvec4);
    }
    else if constexpr (rsl::is_same_v<T, rsl::string>)
    {
      return DataType(DataType::Value::String);
    }
    else if constexpr (rsl::is_same_v<T, rsl::string_view>)
    {
      return DataType(DataType::Value::String);
    }

    else
    {
      static_assert(rsl::internal::always_false<T>, "Invalid type for data type construction");
    }
  }
} // namespace rex