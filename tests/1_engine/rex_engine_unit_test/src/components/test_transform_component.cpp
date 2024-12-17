#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/components/transform_component.h"
#include "rex_engine/engine/types.h"

#include "rex_std/bonus/math/float.h"

namespace rex::test
{
	// It's possible due to floating point precision errors there are very tiny value differences
	// To ignore these error margins, we use a special function to compare matrices
	bool are_matrices_equal(const glm::mat4& lhs, const glm::mat4& rhs)
	{
		bool is_equal = true;
		for (s32 i = 0; i < 4; ++i)
		{
			const glm::vec4& v1 = lhs[i];
			const glm::vec4& v2 = rhs[i];
			is_equal = rsl::equals(v1.x, v2.x) &&
				rsl::equals(v1.y, v2.y) &&
				rsl::equals(v1.z, v2.z) &&
				rsl::equals(v1.w, v2.w);

			if (!is_equal)
			{
				return false;
			}
		}

		return is_equal;
	}
}

TEST_CASE("Are Matrices Equal")
{
	glm::mat4 mat1(1.0f);
	glm::mat4 mat2(1.0f);
	glm::mat4 mat3(2.0f);
	glm::mat4 mat4 =
	{
		1.0000008f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	REX_CHECK(rex::test::are_matrices_equal(mat1, mat1));
	REX_CHECK(rex::test::are_matrices_equal(mat1, mat2));
	REX_CHECK(rex::test::are_matrices_equal(mat1, mat3) == false);
	REX_CHECK(rex::test::are_matrices_equal(mat2, mat3) == false);
	REX_CHECK(rex::test::are_matrices_equal(mat1, mat4));
	REX_CHECK(rex::test::are_matrices_equal(mat2, mat4));
	REX_CHECK(rex::test::are_matrices_equal(mat3, mat4) == false);
}

TEST_CASE("Transform component construction")
{
	rex::gfx::TransformComponent transform{};

	REX_CHECK(transform.translation == glm::vec3());
	REX_CHECK(transform.rotation_euler == glm::vec3());
	REX_CHECK(transform.scale == glm::vec3(1.0f, 1.0f, 1.0f));
	REX_CHECK(rex::test::are_matrices_equal(transform.world_mat(), glm::mat4(1.0f)));
}

TEST_CASE("Transform component translation")
{
	rex::gfx::TransformComponent transform{};
	glm::vec3 position(1.0f, 2.0f, 3.0f);
	transform.translation = position;

	glm::mat4 desired_world_mat
	{
		1.0f, 0.0f, 0.0f, position.x,
		0.0f, 1.0f, 0.0f, position.y,
		0.0f, 0.0f, 1.0f, position.z,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	REX_CHECK(transform.translation == position);
	REX_CHECK(transform.rotation_euler == glm::vec3());
	REX_CHECK(transform.scale == glm::vec3(1.0f, 1.0f, 1.0f));
	REX_CHECK(rex::test::are_matrices_equal(transform.world_mat(), desired_world_mat));
}

TEST_CASE("Transform component rotation")
{
	f32 radians = glm::radians(45.0f);

	glm::mat4 world_mat(1.0f);

	f32 cos_value = cos(radians);
	f32 sin_value = sin(radians);

	// Keep in mind that glm is column major
	glm::mat4 desired_world_mat_x_rotation
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos_value, sin_value, 0.0f,
		0.0f, -sin_value, cos_value, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	glm::mat4 desired_world_mat_y_rotation
	{
		cos_value, 0.0f, -sin_value, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin_value, 0.0f, cos_value, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	glm::mat4 desired_world_mat_z_rotation
	{
		cos_value, sin_value, 0.0f, 0.0f,
		-sin_value, cos_value, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	glm::mat4 world_mat_x_rotation = glm::rotate(world_mat, radians, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 world_mat_y_rotation = glm::rotate(world_mat, radians, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 world_mat_z_rotation = glm::rotate(world_mat, radians, glm::vec3(0.0f, 0.0f, 1.0f));

	REX_CHECK(world_mat_x_rotation == desired_world_mat_x_rotation);
	REX_CHECK(world_mat_y_rotation == desired_world_mat_y_rotation);
	REX_CHECK(world_mat_z_rotation == desired_world_mat_z_rotation);

	rex::gfx::TransformComponent transform{};
	transform.rotate_x(rsl::rad_angle(radians));
	world_mat = glm::transpose(transform.world_mat()); // We return in row major 
	REX_CHECK(rex::test::are_matrices_equal(world_mat, desired_world_mat_x_rotation));

	transform = rex::gfx::TransformComponent();
	transform.rotate_y(rsl::rad_angle(radians));
	world_mat = glm::transpose(transform.world_mat()); // We return in row major 
	REX_CHECK(rex::test::are_matrices_equal(world_mat, desired_world_mat_y_rotation));

	transform = rex::gfx::TransformComponent();
	transform.rotate_z(rsl::rad_angle(radians));
	world_mat = glm::transpose(transform.world_mat()); // We return in row major 
	REX_CHECK(rex::test::are_matrices_equal(world_mat, desired_world_mat_z_rotation));
}

TEST_CASE("Transform component scale")
{
	rex::gfx::TransformComponent transform{};
	glm::vec3 scale = glm::vec3(2.0f, 3.0f, 4.0f);
	transform.scale = scale;

	glm::mat4 desired_world_mat
	{
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	REX_CHECK(transform.translation == glm::vec3());
	REX_CHECK(transform.rotation_euler == glm::vec3());
	REX_CHECK(transform.scale == scale);
	REX_CHECK(rex::test::are_matrices_equal(transform.world_mat(), desired_world_mat));
}

TEST_CASE("Transform component transformation")
{
	rex::gfx::TransformComponent transform{};

	transform.translation = glm::vec3(1.0f, 2.0f, 3.0f);
	transform.rotation_euler = glm::vec3(45.0f, 30.0f, 60.0f); // Pitch, Yaw, Roll
	transform.scale = glm::vec3(2.0f, 2.0f, 2.0f);

	glm::mat4 world_matrix = transform.world_mat();

	// Expected matrix: Translation * Rotation * Scale
	glm::mat4 desired_world_matrix = glm::translate(glm::mat4(1.0f), transform.translation);

	glm::quat rotationQuat = glm::quat(glm::radians(transform.rotation_euler));
	desired_world_matrix *= glm::toMat4(rotationQuat);

	desired_world_matrix = glm::scale(desired_world_matrix, transform.scale);
	desired_world_matrix = glm::transpose(desired_world_matrix);

	rex::test::are_matrices_equal(world_matrix, desired_world_matrix);
}