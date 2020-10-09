#pragma once

#include "rex_graphics_global.h"

#include "core/rendererapi.h"

namespace rex
{
    namespace graphics
    {
        class Mesh;
        class OrthographicCamera;

        class Renderer
        {   
        public:
            REX_GRAPHICS_EXPORT static void initialize(RendererAPI::Type type);
            REX_GRAPHICS_EXPORT static void destroy();

            REX_GRAPHICS_EXPORT static void beginScene(OrthographicCamera& camera);
            REX_GRAPHICS_EXPORT static void endScene();

            REX_GRAPHICS_EXPORT static void submit(Mesh* mesh);
            REX_GRAPHICS_EXPORT static void submit(Mesh* mesh, const glm::mat4& modelMat);
            REX_GRAPHICS_EXPORT static void flush();

            inline static RendererAPI::Type getAPI() { return s_renderer_api->getAPI(); }

        private:
            struct SceneData
            {
                glm::mat4 m_viewprojection_matrix;
            };

            static std::unique_ptr<RendererAPI>     s_renderer_api;
            static std::unique_ptr<SceneData>       s_scene_data;
            static bool                             s_initialized;
        };
    }
}