#include "pokemon/state_task.h"

namespace pokemon
{
	StateTask::StateTask(task_begin_func&& begin, task_tick_func&& tick, task_end_func&& end)
		: m_on_begin(rsl::move(begin))
		, m_on_tick(rsl::move(tick))
		, m_on_end(rsl::move(end))
	{

	}

	void StateTask::begin()
	{
		m_on_begin();
	}
	GameLoopState StateTask::tick()
	{
		return m_on_tick();
	}
	void StateTask::end()
	{
		m_on_end();
	}
}