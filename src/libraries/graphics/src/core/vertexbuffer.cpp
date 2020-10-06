#include "rex_graphics_pch.h"

#include "core/vertexbuffer.h"
#include "core/renderer.h"

#include "api/opengl/core/glvertexbuffer.h"

//-------------------------------------------------------------------------
std::shared_ptr<rex::graphics::VertexBuffer> rex::graphics::VertexBuffer::create(float* vertices, uint32_t size)
{
    switch (rex::graphics::Renderer::getAPI())
    {
    case RendererAPI::Type::NONE:    RX_ASSERT_X(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
    case RendererAPI::Type::OPENGL:  return std::make_shared<rex::opengl::VertexBuffer>(vertices, size);
    }

    RX_ASSERT_X(false, "Unknown RendererAPI!");
    return nullptr;
}