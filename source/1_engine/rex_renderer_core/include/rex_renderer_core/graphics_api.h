#pragma once

namespace rex
{
    enum class GraphicsAPI
    {
        OpenGL,
        DirectX
    };

    #ifdef REX_RENDERER_OPENGL
        constexpr GraphicsAPI g_graphics_api = GraphicsAPI::OpenGL;
    #elif REX_RENDERER_DIRECTX
        constexpr GraphicsAPI g_graphics_api = GraphicsAPI::DirectX;
    #else
        #error unknown graphics API
    #endif
}