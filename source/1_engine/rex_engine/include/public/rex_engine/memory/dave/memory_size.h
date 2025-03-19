#pragma once

#include "util/types.h"

namespace ppp
{
    namespace memory
    {
        constexpr s64 kilo_byte_multiple = 1000;
        constexpr s64 mega_byte_multiple = kilo_byte_multiple * 1000;
        constexpr s64 giga_byte_multiple = mega_byte_multiple * 1000;

        constexpr s64 kibi_byte_multiple = 1024;
        constexpr s64 mini_byte_multiple = kibi_byte_multiple * 1024;
        constexpr s64 gibi_byte_multiple = mini_byte_multiple * 1024;

        class memory_size
        {
        public:
            constexpr memory_size()
                : m_size_in_bytes(0)
            {
            }

            constexpr memory_size(s64 size_in_bytes)
                : m_size_in_bytes(size_in_bytes)
            {
            }

            constexpr operator s64() const
            {
                return size_in_bytes();
            }
            constexpr s64 size_in_bytes() const
            {
                return m_size_in_bytes;
            }

            constexpr memory_size operator+(s64 bytes) const
            {
                return memory_size(m_size_in_bytes + bytes);
            }
            constexpr memory_size& operator+=(s64 bytes)
            {
                m_size_in_bytes += bytes;
                return *this;
            }

            constexpr memory_size operator-(s64 bytes) const
            {
                return memory_size(m_size_in_bytes - bytes);
            }
            constexpr memory_size operator-=(s64 bytes)
            {
                m_size_in_bytes -= bytes;
                return *this;
            }

            constexpr s64 size_in_kb() const
            {
                return size_in_bytes() / kilo_byte_multiple;
            }
            constexpr s64 size_in_mb() const
            {
                return size_in_bytes() / mega_byte_multiple;
            }
            constexpr s64 size_in_gb() const
            {
                return size_in_bytes() / giga_byte_multiple;
            }
            constexpr s64 size_in_kib() const
            {
                return size_in_bytes() / kibi_byte_multiple;
            }
            constexpr s64 size_in_mib() const
            {
                return size_in_bytes() / mini_byte_multiple;
            }
            constexpr s64 size_in_gib() const
            {
                return size_in_bytes() / gibi_byte_multiple;
            }

        private:
            s64 m_size_in_bytes;
        };

    }
}

constexpr ppp::memory::memory_size operator"" _bytes(unsigned long long bytes)
{
    return ppp::memory::memory_size((s64)bytes);
}
constexpr ppp::memory::memory_size operator"" _kb(unsigned long long bytes)
{
    return ppp::memory::memory_size((s64)bytes * ppp::memory::kilo_byte_multiple);
}
constexpr ppp::memory::memory_size operator"" _mb(unsigned long long bytes)
{
    return ppp::memory::memory_size((s64)bytes * ppp::memory::mega_byte_multiple);
}
constexpr ppp::memory::memory_size operator"" _gb(unsigned long long bytes)
{
    return ppp::memory::memory_size((s64)bytes * ppp::memory::giga_byte_multiple);
}

constexpr ppp::memory::memory_size operator"" _kib(unsigned long long bytes)
{
    return ppp::memory::memory_size((s64)bytes * ppp::memory::kibi_byte_multiple);
}
constexpr ppp::memory::memory_size operator"" _mib(unsigned long long bytes)
{
    return ppp::memory::memory_size((s64)bytes * ppp::memory::mini_byte_multiple);
}
constexpr ppp::memory::memory_size operator"" _gib(unsigned long long bytes)
{
    return ppp::memory::memory_size((s64)bytes * ppp::memory::gibi_byte_multiple);
}