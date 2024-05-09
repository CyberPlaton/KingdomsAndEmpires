#include "spritemancer.hpp"

namespace sm
{
	namespace
	{
		constexpr unsigned C_LAYER_COUNT_MAX = 256;
		static unsigned S_LAYER_COUNT = 0;
		static unsigned S_CURRENT_LAYER = 0;
		static array_t<slayer, C_LAYER_COUNT_MAX> S_LAYERS;

		static core::scolor S_WHITE = { 250, 250, 250, 250 };
		static std::atomic_bool S_RUNNING;
		static bool S_FULLSCREEN = false;
		static bool S_VSYNC = false;
		static unsigned S_X = 0, S_Y = 0, S_W = 0, S_H = 0;
		static float S_DT = 0.0f;

		//------------------------------------------------------------------------------------------------------------------------
		void update_window_size(unsigned w, unsigned h)
		{
			S_W = w;
			S_H = h;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void update_window_viewport()
		{
			entry::renderer()->update_viewport(S_X, S_Y, S_W, S_H);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void engine_update()
		{
			//- platforms may need to handle events
			entry::platform()->process_event();

			//- update HID state, such as keyboard and mouse

			entry::app()->on_update(S_DT);

			//- render frame
			entry::renderer()->update_viewport(S_X, S_Y, S_W, S_H);
			entry::renderer()->clear(S_WHITE, true);

			//- most basic layer, does always exist
			S_LAYERS[0].m_want_update = true;
			S_LAYERS[0].m_show = true;
			entry::renderer()->set_blending_mode(blending_mode_normal);
			entry::renderer()->prepare_frame();

			//- layered rendering, from bottom to top
			for (auto i = 0u; i < S_LAYER_COUNT; ++i)
			{
				auto& layer = S_LAYERS[i];

				if (layer.m_show)
				{
					//- bind render target texture and fill it with color
					entry::renderer()->bind_texture(layer.m_target.texture().id());
					if (layer.m_want_update)
					{
						entry::renderer()->update_texture(layer.m_target.texture().id(),
							layer.m_target.image().m_container.m_width,
							layer.m_target.image().m_container.m_height,
							layer.m_target.image().m_container.m_data);

						layer.m_want_update = false;
					}

					entry::renderer()->draw_layer_quad(S_X, S_Y, S_W, S_H, S_WHITE);

					//- render decals/textures for current layer
					for (const auto& decal : layer.m_decals)
					{
						entry::renderer()->draw_decal(decal);
					}
					layer.m_decals.clear();
				}
			}

			//- present everything
			entry::renderer()->display_frame();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void engine_prepare()
		{
			update_window_size(S_W, S_H);
			update_window_viewport();

			//- create default font

			//- create default rendering layer
			create_layer();
			S_LAYERS[0].m_show = true;
			S_LAYERS[0].m_want_update = true;
			bind_layer(0);

			//- sample time for custom frame timing
		}

		//- Main engine thread where the update and rendering happens. Created from outside
		//------------------------------------------------------------------------------------------------------------------------
		void engine_thread()
		{
			//- starting up
			if (entry::platform()->init_on_thread() != opresult_ok)
			{
				return;
			}
			if (entry::platform()->init_gfx(S_FULLSCREEN, S_VSYNC) != opresult_ok)
			{
				return;
			}

			engine_prepare();

			if (entry::app()->on_init())
			{
				S_RUNNING = false;
			}

			//- main loop
			while (S_RUNNING)
			{
				engine_update();
			}

			//- shutting down
			entry::app()->on_shutdown();
			entry::platform()->shutdown_on_thread();
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult init(stringview_t title, unsigned x, unsigned y, unsigned w, unsigned h, bool fullscreen, bool vsync)
	{
		if (entry::platform()->init() != opresult_ok)
		{
			return opresult_fail_platform_init;
		}
		if (entry::platform()->init_mainwindow(title.data(), x, y, w, h, fullscreen) != opresult_ok)
		{
			return opresult_fail_platform_init;
		}

		//- required for later
		S_FULLSCREEN = fullscreen;
		S_VSYNC = vsync;
		S_X = x;
		S_Y = y;
		S_W = w;
		S_H = h;

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult run()
	{
		S_RUNNING = true;

		auto thread = std::thread(&engine_thread);

		entry::platform()->optional_init_event_mainloop();

		thread.join();

		if (entry::platform()->shutdown() != opresult_ok)
		{
			return opresult_fail_platform_init;
		}
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned create_layer()
	{
		if (S_LAYER_COUNT < C_LAYER_COUNT_MAX)
		{
			auto& layer = S_LAYERS[S_LAYER_COUNT];

			layer.m_target.create(S_W, S_H);

			return S_LAYER_COUNT++;
		}
		return MAX(unsigned);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool bind_layer(unsigned i)
	{
		if (i >= 0 && i <= S_LAYER_COUNT)
		{
			S_CURRENT_LAYER = i;
			return true;
		}
		return false;
	}

} //- sm