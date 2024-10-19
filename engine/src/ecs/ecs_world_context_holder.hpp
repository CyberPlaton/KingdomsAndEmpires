#pragma once
#include <core.h>
#include <flecs.h>
#include "../config.hpp"

namespace ecs
{
	class cworld;

	//------------------------------------------------------------------------------------------------------------------------
	class iworld_context_holder
	{
		friend class cworld;
	public:
		iworld_context_holder(cworld* w);
		~iworld_context_holder();

		cworld& world();
		const cworld& world() const;

	private:
		cworld* m_world = nullptr;

		RTTR_ENABLE();
	};

} //- ecs