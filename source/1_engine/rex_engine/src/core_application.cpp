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
    struct IApplication::Internal
    {
        static IApplication* s_instance;
    };

    //-------------------------------------------------------------------------
    IApplication* IApplication::Internal::s_instance = nullptr;

    //-------------------------------------------------------------------------
    IApplication::IApplication()
        :m_internal_ptr(rsl::make_unique<Internal>())
    {
        assert(IApplication::Internal::s_instance == nullptr && "There can only be one application at the time");
        IApplication::Internal::s_instance = this;
    }
    //-------------------------------------------------------------------------
    IApplication::~IApplication()
    {
        IApplication::Internal::s_instance = nullptr;
    }
}