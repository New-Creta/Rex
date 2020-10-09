#include "core/entrypoint.h"

#include "core/application.h"
#include "core/layer.h"
#include "core/imguilayer.h"
#include "core/keycodes.h"
#include "core/deltatime.h"

#include "core/vertexarray.h"
#include "core/bufferlayout.h"
#include "core/vertexbuffer.h"
#include "core/indexbuffer.h"
#include "core/shaderdatatype.h"
#include "core/mesh.h"
#include "core/orthographiccamera.h"
#include "core/renderer.h"

#include "events/event.h"
#include "events/eventdispatcher.h"

#include <glm/gtc/matrix_transform.hpp>

namespace
{
    class Triangle : public rex::graphics::Mesh
    {
    public:
        Triangle(rex::graphics::Shader* shader)
            :Mesh(shader)
        {
            setVertexArray(rex::graphics::VertexArray::create());

            float vertices[3 * 7] =
            {
                -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
                 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
                 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
            };

            auto vertex_buffer = rex::graphics::VertexBuffer::create(vertices, sizeof(vertices));
            {
                rex::graphics::BufferLayout layout =
                {
                    { rex::graphics::ShaderDataType::FLOAT3, "a_Position" },
                    { rex::graphics::ShaderDataType::FLOAT4, "a_Color" }
                };

                vertex_buffer->setLayout(layout);
            }

            setVertexBuffer(vertex_buffer);

            unsigned int indices[3] = { 0, 1, 2 };

            setIndexBuffer(rex::graphics::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
        }
    };
    class Quad : public rex::graphics::Mesh
    {
    public:
        Quad(rex::graphics::Shader* shader)
            :Mesh(shader)
        {
            setVertexArray(rex::graphics::VertexArray::create());

            float vertices[3 * 4] = {
                -0.5f, -0.5f, 0.0f,
                 0.5f, -0.5f, 0.0f,
                 0.5f,  0.5f, 0.0f,
                -0.5f,  0.5f, 0.0f
            };

            auto vertex_buffer = rex::graphics::VertexBuffer::create(vertices, sizeof(vertices));
            {
                rex::graphics::BufferLayout layout =
                {
                    { rex::graphics::ShaderDataType::FLOAT3, "a_Position" },
                };

                vertex_buffer->setLayout(layout);
            }

            setVertexBuffer(vertex_buffer);

            uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };

            setIndexBuffer(rex::graphics::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
        }
    };

    const std::string triangle_vertex_shader = R"(
            #version 330 core
            
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;

            uniform mat4 u_ViewProjection;
            uniform mat4 u_Model;

            out vec3 v_Position;
            out vec4 v_Color;

            void main()
            {
                v_Position = a_Position;
                v_Color = a_Color;

                gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);    
            }
        )";
    const std::string quad_vertex_shader = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Model;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;

				gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);	
			}
        )";

    const std::string triangle_fragment_shader = R"(
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
    const std::string quad_fragment_shader = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
        )";
}

class ExampleLayer : public rex::engine::Layer 
{
public:
    //-------------------------------------------------------------------------
    ExampleLayer()
        :Layer("Example")
        , m_triangle_shader(nullptr)
        , m_quad_shader(nullptr)
        , m_triangle(nullptr)
        , m_quad(nullptr)
        , m_camera(-1.6f, 1.6f, -0.9f, 0.9f)
        , m_camera_position(0.0f, 0.0f, 0.0f)
        , m_camera_rotation(0.0f)
        , m_camera_speed(5.0f)
        , m_camera_rotation_speed(5.0f)
    {
        m_camera.setPosition(m_camera_position);
        m_camera.setRotation(m_camera_rotation);
    }

    //-------------------------------------------------------------------------
    void onAttach() override
    {
        m_triangle_shader = std::make_unique<rex::engine::Shader>(triangle_vertex_shader, triangle_fragment_shader);
        m_quad_shader = std::make_unique<rex::engine::Shader>(quad_vertex_shader, quad_fragment_shader);

        m_triangle = std::make_unique<Triangle>(m_triangle_shader.get());
        m_quad = std::make_unique<Quad>(m_quad_shader.get());
    }

    //-------------------------------------------------------------------------
    void onUpdate(const rex::DeltaTime& dTime) override
    {
        if(rex::engine::Input::instance()->isKeyDown(rex::KeyCode::LEFT))
        {
            m_camera_position.x -= m_camera_speed * dTime.toSeconds();
        }
        else if (rex::engine::Input::instance()->isKeyDown(rex::KeyCode::RIGHT))
        {
            m_camera_position.x += m_camera_speed * dTime.toSeconds();
        }
        
        if (rex::engine::Input::instance()->isKeyDown(rex::KeyCode::DOWN))
        {
            m_camera_position.y -= m_camera_speed * dTime.toSeconds();
        }
        else if (rex::engine::Input::instance()->isKeyDown(rex::KeyCode::UP))
        {
            m_camera_position.y += m_camera_speed * dTime.toSeconds();
        }

        if (rex::engine::Input::instance()->isKeyDown(rex::KeyCode::KEY_Q))
        {
            m_camera_rotation -= m_camera_rotation_speed * dTime.toSeconds();
        }
        else if (rex::engine::Input::instance()->isKeyDown(rex::KeyCode::KEY_W))
        {
            m_camera_rotation += m_camera_rotation_speed * dTime.toSeconds();
        }

        m_camera.setPosition(m_camera_position);
        m_camera.setRotation(m_camera_rotation);

        rex::graphics::Renderer::beginScene(m_camera);
        rex::graphics::Renderer::submit(m_quad.get());
        rex::graphics::Renderer::submit(m_triangle.get());
        rex::graphics::Renderer::endScene();
    }
    //-------------------------------------------------------------------------
    void onEvent(rex::events::Event& event)
    {
        rex::events::EventDispatcher dispatcher(event);
        dispatcher.dispatch<rex::engine::KeyDown>([&](rex::engine::KeyDown& keyEvent) { return onKeyPressed(keyEvent); });
    }

    //-------------------------------------------------------------------------
    bool onKeyPressed(rex::engine::KeyDown& keyEvent)
    {
        switch (*keyEvent.getKey())
        {
        case RX_KEY_ESCAPE: rex::CoreApplication::getInstance()->quit(); break;
        }

        return false;
    }

private:
    rex::graphics::OrthographicCamera m_camera;

    std::unique_ptr<rex::graphics::Shader> m_triangle_shader;
    std::unique_ptr<rex::graphics::Shader> m_quad_shader;

    std::unique_ptr<rex::graphics::Mesh> m_triangle;
    std::unique_ptr<rex::graphics::Mesh> m_quad;

    glm::vec3 m_camera_position;
    float m_camera_rotation;

    float m_camera_speed;
    float m_camera_rotation_speed;
};

//-------------------------------------------------------------------------
sandbox::Application::Application() = default;
//-------------------------------------------------------------------------
sandbox::Application::~Application() = default;

//-------------------------------------------------------------------------
void sandbox::Application::onInitialize()
{
    pushBackLayer(std::make_unique<ExampleLayer>());
}

//-------------------------------------------------------------------------
rex::CoreApplication* rex::createApplication()
{
    return new sandbox::Application();
}