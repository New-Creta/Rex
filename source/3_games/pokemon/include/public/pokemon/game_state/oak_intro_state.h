#pragma once

#include "pokemon/state_task.h"

namespace pokemon
{
	class OakIntroState : public StateTask
	{
	public:
		OakIntroState();

		void begin();
		GameLoopState tick(s32 frameIdx);
		void end();

	private:

	};
}