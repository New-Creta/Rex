#pragma once

#include "rex_engine/gfx/resources/raster_state.h"
#include "rex_engine/gfx/resources/blend_state.h"
#include "rex_engine/gfx/resources/depth_stencil_state.h"

#include "rex_engine/text_processing/json.h"

namespace rex
{
	namespace gfx
	{
		// The configuration of the output merger all bundled into a single struct
		struct OutputMergerDesc
		{
			RasterStateDesc raster_state;					// the rasterizer settings that'll be stored inside the material
			BlendDesc blend_state;								// the blend settings that'll be stored inside the material
			DepthStencilDesc depth_stencil_state;	// the depth stencil settings that'll be stored inside the material
		};

		OutputMergerDesc load_output_merger_from_json(const rex::json::json& jsonBlob);
	}
}