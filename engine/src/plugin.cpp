#include "plugin.hpp"

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	cplugin::cplugin(stringview_t filename) : m_library(filename.data())
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cplugin::cplugin(sinfo&& info) :
		m_info(std::move(info)), m_library(m_info.m_path)
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
	rttr::array_range<rttr::type> cplugin::types() const
	{
		return m_library.get_types();
	}

	//------------------------------------------------------------------------------------------------------------------------
	rttr::array_range<rttr::property> cplugin::props() const
	{
		return m_library.get_global_properties();
	}

	//------------------------------------------------------------------------------------------------------------------------
	rttr::array_range<rttr::method> cplugin::methods() const
	{
		return m_library.get_global_methods();
	}

} //- engine

RTTR_REGISTRATION
{
	using namespace engine;

	rttr::cregistrator<cplugin::sinfo>("cplugin::sinfo")
		.prop("m_name", &cplugin::sinfo::m_name)
		.prop("m_path", &cplugin::sinfo::m_path)
		.prop("m_desc", &cplugin::sinfo::m_desc)
		.prop("m_dependencies", &cplugin::sinfo::m_dependencies)
		.prop("m_version", &cplugin::sinfo::m_version)
		;
}