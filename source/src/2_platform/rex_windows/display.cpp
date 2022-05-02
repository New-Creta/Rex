#include "rex_windows_pch.h"

#include "display.h"

namespace rex
{
    //-------------------------------------------------------------------------
    Display::Display()
        : m_description()
        , m_active_display_mode(nullptr)
    {
    }

    //-------------------------------------------------------------------------
    Display::Display(const Display& other)
        : m_description(other.m_description)
        , m_active_display_mode(other.m_active_display_mode)
    {
    }

    //-------------------------------------------------------------------------
    Display::Display(Display&& other) noexcept 
        : m_description(std::exchange(other.m_description, {}))
        , m_active_display_mode(std::exchange(other.m_active_display_mode, nullptr))
    {
    }
    //-------------------------------------------------------------------------
    Display::Display(const DisplayDescription& description)
        : m_description(description)
    {
    }

    //-------------------------------------------------------------------------
    Display::~Display()
    {
    }

    //-------------------------------------------------------------------------
    Display& Display::operator=(const Display& other)
    {
        m_description = other.m_description;
        m_active_display_mode = other.m_active_display_mode;

        return *this;
    }

    //-------------------------------------------------------------------------
    Display& Display::operator=(Display&& other) noexcept
    {
        m_description = std::exchange(other.m_description, {});
        m_active_display_mode = std::exchange(other.m_active_display_mode, nullptr);

        return *this;
    }

    //-------------------------------------------------------------------------
    const char* Display::get_name() const
    {
        return m_description.name;
    }

    //-------------------------------------------------------------------------
    rex::not_null<const DisplayMode*> Display::set_active_mode(int32 index)
    {
        auto it = std::find_if(std::cbegin(m_description.display_modes), std::cend(m_description.display_modes), [index](const auto& pair)
                               {
                                   return pair.second.get_mode_index() == index;
                               });

        if (it != std::cend(m_description.display_modes))
        {
            m_active_display_mode = &(*it).second;
        }

        return m_active_display_mode;
    }

    //-------------------------------------------------------------------------
    rex::not_null<const DisplayMode*> Display::get_active_mode() const
    {
        return m_active_display_mode;
    }

    //-------------------------------------------------------------------------
    int32 Display::get_display_index() const
    {
        return m_description.display_index;
    }

    //-------------------------------------------------------------------------
    int32 Display::get_display_mode_count() const
    {
        return rtl::numeric_cast<int32>(m_description.display_modes.size());
    }

    //-------------------------------------------------------------------------
    std::optional<DisplayMode> Display::get_display_mode(int32 index) const
    {
        auto it = m_description.display_modes.find(index);

        if (it != std::cend(m_description.display_modes))
        {
            return it->second;
        }

        return {};
    }
}