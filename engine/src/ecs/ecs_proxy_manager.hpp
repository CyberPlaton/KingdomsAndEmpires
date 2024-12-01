#pragma once
#include <core.h>
#include <flecs.h>
#include "ecs_world_context_holder.hpp"
#include "ecs_query_manager.hpp"
#include "../math.hpp"

namespace ecs
{
	namespace world
	{
		//------------------------------------------------------------------------------------------------------------------------
		struct sproxy final
		{
			flecs::entity m_entity;
			entity_proxy_t m_id = MIN(entity_proxy_t);
			unsigned m_query_key = 0;
		};

	} //- world

	//- Class responsible for tracking entity proxies in the world, their creation and removal and their update within
	//- Box2D dynamic tree.
	//------------------------------------------------------------------------------------------------------------------------
	class cproxy_manager final :
		protected b2DynamicTree,
		public iworld_context_holder
	{
	public:
		cproxy_manager(cworld* w);
		~cproxy_manager();

		[[nodiscard]] inline const uset_t<flecs::entity_t>& visible_entities() const { return m_visible_entities; }
		inline bool is_visible(const flecs::entity& e) const { return m_visible_entities.find(e.id()) != m_visible_entities.end();}
		void prepare(const math::caabb& aabb);
		void process_query(cquery& q);

		void update_proxy(flecs::entity e);
		void destroy_proxy(flecs::entity e);
		void create_proxy(flecs::entity e);
		bool has_proxy(flecs::entity e) const;

		bool QueryCallback(int proxy_id);
		float RayCastCallback(const b2RayCastInput& ray_input, int proxy_id);

	private:
		struct sworld_query
		{
			vector_t<flecs::entity> m_entity_array;
			unsigned m_entity_count;
			bool m_any;
		};

		static constexpr auto C_MASTER_QUERY_KEY_MAX = engine::cfg::C_ECS_QUERY_COUNT_MAX;

		umap_t<flecs::id_t, world::sproxy> m_proxies;
		sworld_query m_master_query_result;
		uset_t<flecs::entity_t> m_visible_entities;
		query_type m_master_query_type;
		unsigned m_master_query_key;
	};

} //- ecs