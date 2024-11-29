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
			for (auto i = 0; i < ctx->m_map_data.m_tilesets.size(); ++i)
			{
				const auto& tileset = ctx->m_map_data.m_tilesets.at(i);

				log_trace(fmt::format("Loading tileset: '{}'", tileset.m_source));

				ctx->m_processed_tileset_index = i;

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

			auto* sm = core::cservice_manager::find<sm::cspriteatlas_manager>();
			const auto& texture_names = ctx->m_map_mapping_data.at(ctx->m_name);

			//- Create an active world
			ecs::world::sconfig cfg;
			cfg.m_name = ctx->m_name;
			cfg.m_threads = engine::cthread_service::hardware_threads() / 2;

			ecs::cworld_manager::instance().create(cfg, true);
			auto& world = ecs::cworld_manager::instance().active();
			auto& em = world.em();

			//- Create camera entity
			{
				auto e = em.create_entity();

				e.add<ecs::scamera>();
				e.add<camera::tag::seditor_camera_controller>();

				auto* camera = e.get_mut<ecs::scamera>();
				camera->m_position = { 0.0f, 0.0f };
				camera->m_offset = { 0.0f, 0.0f };
				camera->m_zoom = 0.5f;
				camera->m_rotation = 0.0f;
			}

			//- Iterate map layers
			for (auto i = 0; i < ctx->m_map_data.m_layers.size(); ++i)
			{
				const auto& layer = ctx->m_map_data.m_layers.at(i);
				const auto current_layer_index = sm::create_layer();
				auto& current_sm_layer = sm::get_layer(current_layer_index);

				//- Specify default settings for spritemancer layer
				current_sm_layer.m_flags = sm::layer_flags_2d;
				current_sm_layer.m_show = true;

				for (auto j = 0; j < layer.m_chunks.size(); ++j)
				{
					const auto& layer_name = layer.m_name;
					const auto& chunk = layer.m_chunks.at(j);
					auto x = 0;
					auto y = 0;

					for (auto idx = 0; idx < chunk.m_data.size(); ++idx, ++x)
					{
						if (idx != 0 && idx % chunk.m_width == 0) //- check whether we have reached end of current line and should advance to next
						{
							x = 0;
							++y;
						}

						const auto& map_gid = chunk.m_data.at(idx);

						//- No entity at this location, continue
						if (map_gid == 0)
						{
							continue;
						}

						const auto& associated_layer_name = ctx->m_map_layer_mapping_data.at(ctx->m_name).at(map_gid);
						const auto& associated_layer = ctx->m_layer_info_data.at(ctx->m_name).at(associated_layer_name);

						//- Create actual entities that will populate the world
						auto e = em.create_entity();

						e.add<ecs::stransform>();
						e.add<ecs::smaterial>();
						e.add<ecs::ssprite_renderer>();
						e.add<ecs::shierarchy>();

						auto* transform = e.get_mut<ecs::stransform>();
						auto* material = e.get_mut<ecs::smaterial>();
						auto* sprite_renderer = e.get_mut<ecs::ssprite_renderer>();
						auto* hierarchy = e.get_mut<ecs::shierarchy>();

						const auto& atlas = sm->at(associated_layer.m_atlas_handle);
						const auto& subtexture_name = ctx->m_map_mapping_data.at(ctx->m_name).at(map_gid);
						const auto& subtexture_rect = atlas.at(subtexture_name);

						sprite_renderer->m_source_rect = subtexture_rect;
						sprite_renderer->m_layer = current_layer_index;
						sprite_renderer->m_origin = { associated_layer.m_tile_width, associated_layer.m_tile_height };

						material->m_program = sm::C_INVALID_HANDLE; //- use default shader
						material->m_texture = associated_layer.m_atlas_texture_handle;
						material->m_flags = sm::renderable_flag_origin_center;

						const auto positionx = x * associated_layer.m_tile_width;
						const auto positiony = y * associated_layer.m_tile_height;

						log_debug(fmt::format("Entity Position: '[{}:{}] = {}:{}'",
							x, y, positionx, positiony));

						transform->m_position = { positionx, positiony };
						transform->m_scale = { 1.0f, 1.0f };
						transform->m_rotation = 0.0f;
					}
				}
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