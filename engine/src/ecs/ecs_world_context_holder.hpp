#pragma once
#include <core.h>
#include <flecs.h>

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

		//- Called before shutting down the world and while it is still valid.
		//- Can be used to destroy entities, prefabs etc.
		virtual void on_shutdown() {}

		bool is_prefab(flecs::entity e) const;
		vector_t<std::string> components(flecs::entity e) const;

	private:
		flecs::world* m_world;

	protected:
		flecs::world& world();
		const flecs::world& world() const;

		RTTR_ENABLE();
	};

} //- ecs

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(iworld_context_holder)
	{
		rttr::cregistrator<iworld_context_holder, rttr::detail::no_default>("iworld_context_holder")
			.ctor<rttr::detail::as_raw_pointer, flecs::world&>()
			;
	}

} //- ecs