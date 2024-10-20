#pragma once
#include <core.h>
#include <flecs.h>
#include "ecs_world_context_holder.hpp"

namespace ecs
{
	//- Utility class to define prefabs. Prefabs defined with this are kept track of and can be altered at runtime,
	//- looked up in a manager etc.
	//- A serialized prefab is a distinct scene file, load and save is managed from prefab manager.
	//------------------------------------------------------------------------------------------------------------------------
	class cprefab :
		public iworld_context_holder,
		private core::cnon_copyable
	{
	public:
		cprefab(const string_t& name, cworld* w);
		cprefab(flecs::entity e, cworld* w);
		~cprefab();

		stringview_t name() const;
		stringview_t parent_name() const;
		flecs::entity self() const;

		cprefab& parent(flecs::entity e);
		cprefab& unparent();

		cprefab& variant_of(flecs::entity e);
		cprefab& remove_variant_of(flecs::entity e);

		template<typename TComponent>
		cprefab& add();

		template<typename TComponent>
		cprefab& add(TComponent c);

		template<typename TComponent>
		cprefab& override();

		template<typename TComponent>
		cprefab& override(TComponent c);

		template<typename TComponent>
		cprefab& remove();

	private:
		flecs::entity m_entity;
	};

	//- Add a shared component to prefab. This component will be shared by all instances of the prefab.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TComponent>
	cprefab& cprefab::add()
	{
		m_entity.add<TComponent>();
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TComponent>
	cprefab& cprefab::add(TComponent c)
	{
		m_entity.set<TComponent>({c});
		return *this;
	}

	//- Add a private component to prefab. Each instance of the prefab will have a private copy of it.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TComponent>
	cprefab & cprefab::override()
	{
		m_entity.set_override<TComponent>();
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TComponent>
	cprefab & cprefab::override(TComponent c)
	{
		m_entity.set_override<TComponent>({ c });
		return *this;
	}

	//- Remove a component, be it shared or private.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TComponent>
	cprefab& cprefab::remove()
	{
		m_entity.remove<TComponent>();
		return *this;
	}

} //- ecs