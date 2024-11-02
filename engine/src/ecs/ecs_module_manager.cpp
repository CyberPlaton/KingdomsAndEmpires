#include "ecs_module_manager.hpp"
#include "ecs_world.hpp"

namespace ecs
{
	namespace
	{
		constexpr stringview_t C_FILL = "----";

		//------------------------------------------------------------------------------------------------------------------------
		modules::sconfig get_module_config(stringview_t type)
		{
			if (const auto t = rttr::type::get_by_name(type.data()); t.is_valid())
			{
				if (const auto m = t.get_method(modules::smodule::C_MODULE_CONFIG_FUNC_NAME); m.is_valid())
				{
					return m.invoke({}).convert<modules::sconfig>();
				}
			}
			return {};
		}

		//------------------------------------------------------------------------------------------------------------------------
		system::sconfig get_system_config(stringview_t type)
		{
			if (const auto t = rttr::type::get_by_name(type.data()); t.is_valid())
			{
				if (const auto m = t.get_method(system::ssystem::C_SYSTEM_CONFIG_FUNC_NAME); m.is_valid())
				{
					return m.invoke({}).convert<system::sconfig>();
				}
			}
			return {};
		}

		//- Sort modules based on their dependencies in order of their creation
		//------------------------------------------------------------------------------------------------------------------------
		vector_t<string_t> topsort(const umap_t<string_t, set_t<string_t>>& module_map)
		{
			vector_t<string_t> sorted_order;
			set_t<string_t> visited;
			set_t<string_t> recursion_stack;

			core::cfunction<bool(stringview_t)> dfs = [&](stringview_t node)
				{
					if (recursion_stack.count(node.data()))
					{
						return true; //- Cycle detected
					}
					if (visited.count(node.data()))
					{
						return false; //- Already visited, skip
					}

					// Mark node as visited and add to recursion stack
					visited.insert(node.data());
					recursion_stack.insert(node.data());

					// Process dependencies (neighbors)
					if (module_map.count(node.data()))
					{
						for (const auto& dependency : module_map.at(node.data()))
						{
							if (dfs(dependency))
							{
								return true; //-Propagate cycle detection
							}
						}
					}

					//- Add to sorted order and remove from recursion stack
					recursion_stack.erase(node.data());
					sorted_order.push_back(node.data());
					return false;
				};

			//- Perform DFS from each node in the graph
			for (const auto& [node, _] : module_map)
			{
				if (!visited.count(node))
				{
					if (dfs(node))
					{
						log_critical(fmt::format("Cyclic dependency in module dependency graph detected!"));
						CORE_ASSERT(false, "Invalid operation");
						return {};
					}
				}
			}
			return sorted_order;
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cmodule_manager::cmodule_manager(cworld* w) :
		iworld_context_holder(w)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmodule_manager::do_import_modules()
	{
		//- Compute order for importing and do the import
		const auto order = topsort(m_module_to_module);

		for (const auto& module: order)
		{
			auto type = rttr::type::get_by_name(module.data());

			auto var = type.create({ &world() });
		}

		if constexpr (!engine::cfg::C_ECS_MODULES_AND_SYSTEMS_GATHER_INFORMATION)
		{
			m_module_to_module.clear();
			m_system_to_module.clear();
			m_loaded_modules.clear();
			m_loaded_systems.clear();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmodule_manager& cmodule_manager::import_module(rttr::type type)
	{
		auto config = get_module_config(type.get_name().data());

		//- Skip loading if module is already loaded
		if (m_loaded_modules.find(config.m_name) != m_loaded_modules.end())
		{
			return *this;
		}

		//- Store the system-to-module dependency
		for (const auto& s : config.m_systems)
		{
			m_system_to_module[s] = config.m_name;
		}

		auto& module = m_module_to_module[config.m_name];

		//- Store module-to-module dependency (explicit)
		for (const auto& m : config.m_modules)
		{
			module.insert(m);
		}

		//- If module has dependecies, then try to load them first
		for (const auto& dep : module)
		{
			import_module(dep);
		}

		//- Find and store system-to-module dependency (implicit)
		for (const auto& s : config.m_systems)
		{
			const auto system_config = get_system_config(s);
			const auto& after_deps = system_config.m_run_after;
			const auto& before_deps = system_config.m_run_before;

			for (const auto& after : after_deps)
			{
				if (const auto it = m_system_to_module.find(after); it != m_system_to_module.end())
				{
					if (it->second != config.m_name)
					{
						m_module_to_module[config.m_name].insert(it->second);
					}
				}
			}
			for (const auto& before : before_deps)
			{
				if (const auto it = m_system_to_module.find(before); it != m_system_to_module.end())
				{
					if (it->second != config.m_name)
					{
						m_module_to_module[config.m_name].insert(it->second);
					}
				}
			}
		}

		m_loaded_modules.insert(config.m_name);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmodule_manager& cmodule_manager::import_module(stringview_t type_name)
	{
		if (const auto type = rttr::type::get_by_name(type_name.data()); type.is_valid())
		{
			return import_module(type);
		}

		log_error(fmt::format("Trying to load an invalid or unreflected module '{}'", type_name.data()));
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmodule_manager::dump_adjacency_map(const unsigned depth /*= 0*/) const
	{
		log_debug("Dumping module adjacency map:");

		log_debug("Dumping all entities:");
		world().ecs().each<tag::ssystem>([](flecs::entity e, tag::ssystem&)
			{
				log_debug(fmt::format("---- e: {}", e.name()));
			});

		log_debug("Dumping ecs systems:");
		for (const auto& s : m_systems)
		{
			log_debug(fmt::format("----{}", s.second.name()));
		}

		log_debug("Dumping modules:");
		for (const auto& [module, deps] : m_module_to_module)
		{
			dump_module(module, depth);

			for (const auto& dep : deps)
			{
				dump_module(dep, depth + 1);
			}
		}
	}

	// -Function responsible for creating a system for a world with given configuration, without matching any components
	//- and function to execute.
	//- A task is similar to a normal system, only that it does not match any components and thus no entities.
	//- If entities are required they can be retrieved through the world or a query.
	//- The function itself is executed as is, with only delta time provided.
	//------------------------------------------------------------------------------------------------------------------------
	void cmodule_manager::create_task(flecs::world& w, const system::sconfig& cfg, system::task_callback_t callback)
	{
		CORE_ASSERT(!(algorithm::bit_check(cfg.m_flags, system::system_flag_multithreaded) &&
			algorithm::bit_check(cfg.m_flags, system::system_flag_immediate)), "Invalid operation! A system cannot be multithreaded and immediate at the same time!");

		auto builder = w.system(cfg.m_name.c_str());

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

		if (const auto duplicate_system = find_system(cfg.m_name); duplicate_system)
		{
			log_warn(fmt::format("Trying to create a system with same name twice '{}'. This is not allowed!", cfg.m_name));
			return;
		}

		//- Create function to be called for running the task
		auto function = [=](flecs::iter& it)
			{
				(callback)(it.delta_time());
			};

		auto task = builder.run(std::move(function));

		task.add<tag::ssystem>();

		//- Set options that are required after system entity creation
		{
			for (const auto& after : cfg.m_run_after)
			{
				if (const auto [result, phase] = is_flecs_built_in_phase(after); result)
				{
					task.add(flecs::Phase).depends_on(phase);
				}
				else
				{
					if (auto e = find_system(after.c_str()); e)
					{
						task.add(flecs::Phase).depends_on(e);
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
					task.add(flecs::Phase).depends_on(phase);
				}
				else
				{
					if (auto e = find_system(before.c_str()); e)
					{
						e.add(flecs::Phase).depends_on(task);
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
		m_systems[cfg.m_name] = task;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmodule_manager::dump_module(const string_t& module, const unsigned depth) const
	{
		if (auto module_type = rttr::type::get_by_name(module.data()); module_type.is_valid())
		{
			string_t padding(C_FILL.data(), (depth + 1) * 4);

			auto module_config = module_type.get_method(ecs::modules::smodule::C_MODULE_CONFIG_FUNC_NAME).invoke({}).convert<ecs::modules::sconfig>();

			log_debug(fmt::format("{}{}", padding, module_config.m_name));
			log_debug(fmt::format("{}Deps: {}, Comps: {}, Systems: {}",
				padding, module_config.m_modules.size(), module_config.m_components.size(), module_config.m_systems.size()));

			log_debug(fmt::format("{}{}Systems: {}", padding, padding, fmt::join(module_config.m_systems, ", ")));
			log_debug(fmt::format("{}{}Comps: {}", padding, padding, fmt::join(module_config.m_components, ", ")));
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	std::pair<bool, uint64_t> cmodule_manager::is_flecs_built_in_phase(stringview_t name)
	{
		static array_t<std::pair<string_t, uint64_t>, 8> C_PHASES =
		{
			std::pair("OnLoad",		(uint64_t)flecs::OnLoad),
			std::pair("PostLoad",	(uint64_t)flecs::PostLoad),
			std::pair("PreUpdate",	(uint64_t)flecs::PreUpdate),
			std::pair("OnUpdate",	(uint64_t)flecs::OnUpdate),
			std::pair("OnValidate", (uint64_t)flecs::OnValidate),
			std::pair("PostUpdate", (uint64_t)flecs::PostUpdate),
			std::pair("PreStore",	(uint64_t)flecs::PreStore),
			std::pair("OnStore",	(uint64_t)flecs::OnStore)
		};

		if (const auto& it = algorithm::find_if(C_PHASES.begin(), C_PHASES.end(), [=](const auto& pair)
			{
				return string_utils::compare(pair.first, name.data());

			}); it != C_PHASES.end())
		{
			return { true, (uint64_t)it->second };
		}

		return { false, MAX(uint64_t) };
	}

	//------------------------------------------------------------------------------------------------------------------------
	flecs::system cmodule_manager::find_system(stringview_t name)
	{
		if (const auto it = m_systems.find(name.data()); it != m_systems.end())
		{
			return it->second;
		}
		return flecs::system();
	}

} //- ecs