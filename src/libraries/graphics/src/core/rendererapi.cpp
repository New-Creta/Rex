#include "rex_graphics_pch.h"

#include "core/renderer.h"

#include "api/opengl/core/glrendererapi.h"

//-------------------------------------------------------------------------
std::unique_ptr<rex::graphics::RendererAPI> rex::graphics::RendererAPI::create(RendererAPI::Type type)
{
    switch (type)
    {
    case RendererAPI::Type::NONE:    RX_ASSERT_X(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
    case RendererAPI::Type::OPENGL:  return std::make_unique<rex::opengl::RendererAPI>();
    }

    RX_ASSERT_X(false, "Unknown RendererAPI!");
    return nullptr;
}
