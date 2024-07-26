#pragma once

#include "glm/glm.hpp"

namespace rex
{
	namespace gfx
	{
		class Camera
		{
		public:
			glm::vec3 position() const;
			glm::vec3 forward() const;

		private:
			glm::vec3 m_position;
			glm::vec3 m_forward;
		};
	}
}