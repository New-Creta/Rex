#pragma once

namespace rex
{
  class DataType
  {
  public:
    enum class Value
    {
      None = 0,

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

      Sampler2D,
      Sampler3D,
      SamplerCube,

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
} // namespace rex