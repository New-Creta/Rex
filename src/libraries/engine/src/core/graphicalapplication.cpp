#include "rex_engine_pch.h"

#include "core/graphicalapplication.h"
#include "core/layer.h"
#include "core/bufferlayout.h"

#include "events/event.h"
#include "events/input/keydown.h"

#include "events/eventdispatcher.h"

#include "api/opengl/core/glshader.h"
#include "api/opengl/core/glindexbuffer.h"
#include "api/opengl/core/glvertexbuffer.h"

namespace
{
#ifdef _OPENGL

    //-------------------------------------------------------------------------
    GLenum shaderDataTypeToOpenGLBaseType(rex::graphics::ShaderDataType type)
    {
        switch (type)
        {
        case rex::graphics::ShaderDataType::FLOAT:    return GL_FLOAT;
        case rex::graphics::ShaderDataType::FLOAT2:   return GL_FLOAT;
        case rex::graphics::ShaderDataType::FLOAT3:   return GL_FLOAT;
        case rex::graphics::ShaderDataType::FLOAT4:   return GL_FLOAT;
        case rex::graphics::ShaderDataType::MAT3:     return GL_FLOAT;
        case rex::graphics::ShaderDataType::MAT4:     return GL_FLOAT;
        case rex::graphics::ShaderDataType::INT:      return GL_INT;
        case rex::graphics::ShaderDataType::INT2:     return GL_INT;
        case rex::graphics::ShaderDataType::INT3:     return GL_INT;
        case rex::graphics::ShaderDataType::INT4:     return GL_INT;
        case rex::graphics::ShaderDataType::BOOL:     return GL_BOOL;
        }

        RX_ASSERT_X(false, "Unknown ShaderDataType!");
        return 0;
    }

#endif
}

//-------------------------------------------------------------------------
rex::engine::GraphicalApplication::GraphicalApplication()
    :m_window(nullptr)
    ,m_context(nullptr)
{}
//-------------------------------------------------------------------------
rex::engine::GraphicalApplication::~GraphicalApplication() = default;

//-------------------------------------------------------------------------
rex::Window* rex::engine::GraphicalApplication::getWindow()
{
    return m_window.get();
}
//-------------------------------------------------------------------------
const rex::Window* rex::engine::GraphicalApplication::getWindow() const
{
    return m_window.get();
}

//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::pushBackLayer(std::unique_ptr<Layer> layer)
{
    m_layer_stack.pushBack(std::move(layer));
}
//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::pushFrontLayer(std::unique_ptr<Layer> layer)
{
    m_layer_stack.pushFront(std::move(layer));
}

//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::onEvent(events::Event & event)
{
    RX_TRACE(event.toString());

    for (auto it = m_layer_stack.crbegin(); it != m_layer_stack.rend() && event.isHandled() == false; ++it)
    {
        (*it)->onEvent(event);
    }
}

//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::appInitialize()
{
    m_window = createWindow();
    m_context = createContext();

    Input::createInstance();

    ApplicationContext::makeCurrent(m_context.get());

#ifdef _OPENGL

    if (glewInit() != GLEW_OK)
    {
        RX_ERROR("Failed to init GLEW");
    }

    glGenVertexArrays(1, &m_vertex_array);
    glBindVertexArray(m_vertex_array);

    float vertices[3 * 7] = 
    {
        -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
         0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
    };

    m_vertex_buffer.reset(graphics::VertexBuffer::create(vertices, sizeof(vertices)));

    {
        graphics::BufferLayout layout = {
            { graphics::ShaderDataType::FLOAT3, "a_Position" },
            { graphics::ShaderDataType::FLOAT4, "a_Color" }
        };

        m_vertex_buffer->setLayout(layout);
    }

    uint32_t index = 0;
    const auto& layout = m_vertex_buffer->getLayout();
    for (const auto& element : layout)
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,
            element.getComponentCount(),
            shaderDataTypeToOpenGLBaseType(element.type),
            element.normalized ? GL_TRUE : GL_FALSE,
            layout.getStride(),
#pragma warning( push )
#pragma warning( disable : 4312 )
            reinterpret_cast<const void*>(element.offset));
#pragma warning( pop )
        index++;
    }

    unsigned int indices[3] = { 0, 1, 2 };

    m_index_buffer.reset(graphics::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));

    std::string vertex_source = R"(
            #version 330 core
            
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;

            out vec3 v_Position;
            out vec4 v_Color;

            void main()
            {
                v_Position = a_Position;
                v_Color = a_Color;

                gl_Position = vec4(a_Position, 1.0);    
            }
        )";

    std::string fragment_source = R"(
            #version 330 core
            
            layout(location = 0) out vec4 color;

            in vec3 v_Position;
            in vec4 v_Color;

            void main()
            {
                color = vec4(v_Position * 0.5 + 0.5, 1.0);
                color = v_Color;
            }
        )";

    m_shader = std::make_unique<opengl::Shader>(vertex_source, fragment_source);

#endif

    m_window->show();
}
//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::appUpdate(float dTime)
{
    UNUSED_PARAM(dTime);

    unsigned int width = m_window->getWidth();
    unsigned int height = m_window->getHeight();

    m_window->processEvents();

    ApplicationContext::makeCurrent(m_context.get());

    m_context->resize(width, height);
    m_context->clear();

#ifdef _OPENGL

    m_shader->bind();
    glBindVertexArray(m_vertex_array);
    glDrawElements(GL_TRIANGLES, m_index_buffer->getCount(), GL_UNSIGNED_INT, nullptr);

#endif

    for (const std::unique_ptr<Layer>& layer : m_layer_stack)
    {
        layer->onUpdate();
    }

    m_window->update();
    m_context->swapBuffers();
}
//-------------------------------------------------------------------------
void rex::engine::GraphicalApplication::appQuit()
{
    Input::destroyInstance();

    m_window->hide();

    ApplicationContext::destroy(m_context.get());
}

//-------------------------------------------------------------------------
std::unique_ptr<rex::engine::ApplicationWindow> rex::engine::GraphicalApplication::createWindow()
{
    const std::string WINDOW_TITLE = "Rex";

    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;

    rex::WindowProperties properties =
    {
        WINDOW_TITLE,

        WINDOW_WIDTH,
        WINDOW_HEIGHT,

        [this](events::Event& event) { GraphicalApplication::onEvent(event); }
    };

    return std::make_unique<ApplicationWindow>(properties);
}
//-------------------------------------------------------------------------
std::unique_ptr<rex::engine::ApplicationContext> rex::engine::GraphicalApplication::createContext()
{
    return ApplicationContext::create(m_window->getHandle());
}
