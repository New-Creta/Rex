#include "rex_graphics_pch.h"

#include "core/glcontext.h"

//-------------------------------------------------------------------------
rex::gl::Context::Context(const rex::gl::DriverData& driverData, const rex::gl::GLContextData& glData, const rex::gl::GLContextConfiguration& glConfig)
    : m_driver_data(driverData)
    , m_gl_data(glData)
    , m_gl_config(glConfig)
{}
//-------------------------------------------------------------------------
rex::gl::Context::~Context() = default;

//-------------------------------------------------------------------------
void rex::gl::Context::initialize()
{
    void* context = nullptr;

    create(&context);
    setup(&context);
}

//-------------------------------------------------------------------------
std::string rex::gl::Context::version() const
{
    int major;
    int minor;

    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    std::stringstream stream;

    stream << "OpenGL, v" << major << "." << minor;

    return stream.str();
}

//-------------------------------------------------------------------------
const rex::gl::DriverData& rex::gl::Context::driverData() const
{
    return m_driver_data;
}

//-------------------------------------------------------------------------
const rex::gl::GLContextData& rex::gl::Context::glContextData() const
{
    return m_gl_data;
}
//-------------------------------------------------------------------------
const rex::gl::GLContextConfiguration& rex::gl::Context::glContextConfiguration() const
{
    return m_gl_config;
}

//-------------------------------------------------------------------------
void rex::gl::Context::create(void** context)
{
    *context = glContextConfiguration().force_legacy_opengl == false
        ? createModernContext()
        : nullptr;

    if (*context == nullptr)
    {
        // OPENGL version < 3.x
        *context = createLegacyContext();
        if (*context == nullptr)
        {
            RX_ERROR("Failed to create legacy OpenGL context");
            return;
        }
    }
}
//-------------------------------------------------------------------------
void rex::gl::Context::setup(void** context)
{
    makeCurrent(*context);

    bool result = initalizeGLEW();
    if (!result)
    {
        deleteContext(*context);

        RX_ERROR("Failed to initialize GLEW");
    }
}

//-------------------------------------------------------------------------
bool rex::gl::Context::initalizeGLEW()
{
    return glewInit() == GLEW_OK;
}