#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/components/transform_component.h"
#include "rex_engine/engine/types.h"

TEST_CASE("Transform component construction")
{
	rex::gfx::TransformComponent component{};

	REX_CHECK(component.translation == glm::vec3());
	REX_CHECK(component.rotation == glm::vec3());
	REX_CHECK(component.scale == glm::vec3());
	REX_CHECK(component.world_mat() == glm::mat4(1.0f));
}

TEST_CASE("Transform component translation")
{
	rex::gfx::TransformComponent component{};
	glm::vec3 position(1.0f, 2.0f, 3.0f);

	glm::mat4 desired_world_mat
	{
		1.0f, 0.0f, 0.0f, position.x,
		0.0f, 1.0f, 0.0f, position.y,
		0.0f, 0.0f, 1.0f, position.z,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	REX_CHECK(component.translation == position);
	REX_CHECK(component.rotation == glm::vec3());
	REX_CHECK(component.scale == glm::vec3());
	REX_CHECK(component.world_mat() == desired_world_mat);
}

TEST_CASE("Transform component rotation")
{
	rex::gfx::TransformComponent component{};
	f32 radians = glm::radians(45.0f);

	glm::mat4 world_mat(1.0f);

	f32 cos_value = cos(radians);
	f32 sin_value = sin(radians);

	glm::mat4 desired_world_mat_x_rotation
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos_value, -sin_value, 0.0f,
		0.0f, sin_value, cos_value, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	glm::mat4 desired_world_mat_y_rotation
	{
		cos_value, 0.0f, sin_value, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sin_value, 0.0f, cos_value, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	glm::mat4 desired_world_mat_z_rotation
	{
		cos_value, -sin_value, 0.0f, 0.0f,
		sin_value, cos_value, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	glm::mat4 world_mat_x_rotation = glm::rotate(world_mat, radians, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 world_mat_y_rotation = glm::rotate(world_mat, radians, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 world_mat_z_rotation = glm::rotate(world_mat, radians, glm::vec3(0.0f, 0.0f, 1.0f));

	REX_CHECK(component.translation == glm::vec3());
	REX_CHECK(component.rotation == glm::vec3());
	REX_CHECK(component.scale == glm::vec3());
	REX_CHECK(component.world_mat() == desired_world_mat);
}

TEST_CASE("Transform component scale")
{
	rex::gfx::TransformComponent component{};
	glm::vec3 scale(2.0f, 3.0f, 4.0f);

	glm::mat4 desired_world_mat
	{
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	REX_CHECK(component.translation == glm::vec3());
	REX_CHECK(component.rotation == glm::vec3());
	REX_CHECK(component.scale == scale);
	REX_CHECK(component.world_mat() == desired_world_mat);
}

TEST_CASE("Transform component transformation")
{

}