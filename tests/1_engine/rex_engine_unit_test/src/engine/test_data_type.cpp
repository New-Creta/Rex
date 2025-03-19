#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/engine/data_type.h"

TEST_CASE("TEST - Data Type - construction from type")
{
	REX_CHECK(rex::data_type<s8>() == rex::DataType::Int8);
	REX_CHECK(rex::data_type<s16>() == rex::DataType::Int16);
	REX_CHECK(rex::data_type<s32>() == rex::DataType::Int32);
	REX_CHECK(rex::data_type<s64>() == rex::DataType::Int64);

	REX_CHECK(rex::data_type<u8>() == rex::DataType::UnsignedInt8);
	REX_CHECK(rex::data_type<u16>() == rex::DataType::UnsignedInt16);
	REX_CHECK(rex::data_type<u32>() == rex::DataType::UnsignedInt32);
	REX_CHECK(rex::data_type<u64>() == rex::DataType::UnsignedInt64);

	REX_CHECK(rex::data_type<f32>() == rex::DataType::Float32);
	REX_CHECK(rex::data_type<f64>() == rex::DataType::Float64);

	REX_CHECK(rex::data_type<bool>() == rex::DataType::Bool);

	REX_CHECK(rex::data_type<glm::mat3>() == rex::DataType::Mat3);
	REX_CHECK(rex::data_type<glm::mat4>() == rex::DataType::Mat4);

	REX_CHECK(rex::data_type<glm::vec2>() == rex::DataType::Vec2);
	REX_CHECK(rex::data_type<glm::vec3>() == rex::DataType::Vec3);
	REX_CHECK(rex::data_type<glm::vec4>() == rex::DataType::Vec4);

	REX_CHECK(rex::data_type<glm::ivec2>() == rex::DataType::Ivec2);
	REX_CHECK(rex::data_type<glm::ivec3>() == rex::DataType::Ivec3);
	REX_CHECK(rex::data_type<glm::ivec4>() == rex::DataType::Ivec4);

	REX_CHECK(rex::data_type<glm::uvec2>() == rex::DataType::Uvec2);
	REX_CHECK(rex::data_type<glm::uvec3>() == rex::DataType::Uvec3);
	REX_CHECK(rex::data_type<glm::uvec4>() == rex::DataType::Uvec4);

	REX_CHECK(rex::data_type<rsl::string>() == rex::DataType::String);
	REX_CHECK(rex::data_type<rsl::string_view>() == rex::DataType::String);
}