#include "rex_renderer_core/rendering/scene_renderer.h"

#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/gfx/rhi.h"

namespace rex
{
	namespace gfx
	{
		SceneRenderer::SceneRenderer()
		{
			m_per_view_cb = rhi::create_constant_buffer(sizeof(PerViewData));
			m_per_pass_cb = rhi::create_constant_buffer(sizeof(PerPassData));
			m_per_material_cb = rhi::create_constant_buffer(100_kib);
			m_per_instance_cb = rhi::create_constant_buffer(100_kib);
		}

		void SceneRenderer::begin_scene(const Camera& camera)
		{
			// 1. Update the per view data.
			// This data consistss of data that's specified to the view (eg. camera)
			update_view_data(camera);

			// 2. Update the data per render pass
			// We currently only have a single render pass so this is not a concern for us right now
			update_pass_data();

			// 3. Update the data per material
			// Each object in the scene will have their own material
			// We need to make sure all our object's texture views are uploaded to the gpu (eg. textures)
			update_material_data();

			// 4. Update the per instance data
			// Each object may have data that's specific to each instance of that object (eg. world matrix)
			update_instance_data();			
		}

		void SceneRenderer::end_scene()
		{

		}

		void SceneRenderer::update_view_data(const Camera& camera)
		{
			
		}

		void SceneRenderer::update_pass_data()
		{

		}

		void SceneRenderer::update_material_data()
		{

		}

		void SceneRenderer::update_instance_data()
		{

		}
	}
}