#pragma once
#include <core/core_platform.hpp>
#include <core.h>
#include "spritemancer/sm.hpp"
#include <argparse.h>

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	enum engine_run_result : uint8_t
	{
		engine_run_result_ok = 0,
		engine_run_result_failed_parsing_arguments,
		engine_run_result_failed_starting_spritemancer,
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
			sm::cwindow::sconfig m_window_cfg;
		};

	public:
		STATIC_INSTANCE(cengine, s_cengine);
		~cengine();

		engine_run_result configure(sconfig cfg, int argc = 0, char* argv[] = {});
		engine_run_result run() const;

	private:
		cengine() = default;

	private:
		sconfig m_config;
		engine_run_result m_result;

		void handle_arguments(argparse::ArgumentParser& args, int argc, char* argv[]);
		void register_services(argparse::ArgumentParser& args);
	};

} //- engine