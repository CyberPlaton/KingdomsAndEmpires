#pragma once
#include "sdl_service.hpp"

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	enum window_resolution : uint8_t
	{
		window_resolution_custom = 0,
		window_resolution_switch,
		window_resolution_steamdeck,
		window_resolution_sxga,
		window_resolution_hd,
		window_resolution_hd_plus,
		window_resolution_fhd,
		window_resolution_wuxga,
		window_resolution_qhd,
		window_resolution_wqhd,
		window_resolution_uhd,
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cwindow
	{
	public:
		struct sconfig
		{
			std::string m_title				= {};
			std::string m_window_icon_path	= {};
			window_resolution m_resolution	= window_resolution_custom;
			unsigned m_x					= 0;
			unsigned m_y					= 0;
			unsigned m_width				= 0;
			unsigned m_height				= 0;
			int m_flags						= 0; //- SDL_WindowFlags

			RTTR_ENABLE();
		};

		cwindow(sconfig cfg, window_t handle);
		~cwindow();

	private:
		SDL_Window* m_window;
		window_t m_handle;

	private:
		cwindow() = default;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cwindow_service final : public core::cservice
	{
	public:
		static constexpr uint16_t C_WINDOW_COUNT_MAX = 256;

		cwindow_service() = default;
		~cwindow_service();

		ref_t<cwindow> create_main_window(cwindow::sconfig cfg);
		ref_t<cwindow> main_window();
		void destroy_main_window();

		ref_t<cwindow> create(cwindow::sconfig cfg);
		void destroy(window_t handle);
		ref_t<cwindow> find(window_t handle);

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float dt) override final;

	private:
		ref_t<cwindow> m_main_window;
		umap_t<window_t, ref_t<cwindow>> m_windows;
		window_t m_next_id = 0;

		RTTR_ENABLE(core::cservice);
	};

} //- engine

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cwindow_service)
	{
		rttr::registration::class_<cwindow_service>("cwindow_service")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

} //- engine