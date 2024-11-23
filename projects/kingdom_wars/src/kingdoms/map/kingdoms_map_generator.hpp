#pragma once
#include <core.h>
#include "kingdoms_map_generation_context.hpp"

namespace kingdoms
{
	//- Map resource loaded from Tiled data.
	//------------------------------------------------------------------------------------------------------------------------
	class cmap final : public core::cresource
	{
	public:
		static void destroy(cmap& map);

		explicit cmap(map_generation_context_ref_t& ctx, stringview_t mapname);
		explicit cmap(map_generation_context_ref_t& ctx, const uint8_t* string, unsigned size);
		cmap();
		~cmap();

		bool load_from_file(map_generation_context_ref_t& ctx, stringview_t mapname);
		bool load_from_memory(map_generation_context_ref_t& ctx, const uint8_t* string, unsigned size);

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