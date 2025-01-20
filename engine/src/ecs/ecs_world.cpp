#include "ecs_world.hpp"
#include "../animation/animation_module.hpp"
#include "../render/render_module.hpp"
#include "../camera/camera_module.hpp"

namespace ecs
{
	namespace
	{
		constexpr std::string_view C_ENTITIES_PROP		= "entities";
		constexpr std::string_view C_SYSTEMS_PROP		= "systems";
		constexpr std::string_view C_MODULES_PROP		= "modules";
		constexpr std::string_view C_COMPONENTS_PROP	= "components";

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cworld::cworld(world::sconfig cfg) :
		m_name(cfg.m_name),
		m_entity_manager(this),
		m_component_manager(this),
		m_prefab_manager(this),
		m_query_manager(this),
		m_singleton_manager(this),
		m_module_manager(this),
		m_proxy_manager(this),
		m_used_threads(cfg.m_threads == MAX(unsigned) ? core::cthread_service::hardware_threads() / 2 : cfg.m_threads)
	{
		use_threads(m_used_threads);

		ecs().set_ctx(this);

		//- setup observers
		//- observe transform changes for AABB tree
		ecs().observer<stransform, sidentifier>()
			.event(flecs::OnAdd)
			.event(flecs::OnRemove)
			.ctx(this)
			.each([](flecs::iter& it, size_t i, stransform& transform, sidentifier& id)
				{
					auto* world = reinterpret_cast<cworld*>(it.ctx());

					auto e = it.entity(i);

					if (it.event() == flecs::OnAdd)
					{
						//- add aabb proxy for entity to b2DynamicTree
						world->create_proxy(e);

						//- name of the component will be in form 'ecs.transform', we have to format the name first
						//cm().on_component_added(e, c);
					}
					else if (it.event() == flecs::OnRemove)
					{
						//- remove aabb proxy of entity from b2DynamicTree
						world->destroy_proxy(e);
					}
				});

		//- observe added and removed components of entities
		ecs().observer<>()
			.event(flecs::OnAdd)
			.event(flecs::OnRemove)
			.with(flecs::Wildcard)
			.ctx(this)
			.each([](flecs::iter& it, size_t i)
				{
					auto* world = reinterpret_cast<cworld*>(it.ctx());
					auto entity = it.entity(i);
					auto string = it.event_id().str();

					//- Filter for actual ecs components
					if (const auto offset = string_utils::find_substr(string.c_str(), "ecs."); offset != std::string::npos)
					{
						auto c = string_utils::substr(string.c_str(), offset + strlen("ecs."));

						if (it.event() == flecs::OnAdd)
						{
							//- name of the component will be in form 'ecs.transform', we have to format the name first
							world->cm().on_component_added(entity, c);
						}
						else if (it.event() == flecs::OnRemove)
						{
							world->cm().on_component_removed(entity, c);
						}
					}
				});

		m_transform_change_tracker = ecs().query_builder<const stransform>()
			.cached()
			.build();

		//- Import default modules
		mm().import_module<camera::scamera_module>();
		mm().import_module<render::srender_module>();
		mm().import_module<animation::sanimation_module>();

		for (const auto& m : cfg.m_modules)
		{
			mm().import_module(m);
		}

		mm().do_import_modules();

#if CORE_DEBUG
		if constexpr (engine::cfg::C_ECS_MODULES_AND_SYSTEMS_GATHER_INFORMATION)
		{
			mm().dump_adjacency_map();
		}
#endif
	}

	//------------------------------------------------------------------------------------------------------------------------
	cworld::~cworld()
	{
		m_transform_change_tracker.destruct();
	}

	//------------------------------------------------------------------------------------------------------------------------
	mat4_t cworld::camera_projection_mtx() const
	{
		if (auto e = query_one<const scamera>([](const scamera& c)
			{
				return true;

			}); e.is_valid())
		{
			const auto& c = *e.get<scamera>();

			return glm::ortho(-c.m_zoom, c.m_zoom, -c.m_zoom, c.m_zoom);
		}

		return math::C_MAT4_ID;
	}

	//------------------------------------------------------------------------------------------------------------------------
	mat4_t cworld::camera_view_mtx() const
	{

		if (auto e = query_one<const scamera>([](const scamera& c)
			{
				return true;

			}); e.is_valid())
		{
			const auto& c = *e.get<scamera>();

			return math::transform(c.m_position, c.m_scale, vec2_t(0.0f), c.m_rotation);
		}

		return math::C_MAT4_ID;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::tick(float dt)
	{
		CORE_NAMED_ZONE(cworld::tick);

		prepare();

		ecs().progress(dt);

		//- process any queries, they will be available for systems on next tick,
		//- also clearup memory for already taken and processed queries.
		process_queries();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::prepare()
	{
		CORE_NAMED_ZONE(cworld::prepare);

		//- observe changes and update internal AABB tree
		if (m_transform_change_tracker.changed())
		{
			m_transform_change_tracker.run([this](flecs::iter& it)
				{
					//- Note: this is necessary in order to avoid a crash, I assume this cleans up the iteration stack correctly
					while (it.next())
					{
						for (auto i : it)
						{
							auto e = it.entity(i);

							if (has_proxy(e)) { update_proxy(e); }
						}
					}
				});
		}

		//- get current viewing rect for active camera
		math::caabb aabb(world_visible_area());

		prm().prepare(aabb);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::process_queries()
	{
		CORE_NAMED_ZONE(cworld::process_queries);

		for (auto* query = qm().fetch(); query != nullptr; query = qm().fetch())
		{
			process_query(*query);
		}

		qm().tick();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::process_query(cquery& q)
	{
		prm().process_query(q);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::srect cworld::world_visible_area() const
	{
		const auto window_size = sm::window_size();

		mat4_t matrix = math::C_MAT4_ID;

		if (auto e = query_one<const scamera>([](const scamera& c)
			{
				return true;

			}); e.is_valid())
		{
			const auto& c = *e.get<scamera>();

			matrix = math::transform(c.m_position, c.m_scale * c.m_zoom, vec2_t(0.0f), c.m_rotation);
		}

		const auto topleft = math::unproject_2d({ 0.0f, 0.0f }, matrix);
		const auto bottomright = math::unproject_2d({ window_size.x, window_size.y }, matrix);

		return { topleft.x, topleft.y, bottomright.x, bottomright.y };
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::create_task(const system::sconfig& cfg, system::task_callback_t callback)
	{
		mm().create_task(ecs(), cfg, callback);
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
		CORE_NAMED_ZONE(cworld::serialize_entity);

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
		prm().update_proxy(e);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::destroy_proxy(flecs::entity e)
	{
		prm().destroy_proxy(e);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::create_proxy(flecs::entity e)
	{
		prm().create_proxy(e);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld::has_proxy(flecs::entity e)
	{
		return prm().has_proxy(e);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld::deserialize_entity(const simdjson::dom::object& json)
	{
		CORE_NAMED_ZONE(cworld::deserialize_entity);

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