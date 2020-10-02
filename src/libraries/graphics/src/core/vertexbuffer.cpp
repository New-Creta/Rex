#include "rex_graphics_pch.h"

#include "core/vertexbuffer.h"
#include "core/renderer.h"

#include "api/opengl/core/glvertexbuffer.h"

//-------------------------------------------------------------------------
rex::graphics::VertexBuffer* rex::graphics::VertexBuffer::create(float* vertices, uint32_t size)
{
    switch (rex::graphics::Renderer::getAPI())
    {
    case RendererAPI::NONE:    RX_ASSERT_X(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
    case RendererAPI::OPENGL:  return new rex::opengl::VertexBuffer(vertices, size);
    }

    RX_ASSERT_X(false, "Unknown RendererAPI!");
    return nullptr;
}