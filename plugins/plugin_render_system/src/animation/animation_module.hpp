#pragma once
#include <engine.h>
#include "../config.hpp"

namespace render_system
{
	void animation_system(flecs::entity e, ecs::sanimation& animation);

	//------------------------------------------------------------------------------------------------------------------------
	struct RENDER_API sanimation_system final
	{
		sanimation_system(flecs::world& w)
		{
			ecs::system::sconfig cfg{ w };
			cfg.m_name = "Animation System";

			ecs::system::create_system(cfg, animation_system);
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct RENDER_API sanimation_module final
	{
		sanimation_module(flecs::world& w)
		{
			ecs::modules::sconfig cfg{ w };
			cfg.m_name = "Animation Module";
			cfg.m_components = { "sanimation" };
			cfg.m_systems = { "sanimation_system" };
			cfg.m_modules = {};

			ecs::modules::import_module(cfg);
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	class RENDER_API canimation_module : public ecs::imodule
	{
	public:
		canimation_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<canimation_module>("Animation Module")
				.subsystem<canimation_module, sanimation_system>()
			.end<canimation_module>();
		}

		RTTR_ENABLE(ecs::imodule);
	};

} //- render_system