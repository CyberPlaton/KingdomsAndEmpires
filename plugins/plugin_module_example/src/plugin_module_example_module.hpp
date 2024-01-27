#pragma once
#include "plugin_module_example_common.hpp"
#include "components/example_components.hpp"
#include "systems/example_system.hpp"

class cexample_reflected_class
{
public:
	cexample_reflected_class() = default;

	void some_func()
	{
		logging::log_trace("Hello World, so complicated");
	}

	int m_x;
	float m_y;
};

namespace module_example
{
	//- Trying alternative registering modules and systems without requiring templates.
	//- Note that while we are registering our classes inside RTTR_PLUGIN_REGISTRATION
	//- we do not need to explicitly write EXAMPLE_API for exported classes
	//------------------------------------------------------------------------------------------------------------------------
	class EXAMPLE_API cmy_module : public ecs::imodule
	{
	public:
		cmy_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<cmy_module>()
				->comp<stargeting_component>()
				->subsystem<cmy_system>()
			->end();
		};

		RTTR_ENABLE(ecs::imodule);
	};

	class EXAMPLE_API cmy_second_module : public ecs::imodule
	{
	public:
		cmy_second_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<cmy_second_module>()
				->comp<stargeting_component>()
				->subsystem<cmy_system>()
				->end();
		};

		RTTR_ENABLE(ecs::imodule);
	};

} //- module_example