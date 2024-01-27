#include "plugin_module_example_module.hpp"
#include <rttr/registration>


cclass<cexample_reflected_class>* reg_func()
{

	auto& object = *new cclass<cexample_reflected_class>("cexample_reflected_class");
	
	object.meth("some_func", &cexample_reflected_class::some_func)
		.prop("m_x", &cexample_reflected_class::m_x)
		.prop("m_y", &cexample_reflected_class::m_y);

// 	logging::log_debug(fmt::format("plugin_module_example RTTR Types:"));
// 	for (auto type : rttr::type::get_types())
// 	{
// 		logging::log_debug(fmt::format("\t'{}'", type.get_name().data()));
// 	}

	return &object;
}


namespace module_example
{
	RTTR_PLUGIN_REGISTRATION
	{
		using namespace rttr;

// 		registration::class_<cmy_module>("cmy_module")
// 			.constructor<flecs::world&>();
// 
// 		ECS_MODULE(cmy_module);

		registration::class_<cmy_module>("cmy_module")
			.constructor<flecs::world&>();

		registration::class_<cmy_second_module>("cmy_second_module")
			.constructor<flecs::world&>();

		ecs::cmodule_manager::register_type(rttr::type::get<cexample_reflected_class>());
		ecs::cmodule_manager::register_rttr_object(reg_func);

	};

} //- module_example