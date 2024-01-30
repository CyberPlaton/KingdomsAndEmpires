#include "ecs_module.hpp"
#include "ecs_component.hpp"
#include "ecs_world_manager.hpp"
#include <plugin_logging.h>

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	imodule::imodule(flecs::world& w) :
		iworld_context_holder(w)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	flecs::entity imodule::module() const
	{
		return m_module;
	}

} //- ecs

namespace ecs::example
{
	//------------------------------------------------------------------------------------------------------------------------
	struct sreplicable_component : public icomponent
	{
		DECLARE_COMPONENT(sreplicable_component);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct stransform_component : public icomponent
	{
		DECLARE_COMPONENT(stransform_component);

		float x, y, r;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sidentifier_component : public icomponent
	{
		DECLARE_COMPONENT(sidentifier_component);

		core::cuuid uuid;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class sexample_module_system : public csystem
	{
	public:
		sexample_module_system(flecs::world& w) :
			csystem(w)
		{
			//- use constructor only to define the system
			subsystem([&](flecs::world& w) -> subsystem_registrator_return_t
				{
					//- create dependency graph
					auto transform_update_phase = w.entity()
						.add(flecs::Phase)
						.depends_on(flecs::OnUpdate);

					auto replication_update_phase = w.entity()
						.add(flecs::Phase)
						.depends_on(transform_update_phase);

					//- first system of the module
					auto transform_update_system = w.system<stransform_component>("Transform System")
						.kind(transform_update_phase)
						.each([](stransform_component& trans)
							{
								core::crandom rand;

								trans.x += rand.normalized_float();
								trans.y -= rand.normalized_float();
								trans.r += rand.normalized_float();

								logging::log_info(fmt::format("[Transform] Updating transform [{}:{}:{}]", trans.x, trans.y, trans.r));
							});

					//- second system of the module
					auto replication_update_system = w.system<sreplicable_component, const stransform_component, const sidentifier_component>("Replication System")
						.kind(replication_update_phase)
						.each([](sreplicable_component& rep, const stransform_component& trans, const sidentifier_component& id)
							{
								logging::log_info(fmt::format("[Network] Replicating entity \"{}\": [{}:{}:{}]", id.uuid.view(), trans.x, trans.y, trans.r));
							});

					return { transform_update_system, {replication_update_system} };
				});
		}
	};

	//- Example module definition
	//------------------------------------------------------------------------------------------------------------------------
	class cexample_module : public imodule
	{
	public:
		cexample_module(flecs::world& w) : imodule(w)
		{
			begin<cexample_module>()
				//- .depends_on<cother_module>() //- this will load cother_module first and import it before loading this one
				->comp<sreplicable_component>()
				->comp<stransform_component>()
				->comp<sidentifier_component>()
				->subsystem<sexample_module_system>()
			->end<cexample_module>();
		}
	};
}