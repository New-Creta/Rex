#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/engine/data_type.h"

TEST_CASE("TEST - Data Type - construction from type")
{
	REX_CHECK(rex::data_type<s8>().get() == rex::DataType::Value::Int8);
	REX_CHECK(rex::data_type<s16>().get() == rex::DataType::Value::Int16);
	REX_CHECK(rex::data_type<s32>().get() == rex::DataType::Value::Int32);
	REX_CHECK(rex::data_type<s64>().get() == rex::DataType::Value::Int64);

	REX_CHECK(rex::data_type<u8>().get() == rex::DataType::Value::UnsignedInt8);
	REX_CHECK(rex::data_type<u16>().get() == rex::DataType::Value::UnsignedInt16);
	REX_CHECK(rex::data_type<u32>().get() == rex::DataType::Value::UnsignedInt32);
	REX_CHECK(rex::data_type<u64>().get() == rex::DataType::Value::UnsignedInt64);

	REX_CHECK(rex::data_type<f32>().get() == rex::DataType::Value::Float32);
	REX_CHECK(rex::data_type<f64>().get() == rex::DataType::Value::Float64);

	REX_CHECK(rex::data_type<bool>().get() == rex::DataType::Value::Bool);

	REX_CHECK(rex::data_type<glm::mat3>().get() == rex::DataType::Value::Mat3);
	REX_CHECK(rex::data_type<glm::mat4>().get() == rex::DataType::Value::Mat4);

	REX_CHECK(rex::data_type<glm::vec2>().get() == rex::DataType::Value::Vec2);
	REX_CHECK(rex::data_type<glm::vec3>().get() == rex::DataType::Value::Vec3);
	REX_CHECK(rex::data_type<glm::vec4>().get() == rex::DataType::Value::Vec4);

	REX_CHECK(rex::data_type<glm::ivec2>().get() == rex::DataType::Value::Ivec2);
	REX_CHECK(rex::data_type<glm::ivec3>().get() == rex::DataType::Value::Ivec3);
	REX_CHECK(rex::data_type<glm::ivec4>().get() == rex::DataType::Value::Ivec4);

	REX_CHECK(rex::data_type<glm::uvec2>().get() == rex::DataType::Value::Uvec2);
	REX_CHECK(rex::data_type<glm::uvec3>().get() == rex::DataType::Value::Uvec3);
	REX_CHECK(rex::data_type<glm::uvec4>().get() == rex::DataType::Value::Uvec4);

	REX_CHECK(rex::data_type<rsl::string>().get() == rex::DataType::Value::String);
	REX_CHECK(rex::data_type<rsl::string_view>().get() == rex::DataType::Value::String);
}