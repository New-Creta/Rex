#include "rex_engine_pch.h"

#include "core_application.h"

namespace rex
{
    //-------------------------------------------------------------------------
    CoreApplication* CoreApplication::s_instance = nullptr;

    //-------------------------------------------------------------------------
    rex::CoreApplication* CoreApplication::get_instance()
    {
        return s_instance;
    }

    //-------------------------------------------------------------------------
    CoreApplication::CoreApplication(const ApplicationDescription& description)
        : m_description(description)
    {
        //R_INFO("[APPLICATION] Creating application: {0}", description.name);

        //R_ASSERT_X(CoreApplication::s_instance == nullptr, "There can only be one application at the time");
        CoreApplication::s_instance = this;
    }

    //-------------------------------------------------------------------------
    CoreApplication::~CoreApplication()
    {
        //R_INFO("[APPLICATION] Destroying application");
    }

    //-------------------------------------------------------------------------
    int32 CoreApplication::run()
    {
        if (!initialize())
            return EXIT_FAILURE;
        if (!shutdown())
            return EXIT_FAILURE;

        // auto live_references = memory::get_all_references();
        // if (!live_references.empty())
        // {
        //     R_ERROR("----------------------------------------------------");
        //     R_ERROR("Live references found!");
        //     for (const auto& live_reference : live_references)
        //     {
        //         R_ERROR("Reference [ptr, refcount]: [{0}, {1}]", fmt::ptr(live_reference), live_reference->get_reference_count());
        //     }
        //     R_ERROR("----------------------------------------------------");

        //     DEBUG_BREAK();

        //     return EXIT_FAILURE;
        // }

        return EXIT_SUCCESS;
    }

    //-------------------------------------------------------------------------
    const rex::ApplicationDescription& CoreApplication::get_application_description() const
    {
        return m_description;
    }

    //-------------------------------------------------------------------------
    bool CoreApplication::initialize()
    {
        return platform_initialize();
    }
    //-------------------------------------------------------------------------
    bool CoreApplication::shutdown()
    {
        return platform_shutdown();
    }
}