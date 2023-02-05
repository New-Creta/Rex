#include "rex_engine/core_application.h"

#include "rex_engine/world.h"

#include "rex_engine/frameinfo/frameinfo.h"

#include "rex_std/chrono.h"
#include "rex_std/functional.h"
#include "rex_std/memory.h"
#include "rex_std/math.h"

#include <thread>

#include <assert.h>

namespace rex
{
    struct ICoreApplication::Internal
    {
        static ICoreApplication* s_instance;
    };

    //-------------------------------------------------------------------------
    ICoreApplication* ICoreApplication::Internal::s_instance = nullptr;

    //-------------------------------------------------------------------------
    ICoreApplication::ICoreApplication()
        :m_internal_ptr(rsl::make_unique<Internal>())
    {
        assert(ICoreApplication::Internal::s_instance == nullptr && "There can only be one application at the time");
        ICoreApplication::Internal::s_instance = this;
    }
    //-------------------------------------------------------------------------
    ICoreApplication::~ICoreApplication()
    {
        ICoreApplication::Internal::s_instance = nullptr;
    }
}