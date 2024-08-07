#include "editor_project.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	cproject::cproject(stringview_t filepath)
	{
		if (const auto json = core::fs::load_text_from_file(filepath); json)
		{
			m_cfg = core::io::from_json_blob<cproject::sconfig>(json->data(), json->size());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cproject::cproject(sconfig&& cfg) : m_cfg(std::move(cfg))
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cproject::~cproject()
	{
	}

} //- editor

RTTR_REGISTRATION
{
	using namespace editor;

	rttr::cregistrator<cproject::sconfig>("cproject::sconfig")
		.prop("m_project_name", &cproject::sconfig::m_project_name)
		.prop("m_basepath",		&cproject::sconfig::m_basepath)
		;

	rttr::cregistrator<cproject, rttr::detail::no_default>("cproject")
		.ctor<rttr::detail::as_object, stringview_t>()
		.ctor<rttr::detail::as_object, cproject::sconfig&&>()
		.prop("m_cfg", &cproject::m_cfg)
		;
}