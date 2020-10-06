#include "rex_graphics_pch.h"

#include "core/indexbuffer.h"
#include "core/renderer.h"

#include "api/opengl/core/glindexbuffer.h"

//-------------------------------------------------------------------------
std::shared_ptr<rex::graphics::IndexBuffer> rex::graphics::IndexBuffer::create(uint32_t* indices, uint32_t size)
{
    switch (rex::graphics::Renderer::getAPI())
    {
    case RendererAPI::Type::NONE:    RX_ASSERT_X(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
    case RendererAPI::Type::OPENGL:  return std::make_shared<rex::opengl::IndexBuffer>(indices, size);
    }

    RX_ASSERT_X(false, "Unknown RendererAPI!");
    return nullptr;
}