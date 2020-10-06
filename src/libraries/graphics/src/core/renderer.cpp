#include "rex_graphics_pch.h"

#include "core/orthographiccamera.h"
#include "core/renderer.h"
#include "core/shader.h"
#include "core/mesh.h"

//-------------------------------------------------------------------------
std::unique_ptr<rex::graphics::RendererAPI> rex::graphics::Renderer::s_renderer_api = nullptr;
//-------------------------------------------------------------------------
std::unique_ptr<rex::graphics::Renderer::SceneData> rex::graphics::Renderer::s_scene_data = nullptr;

//-------------------------------------------------------------------------
bool rex::graphics::Renderer::s_initialized = false;

//-------------------------------------------------------------------------
void rex::graphics::Renderer::initialize(RendererAPI::Type type)
{
    s_renderer_api = rex::graphics::RendererAPI::create(type);
    s_scene_data = std::make_unique<SceneData>();

    s_initialized = true;
}
//-------------------------------------------------------------------------
void rex::graphics::Renderer::destroy()
{
    s_renderer_api.reset();

    s_initialized = false;
}

//-------------------------------------------------------------------------
void rex::graphics::Renderer::beginScene(OrthographicCamera& camera)
{
    s_scene_data->m_viewprojection_matrix = camera.getViewProjectionMatrix();

    s_renderer_api->clear();
}
//-------------------------------------------------------------------------
void rex::graphics::Renderer::endScene()
{
    flush();
}

//-------------------------------------------------------------------------
void rex::graphics::Renderer::submit(Mesh* mesh)
{
    mesh->getShader()->bind();
    mesh->getShader()->uploadMat4("u_ViewProjection", s_scene_data->m_viewprojection_matrix);

    s_renderer_api->drawIndexed(mesh->getShader(), mesh->getVertexArray());
}

//-------------------------------------------------------------------------
void rex::graphics::Renderer::flush()
{
    s_renderer_api->flush();
}