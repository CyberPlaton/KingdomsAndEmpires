#include "logger.hpp"

namespace logging
{

	//------------------------------------------------------------------------------------------------------------------------
	bool clog::init(logging_verbosity verbosity)
	{
		//- make sure we did not initialize already
		ASSERT(m_logger.get() == nullptr, "Logger is already initialized");

		m_verbosity = verbosity;
		spdlog::set_pattern("%^[%n] %v%$");
		spdlog::flush_on((spdlog::level::level_enum)verbosity);
		spdlog::set_level((spdlog::level::level_enum)verbosity);
#ifndef NDEBUG
		//- debug mode
		spdlog::flush_every(std::chrono::seconds(1));
		m_logger = spdlog::stdout_color_mt("debug");
#else
		//- release mode
		spdlog::flush_every(std::chrono::seconds(3));
		m_logger = spdlog::basic_logger_mt("release", "Log.log");
#endif
		m_timer.start();

		log_info("Application started...");

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clog::shutdown()
	{
		log_info("Application ending...");
		m_logger.reset();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clog::trace(const char* message)
	{
		if (m_verbosity != logging_verbosity::logging_verbosity_off &&
			m_verbosity <= logging_verbosity::logging_verbosity_trace)
		{
			m_logger->trace(message);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clog::info(const char* message)
	{
		if (m_verbosity != logging_verbosity::logging_verbosity_off &&
			m_verbosity <= logging_verbosity::logging_verbosity_info)
		{
			m_logger->info(message);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clog::warn(const char* message)
	{
		if (m_verbosity != logging_verbosity::logging_verbosity_off &&
			m_verbosity <= logging_verbosity::logging_verbosity_warn)
		{
			m_logger->warn(message);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clog::error(const char* message)
	{
		if (m_verbosity != logging_verbosity::logging_verbosity_off &&
			m_verbosity <= logging_verbosity::logging_verbosity_error)
		{
			m_logger->error(message);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clog::critical(const char* message)
	{
		if (m_verbosity != logging_verbosity::logging_verbosity_off &&
			m_verbosity <= logging_verbosity::logging_verbosity_critical)
		{
			m_logger->critical(message);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clog::debug(const char* message)
	{
		if (m_verbosity != logging_verbosity::logging_verbosity_off &&
			m_verbosity <= logging_verbosity::logging_verbosity_debug)
		{
			m_logger->debug(message);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	float clog::runtime() const
	{
		return m_timer.secs();
	}

	//------------------------------------------------------------------------------------------------------------------------
	float clog::runtime_ms() const
	{
		return m_timer.millisecs();
	}

} //- logging