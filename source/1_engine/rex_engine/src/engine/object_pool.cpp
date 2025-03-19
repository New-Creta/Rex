#include "rex_engine/engine/object_pool.h"
#include "rex_std/vector.h"

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: OBJECTS

namespace rex
{
	namespace internal
	{
		rsl::vector<rsl::unique_ptr<Object>> g_objects;

		Object* track_object(rsl::unique_ptr<Object> object)
		{
			return g_objects.emplace_back(rsl::move(object)).get();
		}
	}

}