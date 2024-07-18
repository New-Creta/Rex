#pragma once

namespace rex
{
	namespace gfx
	{
		class PipelineState;
		class Material;

		// The parameters needed to set on the render context for imgui rendering
		struct ImGuiWindowRenderParams
		{
			PipelineState* pso; // The pso to be set on the render context for imgui rendering
			Material* material; // The material to be set on the render context for imgui rendering
		};
	}
}