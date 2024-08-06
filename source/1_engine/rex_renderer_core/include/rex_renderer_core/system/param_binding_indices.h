#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
	namespace gfx
	{
		// Shader parameter slots are dynamically assigned
		// It's possible a shader needs constant buffers but no textures
		// while another shader needs only no constant buffers and only textures and samplers
		// So the slots below are dynamically filled in, always in the order they're declared here
		struct ParamBindingSlots
		{
			s32 material_cbs_slot = -1;							// Slot in the parameters that holds the constant buffers meant to be set by a material
			s32 material_textures_slot = -1;				// Slot in the parameters that holds the textures meant to be set by a material
			s32 material_samplers_slot = -1;				// Slot in the parameters that holds the samplers meant to be set by a material

			s32 renderpass_cbs_slot = -1;						// Slot in the parameters that holds the constant buffers meant to be set by a renderpass
			s32 renderpass_textures_slot = -1;			// Slot in the parameters that holds the textures meant to be set by a renderpass
			s32 renderpass_samplers_slot = -1;			// Slot in the parameters that holds the samplers meant to be set by a renderpass
		};
	}
}