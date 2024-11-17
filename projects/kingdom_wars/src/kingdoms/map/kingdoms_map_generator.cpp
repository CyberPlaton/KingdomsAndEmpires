#include "kingdoms_map_generator.hpp"
#include <engine.h>

RTTR_REGISTRATION
{
	using namespace kingdoms;
	using namespace kingdoms::detail;

	rttr::cregistrator<stiled_tileset_data>("stiled_tileset_data")
		;

	rttr::cregistrator<stiled_tileset_reference>("stiled_tileset_reference")
		;

	rttr::cregistrator<stiled_map_chunk>("stiled_map_chunk")
		;

	rttr::cregistrator<stiled_map_layer>("stiled_map_layer")
		;

	rttr::cregistrator<stiled_map_data>("stiled_map_data")
		;
}


namespace kingdoms
{
	//------------------------------------------------------------------------------------------------------------------------
	void cmap::destroy(cmap& map)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmap::cmap()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	cmap::cmap(stringview_t mapname)
	{
		load_from_file(mapname);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmap::cmap(const uint8_t* string, unsigned size)
	{
		load_from_memory(string, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmap::~cmap()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmap::load_from_file(stringview_t name)
	{
		//- Find directory with given name and the map file with namin convention:
		//- <name>_map.tmj
		//- Tilesets and used source images are resolved using map data.

		if (const auto* vfs = engine::cengine::service<fs::cvirtual_filesystem>(); vfs)
		{

		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmap::load_from_memory(const uint8_t* string, unsigned size)
	{
		//- Expected is a json data string for the map file

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmap_manager::cmap_manager()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	cmap_manager::~cmap_manager()
	{
		destroy_all(m_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmap_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmap_manager::on_shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmap_manager::on_update(float)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmap_manager::load_sync(stringview_t name, stringview_t mapname)
	{
		return load_of_sync<bool>(name, m_data, mapname);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmap_manager::load_sync(stringview_t name, const uint8_t* string, unsigned size)
	{
		return load_of_sync<bool>(name, m_data, string, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<bool> cmap_manager::load_async(stringview_t name, stringview_t mapname)
	{
		return load_of_async<bool>(name, m_data, mapname);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<bool> cmap_manager::load_async(stringview_t name, const uint8_t* string, unsigned size)
	{
		return load_of_async<bool>(name, m_data, string, size);
	}

} //- kingdoms