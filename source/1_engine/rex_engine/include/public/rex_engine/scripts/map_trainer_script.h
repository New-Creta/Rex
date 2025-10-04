#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/scripts/map_script.h"

namespace rex
{
	struct TrainerScript
	{
		s8 event_flag;
		s8 view_range;
		s8 text_before_battle;
		s8 text_after_battle;
		s8 text_end_battle;
	};
}