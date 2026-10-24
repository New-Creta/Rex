#pragma once

#include "pokemon/state_task.h"

namespace pokemon
{
	class GamefreakLogoState : public StateTask
	{
	public:
		GamefreakLogoState();

		void begin();
		GameLoopState tick(s32 frameIdx);
		void end();

	private:
		void load_small_start_oam();

	};
}