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
	cprefab::cprefab(flecs::entity e, flecs::world& w) :
		iworld_context_holder(w), m_entity(e)
	{
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

	//- Attach this prefab to another entity or prefab establishing a nesting relationship.
	//- When the parent will be instantiated, then this prefab will be too.
	//- TODO: should we forbid parenting too entities and allow only parenting too other prefabs?
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

	//- Set this prefab as variant of another one. It will have all component from the invariant prefab.
	//- TODO: should we forbid variants from non-prefab entities?
	//------------------------------------------------------------------------------------------------------------------------
	ecs::cprefab& cprefab::variant_of(flecs::entity e)
	{
		m_entity.is_a(e);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::cprefab& cprefab::remove_variant_of(flecs::entity e)
	{
		m_entity.remove(flecs::IsA, e);
		return *this;
	}

} //- ecs