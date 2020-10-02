#include "rex_engine_pch.h"
#include "core/consoleapplication.h"

//-------------------------------------------------------------------------
rex::engine::ConsoleApplication::ConsoleApplication() = default;
//-------------------------------------------------------------------------
rex::engine::ConsoleApplication::~ConsoleApplication() = default;

//-------------------------------------------------------------------------
rex::Window* rex::engine::ConsoleApplication::getWindow()
{
    RX_WARN("A console application does not have a window.");

    return nullptr;
}
//-------------------------------------------------------------------------
const rex::Window* rex::engine::ConsoleApplication::getWindow() const
{
    RX_WARN("A console application does not have a window.");

    return nullptr;
}

//-------------------------------------------------------------------------
rex::Context* rex::engine::ConsoleApplication::getContext()
{
    RX_WARN("A console application does not have a context.");

    return nullptr;
}
//-------------------------------------------------------------------------
const rex::Context* rex::engine::ConsoleApplication::getContext() const
{
    RX_WARN("A console application does not have a context.");

    return nullptr;
}


//-------------------------------------------------------------------------
void rex::engine::ConsoleApplication::appInitialize()
{
    // Nothing to implement
}
//-------------------------------------------------------------------------
void rex::engine::ConsoleApplication::appUpdate(float dTime)
{
    UNUSED_PARAM(dTime);

    // Nothing to implement
}
//-------------------------------------------------------------------------
void rex::engine::ConsoleApplication::appQuit()
{
    // Nothing to implement
}
