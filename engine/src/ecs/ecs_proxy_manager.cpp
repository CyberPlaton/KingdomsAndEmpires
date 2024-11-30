#include "ecs_proxy_manager.hpp"
#include "ecs_component.hpp"
#include "ecs_world.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	cproxy_manager::cproxy_manager(cworld* w) :
		iworld_context_holder(w)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	cproxy_manager::~cproxy_manager()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cproxy_manager::prepare(const math::caabb& aabb)
	{
		//- reset
		m_master_query_type = query_type_none;
		m_master_query_result.m_entity_array.clear();
		m_master_query_result.m_entity_count = 0;
		m_master_query_result.m_any = false;

		//- perform a query for all currently visible entities
		m_master_query_type = query_type_entity_array;
		m_master_query_key = (m_master_query_key + 1) % C_MASTER_QUERY_KEY_MAX;

		Query(this, aabb);

		m_visible_entities = std::move(m_master_query_result.m_entity_array);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cproxy_manager::process_query(cquery& q)
	{
		CORE_NAMED_ZONE(cproxy_manager::process_query);

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
	void cproxy_manager::update_proxy(flecs::entity e)
	{
		CORE_NAMED_ZONE(cproxy_manager::update_proxy);

		const auto& proxy = m_proxies.at(e.id());
		const auto& transform = *e.get<stransform>();

		MoveProxy(proxy.m_id, math::caabb(transform.m_position.x, transform.m_position.y,
			transform.m_scale.x * 0.5f, transform.m_scale.y * 0.5f), { 0.0f, 0.0f });
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cproxy_manager::destroy_proxy(flecs::entity e)
	{
		CORE_NAMED_ZONE(cproxy_manager::destroy_proxy);

		if (const auto it = m_proxies.find(e.id()); it != m_proxies.end())
		{
			DestroyProxy(it->second.m_id);

			algorithm::erase_at(m_proxies, it);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cproxy_manager::create_proxy(flecs::entity e)
	{
		CORE_NAMED_ZONE(cproxy_manager::create_proxy);

		auto* id = e.get_mut<sidentifier>();

		auto& proxy = m_proxies.emplace(e.id(), world::sproxy{ e, 0, 0 }).first->second;

		proxy.m_id = CreateProxy(math::caabb(0.0f, 0.0f, 0.0f, 0.0f), (void*)&proxy);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cproxy_manager::has_proxy(flecs::entity e) const
	{
		return m_proxies.find(e.id()) != m_proxies.end();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cproxy_manager::QueryCallback(int proxy_id)
	{
		CORE_NAMED_ZONE(cproxy_manager::QueryCallback);

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
		auto& proxy = *reinterpret_cast<world::sproxy*>(GetUserData(proxy_id));

		if (proxy.m_query_key == m_master_query_key)
		{
			return result;
		}

		proxy.m_query_key = m_master_query_key;

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
	float cproxy_manager::RayCastCallback(const b2RayCastInput& ray_input, int proxy_id)
	{
		CORE_NAMED_ZONE(cproxy_manager::RayCastCallback);

		//- 0.0f signals to stop and 1.0f signals to continue
		float result = 0.0f;

		switch (m_master_query_type)
		{
		case query_type_entity_count: { result = 1.0f; break; }
		case query_type_entity_array: { result = 1.0f; break; }
		case query_type_any_occurance: { result = 0.0f; break; }
		default:
		case query_type_none:
			return false;
		}

		//- assign query key to proxy and avaoid duplicate queries
		auto& proxy = *reinterpret_cast<world::sproxy*>(GetUserData(proxy_id));

		if (proxy.m_query_key == m_master_query_key)
		{
			return result;
		}

		proxy.m_query_key = m_master_query_key;

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

} //- ecs