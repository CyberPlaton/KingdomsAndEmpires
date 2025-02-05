#include "kingdoms_tileset_generator.hpp"
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
		auto result = false;

		const auto& map_name = ctx->m_name;
		const auto& map_first_gid = ctx->m_map_data.m_tilesets.at(ctx->m_processed_tileset_index).m_first_gid;

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

			//- We have two possibilities, either we load a tileset created with 'crunch' that has associated JSON data with it,
			//- or we load an externally created tileset without any data associated with it.
			//- In the first case we map the name from JSON to maps identifiers, in the second case we map the maps identifier
			//- string versions to map identifiers..

			auto* vfs = core::cservice_manager::find<fs::cvirtual_filesystem>();
			auto* sm = core::cservice_manager::find<sm::cspriteatlas_manager>();

			vector_t<string_t> subtexture_names;
			auto tile_count_x = data.m_image_width / data.m_tile_width;
			auto tile_count_y = data.m_image_height / data.m_tile_height;
			auto crunch_tileset = false;

			//- Cache layer information to be cached
			detail::smap_generation_context::slayer_information layer_info;
			layer_info.m_name = data.m_name;
			layer_info.m_tile_width = data.m_tile_width;
			layer_info.m_tile_height = data.m_tile_height;
			layer_info.m_tile_count_x = tile_count_x;
			layer_info.m_tile_count_y = tile_count_y;

			//- Resolve tileset texture data JSON file
			auto stripped_path = string_utils::erase_numeric_in_path(data.m_image);
			const auto image_ext = fs::cfileinfo(stripped_path).extension().string();
			string_utils::erase_substr(stripped_path, image_ext);
			stripped_path = string_utils::join(stripped_path, ".json");

			if (const auto file = vfs->open({ stripped_path }, core::file_mode_read); file && file->opened())
			{
				//- Tileset created using 'crunch'. Resolve JSON data and proceed.
				if (const auto mem = fs::load_text_from_file(file->info().path()); mem && !mem->empty())
				{
					if (const auto image_var = core::io::from_json_blob(rttr::type::get<tiled::stileset_image_data>(), (const byte_t*)mem->data(), mem->size()); image_var.is_valid())
					{
						const auto& image_data = image_var.get_value<tiled::stileset_image_data>();

						//- Manually generate spriteatlas from crunch data
						sm::cspriteatlas atlas;

						auto atlas_width = MIN(int);
						auto atlas_height = MIN(int);

						for (const auto& tex : image_data.m_textures)
						{
							for (auto idx = 0; idx < tex.m_images.size(); ++idx)
							{
								const auto& img = tex.m_images.at(idx);

								const auto id = map_first_gid + idx;

								ctx->m_map_layer_mapping_data[map_name][id] = layer_info.m_name;
								ctx->m_map_mapping_data[map_name][id] = img.m_name;

								const auto x = (float)img.m_x / data.m_image_width;
								const auto y = (float)img.m_y / data.m_image_height;
								const auto w = (float)img.m_width / data.m_image_width;
								const auto h = (float)img.m_height / data.m_image_height;

								atlas.subtexture(img.m_name, {x, y, w, h});

								atlas_width = glm::max(atlas_width, img.m_x + img.m_width);
								atlas_height = glm::max(atlas_height, img.m_y + img.m_height);
							}
						}

						atlas.begin(atlas_width, atlas_height);
						atlas.end();

						const auto spriteatlas_handle = sm->load_sync(data.m_name, atlas);

						if (!sm::is_valid(spriteatlas_handle))
						{
							//- TODO: Erase the last created mappings, as the atlas was not valid
							log_error(fmt::format("Failed generating spriteatlas from 'crunch' tileset data at '{}', because the spriteatlas failed to load!", data.m_name));
							result = false;
						}

						layer_info.m_atlas_handle = spriteatlas_handle;
						crunch_tileset = true;
					}
					else
					{
						result = false;
					}
				}
				else
				{
					log_error(fmt::format("Failed generating tileset because the JSON image data could not be loaded at '{}' for image '{}'", stripped_path, data.m_image));
					result = false;
				}
			}
			else
			{
				//- Tileset created using external tools. Proceed without additional data.
				string_t tile_name; tile_name.reserve(32);

				auto gid = map_first_gid;

				for (auto i = 0; i < tile_count_x; ++i)
				{
					for (auto j = 0; j < tile_count_y; ++j)
					{
						const auto id = gid++;

						ctx->m_map_layer_mapping_data[map_name][id] = layer_info.m_name;
						ctx->m_map_mapping_data[map_name][id] = fmt::to_string(id);
						subtexture_names.emplace_back(fmt::to_string(id));
					}
				}
			}

			if(!crunch_tileset)
			{
				const auto spriteatlas_handle = sm->load_sync(data.m_name, data.m_image_width, data.m_image_height, subtexture_names,
					{ (float)tile_count_x, (float)tile_count_y });

				if (!sm::is_valid(spriteatlas_handle))
				{
					//- TODO: Erase the last created mappings, as the atlas was not valid
					log_error(fmt::format("Failed generating spriteatlas from Tiled tileset data at '{}', because the spriteatlas failed to load!", data.m_name));
					result = false;
				}

				layer_info.m_atlas_handle = spriteatlas_handle;
			}

			auto* tm = core::cservice_manager::find<sm::ctexture_manager>();

			if (const auto file = vfs->open({ data.m_image }, core::file_mode_read); file && file->opened())
			{
				const auto texture_handle = tm->load_sync(file->info().stem(), file->info().path());

				if (!sm::is_valid(texture_handle))
				{
					//- TODO: Erase the last created spriteatals and mappings, as the image was not valid
					log_error(fmt::format("Failed generating spriteatlas from Tiled tileset data at '{}', because the image '{}' failed to load", data.m_name, data.m_image));
					result = false;
				}

				layer_info.m_atlas_texture_handle = texture_handle;
			}
			else
			{
				log_error(fmt::format("Failed generating spriteatlas from Tiled tileset data at '{}', because the image file '{}' could not be found", data.m_name, data.m_image));
				result = false;
			}

			ctx->m_layer_info_data[ctx->m_name][layer_info.m_name] = std::move(layer_info);

			result = true;
		}

		if (result)
		{
			log_info(fmt::format("Successfully generated tilset for '{}'", ctx->m_name));
		}

		return result;
	}

} //- kingdoms