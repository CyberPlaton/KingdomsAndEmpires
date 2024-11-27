#pragma once
#include "../ecs/ecs_component.hpp"

namespace camera
{
	//- Component designed as extension to camera entity. Note: in this and below components it is necessary and
	//- expected that the entity has a scamera component, otherwise the according systems will not be triggered.
	//------------------------------------------------------------------------------------------------------------------------
	struct sfollow_entity final : public ecs::icomponent
	{
		DECLARE_COMPONENT(sfollow_entity);

		static void show_ui(flecs::entity e);

		core::cuuid m_entity_to_follow;

		RTTR_ENABLE(ecs::icomponent);
	};

} //- camera