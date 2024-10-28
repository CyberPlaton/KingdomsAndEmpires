#pragma once
#include "ecs_module.hpp"
#include "ecs_world_context_holder.hpp"

namespace ecs
{
	//- Class responsible for loading and unloading modules for a world. Contains all current active modules.
	//- Module manager imports modules deferred, meaning it first constructs a dependency graph for modules and
	//- systems, and afterwards actually imports everything into flecs
	//-------------------------------------------------------------------------------------------------------------------------
	class cmodule_manager final : public iworld_context_holder
	{
	public:
		cmodule_manager(cworld* w);
		~cmodule_manager() = default;

		void do_import_modules();

		//- Importing is a deferred operation. Here we only store the module type.
		template<typename TModule>
		cmodule_manager& import_module();
		cmodule_manager& import_module(rttr::type type);
		cmodule_manager& import_module(stringview_t type_name);

	private:
		umap_t<string_t, vector_t<modules::sconfig>> m_module_adjacency_map;
		set_t<string_t> m_module_set;

	private:
		bool has_cyclic_dependency();
		void dump_adjacency_map(const unsigned depth = 0) const;
		void dump_module(const string_t& module, const unsigned depth) const;
		bool is_cyclic(const string_t& name, umap_t<string_t, bool>& visited,
			umap_t<string_t, bool>& recursion_stack);
	};

	//-------------------------------------------------------------------------------------------------------------------------
	template<typename TModule>
	cmodule_manager& ecs::cmodule_manager::import_module()
	{
		return import_module(rttr::type::get<TModule>());
	}

} //- ecs