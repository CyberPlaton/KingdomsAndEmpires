#pragma once
#include <engine.h>
#include "../config.hpp"

namespace render_system
{
	//------------------------------------------------------------------------------------------------------------------------
	class RENDER_API cscenerender_system final : public ecs::ctask
	{
	public:
		cscenerender_system(flecs::world& w) :
			ecs::ctask(w, "Scene Render System")
		{
			build([&](float dt)
				{
					CORE_NAMED_ZONE("Scene Render System");
				});

			run_after(flecs::OnUpdate);
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	class RENDER_API cscenedebugrender_system final : public ecs::ctask
	{
	public:
		cscenedebugrender_system(flecs::world& w) :
			ecs::ctask(w, "Debug Scene Render System")
		{
			build([&](float dt)
				{
					CORE_NAMED_ZONE("Debug Scene Render System");
				});

			run_after("Scene Render System");
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	class RENDER_API cscene_render_module : public ecs::imodule
	{
	public:
		cscene_render_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<cscene_render_module>("Scene Render Module")
				.subsystem<cscene_render_module, cscenerender_system>()
				.subsystem<cscene_render_module, cscenedebugrender_system>()
			.end<cscene_render_module>();
		}

		RTTR_ENABLE(ecs::imodule);
	};

} //- render_system