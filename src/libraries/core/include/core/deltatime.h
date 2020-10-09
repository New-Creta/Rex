#pragma once

namespace rex 
{
    using fseconds      = std::chrono::duration<float>;
    using fmiliseconds  = std::chrono::duration<float, std::milli>;

    class DeltaTime
    {
    public:
        DeltaTime(fmiliseconds time)
            : m_time(time)
        {}
                                                      
        inline float toSeconds() const          { return fseconds { m_time }.count(); }
        inline float toMilliseconds() const     { return m_time.count(); }

    private:
        fmiliseconds m_time;
    };

}