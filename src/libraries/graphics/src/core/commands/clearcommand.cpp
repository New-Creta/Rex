#include "rex_graphics_pch.h"

#include "core/commands/clearcommand.h"

//-------------------------------------------------------------------------
rex::graphics::ClearCommand::ClearCommand()
    :m_clear_color(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f))
    ,m_clear_depth(0.0f)
{}
//-------------------------------------------------------------------------
rex::graphics::ClearCommand::~ClearCommand() = default;

//-------------------------------------------------------------------------
void rex::graphics::ClearCommand::setClearColor(const glm::vec4& color)
{
    m_clear_color = color;
}

//-------------------------------------------------------------------------
const glm::vec4& rex::graphics::ClearCommand::getClearColor() const
{
    return m_clear_color;
}

//-------------------------------------------------------------------------
void rex::graphics::ClearCommand::setClearDepth(float depth)
{
    m_clear_depth = depth;
}

//-------------------------------------------------------------------------
float rex::graphics::ClearCommand::getClearDepth() const
{
    return m_clear_depth;
}

//-------------------------------------------------------------------------
size_t rex::graphics::ClearCommand::getCommandTypeID() const
{
    return typeid(rex::graphics::ClearCommand).hash_code();
}
