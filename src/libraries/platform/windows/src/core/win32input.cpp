#include "core/win32input.h"
#include "core/win32inputcontroller.h"
#include "core/win32application.h"
#include "core/win32window.h"

//-------------------------------------------------------------------------
rex::win32::InputImpl::InputImpl()
{
    m_controller = std::make_unique<InputController>();
}
//-------------------------------------------------------------------------
rex::win32::InputImpl::~InputImpl() = default;

//-------------------------------------------------------------------------
void rex::win32::InputImpl::poll()
{
    m_controller->update();
}

//-------------------------------------------------------------------------
bool rex::win32::InputImpl::isKeyPressed(KeyCode code) const
{
    return m_controller->isKeyPressed(code);
}
//-------------------------------------------------------------------------
bool rex::win32::InputImpl::isKeyDown(KeyCode code) const
{
    return m_controller->isKeyDown(code);
}
//-------------------------------------------------------------------------
bool rex::win32::InputImpl::isKeyReleased(KeyCode code) const
{
    return m_controller->isKeyReleased(code);
}
//-------------------------------------------------------------------------
bool rex::win32::InputImpl::isKeyUp(KeyCode code) const
{
    return m_controller->isKeyUp(code);
}

//-------------------------------------------------------------------------
bool rex::win32::InputImpl::isMouseButtonPressed(MouseCode button) const
{
    return m_controller->IsMouseButtonPressed(button);
}
//-------------------------------------------------------------------------
bool rex::win32::InputImpl::isMouseButtonDown(MouseCode button) const
{
    return m_controller->IsMouseButtonDown(button);
}
//-------------------------------------------------------------------------
bool rex::win32::InputImpl::isMouseButtonReleased(MouseCode button) const
{
    return m_controller->IsMouseButtonReleased(button);
}
//-------------------------------------------------------------------------
bool rex::win32::InputImpl::isMouseButtonUp(MouseCode button) const
{
    return m_controller->IsMouseButtonUp(button);
}

//-------------------------------------------------------------------------
rex::Point64 rex::win32::InputImpl::getMousePosition(bool previousFrame) const
{
    return m_controller->getMousePosition(previousFrame);
}

//-------------------------------------------------------------------------
int64 rex::win32::InputImpl::getMouseX(bool previousFrame) const
{
    return m_controller->getMousePosition(previousFrame).x;
}
//-------------------------------------------------------------------------
int64 rex::win32::InputImpl::getMouseY(bool previousFrame) const
{
    return m_controller->getMousePosition(previousFrame).y;
}
