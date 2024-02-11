#pragma once
#include <flecs.h>
#include <core.h>

namespace ecs
{

	//------------------------------------------------------------------------------------------------------------------------
	class centity final
	{
	public:
		centity(flecs::entity e) {}

		virtual void on_update(float) {}


		const auto& uuid() const { return m_uuid; }

	private:
		core::cuuid m_uuid;
		vector_t<rttr::variant> m_components;
	};

} //- ecs