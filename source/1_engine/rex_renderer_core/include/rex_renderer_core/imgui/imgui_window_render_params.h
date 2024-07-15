#pragma once

namespace rex
{
	namespace gfx
	{
		class PipelineState;
		class Material;

		struct ImGuiWindowRenderParams
		{
			PipelineState* pso;
			Material* material;
		};
	}
}