#pragma once
#include "../config.hpp"
#include "../ecs/ecs_world_context_holder.hpp"
#include "../ecs/ecs_component.hpp"
#include "../ecs/ecs_system.hpp"
#include "../ecs/ecs_module.hpp"

namespace render
{
	void scene_render_system(flecs::entity e, const ecs::stransform& transform, const ecs::smaterial& material, const ecs::ssprite_renderer& renderer);
	void scene_debug_render_system(flecs::entity e, const ecs::stransform& transform, const ecs::smaterial& material, const ecs::ssprite_renderer& renderer);

	//------------------------------------------------------------------------------------------------------------------------
	struct srender_frame_begin_system final
	{
		srender_frame_begin_system() = default;
		srender_frame_begin_system(ecs::cworld* w);

		static ecs::system::sconfig config();

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct srender_frame_end_system final
	{
		srender_frame_end_system() = default;
		srender_frame_end_system(ecs::cworld* w);

		static ecs::system::sconfig config();

		RTTR_ENABLE();
	};

	//- System responsible for rendering sprites in current world
	//------------------------------------------------------------------------------------------------------------------------
	struct srender_system final
	{
		srender_system() = default;
		srender_system(ecs::cworld* w);

		static ecs::system::sconfig config();

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct srender_system_renderpass final
	{
		srender_system_renderpass() = default;
		srender_system_renderpass(ecs::cworld* w);

		static ecs::system::sconfig config();

		RTTR_ENABLE();
	};

	//- System responsible for debug rendering on top of rendered sprites in current world.
	//- Note: Eats up a lot of frame time.
	//------------------------------------------------------------------------------------------------------------------------
	struct sdebug_render_system final
	{
		sdebug_render_system() = default;
		sdebug_render_system(ecs::cworld* w);

		static ecs::system::sconfig config();

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct srender_module final
	{
		srender_module() = default;
		srender_module(ecs::cworld* w);

		static ecs::modules::sconfig config();

		RTTR_ENABLE();
	};

} //- render
