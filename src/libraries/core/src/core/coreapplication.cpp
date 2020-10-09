#include "rex_core_pch.h"

#include "core/coreapplication.h"
#include "core/deltatime.h"

#include <chrono>

//-------------------------------------------------------------------------
rex::CoreApplication* rex::CoreApplication::s_application_instance = nullptr;

//-------------------------------------------------------------------------
rex::CoreApplication::CoreApplication()
    :m_running(true)
{
    RX_INFO("Creating application");

    RX_ASSERT_X(s_application_instance == nullptr, "There can only be one application at the time");
    s_application_instance = this;
}
//-------------------------------------------------------------------------
rex::CoreApplication::~CoreApplication()
{
    m_running = false;

    s_application_instance = nullptr;

    RX_INFO("Destroying application");
}

//-------------------------------------------------------------------------
int rex::CoreApplication::run()
{
    RX_TRACE("Initialize CoreApplication");

    platformInitialize();

    while (m_running)
    {
        std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
        DeltaTime delta_time = fmiliseconds(current - m_last_time_point);
        m_last_time_point = current;

        platformUpdate(delta_time);
    }

    RX_TRACE("Quit CoreApplication");

    platformQuit();

    return 0;
}

//-------------------------------------------------------------------------
void rex::CoreApplication::quit()
{
    markForDestroy();
}

//-------------------------------------------------------------------------
void rex::CoreApplication::markForDestroy()
{
    m_running = false;
}
