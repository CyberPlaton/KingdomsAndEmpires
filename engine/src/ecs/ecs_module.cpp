#include "ecs_module.hpp"
#include <plugin_logging.h>

namespace ecs
{
} //- ecs

namespace ecs::example
{
	//------------------------------------------------------------------------------------------------------------------------
	struct sreplicable_component {};

	//------------------------------------------------------------------------------------------------------------------------
	struct stransform_component
	{
		float x, y, r;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sidentifier_component
	{
		core::cuuid uuid;
	};

	//- This module introduces 3 new components to the world, flecs-hierarchical as "ecs.example.sreplicable_component" etc.
	//- Also it registers a new system that is directly using those components. This structure is desirable and interdependecies
	//- between system should not exist, if there are any, redesign or collaps those systems.
	//------------------------------------------------------------------------------------------------------------------------
	class cexample_module : public imodule
	{
		cexample_module(flecs::world& world) :
			imodule(world)
		{
			m_module = world.module<cexample_module>();

			//- register components in module scope
			world.component<sreplicable_component>();
			world.component<stransform_component>();
			world.component<sidentifier_component>();

			//- register any system associated with the module, or not..
			world.system<sreplicable_component,
				const stransform_component,
				const sidentifier_component>("Replication")
				.each([](sreplicable_component& rep,
					const stransform_component& trans,
					const sidentifier_component& id)
					{
						//- send position and identifier over network
						logging::log_info(fmt::format("[Network] Replicating entity \"{}\": [{}:{}:{}]", id.uuid.view(), trans.x, trans.y, trans.r));
					});

		}

	private:
		flecs::entity m_module;
	};
}