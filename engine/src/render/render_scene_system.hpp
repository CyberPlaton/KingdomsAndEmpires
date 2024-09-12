#pragma once
#include "../ecs/ecs.hpp"

namespace render
{
	//- Class specifying the entry for scene rendering systems. It does not match any components and serves as reference
	//- point for other rendering systems.
	//------------------------------------------------------------------------------------------------------------------------
	class cframe_begin_system : public ecs::ctask
	{
	public:
		cframe_begin_system(flecs::world& w) :
			ecs::ctask
			(w, "Render Frame Begin System")
		{
			build([&](float){ /**/});
			run_after(flecs::OnUpdate);
		}
	};

	//- System responsible for rendering sprites in current world.
	//------------------------------------------------------------------------------------------------------------------------
	class csprite_renderer_system :
		public ecs::csystem<ecs::stransform, ecs::smesh, ecs::smaterial, ecs::ssprite_renderer>
	{
	public:
		csprite_renderer_system(flecs::world& w) :
			ecs::csystem<ecs::stransform, ecs::smesh, ecs::smaterial, ecs::ssprite_renderer>
			(w, "Render Sprite Renderer System")
		{
			multithreaded();
			build([&](const ecs::stransform& transform, const ecs::smesh& mesh,
				const ecs::smaterial& material, const ecs::ssprite_renderer& renderer)
				{
					CORE_NAMED_ZONE("Render Sprite Renderer System");
				});
			run_after(flecs::OnUpdate);
		}
	};

} //- render