#pragma once

#include "rex_core_global.h"

#include "eventcategory.h"
#include "eventtype.h"

namespace rex
{
    namespace events
    {
        #define EVENT_CLASS_TYPE(type)          static EventType getStaticType() { return type; }                   \
								                EventType getType() const override { return getStaticType(); }      \

        #define EVENT_CLASS_CATEGORY(category)  EventCategory getCategory() const override { return category; }

        class Event
        {
        public:
            REX_CORE_EXPORT virtual ~Event();

            REX_CORE_EXPORT void                    handle();

            REX_CORE_EXPORT virtual EventType       getType() const = 0;
            REX_CORE_EXPORT virtual EventCategory   getCategory() const = 0;
            REX_CORE_EXPORT virtual std::string     toString() const;
            
            REX_CORE_EXPORT bool                    isInCategory(EventCategory category) const;
            REX_CORE_EXPORT bool                    isHandled() const;

        protected:
            REX_CORE_EXPORT Event();

        private:
            bool m_handled;
        };
    }
}