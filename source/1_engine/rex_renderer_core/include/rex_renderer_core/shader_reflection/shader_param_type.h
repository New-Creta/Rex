#pragma once

namespace rex
{
	namespace gfx
	{
		enum class ShaderParamComponentType
		{
			Unknown,
			Uint16,			// A single unsigned integer of 16 bits
			Uint,				// A single unsigned integer of 32 bits
			Uint64,			// A single unsigned integer of 64 bits
			Sint16,			// A single signed integer of 16 bits
			Sint,				// A single signed integer of 32 bits
			Sint64,			// A single signed integer of 64 bits
			Float16,		// A single float of 16 bits
			Float,			// A single float of 32 bits
			Float64,		// A single float of 64 bits
		};
	}
}