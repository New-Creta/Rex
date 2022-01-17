#pragma once

#ifdef REX_PLATFORM_WINDOWS
    namespace rex
    {
        using namespace win32;
    }
#else
    #error Unsupported Platform
#endif