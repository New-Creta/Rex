#pragma once

namespace rex
{
	namespace gfx
	{
		class VertexBuffer;
		class IndexBuffer;
		class Material;
		class ConstantBuffer;

		// A drawlist holds data needed to draw a single element to the screen
		struct DrawList
		{
			Material* material;
			VertexBuffer* vb;   // The vertex buffer of the object
			IndexBuffer* ib;	// The index buffer of the object
			ConstantBuffer* per_instance_cb; // The constant buffer holding per instance data
		};

		// Renderer interface, very basic implementation and will likely be extended in the future
		class Renderer
		{
		public:
			virtual ~Renderer() = default;

			// Prepare the renderer for a new frame
			virtual void new_frame() = 0;

			// Render a new frame
			virtual void render() = 0;
		};
	}
}