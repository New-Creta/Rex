#include "rex_renderer_core/rendering/camera.h"

namespace rex
{
	namespace gfx
	{
		Camera::Camera(const glm::vec3& pos, rsl::DegAngle fov, f32 width, f32 height, f32 near, f32 far, ProjectionMode projectionMode)
			: m_position(pos) 
			, m_forward(glm::vec3() - m_position) // Always looking at center
			, m_view(glm::translate(glm::mat4(1.0f), m_position))
			, m_projection()
		{
			switch (m_projection_mode)
			{
				case ProjectionMode::Perspective: m_projection = glm::perspectiveFov(glm::radians(fov.get()), width, height, near, far); break;
				case ProjectionMode::Ortographic: m_projection = glm::orthoLH_ZO(0.0f, width, 0.0f, height, near, far); break;
			}
		}

		const glm::vec3& Camera::position() const
		{
			return m_position;
		}
		const glm::vec3& Camera::forward() const
		{
			return m_forward;
		}

		glm::mat4 Camera::view_mat() const
		{
			const glm::vec3 target = m_position + m_forward;
			const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

			return glm::lookAt(m_position, target, up);
		}
		const glm::mat4& Camera::projection_mat() const
		{
			return m_projection;
		}
	}
}