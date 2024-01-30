#pragma once
#include <core.h>
#include <flecs.h>

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	class cworld_manager
	{
	public:
		STATIC_INSTANCE(cworld_manager, s_cworld_manager);

		bool create(stringview_t name);

		bool checkout(stringview_t name);

		const flecs::world& current() const;
		flecs::world& current();

	private:
		unsigned m_current = 0;
		umap_t<unsigned, ref_t<flecs::world>> m_worlds;
	};

} //- ecs