#include "rex_windows_pch.h"

#include "display_mode.h"

namespace rex
{
    //-------------------------------------------------------------------------
    DisplayMode::DisplayMode()
        : m_description()
    {
    }

    //-------------------------------------------------------------------------
    DisplayMode::DisplayMode(const DisplayMode& other)
        : m_description(other.m_description)
    {
    }

    //-------------------------------------------------------------------------
    DisplayMode::DisplayMode(DisplayMode&& other) noexcept 
        : m_description(std::exchange(other.m_description, {}))
    {
    }

    //-------------------------------------------------------------------------
    DisplayMode::DisplayMode(const DisplayModeDescription& description)
        : m_description(description)
    {
    }

    //-------------------------------------------------------------------------
    DisplayMode::~DisplayMode()
    {
    }

    //-------------------------------------------------------------------------
    DisplayMode& DisplayMode::operator=(const DisplayMode& other)
    {
        m_description = other.m_description;

        return *this;
    }

    //-------------------------------------------------------------------------
    DisplayMode& DisplayMode::operator=(DisplayMode&& other) noexcept
    {
        m_description = std::exchange(other.m_description, {});

        return *this;
    }

    //-------------------------------------------------------------------------
    int32 DisplayMode::get_mode_index() const
    {
        return m_description.mode_index;
    }

    //-------------------------------------------------------------------------
    int32 DisplayMode::get_width() const
    {
        return m_description.width;
    }

    //-------------------------------------------------------------------------
    int32 DisplayMode::get_height() const
    {
        return m_description.height;
    }

    //-------------------------------------------------------------------------
    int32 DisplayMode::get_refresh_rate() const
    {
        return m_description.refresh_rate;
    }

    //-------------------------------------------------------------------------
    uint32 DisplayMode::get_pixel_format() const
    {
        return m_description.pixel_format;
    }

    //-------------------------------------------------------------------------
    uint32 DisplayMode::get_bits_per_pixel() const
    {
        return m_description.bits_per_pixel;
    }
}