#pragma once

namespace rex
{
	namespace gfx
	{
		class PipelineState;
		class RenderPass;

		// The parameters needed to set on the render context for imgui rendering
		struct ImGuiWindowRenderParams
		{
			RenderPass* render_pass; // The material to be set on the render context for imgui rendering
		};
	}
}