#include "ecs_singleton_manager.hpp"
#include "ecs_world.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	csingleton_manager::csingleton_manager(cworld* w) :
		iworld_context_holder(w)
	{
		log_debug("Register ecs singletons:");

		for (const auto& type : rttr::type::get<isingleton>().get_derived_classes())
		{
			log_debug(fmt::format("\t'{}'", type.get_name().data()));

			m_registered_singletons.emplace_back(type.get_name().data());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	csingleton_manager& csingleton_manager::add_singleton(rttr::type type)
	{
		if(type.is_valid())
		{
			auto var = type.create({ &world() });
		}
		else
		{
			log_error("Could not add singleton component to world because the RTTR type is not reflected or invalid!");
		}
		return *this;
	};

	//------------------------------------------------------------------------------------------------------------------------
	csingleton_manager& csingleton_manager::add_singleton(stringview_t type_name)
	{
		return add_singleton(rttr::type::get_by_name(type_name.data()));
	};

	//------------------------------------------------------------------------------------------------------------------------
	const vector_t<std::string>& csingleton_manager::singletons() const
	{
		return m_registered_singletons;
	}

} //- ecs