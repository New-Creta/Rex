#pragma once 

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace rex
{
    namespace gfx
    {
        // This struct matches the constant buffer in the shader that holds per view data
		struct PerViewData
		{
			glm::mat4 view;
			glm::mat4 proj;
			glm::mat4 view_proj;
		};

		// This struct matches the constant buffer in the shader that holds per scene data
		struct PerSceneData
		{
			glm::vec3 light_direction;
		};

		// This struct matches the constant buffer in the shader that holds per instance data
		struct PerInstanceData
		{
			glm::mat4 world;
			glm::mat4 worldviewproj;
		};
    }
}