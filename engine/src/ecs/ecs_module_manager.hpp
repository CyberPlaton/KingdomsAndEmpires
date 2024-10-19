#pragma once
#include "ecs_module.hpp"
#include "ecs_world_context_holder.hpp"

namespace ecs
{
	//- Class responsible for loading and unloading modules for a world. Contains all current active modules.
	//-------------------------------------------------------------------------------------------------------------------------
	class cmodule_manager final : public iworld_context_holder
	{
	public:
		cmodule_manager(cworld* w);
		~cmodule_manager() = default;

		template<typename TModule>
		cmodule_manager& import_module();
		cmodule_manager& import_module(rttr::type type);
		cmodule_manager& import_module(stringview_t type_name);

	private:
		vector_t<modules::sconfig> m_modules;
	};

	//-------------------------------------------------------------------------------------------------------------------------
	template<typename TModule>
	cmodule_manager& ecs::cmodule_manager::import_module()
	{
		return import_module(rttr::type::get<TModule>());
	}

} //- ecs