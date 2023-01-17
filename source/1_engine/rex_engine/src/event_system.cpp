//--------------------------------------------------
// File:    event_system.cpp
// Created: 4/4/2022 10:58:59 PM
//
// Created By: nickdb
//
// Copyright (c) REX
//--------------------------------------------------

#include "event_system.h"

#include "rex_std/vector.h"

#include <unordered_map>

namespace rex
{
    std::unordered_map<event_system::EventType, rsl::vector<event_system::EventFunction>> g_delegates;

    void event_system::subscribe(EventType type, EventFunction function)
    {
        g_delegates[type].push_back(function);
    }

    void event_system::fire_event(EventType type)
    {
        const rsl::vector<EventFunction>& delegates = g_delegates[type];

        for (const EventFunction& delegate : delegates)
        {
            delegate();
        }
    }
}