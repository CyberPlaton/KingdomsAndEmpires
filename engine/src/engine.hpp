#pragma once
#include <core/core_platform.hpp>
#include <core.h>
#include "spritemancer/sm.hpp"

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	enum engine_run_result : uint8_t
	{
		engine_run_result_ok = 0,
		engine_run_result_fail = 255,
	};

	//- Central entry point of the application (not counting main). Configures the application to be executed,
	//- configures self and registers services etc. Does create a window and a rendering context and provides the main loop.
	//- Constructile from cengine::instance() only to ensure correct functioning.
	//------------------------------------------------------------------------------------------------------------------------
	class cengine final : core::cnon_copyable
	{
	public:
		struct sconfig
		{
			unsigned m_width;
			unsigned m_height;

			unsigned m_msaa_level;
			unsigned m_swap_interval;

			bool m_highdpi;
			bool m_fullscreen;

			bool m_framebuffer_alpha;

			stringview_t m_title;
		};

	public:
		STATIC_INSTANCE(cengine, s_cengine);
		~cengine();

		void configure(sconfig cfg, int argc = 0, char* argv[] = {});
		engine_run_result run() const;

		//- not intended as public, however, there is no other way as of now
		void private_init();
		void private_frame();
		void private_cleanup();

	private:
		cengine() = default;

	private:
		sconfig m_config;
		engine_run_result m_result;
	};

} //- engine