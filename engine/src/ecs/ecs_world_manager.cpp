#include "ecs_world_manager.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	bool cworld_manager::create(stringview_t name)
	{
		auto h = algorithm::hash(name);

		if (m_worlds.find(h) == m_worlds.end())
		{
			m_worlds.emplace(h, name);
			m_current = h;
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld_manager::set_active(stringview_t name)
	{
		auto h = algorithm::hash(name);

		if (m_worlds.find(h) != m_worlds.end())
		{
			m_current = h;
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_manager::tick(float dt, system_running_phase p)
	{
		if (m_current != 0)
		{
			active().tick(dt, p);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	const cworld& cworld_manager::active() const
	{
		ASSERT(m_current != 0, "Active world was not set for world manager!");

		return m_worlds.at(m_current);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cworld& cworld_manager::active()
	{
		ASSERT(m_current != 0, "Active world was not set for world manager!");

		return m_worlds.at(m_current);
	}

} //- ecs