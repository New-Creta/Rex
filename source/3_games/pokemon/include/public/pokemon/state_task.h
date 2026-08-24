#pragma once

#include "rex_std/functional.h"
#include "pokemon/game_loops/game_loop_state.h"

namespace pokemon
{
	class StateTask
	{
		using task_begin_func = rsl::function<void()>;
		using task_tick_func = rsl::function<GameLoopState()>;
		using task_end_func = rsl::function<void()>;

	public:
		StateTask(task_begin_func&& begin, task_tick_func&& tick, task_end_func&& end);

		void begin();
		GameLoopState tick();
		void end();

	private:
		task_begin_func m_on_begin;
		task_tick_func m_on_tick;
		task_end_func m_on_end;
	};
}