#include "ecs_world.hpp"

namespace ecs
{

	//------------------------------------------------------------------------------------------------------------------------
	cworld::cworld(stringview_t name) :
		m_name(name),
		m_entity_manager(m_world),
		m_system_manager(m_world),
		m_module_manager(m_world)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cworld::~cworld()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld::QueryCallback(int proxy_id)
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float cworld::RayCastCallback(const b2RayCastInput& ray_input, int proxy_id)
	{
		return 0.0f;
	}

} //- ecs