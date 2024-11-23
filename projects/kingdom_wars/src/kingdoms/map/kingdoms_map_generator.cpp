#include "kingdoms_map_generator.hpp"
#include <engine.h>
#include "kingdoms_tileset_generator.hpp"

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
	cmap::cmap(map_generation_context_ref_t& ctx, stringview_t mapname)
	{
		load_from_file(ctx, mapname);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmap::cmap(map_generation_context_ref_t& ctx, const uint8_t* string, unsigned size)
	{
		load_from_memory(ctx, string, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmap::~cmap()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmap::load_from_file(map_generation_context_ref_t& ctx, stringview_t name)
	{
		//- Find directory with given name and the map file '.tmj' Tile map extension.
		//- Tilesets and used source images are resolved using map data.
		if (const auto* vfs = engine::cengine::service<fs::cvirtual_filesystem>(); vfs)
		{
			fs::cfileinfo info(name);

			//- Extract directory name and try to resolve the map directory in default maps folder
			auto directory = info.filename().string();

			if (auto filesystem = vfs->find_filesystem(sgame::C_MAP_DATA_FILEPATH); filesystem)
			{
				for (const auto& file : filesystem->files())
				{
					if (file->info().ext().compare(sgame::C_MAP_DATA_EXT) == 0)
					{
						//- Found the map file. Store data for context and proceed loading
						ctx->m_name = file->info().stem();
						ctx->m_path = file->info().path();
						ctx->m_relative_path = file->info().relative();

						if (const auto mem = fs::load_text_from_file(file->info().path()); mem && !mem->empty())
						{
							return load_from_memory(ctx, (const uint8_t*)mem->data(), mem->size());
						}
					}
				}
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmap::load_from_memory(map_generation_context_ref_t& ctx, const uint8_t* string, unsigned size)
	{
		auto result = true;

		//- Expected is a json data string for the map file
		if (const auto var = core::io::from_json_blob(rttr::type::get<tiled::stiled_map_data>(), (const byte_t*)string, size); var.is_valid())
		{
			ctx->m_map_data = var.get_value<tiled::stiled_map_data>();

			//- Resolve tileset references and request them to be loaded
			for (const auto& tileset : ctx->m_map_data.m_tilesets)
			{
				log_trace(fmt::format("Loading tileset: '{}'", tileset.m_source));

				//- Load the tileset texture and create a tileset. For the tileset path, we take the name and resolve it in same directory
				//- as we are in
				auto filename = fs::cfileinfo(tileset.m_source).stem();

				auto tileset_path = string_utils::join(string_utils::join(sgame::C_MAP_DATA_FILEPATH.data(), fmt::format("{}/", ctx->m_name)),
					fmt::format("{}{}", filename, sgame::C_TILESET_DATA_EXT));

				if (!stileset_generator::load_from_file(ctx, tileset_path))
				{
					result = false;
				}
			}

			//- Iterate map layers
			for (const auto& layer : ctx->m_map_data.m_layers)
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
		auto ctx = std::make_shared<detail::smap_generation_context>();

		return load_of_sync<bool>(name, m_data, ctx, mapname);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmap_manager::load_sync(stringview_t name, const uint8_t* string, unsigned size)
	{
		auto ctx = std::make_shared<detail::smap_generation_context>();

		return load_of_sync<bool>(name, m_data, ctx, string, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<bool> cmap_manager::load_async(stringview_t name, stringview_t mapname)
	{
		auto ctx = std::make_shared<detail::smap_generation_context>();

		return load_of_async<bool>(name, m_data, ctx, mapname);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<bool> cmap_manager::load_async(stringview_t name, const uint8_t* string, unsigned size)
	{
		auto ctx = std::make_shared<detail::smap_generation_context>();

		return load_of_async<bool>(name, m_data, ctx, string, size);
	}

} //- kingdoms