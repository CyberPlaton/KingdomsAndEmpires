#pragma once
#include "../math.hpp"
#include "ecs_system.hpp"
#include "ecs_entity_manager.hpp"
#include "ecs_module_manager.hpp"
#include "ecs_component_manager.hpp"
#include "ecs_query_manager.hpp"
#include "ecs_singleton_manager.hpp"
#include "ecs_prefab_manager.hpp"

namespace ecs
{
	//- Wrapper around flecs::world. Allows queries into current state of the ecs world and also in spatial world.
	//- Note that a world is not intended to be constructed on its own, rather through the world manager.
	//- Systems, tasks and modules can be added though the world object.
	//------------------------------------------------------------------------------------------------------------------------
	class cworld final :
		protected b2DynamicTree,
		private core::cnon_copyable
	{
	public:
		cworld(stringview_t name);
		~cworld();

		void tick(float dt);
		[[nodiscard]] inline const vector_t<flecs::entity>& visible_entities() const { return m_visible_entities; }

		bool load(const core::cpath& path);
		void save(const core::cpath& path);

		void use_threads(unsigned count);
		unsigned used_threads() const;

		const flecs::world& ecs() const { return m_world; }
		flecs::world& ecs() { return m_world; }

		const centity_manager& em() const { return m_entity_manager; }
		centity_manager& em() { return m_entity_manager; }

		const cmodule_manager& mm() const { return m_module_manager; }
		cmodule_manager& mm() { return m_module_manager; }

		const ccomponent_manager& cm() const { return m_component_manager; }
		ccomponent_manager& cm() { return m_component_manager; }

		const cquery_manager& qm() const { return m_query_manager; }
		cquery_manager& qm() { return m_query_manager; }

		template<typename... TComps, typename TCallable>
		flecs::entity query_one(TCallable callback) const;

		const csingleton_manager& sm() const { return m_singleton_manager; }
		csingleton_manager& sm() { return m_singleton_manager; }

		template<typename TSingleton>
		TSingleton& singleton();

		template<typename TSingleton>
		const TSingleton& singleton() const;

		const cprefab_manager& pm() const { return m_prefab_manager; }
		cprefab_manager& pm() { return m_prefab_manager; }

		bool QueryCallback(int proxy_id);
		float RayCastCallback(const b2RayCastInput& ray_input, int proxy_id);

		template<typename... TComps>
		void create_system(const system::sconfig& cfg, system::system_callback_t<TComps...>* callback);

		void create_task(const system::sconfig& cfg, system::task_callback_t* callback);

		template<typename TModule>
		void import_module(const modules::sconfig& cfg);

	private:
		cquery_manager m_query_manager;
		ccomponent_manager m_component_manager;
		cprefab_manager m_prefab_manager;
		csingleton_manager m_singleton_manager;
		centity_manager m_entity_manager;
		cmodule_manager m_module_manager;

		//- TODO: where to use mutex
		core::cmutex m_mutex;
		flecs::query<const stransform> m_transform_change_tracker;

		stringview_t m_name;
		flecs::world m_world;

		struct sentity_proxy
		{
			flecs::entity m_entity;
			entity_proxy_t m_proxy_id = -1;
			unsigned m_proxy_query_key = 0;
		};

		struct sworld_query
		{
			vector_t<flecs::entity> m_entity_array;
			unsigned m_entity_count;
			bool m_any;
		};

		static constexpr auto C_MASTER_QUERY_KEY_MAX = engine::cfg::C_ECS_QUERY_COUNT_MAX;
		umap_t<flecs::id_t, sentity_proxy> m_proxies;
		query_type m_master_query_type;
		unsigned m_master_query_key;
		sworld_query m_master_query_result;

		vector_t<flecs::entity> m_visible_entities;

		unsigned m_used_threads;

	private:
		cworld() = default;

		void prepare();
		void deserialize_entity(const simdjson::dom::object& json);
		void serialize_entity(const flecs::entity e, nlohmann::json& json);
		void update_proxy(flecs::entity e);
		void destroy_proxy(flecs::entity e);
		void create_proxy(flecs::entity e);
		bool has_proxy(flecs::entity e);
		void process_queries();
		void process_query(cquery& q);
		core::srect world_visible_area(const vec2_t& target, const vec2_t& offset, float zoom);
	};

	//- example usage:
	//-
	//- auto e = w.qm().query_one<ecs::stransform>([](const ecs::stransform& transform)
	//- {
	//-		return transform.m_rotation > 45;
	//- });
	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps, typename TCallable>
	flecs::entity ecs::cworld::query_one(TCallable callback) const
	{
		//- use ad-hoc filter, as this does not require building tables
		//- and thus can be used inside a progress tick
		return ecs().query_builder<TComps...>()
			.build()
			.find(callback);
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TSingleton>
	TSingleton& ecs::cworld::singleton()
	{
		return *ecs().get_mut<TSingleton>();
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TSingleton>
	const TSingleton& ecs::cworld::singleton() const
	{
		return *ecs().get<TSingleton>();
	};

	//- Instantiates the module object that does the registration along with systems, components and dependency modules.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TModule>
	void ecs::cworld::import_module(const modules::sconfig& cfg)
	{
		//- Import dependencies
		for (const auto& m : cfg.m_modules)
		{
			if (const auto type = rttr::type::get_by_name(m.data()); type.is_valid())
			{
				//- Calling module constructor that does the actual importing of the module
				type.create({ this });
			}
		}

		ecs().module<TModule>(cfg.m_name.data());

		//- Register components
		for (const auto& c : cfg.m_components)
		{
			if (const auto type = rttr::type::get_by_name(c.data()); type.is_valid())
			{
				//- Calling special component constructor that register the component to provided world
				type.create({ this });
			}
		}

		//- Register systems
		for (const auto& s : cfg.m_systems)
		{
			if (const auto type = rttr::type::get_by_name(s.data()); type.is_valid())
			{
				//- Calling system constructor that does the actual registration of the system
				type.create({ this });
			}
		}
	}

	//- Function responsible for creating a system for a world with given configuration, matching components and function to execute.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	void ecs::cworld::create_system(const system::sconfig& cfg, system::system_callback_t<TComps...>* callback)
	{
		CORE_ASSERT(!(algorithm::bit_check(cfg.m_flags, system::system_flag_multithreaded) &&
			algorithm::bit_check(cfg.m_flags, system::system_flag_immediate)), "Invalid operation! A system cannot be multithreaded and immediate at the same time!");

		auto builder = ecs().system<TComps...>(cfg.m_name.c_str());

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

		auto system = builder.each(*callback);

		//- Set options that are required after system entity creation
		{
			for (const auto& after : cfg.m_run_after)
			{
				if (auto e = ecs().lookup(after.c_str()); e.is_valid())
				{
					system.add(flecs::Phase).depends_on(e);
				}
				else
				{
					log_error(fmt::format("Dependency (run after) system '{}' for system '{}' could not be found!",
						after, cfg.m_name));
				}
			}

			for (const auto& before : cfg.m_run_before)
			{
				if (auto e = ecs().lookup(before.c_str()); e.is_valid())
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
	};

} //- ecs