#include "module.hpp"

namespace io
{
	//------------------------------------------------------------------------------------------------------------------------
	io::cmodule& cmodule::name(stringview_t s)
	{
		m_name.assign(s.data());

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	stringview_t cmodule::name() const
	{
		return m_name;
	}

	//------------------------------------------------------------------------------------------------------------------------
	io::cmodule& cmodule::path_alias(stringview_t alias)
	{
		m_path_alias.assign(alias.data());

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	stringview_t cmodule::path_alias() const
	{
		return m_path_alias;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const vector_t<io::cmodule::sdefinition>& cmodule::definitions() const
	{
		return m_definitions;
	}

	//------------------------------------------------------------------------------------------------------------------------
	io::cmodule& cmodule::definition(const sdefinition& def)
	{
		m_definitions.emplace_back(def);

		return *this;
	}

} //- io

RTTR_REGISTRATION
{
	using namespace io;

	rttr::cregistrator<cmodule::sdefinition>("cmodule::sdefinition")
		.prop("m_directory", &cmodule::sdefinition::m_directory)
		.prop("m_resource_extension", &cmodule::sdefinition::m_resource_extension)
		.prop("m_asset_group", &cmodule::sdefinition::m_asset_group)
		.prop("m_recursive", &cmodule::sdefinition::m_recursive)
		.prop("m_dependencies", &cmodule::sdefinition::m_dependencies)
		;
	
	rttr::cregistrator<cmodule>("cmodule")
		.prop("m_name", &cmodule::m_name)
		.prop("m_path_alias", &cmodule::m_path_alias)
		.prop("m_definitions", &cmodule::m_definitions)
		.meth(core::cresource::C_DESTROY_FUNCTION_NAME.data(), &cmodule::destroy)
		.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS, vector_t<string_t>{".module"})
		;
}