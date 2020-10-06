#include "rex_graphics_pch.h"

#include "api/opengl/core/commands/glclearcommand.h"

//-------------------------------------------------------------------------
rex::opengl::ClearCommand::ClearCommand()
    :rex::opengl::ClearCommand(glm::vec4(0.1f,0.1f,0.1f,1.0f), 0.0f)
{}
//-------------------------------------------------------------------------
rex::opengl::ClearCommand::ClearCommand(const glm::vec4& clearColor, float clearDepth)
{
    setClearColor(clearColor);
    setClearDepth(clearDepth);
}

//-------------------------------------------------------------------------
rex::opengl::ClearCommand::~ClearCommand() = default;

//-------------------------------------------------------------------------
void rex::opengl::ClearCommand::execute()
{
    glm::vec4 clear_color = getClearColor();
    float     clear_depth = getClearDepth();

    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    glClearDepth(clear_depth);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

