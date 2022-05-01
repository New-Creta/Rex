#include "rex_std_pch.h"

#include "memory/ref_counted_object.h"

namespace rex
{
    //-------------------------------------------------------------------------
    RefCountedObject::RefCountedObject()
        : m_references(0)
    {
    }
    //-------------------------------------------------------------------------
    RefCountedObject::~RefCountedObject()
    {
    }

    //-------------------------------------------------------------------------
    std::uint64_t RefCountedObject::increment_reference() const
    {
        ++m_references;
        return m_references;
    }
    //-------------------------------------------------------------------------
    std::uint64_t RefCountedObject::decrement_reference() const
    {
        --m_references;
        return m_references;
    }

    //-------------------------------------------------------------------------
    std::uint64_t RefCountedObject::get_reference_count() const
    {
        return m_references;
    }
}