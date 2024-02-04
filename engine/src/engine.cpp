#include "engine.hpp"
#include <plugin_logging.h>

namespace engine
{
	namespace
	{
	} //- unnamed


	//------------------------------------------------------------------------------------------------------------------------
	cengine::~cengine()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::configure(sconfig cfg, int argc /*= 0*/, char* argv[] /*= {}*/)
	{
		//- parse arguments and configure self

		//- prepare for running
	}

	//------------------------------------------------------------------------------------------------------------------------
	engine::engine_run_result cengine::run() const
	{
		//- enter engine main loop
		return m_result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::private_init()
	{
		//- most basic

		//- logger

		//- sokol gfx and sokol gp

		//- services
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::private_frame()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::private_cleanup()
	{

	}

} //- engine