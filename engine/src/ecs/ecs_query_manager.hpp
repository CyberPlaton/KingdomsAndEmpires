#pragma once
#include <flecs.h>
#include <core.h>
#include "ecs_world_context_holder.hpp"
#include "../math.hpp"

namespace ecs
{
	constexpr std::string_view C_QUERY_FILTER_FUNCTION_NAME = "pass";

	class cworld;

	//------------------------------------------------------------------------------------------------------------------------
	enum query_type : uint8_t
	{
		query_type_none = 0,
		query_type_any_occurance,
		query_type_entity_array,
		query_type_entity_count,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum query_intersection_type : uint8_t
	{
		query_intersection_type_none = 0,
		query_intersection_type_raycast,
		query_intersection_type_aabb,
	};

	//- Inherit and override pass function with an arbitrary filter logic.
	//------------------------------------------------------------------------------------------------------------------------
	struct squery_filter
	{
		virtual bool pass(const flecs::entity) = 0;
	};

	//- Query for the world.
	//- The world is responsible for modifying data inside the query and setting its status to ready
	//- after its initial construction.
	//------------------------------------------------------------------------------------------------------------------------
	class cquery
	{
		friend class cworld;
	public:
		cquery(query_type type, physics::ray_t ray);
		cquery(query_type type, math::caabb aabb);
		cquery(query_type type, physics::ray_t ray, rttr::instance filter);
		cquery(query_type type, math::caabb aabb, rttr::instance filter);

		const query_type type() const;
		const query_intersection_type intersection_type() const;
		const bool has_filter() const;
		rttr::instance filter() const;
		const physics::ray_t raycast() const;
		const math::caabb aabb() const;

		vector_t<flecs::entity> result_entity_array() const;
		unsigned result_entity_count() const;
		bool result_any_occurance() const;

		void finish();
		bool ready() const;

	private:
		const rttr::instance m_filter;
		const query_type m_query_type;
		const query_intersection_type m_intersection_type;
		const bool m_has_filter;
		bool m_ready;

		//- result data
		vector_t<flecs::entity> m_entities;
		unsigned m_count;
		bool m_any;

		//- query data
		physics::ray_t m_ray;
		math::caabb m_aabb;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cquery_manager : public iworld_context_holder
	{
		friend class cworld;
	public:
		cquery_manager(cworld* w);
		~cquery_manager();

		template<typename... ARGS>
		query_t query(ARGS&&... args);

		//- return a query that is ready to be processed. If it is not ready it returns nullptr.
		//- Note: after taking a valid query, it will be destroyed next frame so process it directly and
		//- do not store it for later.
		[[nodiscard]] const cquery* take(unsigned id);

		bool ready(unsigned id);

	private:
		core::cdynamic_pool<cquery> m_queries;

		//- order of queries.
		std::queue<unsigned> m_query_queue;

		//- queries processed by system that queried, thus they can
		//- be released.
		std::queue<unsigned> m_taken_queries;

	private:
		//- Fetch the next query to be processed, return value must not be ignored,
		//- as the query index will be popped and it wont be processed afterwards
		[[nodiscard]] cquery* fetch();

		//- blocking task, inteded to be called every frame by cworld. Process clearing up processed queries.
		//- Note: for now we do not limit time of processing, but it might become relevant later.
		void tick();
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename... ARGS>
	query_t cquery_manager::query(ARGS&&... args)
	{
		query_t i = 0;

		m_queries.create(&i, args...);

		m_query_queue.push(i);

		return i;
	}

} //- ecs