#include "rex_graphics_pch.h"

#include "core/vertexarray.h"
#include "core/renderer.h"

#include "api/opengl/core/glvertexarray.h"

//-------------------------------------------------------------------------
rex::graphics::VertexArray::VertexArray() = default;
//-------------------------------------------------------------------------
rex::graphics::VertexArray::~VertexArray() = default;

//-------------------------------------------------------------------------
std::shared_ptr<rex::graphics::VertexArray> rex::graphics::VertexArray::create()
{
    switch (rex::graphics::Renderer::getAPI())
    {
    case RendererAPI::Type::NONE:    RX_ASSERT_X(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
    case RendererAPI::Type::OPENGL:  return std::make_shared<rex::opengl::VertexArray>();
    }

    RX_ASSERT_X(false, "Unknown RendererAPI!");
    return nullptr;
}
