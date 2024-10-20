#include "ecs_singleton_manager.hpp"
#include "ecs_world.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	csingleton_manager::csingleton_manager(cworld* w) :
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