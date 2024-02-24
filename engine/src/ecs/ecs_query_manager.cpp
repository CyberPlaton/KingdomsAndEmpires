#include "ecs_query_manager.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	cquery::cquery(query_type type, physics::ray_t ray) :
		m_query_type(type), m_intersection_type(query_intersection_type_raycast), m_has_filter(false),
		m_ray(std::move(ray)), m_ready(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cquery::cquery(query_type type, physics::aabb_t aabb) :
		m_query_type(type), m_intersection_type(query_intersection_type_aabb), m_has_filter(false),
		m_aabb(std::move(aabb)), m_ready(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cquery::cquery(query_type type, physics::ray_t ray, rttr::instance filter) :
		m_query_type(type), m_intersection_type(query_intersection_type_raycast), m_has_filter(true),
		m_ray(std::move(ray)), m_filter(filter), m_ready(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cquery::cquery(query_type type, physics::aabb_t aabb, rttr::instance filter) :
		m_query_type(type), m_intersection_type(query_intersection_type_aabb), m_has_filter(true),
		m_aabb(std::move(aabb)), m_filter(filter), m_ready(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	const ecs::query_type cquery::type() const
	{
		return m_query_type;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const ecs::query_intersection_type cquery::intersection_type() const
	{
		return m_intersection_type;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const bool cquery::has_filter() const
	{
		return m_has_filter;
	}

	//------------------------------------------------------------------------------------------------------------------------
	rttr::instance cquery::filter() const
	{
		if (has_filter())
		{
			return m_filter;
		}
		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	const physics::ray_t cquery::raycast() const
	{
		if (intersection_type() == query_intersection_type_raycast)
		{
			return m_ray;
		}
		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	const physics::aabb_t cquery::aabb() const
	{
		if (intersection_type() == query_intersection_type_aabb)
		{
			return m_aabb;
		}
		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	vector_t<flecs::entity> cquery::result_entity_array() const
	{
		if (type() == query_type_entity_array)
		{
			return m_entities;
		}
		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cquery::result_entity_count() const
	{
		if (type() == query_type_entity_count)
		{
			return m_count;
		}
		return 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cquery::result_any_occurance() const
	{
		if (type() == query_type_any_occurance)
		{
			return m_any;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cquery::finish()
	{
		m_ready = true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cquery::ready() const
	{
		return m_ready;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cquery_manager::cquery_manager(flecs::world& w) :
		m_managed_world(w)
	{
		m_queries.init(C_QUERY_COUNT_MAX);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cquery_manager::~cquery_manager()
	{
		m_queries.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	const ecs::cquery* cquery_manager::take(unsigned id)
	{
		if (ready(id))
		{
			m_taken_queries.push(id);
			return m_queries.find(id);
		}
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cquery_manager::ready(unsigned id)
	{
		return m_queries.find(id)->ready();
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::cquery* cquery_manager::fetch()
	{
		if (!m_query_queue.empty())
		{
			auto i = m_query_queue.front();
			m_query_queue.pop();

			return m_queries.modify(i);
		}
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cquery_manager::tick()
	{
		while (!m_taken_queries.empty())
		{
			auto i = m_taken_queries.front();
			m_taken_queries.pop();

			m_queries.destroy(i);
		}
	}

} //- ecs