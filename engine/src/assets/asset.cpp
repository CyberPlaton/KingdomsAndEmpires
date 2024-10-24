#include "asset.hpp"

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	casset::casset(stringview_t source_path, rttr::type resource_type, asset_meta_t meta, rttr::variant options) :
		m_resource_type(resource_type), m_source_path(source_path.data()), m_meta(meta), m_options(options)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	casset::casset(stringview_t source_path, rttr::type resource_type) :
		m_resource_type(resource_type), m_source_path(source_path.data())
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	casset::casset() :
		m_resource_type(rttr::type::get<void>())
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	stringview_t casset::source_path() const
	{
		return m_source_path;
	}

	//------------------------------------------------------------------------------------------------------------------------
	rttr::type casset::resource_type() const
	{
		return m_resource_type;
	}

} //- engine

RTTR_REGISTRATION
{
	using namespace engine;

	rttr::cregistrator<casset>("casset")
		.prop("m_source_path", &casset::m_source_path)
		.prop("m_resource_type", &casset::m_resource_type)
		.prop("m_options", &casset::m_options)
		.prop("m_meta", &casset::m_meta)
		;
	
	rttr::detail::default_constructor<umap_t<string_t, rttr::variant>>();
}