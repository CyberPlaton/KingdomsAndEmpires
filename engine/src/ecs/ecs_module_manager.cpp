#include "ecs_module_manager.hpp"
#include "ecs_world.hpp"

namespace ecs
{
	namespace
	{

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cmodule_manager::cmodule_manager(cworld* w) :
		iworld_context_holder(w)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmodule_manager& cmodule_manager::import_module(rttr::type type)
	{
		if (type.is_valid())
		{
			auto var = type.create({ &world() });
		}
		else
		{
			log_error("Could not import module because the RTTR type is not reflected or invalid!");
		}

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmodule_manager& cmodule_manager::import_module(stringview_t type_name)
	{
		return import_module(rttr::type::get_by_name(type_name.data()));
	}

} //- ecs