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
	class cmy_module : public ecs::imodule
	{
	public:
		cmy_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<cmy_module>("My first module")
				.comp<cmy_module, stargeting_component>()
				.subsystem<cmy_module, cmy_system>()
			.end<cmy_module>();
		}

		RTTR_ENABLE(ecs::imodule);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cmy_second_module : public ecs::imodule
	{
	public:
		cmy_second_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<cmy_second_module>("Module some other dude made")
				.depends_on<cmy_second_module, cmy_module>()
				.comp<cmy_second_module, stargeting_component>()
				.subsystem<cmy_second_module, cmy_system>()
				.subsystem<cmy_second_module, cmy_second_system>()
				.subsystem<cmy_second_module, cmy_third_system>()
			.end<cmy_second_module>();

			//- this is not the intended way to create entities
			auto test = w.entity("Walther");

			test.set<stargeting_component>({});
		}

		RTTR_ENABLE(ecs::imodule);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cmy_third_module : public ecs::imodule
	{
	public:
		cmy_third_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<cmy_third_module>("My favorite module")
				.comp<cmy_third_module, sreplicable_component>()
				.comp<cmy_third_module, stransform_component>()
				.comp<cmy_third_module, sidentifier_component>()
				.subsystem<cmy_third_module, sexample_module_system>()
			.end<cmy_third_module>();

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

		RTTR_ENABLE(ecs::imodule);
	};

} //- module_example

namespace module_example
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cmy_module)
	{
		rttr::registration::class_<cmy_module>("cmy_module")
			.constructor<flecs::world&>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cmy_second_module)
	{
		rttr::registration::class_<cmy_second_module>("cmy_second_module")
			.constructor<flecs::world&>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cmy_third_module)
	{
		rttr::registration::class_<cmy_third_module>("cmy_third_module")
			.constructor<flecs::world&>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

} //- module_example