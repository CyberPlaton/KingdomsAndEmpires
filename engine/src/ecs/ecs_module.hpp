#pragma once
#include <core.h>

//- 
//-------------------------------------------------------------------------------------------------------------------------
#define DECLARE_MODULE(m) \
m() = default; \
m(ecs::cworld* w) { w->import_module<m>(config()); }

//- Use macro to reflect your module, the module functions must be declared and implemented.
//-------------------------------------------------------------------------------------------------------------------------
#define REGISTER_MODULE(m) \
	rttr::cregistrator<m>(STRINGIFY(m)) \
		.ctor<ecs::cworld*>() \
		.meth(ecs::modules::smodule::C_MODULE_CONFIG_FUNC_NAME, &m::config) \
		;


namespace ecs
{
	namespace modules
	{
		//-------------------------------------------------------------------------------------------------------------------------
		struct sconfig final
		{
			string_t m_name;
			vector_t<string_t> m_components;//- Components that this module is using
			vector_t<string_t> m_systems;	//- Systems that this module is using
			vector_t<string_t> m_modules;	//- Dependent modules that must be loaded before this one
		};

		//- This is a dummy module to show how one should be defined. While creating one you must not inherit from it.
		//- Define all required functions and reflect them to RTTR using the macro REGISTER_MODULE().
		//------------------------------------------------------------------------------------------------------------------------
		struct smodule final
		{
			static constexpr rttr::string_view C_MODULE_CONFIG_FUNC_NAME		= "config";
			static constexpr array_t<rttr::string_view, 1> C_MODULE_FUNC_NAMES	= { C_MODULE_CONFIG_FUNC_NAME };

			static sconfig config() { return {}; }

			RTTR_ENABLE();
		};

	} //-- modules

} //- ecs