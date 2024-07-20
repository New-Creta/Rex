#pragma once

#include "rex_renderer_core/resources/resource.h"

namespace rex
{
	namespace gfx
	{
		// A sampler is an object that holds the settings how a texture is applied on a triangle
		class Sampler2D : public Resource
		{
		public:
			virtual ~Sampler2D() = default;

		private:

		};
	}
}