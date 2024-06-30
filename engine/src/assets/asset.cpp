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