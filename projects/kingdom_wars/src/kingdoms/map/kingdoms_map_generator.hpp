#pragma once
#include <core.h>

namespace kingdoms
{
	namespace detail
	{
		//- Tileset data imported from Tiled as json with fileextension .tsj.
		//------------------------------------------------------------------------------------------------------------------------
		struct stiled_tileset_data final
		{
			string_t m_name;
			string_t m_type;
			string_t m_image;
			string_t m_version;
			string_t m_tiled_version;
			unsigned m_columns = 0;
			unsigned m_image_height = 0;
			unsigned m_image_width = 0;
			unsigned m_margin = 0;
			unsigned m_spacing = 0;
			unsigned m_tile_count = 0;
			unsigned m_tile_height = 0;
			unsigned m_tile_width = 0;

			RTTR_ENABLE();
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct stiled_tileset_reference final
		{
			unsigned m_first_gid = 0;
			string_t m_source;

			RTTR_ENABLE();
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct stiled_map_chunk final
		{
			vector_t<size_t> m_data;
			unsigned m_height = 0;
			unsigned m_width = 0;
			int m_x = 0; //- Origin of the top-left tile. Should probably convert from negative space to positive.
			int m_y = 0;

			RTTR_ENABLE();
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct stiled_map_layer final
		{
			vector_t<stiled_map_chunk> m_chunks;
			string_t m_name;
			string_t m_type;
			int m_id = 0;
			unsigned m_height = 0;
			unsigned m_width = 0;
			int m_x = 0;
			int m_y = 0;
			int m_opacity = 0;
			int m_startx = 0;
			int m_starty = 0;
			bool m_visible = true;

			RTTR_ENABLE();
		};

		//- Map data imported from Tiled program as json with fileextension .tmj.
		//------------------------------------------------------------------------------------------------------------------------
		struct stiled_map_data final
		{
			vector_t<stiled_map_layer> m_layers;
			vector_t<stiled_tileset_reference> m_tilesets;
			string_t m_orientation;
			string_t m_render_order;
			string_t m_tiled_version;
			string_t m_type;
			string_t m_version;
			int m_compression_level = -1;
			int m_next_layer_id = 0;
			int m_next_object_id = 0;
			unsigned m_width = 0;
			unsigned m_height = 0;
			unsigned m_tile_height = 0;
			unsigned m_tile_width = 0;
			bool m_infinite = false;

			RTTR_ENABLE();
		};

	} //- detail

	//- Map resource loaded from Tiled data.
	//------------------------------------------------------------------------------------------------------------------------
	class cmap final : public core::cresource
	{
	public:
		static void destroy(cmap& map);

		explicit cmap(stringview_t mapname);
		explicit cmap(const uint8_t* string, unsigned size);
		cmap();
		~cmap();

		bool load_from_file(stringview_t mapname);
		bool load_from_memory(const uint8_t* string, unsigned size);

	private:
		detail::stiled_map_data m_data;

		RTTR_ENABLE(core::cresource);
	};

	//- Manager responsible for loading and unloading map resources.
	//------------------------------------------------------------------------------------------------------------------------
	class cmap_manager final :
		public core::cservice,
		public core::cresource_manager<cmap>
	{
	public:
		cmap_manager();
		~cmap_manager();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		bool load_sync(stringview_t name, stringview_t mapname);
		bool load_sync(stringview_t name, const uint8_t* string, unsigned size);

		core::cfuture_type<bool> load_async(stringview_t name, stringview_t mapname);
		core::cfuture_type<bool> load_async(stringview_t name, const uint8_t* string, unsigned size);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<cmap>);
	};


} //- kingdoms