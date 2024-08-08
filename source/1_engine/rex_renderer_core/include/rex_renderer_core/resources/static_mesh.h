#pragma once

#include "rex_renderer_core/primitives/mesh_factory.h"

#include "rex_renderer_core/resources/vertex_buffer.h"
#include "rex_renderer_core/resources/index_buffer.h"

namespace rex
{
	namespace gfx
	{
		class StaticMesh
		{
		public:
			StaticMesh(rsl::unique_ptr<VertexBuffer> vb, rsl::unique_ptr<IndexBuffer> ib);

			// The vertex buffer of the static mesh
			VertexBuffer* vb() const;
			// The index buffer of the static mesh
			IndexBuffer* ib() const;

		private:
			rsl::unique_ptr<VertexBuffer> m_vertex_buffer;
			rsl::unique_ptr<IndexBuffer> m_index_buffer;
		};
	}
}