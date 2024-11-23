#include "kingdoms_map_gid_name_mapping_service.hpp"

namespace kingdoms
{
	//------------------------------------------------------------------------------------------------------------------------
	cmap_gid_name_mapping_service::~cmap_gid_name_mapping_service()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmap_gid_name_mapping_service::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmap_gid_name_mapping_service::on_shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmap_gid_name_mapping_service::on_update(float dt)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	cmap_gid_name_mapping_service& cmap_gid_name_mapping_service::map(stringview_t mapname, int gid, stringview_t texturename)
	{
		m_mappings[mapname][gid] = texturename;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const umap_t<int, stringview_t>& cmap_gid_name_mapping_service::map_names(stringview_t mapname) const
	{
		return m_mappings.at(mapname);
	}

	//------------------------------------------------------------------------------------------------------------------------
	stringview_t cmap_gid_name_mapping_service::texture_name(stringview_t mapname, int gid) const
	{
		return map_names(mapname).at(gid);
	}

} //- kingdoms