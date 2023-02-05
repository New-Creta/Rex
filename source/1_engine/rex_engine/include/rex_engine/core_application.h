#pragma once

#include "rex_std/memory.h"

#include "rex_engine/types.h"

namespace rex
{
    struct FrameInfo;

    class IApplication
    {
    public:
        IApplication();
        virtual ~IApplication();

        virtual bool is_running() const = 0;

        virtual s32 run() = 0;
        virtual void quit() = 0;

    private:
      struct Internal;
      rsl::unique_ptr<Internal> m_internal_ptr;
    };
}