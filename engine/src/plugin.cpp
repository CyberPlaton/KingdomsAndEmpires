#include "plugin.hpp"

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	cplugin::cplugin(stringview_t filename) : m_library(filename.data())
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cplugin::cplugin(sconfig&& cfg) :
		m_cfg(std::move(cfg)), m_library(m_cfg.m_path)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cplugin::~cplugin()
	{
		if (m_library.is_loaded())
		{
			unload();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cplugin::load()
	{
		return m_library.load();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cplugin::unload()
	{
		m_library.unload();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cplugin::types_t cplugin::types() const
	{
		return m_library.get_types();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cplugin::props_t cplugin::props() const
	{
		return m_library.get_global_properties();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cplugin::methods_t cplugin::methods() const
	{
		return m_library.get_global_methods();
	}

} //- engine

RTTR_REGISTRATION
{
	using namespace engine;

	rttr::cregistrator<cplugin::sconfig>("cplugin::sconfig")
		.prop("m_name",			&cplugin::sconfig::m_name)
		.prop("m_path",			&cplugin::sconfig::m_path)
		.prop("m_desc",			&cplugin::sconfig::m_desc)
		.prop("m_dependencies", &cplugin::sconfig::m_dependencies)
		.prop("m_version",		&cplugin::sconfig::m_version)
		;
}