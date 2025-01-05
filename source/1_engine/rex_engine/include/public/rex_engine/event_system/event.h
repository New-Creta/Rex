#pragma once

#include "rex_engine/engine/types.h"

#include "rex_std/bonus/utility.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
	// the base for event type.
	// stores the type id of the derived event type
	// this is used so the event system knows which dispatcher to call
  class EventBase
  {
	public:
		EventBase(rsl::type_id_t typeId, rsl::memory_size sizeOfEvent)
			: m_type_id(typeId)
			, m_size_of_event(sizeOfEvent)
		{}

		rsl::type_id_t type_id() const
		{
			return m_type_id;
		}

		rsl::memory_size event_size() const
		{
			return m_size_of_event;
		}

	private:
		rsl::memory_size m_size_of_event; // total size of this event object
		rsl::type_id_t m_type_id; // type id of the most derived event object's type
  };

	// templated event type
	// users should inherit from this event type
	// it gets the type id of the template and passed it on the its base class
	// which can then later be queried by the event system
	template <typename T>
	class Event : public EventBase
	{
	public:
		Event()
			: EventBase(rsl::type_id<T>(), sizeof(T))
		{}
	};
} // namespace rex