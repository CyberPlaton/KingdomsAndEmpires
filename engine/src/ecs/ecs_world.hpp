#pragma once
#include "ecs_entity_manager.hpp"
#include "ecs_system_manager.hpp"
#include "ecs_module_manager.hpp"
#include "ecs_component_manager.hpp"
#include "ecs_query_manager.hpp"
#include "ecs_singleton_manager.hpp"
#include "../physics/b2_physics.hpp"

namespace ecs
{
	//- Wrapper around flecs::world. Allows queries into current state of the ecs world and
	//- also in spatial world.
	//- Note that a world is not intended to be constructed on its own, rather through
	//- the world manager.
	//------------------------------------------------------------------------------------------------------------------------
	class cworld final : protected b2DynamicTree, core::cnon_copyable
	{
	public:
		cworld(stringview_t name);
		~cworld();

		void tick(float dt);

		flecs::snapshot snapshot() const;

		bool load(const core::cpath& path);
		void save(const core::cpath& path);

		const flecs::world& world() const { return m_world; }
		flecs::world& world() { return m_world; }

		const centity_manager& enttm() const { return m_entity_manager; }
		centity_manager& enttm() { return m_entity_manager; }

		const csystem_manager& sysm() const { return m_system_manager; }
		csystem_manager& sysm() { return m_system_manager; }

		const cmodule_manager& modm() const { return m_module_manager; }
		cmodule_manager& modm() { return m_module_manager; }

		const ccomponent_manager& compm() const { return m_component_manager; }
		ccomponent_manager& compm() { return m_component_manager; }

		const cquery_manager& querym() const { return m_query_manager; }
		cquery_manager& querym() { return m_query_manager; }

		const csingleton_manager& singlm() const { return m_singleton_manager; }
		csingleton_manager& singlm() { return m_singleton_manager; }

		bool QueryCallback(int proxy_id);
		float RayCastCallback(const b2RayCastInput& ray_input, int proxy_id);

	private:
		cquery_manager m_query_manager;
		csystem_manager m_system_manager;
		ccomponent_manager m_component_manager;
		cmodule_manager m_module_manager;
		csingleton_manager m_singleton_manager;
		centity_manager m_entity_manager;

		stringview_t m_name;
		flecs::world m_world;

		struct sworld_query
		{
			vector_t<flecs::entity> m_entity_array;
			unsigned m_entity_count;
			bool m_any;
		};

		static constexpr unsigned C_MASTER_QUERY_KEY_MAX = cquery_manager::C_QUERY_COUNT_MAX;
		query_type m_master_query_type;
		unsigned m_master_query_key;
		sworld_query m_master_query_result;

	private:
		void deserialize_entity(const simdjson::dom::object& json);
		void serialize_entity(const flecs::entity e, nlohmann::json& json);
		void update_proxy(flecs::entity e);
		void destroy_proxy(flecs::entity e);
		void create_proxy(flecs::entity e);
		bool has_proxy(flecs::entity e);
		void process_queries();
	};

} //- ecs