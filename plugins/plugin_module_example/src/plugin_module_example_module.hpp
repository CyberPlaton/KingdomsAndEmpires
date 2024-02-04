#pragma once
#include "plugin_module_example_common.hpp"
#include "components/example_components.hpp"
#include "systems/example_system.hpp"

namespace module_example
{
	//- Trying alternative registering modules and systems without requiring templates.
	//- Note that while we are registering our classes inside RTTR_PLUGIN_REGISTRATION
	//- we do not need to explicitly write EXAMPLE_API for exported classes
	//------------------------------------------------------------------------------------------------------------------------
	class cmy_module : public ecs::imodule<cmy_module>
	{
	public:
		cmy_module(flecs::world& w) : ecs::imodule<cmy_module>(w)
		{
			begin("My first module")
				.comp<stargeting_component>()
				.subsystem<cmy_system>()
			.end();
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cmy_second_module : public ecs::imodule<cmy_second_module>
	{
	public:
		cmy_second_module(flecs::world& w) : ecs::imodule<cmy_second_module>(w)
		{
			begin("Module some other dude made")
				.depends_on<cmy_module>()
				.comp<stargeting_component>()
				.subsystem<cmy_system>()
			.end();

			//- this is not the intended way to create entities
			auto test = w.entity("Walther");

			test.set<stargeting_component>({});
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cmy_third_module : public ecs::imodule<cmy_third_module>
	{
	public:
		cmy_third_module(flecs::world& w) : ecs::imodule<cmy_third_module>(w)
		{
			begin("My favorite module")
				.comp<sreplicable_component>()
				.comp<stransform_component>()
				.comp<sidentifier_component>()
				.subsystem<sexample_module_system>()
			.end();

			//- this is not the intended way to create entities
			auto e = w.entity("Manfred");

			e.set<stransform_component>({})
				.set<sreplicable_component>({})
				.set<sidentifier_component>({});

			auto* transform = e.get_mut<stransform_component>();
			transform->x = 24.0f;
			transform->y = 1.0f;
			transform->rotation = 11.0f;
		}
	};

} //- module_example