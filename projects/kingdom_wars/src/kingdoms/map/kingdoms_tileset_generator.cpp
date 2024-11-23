#include "kingdoms_tileset_generator.hpp"
#include "../services/kingdoms_map_gid_name_mapping_service.hpp"
#include <engine.h>
#include <spritemancer.h>

namespace kingdoms
{
	//------------------------------------------------------------------------------------------------------------------------
	bool stileset_generator::load_from_file(map_generation_context_ref_t& ctx, stringview_t tilesetname)
	{
		auto result = true;

		auto* vfs = engine::cengine::service<fs::cvirtual_filesystem>();

		if (const auto file = vfs->open(tilesetname, core::file_mode_read); file && file->opened())
		{
			if (const auto mem = fs::load_text_from_file(file->info().path()); mem && !mem->empty())
			{
				return load_from_memory(ctx, (const uint8_t*)mem->data(), mem->size());
			}
			else
			{
				log_critical(fmt::format("Failed to read tileset JSON data at path '{}'", file->info().path()));
				result = false;
			}
		}
		else
		{
			log_critical(fmt::format("Failed to open tileset file at path '{}'", tilesetname.data()));
			result = false;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool stileset_generator::load_from_memory(map_generation_context_ref_t& ctx, const uint8_t* string, unsigned size)
	{
		//- TODO: we require some map data to generate correctly
		string_t map_name = "TODO";
		int map_gid = 0;

		auto result = true;

		//- Expected is a json data string for the map file
		if (const auto var = core::io::from_json_blob(rttr::type::get<tiled::stiled_tileset_data>(), (const byte_t*)string, size); var.is_valid())
		{
			const auto& data = var.get_value<tiled::stiled_tileset_data>();

			log_info(fmt::format("Loading Tiled tileset:"));
			log_info(fmt::format("\tName: '{}'", data.m_name));
			log_info(fmt::format("\tImage: '{}'", data.m_image));
			log_info(fmt::format("\tColumns: '{}'", data.m_columns));
			log_info(fmt::format("\tImage Height '{}':", data.m_image_height));
			log_info(fmt::format("\tImage Width: '{}'", data.m_image_width));
			log_info(fmt::format("\tTile Count: '{}'", data.m_tile_count));
			log_info(fmt::format("\tTile Height: '{}'", data.m_tile_height));
			log_info(fmt::format("\tTile Width: '{}'", data.m_tile_width));

			vector_t<string_t> names; names.reserve(data.m_tile_count);

			//- Resolve tileset texture data JSON file
			if (const auto info = fs::cfileinfo(string_utils::erase_numeric_in_path(data.m_image)); info.exists())
			{
				if (const auto mem = fs::load_text_from_file(info.path()); mem && !mem->empty())
				{
					if (const auto image_var = core::io::from_json_blob(rttr::type::get<tiled::stileset_image_data>(), (const byte_t*)mem->data(), mem->size()); image_var.is_valid())
					{
						const auto& image_data = image_var.get_value<tiled::stileset_image_data>();

						auto* nms = core::cservice_manager::find<cmap_gid_name_mapping_service>();

						for (const auto& tex : image_data.m_textures)
						{
							for (auto idx = 0; idx < tex.m_images.size(); ++idx)
							{
								const auto& img = tex.m_images.at(idx);

								nms->map(map_name, map_gid + idx, img.m_name);

								names.emplace_back(img.m_name);
							}
						}
					}
					else
					{
						result = false;
					}
				}
				else
				{
					log_error(fmt::format("Failed generating tileset because the JSON image data could not be loaded at '{}' for image '{}'", info.path(), data.m_image));
					result = false;
				}
			}
			else
			{
				log_error(fmt::format("Failed generating tileset because the JSON image data was not found for image '{}'", data.m_image));
				result = false;
			}

			auto* sm = core::cservice_manager::find<sm::cspriteatlas_manager>();

			auto spriteatlas_handle = sm->load_sync(data.m_name, data.m_image_width, data.m_image_height, names, { 0.0f, 0.0f });


		}

		return result;
	}

} //- kingdoms