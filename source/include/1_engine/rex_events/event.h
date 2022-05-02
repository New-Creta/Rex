#pragma once

// #include "blob.h"

#include "event_category.h"

namespace rex
{
    namespace events
    {
    #define EVENT_CLASS_TYPE(type)                                  \
        static size_t get_static_type()                             \
        {                                                           \
            return typeid(type).hash_code();                        \
        }                                                           \
        size_t get_type() const override                            \
        {                                                           \
            return get_static_type();                               \
        }

    #define EVENT_CLASS_CATEGORY(category)                          \
        rex::events::EventCategory get_category() const override    \
        {                                                           \
            return category;                                        \
        }

        class BaseEvent
        {
        public:
            virtual ~BaseEvent();

            void handle();

            virtual bool can_consume() const = 0;
            virtual size_t get_type() const = 0;
            virtual EventCategory get_category() const = 0;

            virtual std::string to_string() const;

            bool is_in_category(EventCategory category) const;
            bool is_handled() const;

            // const memory::Blob& get_native_event() const;

        protected:
            BaseEvent(/*const memory::Blob& nativeEvent*/);

        private:
            bool m_handled;
            // memory::Blob m_native_event;
        };

        //-------------------------------------------------------------------------
        std::ostream& operator<<(std::ostream& os, const std::shared_ptr<BaseEvent>& e);

        //-------------------------------------------------------------------------
        template <typename TEvent>
        bool is_consumed(const std::shared_ptr<TEvent>& evt)
        {
            static_assert(std::is_base_of_v<BaseEvent, TEvent>, "TEvent is not derived from BaseEvent");

            return evt->is_handled() && evt->can_consume();
        }
        //-------------------------------------------------------------------------
        template <typename TEvent, typename... Args>
        std::shared_ptr<BaseEvent> make_new(Args&... args)
        {
            static_assert(std::is_base_of_v<BaseEvent, TEvent>, "TEvent is not derived from BaseEvent");

            return std::make_shared<TEvent>(std::forward<Args>(args)...);
        }

        //-------------------------------------------------------------------------
        using EventCallbackFn = std::function<void(std::shared_ptr<BaseEvent>&)>;
        using Event = std::shared_ptr<BaseEvent>;
    }
}