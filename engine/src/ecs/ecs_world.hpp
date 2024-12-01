#pragma once
#include "../math.hpp"
#include "ecs_system.hpp"
#include "ecs_entity_manager.hpp"
#include "ecs_module_manager.hpp"
#include "ecs_component_manager.hpp"
#include "ecs_query_manager.hpp"
#include "ecs_singleton_manager.hpp"
#include "ecs_prefab_manager.hpp"
#include "ecs_proxy_manager.hpp"

namespace ecs
{
	namespace world
	{
		using world_flags_t = int;

		//------------------------------------------------------------------------------------------------------------------------
		struct sconfig final
		{
			string_t m_name;
			vector_t<string_t> m_modules;
			unsigned m_threads				= 1;
			world_flags_t m_flags			= 0;
		};

	} //- world

	//- Wrapper around flecs::world. Allows queries into current state of the ecs world and also in spatial world.
	//- Note that a world is not intended to be constructed on its own, rather through the world manager.
	//- Systems, tasks and modules can be added though the world object.
	//------------------------------------------------------------------------------------------------------------------------
	class cworld final : private core::cnon_copyable
	{
	public:
		cworld(world::sconfig cfg);
		~cworld();

		void tick(float dt);
		core::srect world_visible_area(const vec2_t& target, const vec2_t& offset, float zoom) const;
		[[nodiscard]] const auto& visible_entities() const { return prm().visible_entities(); }
		inline bool is_visible(const flecs::entity& e) const { return prm().is_visible(e); };

		stringview_t name() const { return m_name; }

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

		const cprefab_manager& pm() const { return m_prefab_manager; }
		cprefab_manager& pm() { return m_prefab_manager; }

		const csingleton_manager& sm() const { return m_singleton_manager; }
		csingleton_manager& sm() { return m_singleton_manager; }

		const cproxy_manager& prm() const { return m_proxy_manager; }
		cproxy_manager& prm() { return m_proxy_manager; }

		template<typename... TComps, typename TCallable>
		flecs::entity query_one(TCallable callback) const;

		template<typename TSingleton>
		TSingleton& singleton();

		template<typename TSingleton>
		const TSingleton& singleton() const;

		template<typename... TComps>
		void create_system(const system::sconfig& cfg, system::system_callback_t<TComps...> callback);

		void create_task(const system::sconfig& cfg, system::task_callback_t callback);

		template<typename TModule>
		void import_module(const modules::sconfig& cfg);

	private:
		cquery_manager m_query_manager;
		ccomponent_manager m_component_manager;
		cprefab_manager m_prefab_manager;
		csingleton_manager m_singleton_manager;
		centity_manager m_entity_manager;
		cmodule_manager m_module_manager;
		cproxy_manager m_proxy_manager;

		//- TODO: where to use mutex
		core::cmutex m_mutex;
		flecs::query<const stransform> m_transform_change_tracker;

		stringview_t m_name;
		flecs::world m_world;

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
	};

	//- Example usage:
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
			else
			{
				log_error(fmt::format("Failed creating dependency module '{}' for world '{}' and module '{}', as the component is not reflected to RTTR!",
					m.data(), name().data(), cfg.m_name.data()));
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
			else
			{
				log_error(fmt::format("Failed creating component '{}' for world '{}' and module '{}', as the component is not reflected to RTTR!",
					c.data(), name().data(), cfg.m_name.data()));
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
			else
			{
				log_error(fmt::format("Failed creating system '{}' for world '{}' and module '{}', as the system is not reflected to RTTR!",
					s.data(), name().data(), cfg.m_name.data()));
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	void ecs::cworld::create_system(const system::sconfig& cfg, system::system_callback_t<TComps...> callback)
	{
		mm().create_system<TComps...>(ecs(), cfg, callback);
	};

} //- ecs