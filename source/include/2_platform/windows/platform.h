#pragma once

#ifdef REX_PLATFORM_WINDOWS
    namespace rex
    {
        namespace win32
        {
            // We require the namespace "win32" here in order to use the "using namespace"
        }

        using namespace win32;
    }
#else
    #error Unsupported Platform
#endif