#pragma once
#include "ecs_world.hpp"

namespace ecs
{
	//- Central place for storing and accessing all worlds.
	//------------------------------------------------------------------------------------------------------------------------
	class cworld_manager final
	{
	public:
		STATIC_INSTANCE(cworld_manager, s_cworld_manager);

		bool create(stringview_t name);
		bool set_active(stringview_t name);
		void tick(float dt);
		const cworld& active() const;
		cworld& active();
		decltype(auto) has_active() const {return m_current != 0;}

	private:
		unsigned m_current = 0;
		umap_t<unsigned, cworld> m_worlds;

	private:
		cworld_manager();
		~cworld_manager();

		void init_ecs_os_api();
	};

} //- ecs
