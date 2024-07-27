#include "rex_renderer_core/rendering/camera.h"

namespace rex
{
	namespace gfx
	{
		Camera::Camera(rsl::DegAngle fov, f32 width, f32 height, f32 near, f32 far)
			: m_position(0, 5, -5)
			, m_forward(glm::vec3() - m_position)
			, m_view(glm::translate(glm::mat4(1.0f), m_position))
			, m_projection(glm::perspectiveFov(0.25f * glm::pi<f32>(), width, height, near, far))
		{}

		glm::vec3 Camera::position() const
		{
			return m_position;
		}
		glm::vec3 Camera::forward() const
		{
			return m_forward;
		}

		glm::mat4 Camera::view_mat() const
		{
			glm::vec3 eye_pos_w;
			eye_pos_w.x = 15.0f * sinf(0.2f * glm::pi<f32>()) * cosf(1.5f * glm::pi<f32>());
			eye_pos_w.y = 15.0f * cosf(0.2f * glm::pi<f32>());
			eye_pos_w.z = 35.0f * sinf(0.2f * glm::pi<f32>()) * sinf(1.5f * glm::pi<f32>());

			const glm::vec3 pos = glm::vec3(eye_pos_w.x, eye_pos_w.y, eye_pos_w.z);
			const glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
			const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

			return glm::lookAt(pos, target, up);
		}
		glm::mat4 Camera::projection_mat() const
		{
			return m_projection;
		}
	}
}