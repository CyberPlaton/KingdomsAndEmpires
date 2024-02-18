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

		auto json = core::cfile::load_text(path.view());

		parser.parse(json.data(), json.length()).get(element);

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
	void cworld::save(const core::cpath& path)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::deserialize_entity(const simdjson::dom::object& json)
	{
		simdjson::dom::array components_array;

		if (json.at_key(C_COMPONENTS_PROP).get(components_array) == simdjson::SUCCESS)
		{
			for (auto it : components_array)
			{
				simdjson::dom::object component_object;

				component_object = it.get<simdjson::dom::object>();

				std::string type_name;

				if (component_object.at_key(core::io::C_OBJECT_TYPE_NAME).get(type_name) == simdjson::SUCCESS)
				{
					if (auto type = rttr::type::get_by_name(type_name); type.is_valid())
					{
						if (auto m = type.get_method(C_COMPONENT_DESERIALIZE_FUNC_NAME.data()); m.is_valid())
						{
							//- TODO: at this point we already require an existing flecs entity in order to components to him
							m.invoke({}, e, component_object);
						}
					}
				}
			}
		}
	}

} //- ecs