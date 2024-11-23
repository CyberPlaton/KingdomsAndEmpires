#include "kingdoms_map_generator.hpp"
#include <engine.h>
#include "tiled_integration/kingdoms_tiled_tileset.hpp"

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
		//- Find directory with given name and the map file with naming convention:
		//- <name>_map.tmj
		//- Tilesets and used source images are resolved using map data.

		if (const auto* vfs = engine::cengine::service<fs::cvirtual_filesystem>(); vfs)
		{
			fs::cfileinfo info(name);

			auto directory = info.filename().string();

			if (auto filesystem = vfs->find_filesystem("/maps/"); filesystem)
			{
				for (const auto& file : filesystem->files())
				{
					if (file->info().ext().compare(".tmj") == 0)
					{
						if (const auto mem = fs::load_text_from_file(file->info().path()); mem && !mem->empty())
						{
							return load_from_memory((const uint8_t*)mem->data(), mem->size());
						}
					}
				}
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmap::load_from_memory(const uint8_t* string, unsigned size)
	{
		auto result = true;

		//- Expected is a json data string for the map file
		if (const auto var = core::io::from_json_blob(rttr::type::get<tiled::stiled_map_data>(), (const byte_t*)string, size); var.is_valid())
		{
			m_data = var.get_value<tiled::stiled_map_data>();

			log_debug(fmt::format("Map Data: '{}'", string_t((const char*)string, size)));

			//- Resolve tileset references and request them to be loaded
			for (const auto& tileset : m_data.m_tilesets)
			{
				//- Load the tileset texture and create a tileset
				if (!stileset_generator::load_from_file(tileset.m_source))
				{
					result = false;
				}
			}

			//- Iterate map layers
			for (const auto& layer : m_data.m_layers)
			{

			}
		}

		return result;
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