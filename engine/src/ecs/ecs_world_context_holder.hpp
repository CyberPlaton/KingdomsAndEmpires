#pragma once
#include <core.h>
#include <flecs.h>

namespace ecs
{
	//- Note: context holds pinter in order to make the class move constructible,
	//- this is required by flecs.
	//------------------------------------------------------------------------------------------------------------------------
	class iworld_context_holder
	{
	public:
		iworld_context_holder(flecs::world& w);
		~iworld_context_holder();

	private:
		flecs::world* m_world;

	protected:
		flecs::world& world();
		const flecs::world& world() const;

		RTTR_ENABLE();
		RTTR_REFLECTABLE();
	};

} //- ecs

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(iworld_context_holder)
	{
		rttr::registration::class_<iworld_context_holder>("iworld_context_holder")
			.constructor<flecs::world&>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

} //- ecs