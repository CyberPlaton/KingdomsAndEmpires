#include "ecs_module_manager.hpp"
#include "ecs_world.hpp"

namespace ecs
{
	namespace
	{
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

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cmodule_manager::cmodule_manager(cworld* w) :
		iworld_context_holder(w)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmodule_manager::do_import_modules()
	{
		dump_adjacency_map();

		for (const auto& [module, config] : m_module_adjacency_map)
		{
			auto type = rttr::type::get_by_name(module.data());

			auto var = type.create({ &world() });
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmodule_manager& cmodule_manager::import_module(rttr::type type)
	{
		if (const auto m = type.get_method(modules::smodule::C_MODULE_CONFIG_FUNC_NAME); m.is_valid())
		{
			auto config = m.invoke({}).convert<modules::sconfig>();

			m_module_set.insert(config.m_name);
			m_module_adjacency_map[config.m_name];

			//- Load module dependencies
			for (const auto& dep : config.m_modules)
			{
				m_module_adjacency_map[config.m_name].push_back(get_module_config(dep));

				import_module(rttr::type::get_by_name(dep.data()));
			}

			//- Sanity check
			const auto cyclic = has_cyclic_dependency();

			if (cyclic)
			{
				log_error(fmt::format(""));

				CORE_ASSERT(false, "Invalid operation. Module introduces a cyclic dependency!");
			}
		}
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmodule_manager& cmodule_manager::import_module(stringview_t type_name)
	{
		return import_module(rttr::type::get_by_name(type_name.data()));
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmodule_manager::has_cyclic_dependency()
	{
		umap_t<string_t, bool> visited;
		umap_t<string_t, bool> recursion_stack;

		for (const auto& module: m_module_set)
		{
			visited[module] = false;
			recursion_stack[module] = false;
		}

		for (const auto& module : m_module_set)
		{
			if (!visited[module] && is_cyclic(module, visited, recursion_stack))
			{
				return true;
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmodule_manager::dump_adjacency_map(const unsigned depth /*= 0*/) const
	{
		log_trace("Dumping module adjacency map:");

		for (const auto& [module, deps] : m_module_adjacency_map)
		{
			dump_module(module, depth);

			for (const auto& dep : deps)
			{
				dump_module(dep.m_name, depth + 1);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmodule_manager::dump_module(const string_t& module, const unsigned depth) const
	{
		constexpr stringview_t C_FILL = "----";

		string_t padding(C_FILL.data(), depth);

		log_trace(fmt::format("{}{}", padding, module.data()));
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmodule_manager::is_cyclic(const string_t& name, umap_t<string_t, bool>& visited,
		umap_t<string_t, bool>& recursion_stack)
	{
		visited[name] = true;
		recursion_stack[name] = true;

		for (const auto& config : m_module_adjacency_map.at(name))
		{
			if ((!visited.at(config.m_name) && is_cyclic(config.m_name, visited, recursion_stack)) ||
				recursion_stack.at(config.m_name))
			{
				return true;
			}
		}

		recursion_stack[name] = false;
		return false;
	}

} //- ecs