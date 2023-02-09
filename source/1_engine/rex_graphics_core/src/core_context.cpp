#include "core_context.h"

namespace rex
{
    //-------------------------------------------------------------------------
    IGraphicsContext* IGraphicsContext::s_instance = nullptr;

    //-------------------------------------------------------------------------
    rex::IGraphicsContext* IGraphicsContext::get_instance()
    {
        return s_instance;
    }

    //-------------------------------------------------------------------------
    IGraphicsContext::IGraphicsContext()
    {
        REX_ASSERT_X(IGraphicsContext::s_instance == nullptr, "There can only be one graphics context at the time");
        IGraphicsContext::s_instance = this;
    }
    //-------------------------------------------------------------------------
    IGraphicsContext::~IGraphicsContext()
    {
        IGraphicsContext::s_instance = nullptr;
    }
}

