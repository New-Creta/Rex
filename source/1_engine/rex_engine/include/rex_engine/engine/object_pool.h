#pragma once

#include "rex_std/memory.h"
#include "rex_std/utility.h"

namespace rex
{
	class Object;
	namespace internal
	{
		Object* track_object(rsl::unique_ptr<Object> object);
	}

	class Object
	{
	public:
		virtual ~Object() = default;
	};

	template <typename Object, typename ... Args>
	rsl::shared_ptr<Object> load_object(Args&& ... args)
	{
		rsl::shared_ptr<Object> object = rsl::make_shared<Object>(rsl::forward<Args>(args)...);;
		//internal::track_object(rsl::weak_ptr<Object>(object));
		return object;
	}
}