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

	namespace tag
	{
		//- Tag indicating that camera can be controlled with a default control scheme. Designed for editor and debugging purposes.
		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_TAG(seditor_camera_controller);

	} //- tag

} //- camera