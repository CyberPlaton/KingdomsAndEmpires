#include "ecs_system_manager.hpp"

namespace ecs
{

	//------------------------------------------------------------------------------------------------------------------------
	csystem_manager::csystem_manager(flecs::world& w) :
		iworld_context_holder(w)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	csystem_manager::~csystem_manager()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	csystem_manager::ssystem_info csystem_manager::system_info(stringview_t name) const
	{
		auto h = algorithm::hash(name);

		if(m_system_info.find(h) != m_system_info.end())
		{
			return m_system_info.at(h);
		}

		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	void csystem_manager::store_system_info(ref_t<csystem> system)
	{
		auto h = algorithm::hash(system->name());
		
		ssystem_info info{};
		info.m_system = system->name();
		
		for(const auto& s: system->subsystems())
		{
			info.m_subsystems.emplace_back(s.name());
		}
		for(const auto& pair: m_system_info)
		{
			for(const auto& subsystem: pair.second.m_subsystems)
			{
				if(subsystem.m_system == info.m_system)
				{
					info.m_dependencies.emplace_back(subsystem.m_system);
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	flecs::system csystem_manager::system(stringview_t name) const
	{
		//- merely upcast the retrieved entity to system
		return flecs::system(world().lookup(name));
	}

} //- ecs