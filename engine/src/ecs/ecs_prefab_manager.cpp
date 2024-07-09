#include "ecs_prefab_manager.hpp"
#include "ecs_component.hpp"
#include <plugin_logging.h>

namespace ecs
{
	namespace
	{
		constexpr std::string_view C_NAME_PROP = "name";
		constexpr std::string_view C_PARENT_PROP = "parent";
		constexpr std::string_view C_COMPONENTS_PROP = "components";

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cprefab_manager::cprefab_manager(flecs::world& w) :
		iworld_context_holder(w)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprefab_manager::~cprefab_manager()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprefab_manager::on_shutdown()
	{
		m_prefabs.clear();
	}

	//- Create a new prefab entity with given name.
	//------------------------------------------------------------------------------------------------------------------------
	ecs::cprefab& cprefab_manager::create(stringview_t name)
	{
		auto h = algorithm::hash(name);

		if (m_prefabs.find(h) == m_prefabs.end())
		{
			m_prefabs.try_emplace(h, string_t(name), world());
		}
		else
		{
			//- report warning
			logging::log_warn(fmt::format("Trying to create a prefab with a duplicate name '{}'", name));
		}

		return m_prefabs.at(h);
	}

	//- Convert an entity to prefab, creating merely a new cprefab object.
	//------------------------------------------------------------------------------------------------------------------------
	ecs::cprefab& cprefab_manager::create(flecs::entity e)
	{
		stringview_t name = e.name().c_str();
		auto h = algorithm::hash(name);

		if (m_prefabs.find(h) == m_prefabs.end())
		{
			m_prefabs.try_emplace(h, e, world());
		}
		else
		{
			//- report warning
			logging::log_warn(fmt::format("Trying to create a prefab from entity with a duplicate name '{}'", name));
		}

		return m_prefabs.at(h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::cprefab& cprefab_manager::find(stringview_t name)
	{
		auto h = algorithm::hash(name);

		if (m_prefabs.find(h) == m_prefabs.end())
		{
			m_prefabs.try_emplace(h, string_t(name), world());
		}
		else
		{
			//- report warning
			logging::log_warn(fmt::format("Prefab with name '{}' does not exist! Creating new one...", name));
		}

		return m_prefabs.at(h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprefab_manager::destroy(stringview_t name)
	{
		auto h = algorithm::hash(name);

		m_prefabs.erase(h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cprefab_manager::load(const core::cpath& path)
	{
		simdjson::dom::parser parser;
		simdjson::dom::element element;
		simdjson::dom::element component_element;
		stringview_t type_name;

		auto memory = core::fs::load_text_from_file(path.view());

		//- loading component overrides
		if (parser.parse((const char*)memory->data(), (unsigned)memory->size()).get(element) == simdjson::SUCCESS)
		{
			//- TODO: load common prefab data. If prefab has a parent then load it first.

			//- load components
			simdjson::dom::array components_array;

			if (element.at_key(C_COMPONENTS_PROP).get(components_array) == simdjson::SUCCESS)
			{
				for (auto it : components_array)
				{
					if (it.at_key(core::io::C_OBJECT_TYPE_NAME).get(type_name) == simdjson::SUCCESS)
					{
						if (auto type = rttr::type::get_by_name(type_name.data()); type.is_valid())
						{
							if (it.at_key(type_name).get(component_element) == simdjson::SUCCESS)
							{
								auto var = core::io::from_json_object(type, component_element);

								//- TODO: emplace component to prefab

								if (var.is_valid())
								{
									//-- success. Component deserialized
									logging::log_info(fmt::format("\tdeserialized component '{}'", type_name));
								}
								else
								{
									//-- error. Could not deserialize component from JSON
									logging::log_error(fmt::format("\tfailed deserializing component '{}' from JSON", type_name));
								}
							}
							else
							{
								//-- JSON error. Cant find component object by given type key
								logging::log_error(fmt::format("\tcould not find component by type key '__type__' = '{}'", type_name));
							}
						}
						else
						{
							//-- RTTR error. Cant find type by name
							logging::log_error(fmt::format("\tcould not locate component '{}' within RTTR", type_name));
						}
					}
					else
					{
						//-- JSON error. Cant find type name
						logging::log_error("\tcould not locate '__type__' of component");
					}
				}
			}

			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprefab_manager::save(stringview_t name, const core::cpath& path)
	{
		auto& prefab = find(name);

		nlohmann::json json;

		json = nlohmann::json::object();

		json[C_NAME_PROP] = prefab.name();
		json[C_PARENT_PROP] = prefab.parent_name();
		json[C_COMPONENTS_PROP] = nlohmann::json::array();

		auto i = 0;
		for (const auto& c : components(prefab.self()))
		{
			auto type = rttr::type::get_by_name(c);

			if (auto serialize_method = type.get_method(ecs::detail::C_COMPONENT_SERIALIZE_FUNC_NAME.data()); serialize_method.is_valid())
			{
				logging::log_error(fmt::format("\tserializing component '{}'", c));

				serialize_method.invoke({}, prefab.self(), json[C_COMPONENTS_PROP][i++]);
			}
			else
			{
				//- RTTR error, component does not have a serialize function
				logging::log_error(fmt::format("\tcould not find 'serialize' method for component '{}'", c));
			}
		}
	}

} //- ecs