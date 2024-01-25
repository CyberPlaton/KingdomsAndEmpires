#include "ecs_module_manager.hpp"

namespace ecs
{
	uset_t<stringview_t> cmodule_manager::s_registered_modules;

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
				}
			}
			else
			{
				//- could not find the module in RTTR
			}
		}
	}

} //- ecs