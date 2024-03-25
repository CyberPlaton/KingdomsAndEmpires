#include "ecs_prefab.hpp"

namespace ecs
{
	//- create a prefab entity to be ready for definition
	//------------------------------------------------------------------------------------------------------------------------
	cprefab::cprefab(const std::string& name, flecs::world& w) :
		iworld_context_holder(w)
	{
		m_entity = world().prefab(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprefab::~cprefab()
	{
		m_entity.destruct();
	}

	//------------------------------------------------------------------------------------------------------------------------
	stringview_t cprefab::name() const
	{
		return m_entity.name().c_str();
	}

	//------------------------------------------------------------------------------------------------------------------------
	stringview_t cprefab::parent_name() const
	{
		if (auto p = m_entity.parent(); p.is_valid())
		{
			return p.name().c_str();
		}
		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	flecs::entity cprefab::self() const
	{
		return m_entity;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::cprefab& cprefab::parent(flecs::entity e)
	{
		m_entity.child_of(e);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::cprefab& cprefab::unparent()
	{
		m_entity.remove(flecs::ChildOf, m_entity.parent());
		return *this;
	}

} //- ecs