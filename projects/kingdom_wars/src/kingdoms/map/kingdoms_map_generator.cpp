#include "kingdoms_map_generator.hpp"
#include <engine.h>

RTTR_REGISTRATION
{
	using namespace kingdoms;
	using namespace kingdoms::detail;

	rttr::cregistrator<stiled_tileset_data>("stiled_tileset_data")
		.prop("m_name", &stiled_tileset_data::m_name)
		.prop("m_type", &stiled_tileset_data::m_type)
		.prop("m_image", &stiled_tileset_data::m_image)
		.prop("m_version", &stiled_tileset_data::m_version)
		.prop("m_tiled_version", &stiled_tileset_data::m_tiled_version)
		.prop("m_columns", &stiled_tileset_data::m_columns)
		.prop("m_image_height", &stiled_tileset_data::m_image_height)
		.prop("m_image_width", &stiled_tileset_data::m_image_width)
		.prop("m_margin", &stiled_tileset_data::m_margin)
		.prop("m_spacing", &stiled_tileset_data::m_spacing)
		.prop("m_tile_count", &stiled_tileset_data::m_tile_count)
		.prop("m_tile_height", &stiled_tileset_data::m_tile_height)
		.prop("m_tile_width", &stiled_tileset_data::m_tile_width)
		;

	rttr::cregistrator<stiled_tileset_reference>("stiled_tileset_reference")
		.prop("m_first_gid", &stiled_tileset_reference::m_first_gid)
		.prop("m_source", &stiled_tileset_reference::m_source)
		;

	rttr::cregistrator<stiled_map_chunk>("stiled_map_chunk")
		.prop("m_data", &stiled_map_chunk::m_data)
		.prop("m_height", &stiled_map_chunk::m_height)
		.prop("m_width", &stiled_map_chunk::m_width)
		.prop("m_x", &stiled_map_chunk::m_x)
		.prop("m_y", &stiled_map_chunk::m_y)
		;

	rttr::cregistrator<stiled_map_layer>("stiled_map_layer")
		.prop("m_chunks", &stiled_map_layer::m_chunks)
		.prop("m_name", &stiled_map_layer::m_name)
		.prop("m_type", &stiled_map_layer::m_type)
		.prop("m_id", &stiled_map_layer::m_id)
		.prop("m_height", &stiled_map_layer::m_height)
		.prop("m_width", &stiled_map_layer::m_width)
		.prop("m_x", &stiled_map_layer::m_x)
		.prop("m_y", &stiled_map_layer::m_y)
		.prop("m_opacity", &stiled_map_layer::m_opacity)
		.prop("m_startx", &stiled_map_layer::m_startx)
		.prop("m_starty", &stiled_map_layer::m_starty)
		.prop("m_visible", &stiled_map_layer::m_visible)
		;

	rttr::cregistrator<stiled_map_data>("stiled_map_data")
		.prop("m_layers", &stiled_map_data::m_layers)
		.prop("m_tilesets", &stiled_map_data::m_tilesets)
		.prop("m_orientation", &stiled_map_data::m_orientation)
		.prop("m_render_order", &stiled_map_data::m_render_order)
		.prop("m_tiled_version", &stiled_map_data::m_tiled_version)
		.prop("m_type", &stiled_map_data::m_type)
		.prop("m_version", &stiled_map_data::m_version)
		.prop("m_compression_level", &stiled_map_data::m_compression_level)
		.prop("m_next_layer_id", &stiled_map_data::m_next_layer_id)
		.prop("m_next_object_id", &stiled_map_data::m_next_object_id)
		.prop("m_width", &stiled_map_data::m_width)
		.prop("m_height", &stiled_map_data::m_height)
		.prop("m_tile_height", &stiled_map_data::m_tile_height)
		.prop("m_tile_width", &stiled_map_data::m_tile_width)
		.prop("m_infinite", &stiled_map_data::m_infinite)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cmap_manager>("cmap_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
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
		//- Expected is a json data string for the map file
		if (const auto var = core::io::from_json_blob(rttr::type::get<detail::stiled_map_data>(), (const byte_t*)string, size); var.is_valid())
		{
			const auto& data = var.get_value<detail::stiled_map_data>();

			log_debug(fmt::format("Map Data: '{}'", string_t((const char*)string, size)));
		}

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