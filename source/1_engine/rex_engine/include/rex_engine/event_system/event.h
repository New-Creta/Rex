#pragma once

#include "rex_engine/engine/types.h"

#include "rex_std/bonus/utility.h"

namespace rex
{
	// the base for event type.
	// stores the type id of the derived event type
	// this is used so the event system knows which dispatcher to call
  class EventBase
  {
	public:
		EventBase(rsl::type_id_t typeId)
			: m_type_id(typeId)
		{}

		rsl::type_id_t type_id() const
		{
			return m_type_id;
		}

	private:
		rsl::type_id_t m_type_id;
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
			: EventBase(rsl::type_id<T>())
		{}
	};
} // namespace rex