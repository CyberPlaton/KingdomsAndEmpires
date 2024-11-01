#pragma once
#include "ecs_module.hpp"
#include "ecs_system.hpp"
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

		void dump_adjacency_map(const unsigned depth = 0) const;

		template<typename... TComps>
		void create_system(flecs::world& w, const system::sconfig& cfg, system::system_callback_t<TComps...> callback);
		void create_task(flecs::world& w, const system::sconfig& cfg, system::task_callback_t callback);

	private:
		umap_t<string_t, set_t<string_t>> m_module_to_module;	//- Module-to-module dependency
		umap_t<string_t, string_t> m_system_to_module;			//- System-to-module dependency
		set_t<string_t> m_loaded_modules;
		set_t<string_t> m_loaded_systems;
		umap_t<string_t, flecs::system> m_systems;

	private:
		void dump_module(const string_t& module, const unsigned depth) const;
		std::pair<bool, uint64_t> is_flecs_built_in_phase(stringview_t name);
		flecs::system find_system(stringview_t name);
	};

	//- Function responsible for creating a system for a world with given configuration, matching components and function to execute.
	//-------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	void ecs::cmodule_manager::create_system(flecs::world& w, const system::sconfig& cfg,
		system::system_callback_t<TComps...> callback)
	{
		CORE_ASSERT(!(algorithm::bit_check(cfg.m_flags, system::system_flag_multithreaded) &&
			algorithm::bit_check(cfg.m_flags, system::system_flag_immediate)), "Invalid operation! A system cannot be multithreaded and immediate at the same time!");

		auto builder = w.system<TComps...>(cfg.m_name.c_str());

		//- Set options that are required before system entity creation
		{
			if (algorithm::bit_check(cfg.m_flags, system::system_flag_multithreaded))
			{
				builder.multi_threaded();
			}
			if (algorithm::bit_check(cfg.m_flags, system::system_flag_immediate))
			{
				builder.immediate();
			}
		}

		auto system = builder.each(callback);

		system.add<tag::ssystem>();

		//- Set options that are required after system entity creation
		{
			for (const auto& after : cfg.m_run_after)
			{
				if (const auto [result, phase] = is_flecs_built_in_phase(after); result)
				{
					system.add(flecs::Phase).depends_on(phase);
				}
				else
				{
					if (auto e = find_system(after.c_str()); e)
					{
						system.add(flecs::Phase).depends_on(e);
					}
					else
					{
						log_error(fmt::format("Dependency (run after) system '{}' for system '{}' could not be found!",
							after, cfg.m_name));
					}
				}
			}

			for (const auto& before : cfg.m_run_before)
			{
				if (const auto [result, phase] = is_flecs_built_in_phase(before); result)
				{
					system.add(flecs::Phase).depends_on(phase);
				}
				else
				{
					if (auto e = find_system(before.c_str()); e)
					{
						e.add(flecs::Phase).depends_on(system);
					}
					else
					{
						log_error(fmt::format("Dependent (run before) system '{}' for system '{}' could not be found!",
							before, cfg.m_name));
					}
				}
			}
		}

		m_loaded_systems.insert(cfg.m_name);
		m_systems[cfg.m_name] = system;
	}

	//-------------------------------------------------------------------------------------------------------------------------
	template<typename TModule>
	cmodule_manager& ecs::cmodule_manager::import_module()
	{
		return import_module(rttr::type::get<TModule>());
	}

} //- ecs