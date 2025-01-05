#pragma once

#include "rex_engine/engine/globals.h"

// The following code is implemented in rex_engine/engine/globals.cpp
// We didn't bother creating a different translation unit for the mutable and non-mutable version

namespace rex
{
	Globals& mut_globals();
}