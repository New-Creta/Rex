#pragma once

namespace rex
{
	namespace gfx
	{
		// A resource view is a pointer like object pointing to an actual resource on the gpu
		// It holds no data in the base class as it just acts as an interface
		class ResourceView
		{
		public:
			virtual ~ResourceView() = default;
		};
	}
}