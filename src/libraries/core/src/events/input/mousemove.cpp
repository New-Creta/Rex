#include "events/input/MouseMove.h"

//-------------------------------------------------------------------------
rex::events::MouseMove::MouseMove(rex::Window* window, const MouseKeyMetaData& keyMetaData, const MousePosition& mousePos)
    : MouseEvent(window, keyMetaData, mousePos)
{
    RX_TRACE("Mouse move event created");
}

rex::events::MouseMove::~MouseMove() = default;

//-------------------------------------------------------------------------
std::string rex::events::MouseMove::toString() const
{
    std::stringstream s;
    s << "Mouse Moved: [" << getScreenPosition().x << "," << getScreenPosition().y << "]";
    return s.str();
}
