#include "rex_graphics_pch.h"

#include "core/commands/drawcommand.h"

//-------------------------------------------------------------------------
rex::graphics::DrawCommand::DrawCommand() = default;
//-------------------------------------------------------------------------
rex::graphics::DrawCommand::~DrawCommand() = default;

//-------------------------------------------------------------------------
size_t rex::graphics::DrawCommand::getCommandTypeID() const
{
    return typeid(rex::graphics::DrawCommand).hash_code();
}

