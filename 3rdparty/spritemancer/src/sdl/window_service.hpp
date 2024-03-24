#pragma once
#include "sdl_service.hpp"

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	class cwindow_service final : public core::cservice
	{
	public:
		static constexpr uint16_t C_WINDOW_COUNT_MAX = 256;

		cwindow_service() = default;
		~cwindow_service();

		ref_t<sdl::cwindow> create_main_window(sdl::cwindow::sconfig cfg);
		ref_t<sdl::cwindow> main_window();
		void destroy_main_window();

		ref_t<sdl::cwindow> create(sdl::cwindow::sconfig cfg);
		void destroy(window_t handle);
		ref_t<sdl::cwindow> find(window_t handle);

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float dt) override final;

	private:
		ref_t<sdl::cwindow> m_main_window;
		umap_t<window_t, ref_t<sdl::cwindow>> m_windows;
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