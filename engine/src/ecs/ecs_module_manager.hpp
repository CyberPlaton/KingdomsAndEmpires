#pragma once
#include "ecs_module.hpp"

namespace ecs
{
	//- class responsible for loading and unloading modules for a world. Contains all current active modules.
	//-------------------------------------------------------------------------------------------------------------------------
	class ENGINE_API cmodule_manager final : public iworld_context_holder
	{
	public:
		cmodule_manager(flecs::world& w);
		~cmodule_manager() = default;

		template<class TModuleType>
		cmodule_manager& import_module();

		const vector_t<imodule::smodule_info>& active_modules() const;

	private:
		vector_t<imodule::smodule_info> m_active_modules;
	};

	//-------------------------------------------------------------------------------------------------------------------------
	template<class TModuleType>
	cmodule_manager& ecs::cmodule_manager::import_module()
	{
		static_assert(std::is_base_of<imodule, TModuleType>::value, "TModuleType must be derived from imodule!");

		TModuleType m(world());

		m_active_modules.emplace_back(m.info());

		return *this;
	}

} //- ecs