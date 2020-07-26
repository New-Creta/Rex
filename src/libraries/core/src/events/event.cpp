#include "rex_core_pch.h"
#include "events/event.h"

//-------------------------------------------------------------------------
rex::events::Event::Event()
    :m_handled(false)
{}
//-------------------------------------------------------------------------
rex::events::Event::~Event() = default;

//-------------------------------------------------------------------------
void rex::events::Event::handle()
{
    m_handled = true;
}

//-------------------------------------------------------------------------
std::string rex::events::Event::toString() const
{
    return typeid(this).name();
}

//-------------------------------------------------------------------------
bool rex::events::Event::isInCategory(EventCategory category) const
{
    return getCategory() & category;
}
//-------------------------------------------------------------------------
bool rex::events::Event::isHandled() const
{
    return m_handled;
}