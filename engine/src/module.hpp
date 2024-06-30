#pragma once
#include <core.h>

namespace io
{
	//- This is a dummy module to show how one should be defined. While creating one youu must not inherit from it.
	//- Remember to define all functions as shown below and reflect the using the macro REGISTER_MODULE()
	//------------------------------------------------------------------------------------------------------------------------
	class cmodule final
	{
	public:
		struct sdefinition
		{
			string_t m_name;
			string_t m_path_alias;

			RTTR_ENABLE();
		};

		static constexpr std::string_view C_MODULE_ON_LOADING_FUNC_NAME		= "on_loading";
		static constexpr std::string_view C_MODULE_ON_UNLOADING_FUNC_NAME	= "on_unloading";
		static constexpr std::string_view C_MODULE_STATUS_FUNC_NAME			= "status";
		static constexpr std::string_view C_MODULE_DEFINITION_FUNC_NAME		= "definition";
		static constexpr array_t<std::string_view, 4> C_MODULE_FUNC_NAMES	= { C_MODULE_ON_LOADING_FUNC_NAME, C_MODULE_ON_UNLOADING_FUNC_NAME,
																				C_MODULE_STATUS_FUNC_NAME, C_MODULE_DEFINITION_FUNC_NAME };

		static bool					on_loading(const sdefinition& definition) { return false; }
		static void					on_unloading() {}
		static core::module_status	status() { return core::module_status_none; }
		static const sdefinition&	definition() { return {}; }
	};

} //- io

//- Use this macro to reflect you module, the module functions must be declared and implemented
//------------------------------------------------------------------------------------------------------------------------
#define REGISTER_MODULE(_module) \
REFLECT_INLINE(_module) \
{ \
	rttr::cregistrator<_module>(STRINGIFY(layer)) \
		.meth(io::cmodule::C_MODULE_ON_LOADING_FUNC_NAME,		&_module::on_loading) \
		.meth(io::cmodule::C_MODULE_ON_UNLOADING_FUNC_NAME,		&_module::on_unloading) \
		.meth(io::cmodule::C_MODULE_STATUS_FUNC_NAME,			&_module::status) \
		.meth(io::cmodule::C_MODULE_DEFINITION_FUNC_NAME,		&_module::definition) \
		; \
}