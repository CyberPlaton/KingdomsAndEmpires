#include "ecs_world_manager.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	bool cworld_manager::create(stringview_t name)
	{
		auto h = algorithm::hash(name);

		if (m_worlds.find(h) == m_worlds.end())
		{
			m_worlds[h] = std::move(std::make_shared<flecs::world>());
			m_current = h;
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld_manager::checkout(stringview_t name)
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
	const flecs::world& cworld_manager::current() const
	{
		ASSERT(m_current != 0, "Active world was not set for world manager!");
		return *m_worlds.at(m_current).get();
	}

	//------------------------------------------------------------------------------------------------------------------------
	flecs::world& cworld_manager::current()
	{
		ASSERT(m_current != 0, "Active world was not set for world manager!");
		return *m_worlds.at(m_current).get();
	}

} //- ecs