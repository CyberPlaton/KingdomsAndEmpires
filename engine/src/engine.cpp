#include "engine.hpp"
#include <plugin_logging.h>

namespace engine
{
	namespace
	{
		//- Hijacking sokol app callbacks with engine functionality
		//------------------------------------------------------------------------------------------------------------------------
		static void sokol_init_callback()
		{
			auto& inst = cengine::instance(); inst.private_init();
		}

		//- Hijacking sokol app callbacks with engine functionality
		//------------------------------------------------------------------------------------------------------------------------
		static void sokol_frame_callback()
		{
			auto& inst = cengine::instance(); inst.private_frame();
		}

		//- Hijacking sokol app callbacks with engine functionality
		//------------------------------------------------------------------------------------------------------------------------
		static void sokol_cleanup_callback()
		{
			auto& inst = cengine::instance(); inst.private_cleanup();
		}

		//- Hijacking sokol app callbacks with engine functionality
		//------------------------------------------------------------------------------------------------------------------------
		static void sokol_logging_callback(const char* /*tag = "sg"*/, uint32_t level, uint32_t item_id, const char* message, uint32_t line,
			const char* filename, void* /*data*/)
		{
			//- note: message and filename may be nullptr, i.e. in release (sokol dependent)
			//- note: main message should come first, after that we can display additional information

			switch (level)
			{
			//- panic: basically a crash, as for sokol it would be undefined behavior
			case 0:
			{
				logging::log_critical(fmt::format("[sokol] '{}'\n'{}'\n'{}':'{}'",
									message ? message : "<No message>",
									algorithm::enum_to_string((sg_log_item)item_id),
									filename ? filename : "<Unknown file>",
									line));
				break;
			}
			//- error
			case 1:
			{
				logging::log_error(fmt::format("[sokol] '{}'\n'{}'\n'{}':'{}'",
					message ? message : "<No message>",
					algorithm::enum_to_string((sg_log_item)item_id),
					filename ? filename : "<Unknown file>",
					line));
				break;
			}
			//- warning
			case 2:
			{
				logging::log_warn(fmt::format("[sokol] '{}'\n'{}'\n'{}':'{}'",
					message ? message : "<No message>",
					algorithm::enum_to_string((sg_log_item)item_id),
					filename ? filename : "<Unknown file>",
					line));
				break;
			}
			//- info
			default:
			case 3:
			{
				logging::log_debug(fmt::format("[sokol] '{}'\n'{}'\n'{}':'{}'",
					message ? message : "<No message>",
					algorithm::enum_to_string((sg_log_item)item_id),
					filename ? filename : "<Unknown file>",
					line));
				break;
			}
			}
		}

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
		m_config_desc = sapp_desc{ 0 };
		m_config_desc.init_cb = sokol_init_callback;
		m_config_desc.frame_cb = sokol_frame_callback;
		m_config_desc.cleanup_cb = sokol_cleanup_callback;
		m_config_desc.logger.func = sokol_logging_callback;
	}

	//------------------------------------------------------------------------------------------------------------------------
	engine::engine_run_result cengine::run() const
	{
		//- enter engine main loop
		sapp_run(&m_config_desc);
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