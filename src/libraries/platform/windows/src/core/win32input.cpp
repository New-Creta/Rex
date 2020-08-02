#include "core/win32input.h"
#include "core/win32application.h"
#include "core/win32window.h"

//-------------------------------------------------------------------------
bool rex::win32::InputImpl::isKeyPressed(KeyCode code) const
{
    UNUSED_PARAM(code);

    //auto application = static_cast<Application*>(CoreApplication::getInstance());
    //auto window = static_cast<Window*>(application->getWindow());

    return false;
}
//-------------------------------------------------------------------------
bool rex::win32::InputImpl::isKeyReleased(KeyCode code) const
{
    UNUSED_PARAM(code);

    //auto application = static_cast<Application*>(CoreApplication::getInstance());
    //auto window = static_cast<Window*>(application->getWindow());

    return false;
}

//-------------------------------------------------------------------------
bool rex::win32::InputImpl::isMouseButtonPressed(MouseCode button) const
{
    UNUSED_PARAM(button);

    //auto application = static_cast<Application*>(CoreApplication::getInstance());
    //auto window = static_cast<Window*>(application->getWindow());

    return false;
}
//-------------------------------------------------------------------------
bool rex::win32::InputImpl::isMouseButtonReleased(MouseCode button) const
{
    UNUSED_PARAM(button);

    //auto application = static_cast<Application*>(CoreApplication::getInstance());
    //auto window = static_cast<Window*>(application->getWindow());

    return false;
}

//-------------------------------------------------------------------------
rex::Point rex::win32::InputImpl::getMousePosition() const
{
    //auto application = static_cast<Application*>(CoreApplication::getInstance());
    //auto window = static_cast<Window*>(application->getWindow());

    return { 0, 0 };
}

//-------------------------------------------------------------------------
unsigned int rex::win32::InputImpl::getMouseX() const
{
    //auto application = static_cast<Application*>(CoreApplication::getInstance());
    //auto window = static_cast<Window*>(application->getWindow());

    return 0;
}
//-------------------------------------------------------------------------
unsigned int rex::win32::InputImpl::getMouseY() const
{
    //auto application = static_cast<Application*>(CoreApplication::getInstance());
    //auto window = static_cast<Window*>(application->getWindow());

    return 0;
}
