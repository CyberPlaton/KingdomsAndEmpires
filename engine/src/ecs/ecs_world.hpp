#pragma once
#include "ecs_entity_manager.hpp"
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


		void tick(float dt, system_running_phase p);

		flecs::snapshot snapshot() const;

		bool load(const core::cpath& path);
		void save(const core::cpath& path);

		void use_threads(unsigned count);
		unsigned used_threads() const;

		const flecs::world& world() const { return m_world; }
		flecs::world& world() { return m_world; }

		const centity_manager& em() const { return m_entity_manager; }
		centity_manager& em() { return m_entity_manager; }

		const cmodule_manager& mm() const { return m_module_manager; }
		cmodule_manager& mm() { return m_module_manager; }

		const ccomponent_manager& cm() const { return m_component_manager; }
		ccomponent_manager& cm() { return m_component_manager; }

		const cquery_manager& qm() const { return m_query_manager; }
		cquery_manager& qm() { return m_query_manager; }

		const csingleton_manager& sm() const { return m_singleton_manager; }
		csingleton_manager& sm() { return m_singleton_manager; }

		bool QueryCallback(int proxy_id);
		float RayCastCallback(const b2RayCastInput& ray_input, int proxy_id);

	private:
		cquery_manager m_query_manager;
		ccomponent_manager m_component_manager;
		cmodule_manager m_module_manager;
		csingleton_manager m_singleton_manager;
		centity_manager m_entity_manager;

		stringview_t m_name;
		flecs::world m_world;

		float m_world_tick_dt;
		float m_world_tick_fixed_dt = 0.016f;
		flecs::entity m_update_pipeline;
		flecs::entity m_world_render_pipeline;
		flecs::entity m_ui_render_pipeline;
		flecs::entity m_post_update_pipeline;

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

		unsigned m_used_threads;

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