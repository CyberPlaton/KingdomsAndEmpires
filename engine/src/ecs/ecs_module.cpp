#include "ecs_module.hpp"
#include <plugin_logging.h>

namespace ecs
{
	namespace detail
	{

		void cmodule_database::module_add(smodule_info& info)
		{
			auto h = algorithm::hash(info.m_name);

			m_modules[h] = std::move(info);
		}

	} //- detail

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

	//------------------------------------------------------------------------------------------------------------------------
	class sexample_module_system : public csystem
	{
	public:
		sexample_module_system(flecs::world& w) :
			csystem(w)
		{
			//- use constructor only to define the system
			subsystem([&](flecs::world& w)
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
				});
		}
	};

	//- This module definition is outdated. See ecs_module.hpp
	//------------------------------------------------------------------------------------------------------------------------
	class cexample_module : public imodule<cexample_module>
	{
		cexample_module(flecs::world& w) :
			imodule(w)
		{
			begin_module()
				//- .depends_on<cother_module>()
				.component<sreplicable_component>()
				.component<stransform_component>()
				.component<sidentifier_component>()
				.subsystem<sexample_module_system>()
			.end_module();
		}

	private:
		flecs::entity m_module;
	};
}