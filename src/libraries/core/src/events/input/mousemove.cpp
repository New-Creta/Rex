#include "events/input/MouseMove.h"

//-------------------------------------------------------------------------
rex::events::MouseMove::MouseMove(rex::Window* window, const MouseKeyMetaData& keyMetaData, const MousePosition& mousePos)
    : MouseEvent(window, keyMetaData, mousePos)
{
    RX_TRACE("Mouse move event created");
}

rex::events::MouseMove::~MouseMove() = default;