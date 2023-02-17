#pragma once

#ifdef REX_DEBUG

#include "rex_engine/defines.h"
#include "rex_engine/types.h"

#include "rex_std/string_view.h"

namespace rex
{
    namespace opengl
    {

        rsl::string_view get_error_message(u32 error);
        u32 get_current_error();
        bool log_call(const char* function, const char* file, int32 line);
    }
}

#define GL_CALL(x) rex::opengl::get_current_error();\
		x; \
		if (!rex::opengl::log_call(#x, __FILE__, __LINE__)) DEBUG_BREAK();

#else

#define GL_CALL(x) x

#endif