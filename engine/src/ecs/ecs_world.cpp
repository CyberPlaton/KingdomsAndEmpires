#include "ecs_world.hpp"

namespace ecs
{
	namespace
	{
		constexpr auto C_ENTITIES_PROP = "entities";
		constexpr auto C_SYSTEMS_PROP = "systems";
		constexpr auto C_MODULES_PROP = "modules";
		constexpr auto C_COMPONENTS_PROP = "components";
		constexpr auto C_AVRG_ENTITY_COMPONENT_COUNT = 8;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cworld::cworld(stringview_t name) :
		m_name(name),
		m_entity_manager(m_world),
		m_system_manager(m_world),
		m_module_manager(m_world),
		m_component_manager(m_world),
		m_query_manager(m_world)
	{
		m_world.observer<stransform, sidentifier>()
			.event(flecs::OnAdd)
			.event(flecs::OnRemove)
			.event(flecs::OnSet)
			.each([&](flecs::iter& it, size_t index, stransform& transform, sidentifier& id)
				{
					flecs::entity e = it.entity(index);
					std::string c = it.event_id().str().c_str();

					if (it.event() == flecs::OnAdd)
					{
						//- add aabb proxy for entity to b2DynamicTree
						create_proxy(e);
					}
					else if (it.event() == flecs::OnRemove)
					{
						//- remove aabb proxy of entity from b2DynamicTree
						destroy_proxy(e);
					}
					else if (it.event() == flecs::OnSet)
					{
						//- update internal b2DynamicTree aabb proxy with new position of entity
						update_proxy(e);
					}
				});
	}

	//------------------------------------------------------------------------------------------------------------------------
	cworld::~cworld()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::tick(float dt)
	{
		m_world.progress(dt);

		//- process any queries, they will be available for systems on next tick,
		//- also clearup memory for already taken and processed queries.
		process_queries();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::process_queries()
	{
		for (auto* query = qm().fetch(); query != nullptr; query = qm().fetch())
		{
			//- consider using 'unlikely' here, or just consider invalid queries as working
			if (query->type() == query_type_none ||
				query->intersection_type() == query_intersection_type_none)
			{
				continue;
			}

			//- prepare query and let it run
			m_master_query_type = query->type();
			m_master_query_key = (m_master_query_key + 1) % C_MASTER_QUERY_KEY_MAX;

			if (query->intersection_type() == query_intersection_type_aabb)
			{
				Query(this, query->aabb());
			}
			else if (query->intersection_type() == query_intersection_type_raycast)
			{
				RayCast(this, query->raycast());
			}

			//- store back the results
			switch (query->type())
			{
			case query_type_any_occurance:
			{
				query->m_any = m_master_query_result.m_any;
				break;
			}
			case query_type_entity_array:
			{
				query->m_entities = m_master_query_result.m_entity_array;
				break;
			}
			case query_type_entity_count:
			{
				query->m_count = m_master_query_result.m_entity_count;
				break;
			}
			default:
			case query_type_none:
			{
				break;
			}
			}

			query->finish();
		}

		qm().tick();

		m_master_query_type = query_type_none;
		m_master_query_result = sworld_query{};
	}

	//------------------------------------------------------------------------------------------------------------------------
	flecs::snapshot cworld::snapshot() const
	{
		return m_world.snapshot();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld::QueryCallback(int proxy_id)
	{
		bool result = false;

		switch (m_master_query_type)
		{
		case query_type_entity_count: {result = true; break;}
		case query_type_entity_array: {result = true; break;}
		case query_type_any_occurance: {result = false; break;}
		default:
		case query_type_none:
			return false;
		}

		//- assign query key to proxy and avaoid duplicate queries
		auto* id = reinterpret_cast<sidentifier*>(GetUserData(proxy_id));

		if (id->m_aabb_proxy_query_key == m_master_query_key)
		{
			return result;
		}

		id->m_aabb_proxy_query_key = m_master_query_key;

		//- TODO: apply filter

		switch (m_master_query_type)
		{
		case query_type_entity_count: {++m_master_query_result.m_entity_count; break; }
		case query_type_entity_array: {m_master_query_result.m_entity_array.push_back(id->m_self); break; }
		case query_type_any_occurance: {m_master_query_result.m_any = true; break; }
		default:
		case query_type_none:
			return false;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float cworld::RayCastCallback(const b2RayCastInput& ray_input, int proxy_id)
	{
		//- 0.0f signals to stop and 1.0f signals to continue
		float result = 0.0f;

		switch (m_master_query_type)
		{
		case query_type_entity_count: {result = 1.0f; break; }
		case query_type_entity_array: {result = 1.0f; break; }
		case query_type_any_occurance: {result = 0.0f; break; }
		default:
		case query_type_none:
			return false;
		}

		//- assign query key to proxy and avaoid duplicate queries
		auto* id = reinterpret_cast<sidentifier*>(GetUserData(proxy_id));

		if (id->m_aabb_proxy_query_key == m_master_query_key)
		{
			return result;
		}

		id->m_aabb_proxy_query_key = m_master_query_key;

		//- TODO: apply filter

		switch (m_master_query_type)
		{
		case query_type_entity_count: {++m_master_query_result.m_entity_count; break; }
		case query_type_entity_array: {m_master_query_result.m_entity_array.push_back(id->m_self); break; }
		case query_type_any_occurance: {m_master_query_result.m_any = true; break; }
		default:
		case query_type_none:
			return false;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld::load(const core::cpath& path)
	{
		simdjson::dom::parser parser;
		simdjson::dom::element element;

		auto string = core::cfile::load_text(path.view());

		if(parser.parse(string.data(), string.length()).get(element) == simdjson::SUCCESS)
		{
			simdjson::dom::array entities_array;
			simdjson::dom::array systems_array;
			simdjson::dom::array modules_array;

			if (element.at_key(C_ENTITIES_PROP).get(entities_array) == simdjson::SUCCESS)
			{
				for (auto it : entities_array)
				{
					if (it.type() == simdjson::dom::element_type::OBJECT)
					{
						deserialize_entity(it.get<simdjson::dom::object>());
					}
					else
					{
						logging::log_error("Could not deserialize entity, as it is not a JSON object");
					}
				}
			}
			if (element.at_key(C_SYSTEMS_PROP).get(systems_array) == simdjson::SUCCESS)
			{

			}
			if (element.at_key(C_MODULES_PROP).get(modules_array) == simdjson::SUCCESS)
			{

			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::save(const core::cpath& path)
	{
		nlohmann::json json;

		json = nlohmann::json::object();

		json[C_ENTITIES_PROP] = nlohmann::json::array();

		auto i = 0;
		for (const auto e : em().entities())
		{
			serialize_entity(e, json[C_ENTITIES_PROP][i++]);
		}

		core::cfile::save_text(path.view(), json.dump(4));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::serialize_entity(const flecs::entity e, nlohmann::json& json)
	{
		json = nlohmann::json::object();

		json["name"] = e.name();

		json[C_COMPONENTS_PROP] = nlohmann::json::array();

		vector_t<std::string> names; names.reserve(C_AVRG_ENTITY_COMPONENT_COUNT);

		auto i = 0;
		e.each([&](flecs::id c)
			{
				//- retrieve usable name of component without namespaces etc
				std::string name = c.str();
				core::string_utils::split(name, '.', names);

				if (auto component_type = rttr::type::get_by_name(names[names.size() - 1]); component_type.is_valid())
				{
					if (auto serialize_method = component_type.get_method(ecs::C_COMPONENT_SERIALIZE_FUNC_NAME.data()); serialize_method.is_valid())
					{
						logging::log_error(fmt::format("\tserializing component '{}'", name));

						serialize_method.invoke({}, e, json[C_COMPONENTS_PROP][i++]);
					}
					else
					{
						//- RTTR error, component does not have a serialize function
						logging::log_error(fmt::format("\tcould not find 'serialize' method for component '{}'", name));
					}
				}
				else
				{
					//- RTTR error, component not found with given name
					logging::log_error(fmt::format("\tcould not find component '{} ({})' with RTTR", names[names.size() - 1], name));
				}

				names.clear();
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::update_proxy(flecs::entity e)
	{
		const auto* id = e.get<sidentifier>();
		const auto* tr = e.get<stransform>();

		const auto& prev_aabb = GetFatAABB(id->m_aabb_proxy);

		auto dx = (prev_aabb.GetCenter().x - prev_aabb.GetExtents().x) - tr->m_x;
		auto dy = (prev_aabb.GetCenter().y - prev_aabb.GetExtents().y) - tr->m_y;

		MoveProxy(id->m_aabb_proxy, physics::aabb(tr->m_x, tr->m_y, tr->m_w, tr->m_h), { dx, dy });
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::destroy_proxy(flecs::entity e)
	{
		const auto* id = e.get<sidentifier>();

		DestroyProxy(id->m_aabb_proxy);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::create_proxy(flecs::entity e)
	{
		auto* id = e.get_mut<sidentifier>();
		const auto* tr = e.get<stransform>();

		id->m_aabb_proxy = CreateProxy(physics::aabb(tr->m_x, tr->m_y, tr->m_w, tr->m_h), &id);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld::has_proxy(flecs::entity e)
	{
		const auto* id = e.get<sidentifier>();

		return GetUserData(id->m_aabb_proxy) != nullptr;
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
	}

} //- ecs