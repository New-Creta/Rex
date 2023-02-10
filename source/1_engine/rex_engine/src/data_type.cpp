#include "rex_engine/data_type.h"
#include "rex_engine/types.h"
#include "rex_engine/diagnostics/logging.h"

#include "rex_std/unordered_map.h"

#include <glm/glm.hpp>

namespace rex
{
    namespace data_type
    {
        //-----------------------------------------------------------------------
        rsl::unordered_map<DataType::Value, size_t>& get_data_type_typeid_map()
        {
            static rsl::unordered_map<DataType::Value, size_t> type_map = 
            {
                {DataType::Value::INT8, typeid(s8).hash_code()},
                {DataType::Value::INT16, typeid(s16).hash_code()},
                {DataType::Value::INT32, typeid(s32).hash_code()},
                {DataType::Value::INT64, typeid(s64).hash_code()},

                {DataType::Value::UNSIGNED_INT8, typeid(uint8).hash_code()},
                {DataType::Value::UNSIGNED_INT16, typeid(uint16).hash_code()},
                {DataType::Value::UNSIGNED_INT32, typeid(uint32).hash_code()},
                {DataType::Value::UNSIGNED_INT64, typeid(uint64).hash_code()},

                {DataType::Value::FLOAT32, typeid(float).hash_code()},
                {DataType::Value::FLOAT64, typeid(double).hash_code()},

                {DataType::Value::MAT3, typeid(glm::mat3).hash_code()},
                {DataType::Value::MAT4, typeid(glm::mat4).hash_code()},

                {DataType::Value::VEC2, typeid(glm::vec2).hash_code()},
                {DataType::Value::VEC3, typeid(glm::vec3).hash_code()},
                {DataType::Value::VEC4, typeid(glm::vec4).hash_code()},

                {DataType::Value::IVEC2, typeid(glm::ivec2).hash_code()},
                {DataType::Value::IVEC3, typeid(glm::ivec3).hash_code()},
                {DataType::Value::IVEC4, typeid(glm::ivec4).hash_code()},

                {DataType::Value::UIVEC2, typeid(glm::uvec2).hash_code()},
                {DataType::Value::UIVEC3, typeid(glm::uvec3).hash_code()},
                {DataType::Value::UIVEC4, typeid(glm::uvec4).hash_code()},

                {DataType::Value::BOOL, typeid(bool).hash_code()},

                {DataType::Value::STRING, typeid(std::string).hash_code()}
            };

            return type_map;
        }

        //-----------------------------------------------------------------------
        rsl::unordered_map<DataType::Value, size_t>& get_data_type_size_map()
        {
            static rsl::unordered_map<DataType::Value, size_t> type_map =
            {
                {DataType::Value::INT8, sizeof(s8)},
                {DataType::Value::INT16, sizeof(s16)},
                {DataType::Value::INT32, sizeof(s32)},
                {DataType::Value::INT64, sizeof(s64)},

                {DataType::Value::UNSIGNED_INT8, sizeof(uint8)},
                {DataType::Value::UNSIGNED_INT16, sizeof(uint16)},
                {DataType::Value::UNSIGNED_INT32, sizeof(uint32)},
                {DataType::Value::UNSIGNED_INT64, sizeof(uint64)},

                {DataType::Value::FLOAT32, sizeof(float)},
                {DataType::Value::FLOAT64, sizeof(double)},

                {DataType::Value::MAT3, sizeof(glm::mat3)},
                {DataType::Value::MAT4, sizeof(glm::mat4)},

                {DataType::Value::VEC2, sizeof(glm::vec2)},
                {DataType::Value::VEC3, sizeof(glm::vec3)},
                {DataType::Value::VEC4, sizeof(glm::vec4)},

                {DataType::Value::IVEC2, sizeof(glm::ivec2)},
                {DataType::Value::IVEC3, sizeof(glm::ivec3)},
                {DataType::Value::IVEC4, sizeof(glm::ivec4)},

                {DataType::Value::UIVEC2, sizeof(glm::uvec2)},
                {DataType::Value::UIVEC3, sizeof(glm::uvec3)},
                {DataType::Value::UIVEC4, sizeof(glm::uvec4)},

                {DataType::Value::BOOL, sizeof(bool)},

                {DataType::Value::STRING, sizeof(std::string)}
            };

            return type_map;
        }
    }

    //-----------------------------------------------------------------------
    DataType::DataType(const DataType& other)
        :m_value(other.m_value)
    {

    }
    //-----------------------------------------------------------------------
    DataType::DataType(DataType&& other) noexcept
        :m_value(std::exchange(other.m_value, DataType::Value::NONE))
    {

    }

    //-----------------------------------------------------------------------
    DataType::Value DataType::get() const
    {
        return m_value;
    }

    //-----------------------------------------------------------------------
    DataType& DataType::operator=(const DataType& other)
    {
        if (*this == other)
        {
            return *this;
        }

        m_value = other.m_value;

        return *this;
    }
    //-----------------------------------------------------------------------
    DataType& DataType::operator=(DataType&& other) noexcept
    {
        if (*this == other)
        {
            return *this;
        }

        m_value = std::exchange(other.m_value, DataType::Value::NONE);

        return *this;
    }

    //-----------------------------------------------------------------------
    DataType::DataType(Value value)
        :m_value(value)
    {

    }

    //-----------------------------------------------------------------------
    size_t DataType::to_type_id() const
    {
        auto& typeid_map = data_type::get_data_type_typeid_map();
        auto typeid_it = typeid_map.find(m_value);

        if (typeid_it == std::cend(typeid_map))
        {
            REX_ERROR("Unsupported DataType to convert to typeid: {0}", (int32)m_value);
            return 0;
        }

        return typeid_it->second();
    }
    //-----------------------------------------------------------------------
    size_t DataType::to_byte_size() const
    {
        auto& size_map = data_type::get_data_type_size_map();
        auto size_it = size_map.find(m_value);

        if (size_it == std::cend(size_map))
        {
            REX_ERROR("Unsupported DataType to convert to byte size: {0}", (int32)m_value);
            return 0;
        }

        return size_it->second();
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
}