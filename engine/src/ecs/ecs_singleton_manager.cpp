#include "ecs_singleton_manager.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	csingleton_manager::csingleton_manager(flecs::world& w) :
		iworld_context_holder(w)
	{
		//- gather all singleton component types registered to RTTR
		for (auto type : rttr::type::get_types())
		{
			for (auto base : type.get_base_classes())
			{
				if (base == rttr::type::get<isingleton>())
				{
					m_registered_singletons.emplace_back(type.get_name().data());
					break;
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	const vector_t<std::string>& csingleton_manager::singletons() const
	{
		return m_registered_singletons;
	}

} //- ecs