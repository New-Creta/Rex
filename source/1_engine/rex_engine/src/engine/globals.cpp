#include "rex_engine/engine/globals.h"
#include "rex_engine/engine/mutable_globals.h"

#include "rex_engine/diagnostics/assert.h"

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: REX GLOBALS

namespace rex
{
	Globals& mut_globals()
	{
		static Globals globals{};
		return globals;
	}

	const Globals& globals()
	{
		return mut_globals();
	}


}
