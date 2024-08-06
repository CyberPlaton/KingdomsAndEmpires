#pragma once
#include <core.h>
#include <flecs.h>
#include "../config.hpp"

namespace ecs
{
	class cworld;

	//- Note: context holds pointer in order to make the class move constructible,
	//- this is required by flecs.
	//------------------------------------------------------------------------------------------------------------------------
	class iworld_context_holder
	{
		friend class cworld;
	public:
		iworld_context_holder(flecs::world& w);
		~iworld_context_holder();

		//- TODO: this is fini, consider creating a callback or something, this is not the place for it
		//- Called before shutting down the world and while it is still valid.
		//- Can be used to destroy entities, prefabs etc.
		virtual void on_shutdown() {}

		//- TODO: consider moving to entity manager.
		bool is_prefab(flecs::entity e) const;

		//- TODO: consider moving to component manager, seems to make more sense,
		//- in case we remove component manager then probably should be in cworld.
		vector_t<std::string> components(flecs::entity e) const;

	private:
		flecs::world* m_world;

	protected:
		flecs::world& world();
		const flecs::world& world() const;

		RTTR_ENABLE();
	};

} //- ecs