#include "plugin_rttr_example_module.hpp"

RTTR_PLUGIN_REGISTRATION
{
	rttr::cregistrator<smy_class>("smy_class")
		.meth("function", &smy_class::function)
		.prop("value", &smy_class::value)
		.prop("name", &smy_class::name)
		.prop("data", &smy_class::data)
		;
}