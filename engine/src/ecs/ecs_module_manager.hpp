#pragma once
#include "ecs_module.hpp"

namespace ecs
{
	//- Class responsible for loading and unloading modules for a world. Contains all current active modules.
	//-------------------------------------------------------------------------------------------------------------------------
	class cmodule_manager final : public iworld_context_holder
	{
	public:
		cmodule_manager(flecs::world& w);
		~cmodule_manager() = default;

		template<class TModuleType>
		cmodule_manager& import_module();

		cmodule_manager& import_module(rttr::type type);
		cmodule_manager& import_module(stringview_t type_name);

	private:
	};

	//-------------------------------------------------------------------------------------------------------------------------
	template<class TModuleType>
	cmodule_manager& ecs::cmodule_manager::import_module()
	{
		TModuleType m(world());

		return *this;
	}

} //- ecs