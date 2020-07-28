#include "rex_core_pch.h"

#include "core/coreapplication.h"
#include "core/window.h"

#include "events/event.h"

//-------------------------------------------------------------------------
rex::CoreApplication::CoreApplication() = default;
//-------------------------------------------------------------------------
rex::CoreApplication::~CoreApplication() = default;

//-------------------------------------------------------------------------
int rex::CoreApplication::run()
{
    m_window = createWindow();
    m_window->show();

    while (true)
    {
        m_window->update();
    }

    return 0;
}

//-------------------------------------------------------------------------
void rex::CoreApplication::onEvent(const rex::events::Event& event)
{
    UNUSED_PARAM(event);
}