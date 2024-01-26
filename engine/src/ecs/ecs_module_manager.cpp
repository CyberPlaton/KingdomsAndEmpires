#include "ecs_module_manager.hpp"
#include <plugin_logging.h>

namespace ecs
{
	namespace
	{
		inline static vector_t<stringview_t> s_registered_modules;
	}

	//-------------------------------------------------------------------------------------------------------------------------
	cmodule_manager::cmodule_manager(flecs::world& w) :
		m_world(w)
	{
	}

	//-------------------------------------------------------------------------------------------------------------------------
	cmodule_manager::~cmodule_manager()
	{

	}

	//-------------------------------------------------------------------------------------------------------------------------
	void cmodule_manager::register_module(rttr::type type)
	{
		s_registered_modules.push_back(type.get_name().data());
	}

	//-------------------------------------------------------------------------------------------------------------------------
	vector_t<stringview_t> cmodule_manager::registered_modules()
	{
		return s_registered_modules;
	}

	//-------------------------------------------------------------------------------------------------------------------------
	void cmodule_manager::init_modules()
	{
		for (const auto& m : s_registered_modules)
		{
			if (auto module_type = rttr::type::get_by_name(m.data()); module_type.is_valid())
			{
				if (auto ctor = module_type.get_constructor({ rttr::type::get<flecs::world&>() }); ctor.is_valid())
				{
					//- this call will register the module in ecs world
					auto module_var = ctor.invoke(m_world);

					auto self_module_method = module_type.get_method("self_module");

					auto module_entity = self_module_method.invoke(module_var);

					m_active_modules[m] = module_entity.get_value<flecs::entity>();
				}
				else
				{
					//- could not construct the module
					logging::log_error(fmt::format("Could not construct module '{}'", m.data()));
				}
			}
			else
			{
				//- could not find the module in RTTR
				logging::log_error(fmt::format("Could not find module '{}' in RTTR", m.data()));
			}
		}
	}

} //- ecs