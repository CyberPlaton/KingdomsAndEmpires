#include "kingdoms_module.hpp"
#include "kingdoms_common.hpp"
#include "kingdoms_layer_ui.hpp"
#include "kingdoms/map/tiled_integration/kingdoms_tiled_map.hpp"
#include "kingdoms/map/tiled_integration/kingdoms_tiled_tileset.hpp"
#include <engine.h>

namespace kingdoms
{
	namespace
	{
		sgame_context S_CTX;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool ckingdoms::init()
	{
		bool result = true;

		//- Create virtual file paths for commonly accessed resources
		auto* vfs = engine::cengine::service<fs::cvirtual_filesystem>();

		const auto& fs = vfs->find_filesystem("/");
		const auto path = fs->base_path();

		auto races_fs = std::make_shared<io::cnative_filesystem>();
		auto maps_fs = std::make_shared<io::cnative_filesystem>();

		const auto races_path = string_utils::join(path, sgame::C_RACE_DATA_FILEPATH.data());
		const auto maps_path = string_utils::join(path, sgame::C_MAP_DATA_FILEPATH.data());

		if (races_fs->init(races_path, sgame::C_RACE_DATA_FILEPATH) &&
			maps_fs->init(maps_path, sgame::C_MAP_DATA_FILEPATH))
		{
			vfs->add_filesystem(sgame::C_RACE_DATA_FILEPATH, races_fs);
			vfs->add_filesystem(sgame::C_MAP_DATA_FILEPATH, maps_fs);
		}
		else
		{
			result = false;
		}

		//- Create resource managers
		core::cservice_manager::emplace<cmap_manager>();

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms::shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms::on_update(float dt)
	{
		S_CTX.m_command_system.on_update(S_CTX, dt);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms::on_world_render()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms::on_ui_render()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms::on_post_update(float dt)
	{

	}

} //- kingdoms

RTTR_REGISTRATION
{
	using namespace kingdoms;
	using namespace kingdoms::tiled;

	//------------------------------------------------------------------------------------------------------------------------
	REGISTER_LAYER(ckingdoms);
	REGISTER_LAYER(ckingdoms_layer_ui);

	//------------------------------------------------------------------------------------------------------------------------
	rttr::detail::default_constructor<vector_t<size_t>>();
	rttr::detail::default_constructor<vector_t<stiled_map_layer>>();
	rttr::detail::default_constructor<vector_t<stiled_map_chunk>>();
	rttr::detail::default_constructor<vector_t<stiled_tileset_reference>>();

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<stiled_tileset_reference>("stiled_tileset_reference")
		.prop("firstgid", &stiled_tileset_reference::m_first_gid)
		.prop("source", &stiled_tileset_reference::m_source)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<stiled_map_chunk>("stiled_map_chunk")
		.prop("data", &stiled_map_chunk::m_data)
		.prop("height", &stiled_map_chunk::m_height)
		.prop("width", &stiled_map_chunk::m_width)
		.prop("x", &stiled_map_chunk::m_x)
		.prop("y", &stiled_map_chunk::m_y)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<stiled_map_layer>("stiled_map_layer")
		.prop("chunks", &stiled_map_layer::m_chunks)
		.prop("name", &stiled_map_layer::m_name)
		.prop("type", &stiled_map_layer::m_type)
		.prop("id", &stiled_map_layer::m_id)
		.prop("height", &stiled_map_layer::m_height)
		.prop("width", &stiled_map_layer::m_width)
		.prop("x", &stiled_map_layer::m_x)
		.prop("y", &stiled_map_layer::m_y)
		.prop("opacity", &stiled_map_layer::m_opacity)
		.prop("startx", &stiled_map_layer::m_startx)
		.prop("starty", &stiled_map_layer::m_starty)
		.prop("visible", &stiled_map_layer::m_visible)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<stiled_map_data>("stiled_map_data")
		.prop("layers",				&stiled_map_data::m_layers)
		.prop("tilesets",			&stiled_map_data::m_tilesets)
		.prop("orientation",		&stiled_map_data::m_orientation)
		.prop("renderorder",		&stiled_map_data::m_render_order)
		.prop("tiledversion",		&stiled_map_data::m_tiled_version)
		.prop("type",				&stiled_map_data::m_type)
		.prop("version",			&stiled_map_data::m_version)
		.prop("compressionlevel",	&stiled_map_data::m_compression_level)
		.prop("nextlayerid",		&stiled_map_data::m_next_layer_id)
		.prop("nextobjectid",		&stiled_map_data::m_next_object_id)
		.prop("width",				&stiled_map_data::m_width)
		.prop("height",				&stiled_map_data::m_height)
		.prop("tileheight",			&stiled_map_data::m_tile_height)
		.prop("tilewidth",			&stiled_map_data::m_tile_width)
		.prop("infinite",			&stiled_map_data::m_infinite)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<stiled_tileset_data>("stiled_tileset_data")
		.prop("name", &stiled_tileset_data::m_name)
		.prop("type", &stiled_tileset_data::m_type)
		.prop("image", &stiled_tileset_data::m_image)
		.prop("version", &stiled_tileset_data::m_version)
		.prop("tiledversion", &stiled_tileset_data::m_tiled_version)
		.prop("columns", &stiled_tileset_data::m_columns)
		.prop("imageheight", &stiled_tileset_data::m_image_height)
		.prop("imagewidth", &stiled_tileset_data::m_image_width)
		.prop("margin", &stiled_tileset_data::m_margin)
		.prop("spacing", &stiled_tileset_data::m_spacing)
		.prop("tilecount", &stiled_tileset_data::m_tile_count)
		.prop("tileheight", &stiled_tileset_data::m_tile_height)
		.prop("tilewidth", &stiled_tileset_data::m_tile_width)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<stileset_image_data::simages>("stileset_image_data::simages")
		.prop("name", &stileset_image_data::simages::m_name)
		.prop("x", &stileset_image_data::simages::m_x)
		.prop("y", &stileset_image_data::simages::m_y)
		.prop("w", &stileset_image_data::simages::m_width)
		.prop("h", &stileset_image_data::simages::m_height)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<stileset_image_data::stextures>("stileset_image_data::stextures")
		.prop("name", &stileset_image_data::stextures::m_name)
		.prop("images", &stileset_image_data::stextures::m_images)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<stileset_image_data>("stileset_image_data")
		.prop("textures", &stileset_image_data::m_textures)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cmap_manager>("cmap_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}