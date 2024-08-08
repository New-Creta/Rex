#include "rex_renderer_core/resources/static_mesh.h"

#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/gfx/graphics.h"

namespace rex
{
	namespace gfx
	{
		StaticMesh::StaticMesh(rsl::unique_ptr<VertexBuffer> vb, rsl::unique_ptr<IndexBuffer> ib)
			: m_vertex_buffer(rsl::move(vb))
			, m_index_buffer(rsl::move(ib))
		{
		}

		VertexBuffer* StaticMesh::vb() const
		{
			return const_cast<VertexBuffer*>(m_vertex_buffer.get());
		}
		IndexBuffer* StaticMesh::ib() const
		{
			return const_cast<IndexBuffer*>(m_index_buffer.get());
		}
	}
}