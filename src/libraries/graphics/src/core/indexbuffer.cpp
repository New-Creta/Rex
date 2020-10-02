#include "rex_graphics_pch.h"

#include "core/indexbuffer.h"
#include "core/renderer.h"

#include "api/opengl/core/glindexbuffer.h"

//-------------------------------------------------------------------------
rex::graphics::IndexBuffer* rex::graphics::IndexBuffer::create(uint32_t* indices, uint32_t size)
{
    switch (rex::graphics::Renderer::getAPI())
    {
    case RendererAPI::NONE:    RX_ASSERT_X(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
    case RendererAPI::OPENGL:  return new rex::opengl::IndexBuffer(indices, size);
    }

    RX_ASSERT_X(false, "Unknown RendererAPI!");
    return nullptr;
}