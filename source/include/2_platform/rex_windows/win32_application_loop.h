#pragma once

#include "core_application_loop.h"

namespace rex
{
    CREATE_STRONG_TYPE(RefreshRate, int32);

    struct FrameInfo;

    namespace win32
    {
        using UpdateDelegate = std::function<void(const FrameInfo&)>;

        class ApplicationLoop : public CoreApplicationLoop
        {
        public:
            ApplicationLoop(const UpdateDelegate& updateDelegate, const RefreshRate& refreshRate);
            ~ApplicationLoop() override;

            void exec() override;
            void stop() override;

        protected:
            void tick() override;

        private:
            bool m_is_running;
            UpdateDelegate m_update_delegate;
            RefreshRate m_refresh_rate;
        };
    }
}

//-------------------------------------------------------------------------
constexpr rex::RefreshRate operator"" _hz(unsigned long long value)
{
    return rex::RefreshRate{ rtl::numeric_cast<int32>(value) };
}