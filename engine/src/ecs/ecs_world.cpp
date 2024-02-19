#include "ecs_world.hpp"

namespace ecs
{
	namespace
	{
		constexpr auto C_ENTITIES_PROP = "entities";
		constexpr auto C_SYSTEMS_PROP = "systems";
		constexpr auto C_MODULES_PROP = "modules";
		constexpr auto C_COMPONENTS_PROP = "components";

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cworld::cworld(stringview_t name) :
		m_name(name),
		m_entity_manager(m_world),
		m_system_manager(m_world),
		m_module_manager(m_world)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cworld::~cworld()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld::QueryCallback(int proxy_id)
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float cworld::RayCastCallback(const b2RayCastInput& ray_input, int proxy_id)
	{
		return 0.0f;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld::load(const core::cpath& path)
	{
		simdjson::dom::parser parser;
		simdjson::dom::element element;

		auto string = core::cfile::load_text(path.view());

// 		const auto& json = nlohmann::json::parse(string);
// 
// 		for (const auto& e : json.at("entities"))
// 		{
// 			deserialize_entity(e);
// 		}

		parser.parse(string.data(), string.length()).get(element);

		simdjson::dom::array entities_array;
		simdjson::dom::array systems_array;
		simdjson::dom::array modules_array;

		if (element.at_key(C_ENTITIES_PROP).get(entities_array) == simdjson::SUCCESS)
		{
			for (auto it : entities_array)
			{
				simdjson::dom::object entity_object;

				if (it.type() != simdjson::dom::element_type::OBJECT)
				{
					//- error
				}

				entity_object = it.get<simdjson::dom::object>();

				deserialize_entity(entity_object);
			}
		}
		if (element.at_key(C_SYSTEMS_PROP).get(systems_array) == simdjson::SUCCESS)
		{

		}
		if (element.at_key(C_MODULES_PROP).get(modules_array) == simdjson::SUCCESS)
		{

		}

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::deserialize_entity(const nlohmann::json& json)
	{
		for (const auto& c : json.at("components"))
		{
			logging::log_debug(fmt::format("\t c: '{}'", c.dump(4)));

			auto t = c.at(core::io::C_OBJECT_TYPE_NAME);

			if (auto type = rttr::type::get_by_name(t.template get<std::string_view>().data()); type.is_valid())
			{
				auto var = type.create();

				const auto& comp = c.at(t);

				logging::log_debug(fmt::format("\t comp: '{}'", comp.dump(4)));

				var = core::io::from_json_string(type, comp.dump());

				if (var.is_valid())
				{
					auto s = core::io::to_json_string(var, true);

					logging::log_debug(fmt::format("\t component: '{}'", s));
				}
				else
				{
					logging::log_debug(fmt::format("\t failed to load component with type '{}'", type.get_name().data()));
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::save(const core::cpath& path)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::deserialize_entity(const simdjson::dom::object& json)
	{
		simdjson::dom::array components_array;
		simdjson::dom::element component_element;
		std::string_view type_name;

		if (json.at_key(C_COMPONENTS_PROP).get(components_array) == simdjson::SUCCESS)
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

							if (var.is_valid())
							{
								//-- success. Component deserialized.
							}
							else
							{
								//-- error. Could not deserialize component from JSON
							}
						}
						else
						{
							//-- JSON error. Cant find component object by given type key
						}
					}
					else
					{
						//-- RTTR error. Cant find type by name
					}
				}
				else
				{
					//-- JSON error. Cant find type name
				}
			}
		}
	}

} //- ecs