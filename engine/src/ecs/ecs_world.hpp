#pragma once
#include <flecs.h>

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	class cworld
	{
	public:
		cworld();
		~cworld();



	private:
		flecs::world m_world;
	};

} //- ecs