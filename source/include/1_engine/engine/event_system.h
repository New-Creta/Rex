//--------------------------------------------------
// File:    event_system.h
// Created: 4/4/2022 10:58:40 PM
//
// Created By: nickdb
// 
// Copyright (c) REX
//--------------------------------------------------

#pragma once

#include <functional>

namespace rex
{
    namespace event_system
    {
        enum class EventType
        {
            WindowClose
        };

        using EventFunction = std::function<void()>;

        void subscribe(EventType type, EventFunction function);

        void fire_event(EventType type);
    }
}