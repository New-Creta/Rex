#pragma once

namespace rex
{
    class DataType
    {
    public:
        enum class Value
        {
            NONE = 0,

            INT8,
            INT16,
            INT32,
            INT64,

            UNSIGNED_INT8,
            UNSIGNED_INT16,
            UNSIGNED_INT32,
            UNSIGNED_INT64,

            FLOAT32,
            FLOAT64,

            MAT3,
            MAT4,

            VEC2,
            VEC3,
            VEC4,

            IVEC2,
            IVEC3,
            IVEC4,

            UIVEC2,
            UIVEC3,
            UIVEC4,

            BOOL,

            SAMPLER2D,
            SAMPLER3D,
            SAMPLERCUBE,

            STRING
        };

    public:
        DataType(Value value);
        
        DataType(const DataType& other) = default;
        DataType(DataType&& other) noexcept = default;
        
        DataType& operator= (const DataType& other);
        DataType& operator= (DataType&& other) noexcept;

        bool operator==(const DataType& other) const;
        bool operator!=(const DataType& other) const;

        Value get() const;

        size_t to_type_id() const;
        size_t to_byte_size() const;

    private:
        Value m_value;
    };
}