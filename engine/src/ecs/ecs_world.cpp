#include "ecs_world.hpp"
#include "../animation/animation_module.hpp"
#include "../render/render_module.hpp"

namespace ecs
{
	namespace
	{
		constexpr std::string_view C_ENTITIES_PROP		= "entities";
		constexpr std::string_view C_SYSTEMS_PROP		= "systems";
		constexpr std::string_view C_MODULES_PROP		= "modules";
		constexpr std::string_view C_COMPONENTS_PROP	= "components";

	} //- unnamed

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		std::pair<bool, uint64_t> is_flecs_built_in_phase(const string_t& name)
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
					return string_utils::compare(pair.first, name);

				}); it != C_PHASES.end())
			{
				return { true, (uint64_t)it->second };
			}

			return { false, MAX(uint64_t) };
		}

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	cworld::cworld(stringview_t name) :
		m_name(name),
		m_entity_manager(this),
		m_component_manager(this),
		m_prefab_manager(this),
		m_query_manager(this),
		m_singleton_manager(this),
		m_module_manager(this),
		m_used_threads(1)
	{
		use_threads(m_used_threads);

		//- setup observers
		//- observe transform changes for AABB tree
		ecs().observer<stransform, sidentifier>()
			.event(flecs::OnAdd)
			.event(flecs::OnRemove)
			.each([&](flecs::iter& it, size_t i, stransform& transform, sidentifier& id)
				{
					auto e = it.entity(i);

					if (it.event() == flecs::OnAdd)
					{
						//- add aabb proxy for entity to b2DynamicTree
						create_proxy(e);

						//- name of the component will be in form 'ecs.transform', we have to format the name first
						//cm().on_component_added(e, c);
					}
					else if (it.event() == flecs::OnRemove)
					{
						//- remove aabb proxy of entity from b2DynamicTree
						destroy_proxy(e);
					}
				});

		//- observe added and removed components of entities
		ecs().observer<>()
			.event(flecs::OnAdd)
			.event(flecs::OnRemove)
			.with(flecs::Wildcard)
			.each([&](flecs::iter& it, size_t i)
				{
					auto entity = it.entity(i);
					auto string = it.event_id().str();

					//- Filter for actual ecs components
					if (const auto offset = string_utils::find_substr(string.c_str(), "ecs."); offset != std::string::npos)
					{
						auto c = string_utils::substr(string.c_str(), offset + strlen("ecs."));

						if (it.event() == flecs::OnAdd)
						{
							//- name of the component will be in form 'ecs.transform', we have to format the name first
							cm().on_component_added(entity, c);
						}
						else if (it.event() == flecs::OnRemove)
						{
							cm().on_component_removed(entity, c);
						}
					}
				});

		m_transform_change_tracker = ecs().query_builder<const stransform>()
			.cached()
			.build();

		//- Import default modules
		mm().import_module<render::srender_module>();
		mm().import_module<animation::sanimation_module>();
		mm().do_import_modules();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cworld::~cworld()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::tick(float dt)
	{
		CORE_NAMED_ZONE("cworld::tick");

		prepare();

		ecs().progress(dt);

		//- process any queries, they will be available for systems on next tick,
		//- also clearup memory for already taken and processed queries.
		process_queries();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::prepare()
	{
		CORE_NAMED_ZONE("cworld::prepare");

		//- observe changes and update internal AABB tree
		if (m_transform_change_tracker.changed())
		{
			m_transform_change_tracker.run([this](flecs::iter& it)
				{
					for (auto i : it)
					{
						auto e = it.entity(i);

						if (has_proxy(e)) { update_proxy(e); }
					}
				});
		}

		//- get current viewing rect for active camera
		if (auto e = query_one<const scamera>([](const scamera& c)
			{
				return c.m_active == true;

			}); e.is_valid())
		{
			const auto& c = *e.get<scamera>();

			math::caabb aabb(world_visible_area(c.m_position, c.m_offset, c.m_zoom));

			//- perform a query for all currently visible entities
			m_master_query_type = query_type_entity_array;
			m_master_query_key = (m_master_query_key + 1) % C_MASTER_QUERY_KEY_MAX;

			Query(this, aabb);

			m_visible_entities = std::move(m_master_query_result.m_entity_array);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::process_queries()
	{
		CORE_NAMED_ZONE("cworld::process_queries");

		for (auto* query = qm().fetch(); query != nullptr; query = qm().fetch())
		{
			process_query(*query);
		}

		qm().tick();

		//- reset
		m_master_query_type = query_type_none;
		m_master_query_result.m_entity_array.clear();
		m_master_query_result.m_entity_count = 0;
		m_master_query_result.m_any = false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::process_query(cquery& q)
	{
		CORE_NAMED_ZONE("cworld::process_query");

		if (CORE_UNLIKELY(q.type() == query_type_none || q.intersection_type() == query_intersection_type_none))
		{
			return;
		}

		//- prepare query and let it run
		m_master_query_type = q.type();
		m_master_query_key = (m_master_query_key + 1) % C_MASTER_QUERY_KEY_MAX;

		if (q.intersection_type() == query_intersection_type_aabb)
		{
			Query(this, q.aabb());
		}
		else if (q.intersection_type() == query_intersection_type_raycast)
		{
			RayCast(this, q.raycast());
		}

		//- store back the results
		switch (q.type())
		{
		case query_type_any_occurance:
		{
			q.m_any = m_master_query_result.m_any;
			break;
		}
		case query_type_entity_array:
		{
			q.m_entities = m_master_query_result.m_entity_array;
			break;
		}
		case query_type_entity_count:
		{
			q.m_count = m_master_query_result.m_entity_count;
			break;
		}
		default:
		case query_type_none:
		{
			break;
		}
		}
		q.finish();
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::srect cworld::world_visible_area(const vec2_t& target, const vec2_t& offset, float zoom)
	{
		return {0.0f, 0.0f, 1.0f, 1.0f};

// 		raylib::Camera2D camera;
// 		camera.target = { target.x, target.y };
// 		camera.offset = { offset.x, offset.y };
// 		camera.zoom = zoom;
// 		camera.rotation = 0;
// 
// 		auto tl = raylib::GetScreenToWorld2D({ 0.0f, 0.0f }, camera);
// 		auto br = raylib::GetScreenToWorld2D({ 1280.0f, 1024.0f }, camera);
// 
// 		return { tl.x, tl.y, br.x, br.y };

// 		return { target.x - (1.0f / zoom) * offset.x + 5.0f, target.y - (1.0f / zoom) * offset.y + 5.0f,
// 				(1.0f / zoom) * offset.x * 2.0f - 5.0f, (1.0f / zoom) * offset.y * 2.0f - 5.0f };
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld::QueryCallback(int proxy_id)
	{
		CORE_NAMED_ZONE("cworld::QueryCallback");

		bool result = false;

		switch (m_master_query_type)
		{
		case query_type_entity_count: { result = true; break; }
		case query_type_entity_array: { result = true; break; }
		case query_type_any_occurance: { result = false; break; }
		default:
		case query_type_none:
			return false;
		}

		//- assign query key to proxy and avaoid duplicate queries
		auto& proxy = *reinterpret_cast<sentity_proxy*>(GetUserData(proxy_id));

		if (proxy.m_proxy_query_key == m_master_query_key)
		{
			return result;
		}

		proxy.m_proxy_query_key = m_master_query_key;

		//- TODO: apply filter

		switch (m_master_query_type)
		{
		case query_type_entity_count: { ++m_master_query_result.m_entity_count; break; }
		case query_type_entity_array: { m_master_query_result.m_entity_array.push_back(proxy.m_entity); break; }
		case query_type_any_occurance: { m_master_query_result.m_any = true; break; }
		default:
		case query_type_none:
			return false;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float cworld::RayCastCallback(const b2RayCastInput& ray_input, int proxy_id)
	{
		CORE_NAMED_ZONE("cworld::RayCastCallback");

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
		auto& proxy = *reinterpret_cast<sentity_proxy*>(GetUserData(proxy_id));

		if (proxy.m_proxy_query_key == m_master_query_key)
		{
			return result;
		}

		proxy.m_proxy_query_key = m_master_query_key;

		//- TODO: apply filter

		switch (m_master_query_type)
		{
		case query_type_entity_count: {++m_master_query_result.m_entity_count; break; }
		case query_type_entity_array: {m_master_query_result.m_entity_array.push_back(proxy.m_entity); break; }
		case query_type_any_occurance: {m_master_query_result.m_any = true; break; }
		default:
		case query_type_none:
			return false;
		}

		return result;
	}

	// -Function responsible for creating a system for a world with given configuration, without matching any components
	//- and function to execute.
	//- A task is similar to a normal system, only that it does not match any components and thus no entities.
	//- If entities are required they can be retrieved through the world or a query.
	//- The function itself is executed as is, with only delta time provided.
	//------------------------------------------------------------------------------------------------------------------------
	void cworld::create_task(const system::sconfig& cfg, system::task_callback_t callback)
	{
		CORE_ASSERT(!(algorithm::bit_check(cfg.m_flags, system::system_flag_multithreaded) &&
			algorithm::bit_check(cfg.m_flags, system::system_flag_immediate)), "Invalid operation! A system cannot be multithreaded and immediate at the same time!");

		auto builder = ecs().system(cfg.m_name.c_str());

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

		//- Create function to be called for running the task
		auto function = [=](flecs::iter& it)
			{
				(callback)(it.delta_time());
			};

		auto task = builder.run(std::move(function));

		//- Set options that are required after system entity creation
		{
			for (const auto& after : cfg.m_run_after)
			{
				if (const auto [result, phase] = detail::is_flecs_built_in_phase(after); result)
				{
					task.add(flecs::Phase).depends_on(phase);
				}
				else
				{
					if (auto e = ecs().lookup(after.c_str()); e.is_valid())
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
				if (const auto [result, phase] = detail::is_flecs_built_in_phase(before); result)
				{
					task.add(flecs::Phase).depends_on(phase);
				}
				else
				{
					if (auto e = ecs().lookup(before.c_str()); e.is_valid())
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
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld::load(const core::cpath& path)
	{
		simdjson::dom::parser parser;
		simdjson::dom::element element;

		auto memory = fs::load_text_from_file(path.view());

		if(parser.parse((const char*)memory->data(), (size_t)memory->size()).get(element) == simdjson::SUCCESS)
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
						log_error("Could not deserialize entity, as it is not a JSON object");
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
		for (const auto& e : em().entities())
		{
			serialize_entity(e, json[C_ENTITIES_PROP][i++]);
		}

		fs::save_text_to_file(path.view(), json.dump(4).data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::use_threads(unsigned count)
	{
		m_used_threads = count;
		ecs_set_threads(ecs(), SCAST(int32_t, m_used_threads));
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cworld::used_threads() const
	{
		return m_used_threads;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::serialize_entity(const flecs::entity e, nlohmann::json& json)
	{
		CORE_NAMED_ZONE("cworld::serialize_entity");

		json = nlohmann::json::object();

		json["name"] = e.name();

		json[C_COMPONENTS_PROP] = nlohmann::json::array();

		auto i = 0;
		for (const auto& c : cm().all(e))
		{
			auto type = rttr::type::get_by_name(c);

			if (auto serialize_method = type.get_method(ecs::detail::C_COMPONENT_SERIALIZE_FUNC_NAME.data()); serialize_method.is_valid())
			{
				log_error(fmt::format("\tserializing component '{}'", c));

				serialize_method.invoke({}, e, json[C_COMPONENTS_PROP][i++]);
			}
			else
			{
				//- RTTR error, component does not have a serialize function
				log_error(fmt::format("\tcould not find 'serialize' method for component '{}'", c));
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::update_proxy(flecs::entity e)
	{
		CORE_NAMED_ZONE("cworld::update_proxy");

		const auto& proxy = m_proxies.at(e.id());
		const auto& transform = *e.get<stransform>();

		MoveProxy(proxy.m_proxy_id, math::caabb(transform.m_position.x, transform.m_position.y,
			transform.m_scale.x * 0.5f, transform.m_scale.y * 0.5f), { 0.0f, 0.0f });
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::destroy_proxy(flecs::entity e)
	{
		CORE_NAMED_ZONE("cworld::destroy_proxy");

		const auto& proxy = m_proxies.at(e.id());

		DestroyProxy(proxy.m_proxy_id);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::create_proxy(flecs::entity e)
	{
		CORE_NAMED_ZONE("cworld::create_proxy");

		auto* id = e.get_mut<sidentifier>();

		auto& proxy = m_proxies.emplace(e.id(), sentity_proxy{ e, 0, 0 }).first->second;

		proxy.m_proxy_id = CreateProxy(math::caabb(0.0f, 0.0f, 0.0f, 0.0f), (void*)&proxy);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld::has_proxy(flecs::entity e)
	{
		return m_proxies.find(e.id()) != m_proxies.end();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::deserialize_entity(const simdjson::dom::object& json)
	{
		CORE_NAMED_ZONE("cworld::deserialize_entity");

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

							//- TODO: emplace component to entity

							if (var.is_valid())
							{
								//-- success. Component deserialized.
								log_info(fmt::format("\tdeserialized component '{}'", type_name));
							}
							else
							{
								//-- error. Could not deserialize component from JSON
								log_error(fmt::format("\tfailed deserializing component '{}' from JSON", type_name));
							}
						}
						else
						{
							//-- JSON error. Cant find component object by given type key
							log_error(fmt::format("\tcould not find component by type key '__type__' = '{}'", type_name));
						}
					}
					else
					{
						//-- RTTR error. Cant find type by name
						log_error(fmt::format("\tcould not locate component '{}' within RTTR", type_name));
					}
				}
				else
				{
					//-- JSON error. Cant find type name
					log_error("\tcould not locate '__type__' of component");
				}
			}
		}
	}

} //- ecs